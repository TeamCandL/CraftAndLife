#include "SessionManager.h"
#include "../PlayScene/StateValueCode.h"
#include <ranges>
#include <type_traits>

namespace
{
	bool IsStateValueEqual(
	    const StateValue& a,
	    const StateValue& b)
	{
		if (a.index() != b.index())
		{
			return false;
		}

		return std::visit(
		    [](const auto& lhs, const auto& rhs)
		    {
			    using L = std::decay_t<decltype(lhs)>;
			    using R = std::decay_t<decltype(rhs)>;

			    if constexpr (std::is_same_v<L, R>)
			    {
				    return lhs == rhs;
			    }
			    else
			    {
				    return false;
			    }
		    },
		    a,
		    b);
	}

	/// @brief プレイヤー・敵共通のステート更新処理
	template <typename TIds, typename TResolver>
	bool UpdateStates(
	    const TIds& _ids,
	    std::string_view _stateNames,
	    SizedBuffer<uint8_t>& _values,
	    TResolver&& _resolver)
	{
		constexpr bool isSingleId = std::is_integral_v<std::remove_cvref_t<TIds>>;

		// ステート名を改行で分割
		std::vector<std::string> stateNames;

		for (auto&& lineRange : _stateNames | std::views::split('\n'))
		{
			std::string_view name{ lineRange.begin(), lineRange.end() };

			// CRLF対策
			if (!name.empty() && name.back() == '\r')
				name.remove_suffix(1);

			if (name.empty())
			{
				VCU_LOG("ステート名が不正です");
				return false;
			}

			stateNames.emplace_back(name);
		}

		// 値は1ステートにつき5バイト
		const size_t valueCount = _values.Size() / StateValueCode::ValueSize;

		if (_values.Size() % StateValueCode::ValueSize != 0)
		{
			VCU_LOG("ステート値のデータサイズが不正です");
			return false;
		}

		// プレイヤーは複数ステート、敵は複数IDに対応
		const size_t count = stateNames.size();

		if (count != valueCount)
		{
			VCU_LOG("ステート名と値の数が一致しません");
			return false;
		}

		if constexpr (!isSingleId)
		{
			if (_ids.Size() != count)
			{
				VCU_LOG("敵IDとステート名の数が一致しません");
				return false;
			}
		}

		bool updated = false;

		for (size_t i = 0; i < count; ++i)
		{
			// プレイヤーなら全ステートを同じIDに適用
			// 敵なら各ステートに対応するIDを取得
			int32_t id{};

			if constexpr (isSingleId)
			{
				// プレイヤー：すべてのステートで同じIDを使用
				id = static_cast<int32_t>(_ids);
			}
			else
			{
				// 敵：ステートごとに対応するIDを取得
				id = _ids[i];
			}

			auto* status = _resolver(id);

			if (status == nullptr)
				continue;

			// 5バイト単位で値をデコード
			StateValue value;

			const uint8_t* data = &_values[i * StateValueCode::ValueSize];

			if (!StateValueCode::Decode(data, StateValueCode::ValueSize, value))
			{
				VCU_LOG("ステート値のデコードに失敗しました");
				return false;
			}

			const std::string& name = stateNames[i];

			std::visit(
			    [id, &name](const auto& stateValue)
			    {
				    using T = std::decay_t<decltype(stateValue)>;

				    std::string valueText;

				    if constexpr (std::is_same_v<T, bool>)
					    valueText = stateValue ? "true" : "false";
				    else if constexpr (std::is_same_v<T, std::string>)
					    valueText = stateValue;
				    else
					    valueText = std::to_string(stateValue);

				    VCU_LOG("ステート受信: ID={}, Name={}, Value={}", id, name, valueText);
			    },
			    value);

			auto& states = status->states;
			auto stateIt = states.find(name);

			if (stateIt == states.end())
			{
				states.emplace(name, value);
				updated = true;
			}
			else if (!IsStateValueEqual(stateIt->second, value))
			{
				stateIt->second = value;
				updated         = true;
			}
		}

		return updated;
	}
}  // namespace

void SessionManager::Reset()
{
	inSessionStatus_ = {};
}

void SessionManager::SetPlayerName(
    const std::string_view _name)
{
	inSessionStatus_.name = _name;
}

void SessionManager::SetPlayerRole(
    int32_t _playerId,
    int32_t _roleId)
{
	// プレイヤーIDに対応する役割を設定
	inSessionStatus_.memberRoles[_playerId] = _roleId;
}

bool SessionManager::TryGetPlayerRole(
    int32_t _playerId,
    int32_t& _outRoleId) const
{
	// 役割を検索
	auto it = inSessionStatus_.memberRoles.find(_playerId);
	if (it == inSessionStatus_.memberRoles.end())
	{
		return false;
	}

	_outRoleId = it->second;
	return true;
}

void SessionManager::SetMemberInfo(
    const std::string_view _members)
{
	VCU_LOG("SetMemberInfo 呼び出し");
	VCU_LOG("受信したメンバー情報: {}", _members);

	inSessionStatus_.memberNames.clear();
	inSessionStatus_.memberId.clear();
	inSessionStatus_.memberRoles.clear();

	// 文字列を改行で分解してく
	for (auto&& line_range : _members | std::views::split('\n'))
	{
		std::string_view line{ line_range.begin(), line_range.end() };

		// CRLF 対策
		if (!line.empty() && line.back() == '\r')
		{
			line.remove_suffix(1);
		}

		if (line.empty())
		{
			continue;
		}

		// ID、名前、RoleID の区切り位置を取得
		const size_t firstComma = line.find(',');
		if (firstComma == std::string_view::npos)
		{
			continue;
		}

		const size_t secondComma = line.find(',', firstComma + 1);
		if (secondComma == std::string_view::npos)
		{
			continue;
		}

		// 各情報を取り出す
		const std::string_view idText = line.substr(0, firstComma);

		const std::string_view name = line.substr(firstComma + 1, secondComma - firstComma - 1);

		const std::string_view roleText = line.substr(secondComma + 1);

		// 数値に変換
		const int32_t playerId = std::stoi(std::string{ idText });
		const int32_t roleId   = std::stoi(std::string{ roleText });

		inSessionStatus_.memberNames.push_back(std::string{ name });
		inSessionStatus_.memberId.push_back(playerId);
		inSessionStatus_.memberRoles[playerId] = roleId;

		VCU_LOG("メンバー登録: ID={}, Name={}, RoleID={}", playerId, name, roleId);
	}
	VCU_LOG("登録完了: メンバー数={}", inSessionStatus_.memberNames.size());
}

void SessionManager::SetJoinRoomName(
    const std::string_view _name)
{
	inSessionStatus_.joinRoomName = _name;
}

void SessionManager::SetLobbyHost(
    bool _imLobbyHost)
{
	inSessionStatus_.imLobbyHost = _imLobbyHost;
}

void SessionManager::UpdatePlayers(
    const std::string_view _names,
    SizedBuffer<float>& _positions,
    SizedBuffer<float>& _rotations,
    SizedBuffer<int32_t>& _haveItemIds,
    SizedBuffer<int32_t>& _attentionLevel)
{
	inSessionStatus_.updatedFlag = !inSessionStatus_.updatedFlag;
	auto& playerStatus           = inSessionStatus_.playersStatus;

	size_t i = 0;
	// 文字列を改行で分解してく
	for (auto&& line_range : _names | std::views::split('\n'))
	{
		std::string_view line{ line_range.begin(), line_range.end() };

		// CRLF 対策
		if (!line.empty() && line.back() == '\r')
		{
			line.remove_suffix(1);
		}

		// 空行が来たら無視（サーバー側の末尾改行などで混入することがある）
		if (line.empty())
		{
			VCU_LOG("UpdatePlayers: 空の名前行をスキップ");
			continue;  // i は増やさない
		}

		if (Vec3::AT_MAX * i >= _positions.Size())
		{
			VCU_LOG("取得した名前分の座標データのサイズが足りない({}個以上必要)", Vec3::AT_MAX * i);
			break;
		}

		if (Vec2::AT_MAX * i >= _rotations.Size())
		{
			VCU_LOG("取得した名前分の回転データのサイズが足りない({}個以上必要)", Vec3::AT_MAX * i);
			break;
		}

		if (i >= _haveItemIds.Size())
		{
			VCU_LOG("取得した名前分のアイテムIdのサイズが足りない({}個以上必要)", i);
			break;
		}

		if (i >= _attentionLevel.Size())
		{
			VCU_LOG("取得した名前分の注目度データのサイズが足りない({}個以上必要)", i);
			break;
		}

		// CRLF 対策OtherPlayer
		if (!line.empty() && line.back() == '\r')
		{
			line.remove_suffix(1);
		}

		InSessionStatus::PlayerStatus& playerStatus = inSessionStatus_.playersStatus[std::string{ line }];
		playerStatus.position                       = Vec3{
            _positions[i * Vec3::AT_MAX + Vec3::AT_X],
            _positions[i * Vec3::AT_MAX + Vec3::AT_Y],
            _positions[i * Vec3::AT_MAX + Vec3::AT_Z],
		};

		playerStatus.rotation = Vec2{
			_rotations[i * Vec2::AT_MAX + Vec2::AT_X],
			_rotations[i * Vec2::AT_MAX + Vec2::AT_Y],
		};
		playerStatus.haveItemId     = _haveItemIds[i];
		playerStatus.attentionLevel = _attentionLevel[i];
		playerStatus.updatedFlag    = !playerStatus.updatedFlag;  // 更新フラグを反転

		// VCU_LOG(
		//     "Update position[{}] = ({}, {}, {})",
		//     line,
		//     playerStatus.position.x,
		//     playerStatus.position.y,
		//     playerStatus.position.z);

		// VCU_LOG("Update rotation[{}] = ({}, {})", line, playerStatus.rotation.x, playerStatus.rotation.y);

		i++;
	}

	// 更新フラグが不一致は削除していく
	// TODO: ittannむし！！！
	/*std::erase_if(
	    inSessionStatus_.playersStatus,
	    [this](const auto& nameAndStatus)
	    {
	        return nameAndStatus.second.updatedFlag != inSessionStatus_.updatedFlag;
	    });*/
}

const InSessionStatus::PlayerStatus& SessionManager::RefPlayerStatus(
    const std::string& _playerName) const
{
	return inSessionStatus_.playersStatus.at(_playerName);
}

bool SessionManager::IsExistPlayer(
    const std::string& _playerName) const
{
	if (inSessionStatus_.playersStatus.count(_playerName) == 0)
	{
		VCU_LOG("なんでだよおおおおおおお");
	}

	// TODO: ここで文字化けしているためうまく判定できていない可能性がある。
	return inSessionStatus_.playersStatus.count(_playerName) != 0;
}

bool SessionManager::TryGetPlayerName(
    int32_t _playerId,
    std::string& _outName) const
{
	const auto& ids   = inSessionStatus_.memberId;
	const auto& names = inSessionStatus_.memberNames;

	if (ids.size() != names.size())
		return false;

	auto it = std::ranges::find(ids, _playerId);

	if (it == ids.end())
		return false;

	const size_t index = std::distance(ids.begin(), it);

	_outName = names[index];
	return true;
}

bool SessionManager::TryGetPlayerId(
    std::string_view _playerName,
    int32_t& _outId) const
{
	const auto& ids   = inSessionStatus_.memberId;
	const auto& names = inSessionStatus_.memberNames;

	if (ids.size() != names.size())
		return false;

	auto it = std::ranges::find(names, _playerName);

	if (it == names.end())
		return false;

	const size_t index = std::distance(names.begin(), it);

	_outId = ids[index];
	return true;
}

bool SessionManager::HasAllPlayersStatus() const
{
	return inSessionStatus_.playersStatus.size() > 0;
}

bool SessionManager::UpdatePlayerStates(
    int32_t _playerId,
    std::string_view _stateNames,
    SizedBuffer<uint8_t>& _values)
{
	return UpdateStates(
	    _playerId,
	    _stateNames,
	    _values,
	    [this](int32_t playerId) -> InSessionStatus::PlayerStatus*
	    {
		    std::string playerName;

		    // プレイヤーIDから名前を取得
		    if (!TryGetPlayerName(playerId, playerName))
		    {
			    VCU_LOG("プレイヤーステート更新失敗：ID={} の名前を取得できません", playerId);
			    return nullptr;
		    }

		    // 名前からプレイヤーステータスを取得
		    auto it = inSessionStatus_.playersStatus.find(playerName);

		    if (it == inSessionStatus_.playersStatus.end())
		    {
			    VCU_LOG("プレイヤーステート更新失敗：{} が存在しません", playerName);
			    return nullptr;
		    }

		    return &it->second;
	    });
}

void SessionManager::UpdateEnemies(
    SizedBuffer<int>& _enemyIds,
    SizedBuffer<int>& _enemyTypes,
    SizedBuffer<float>& _positions,
    SizedBuffer<float>& _rotations)
{
	const size_t count = _enemyIds.Size();

	// 受信データ数を確認
	if (_enemyTypes.Size() < count || _positions.Size() < count * Vec3::AT_MAX ||
	    _rotations.Size() < count * Vec2::AT_MAX)
	{
		VCU_LOG(
		    "敵データ不足 ID={}, 種類={}, 座標={}, 回転={}",
		    count,
		    _enemyTypes.Size(),
		    _positions.Size(),
		    _rotations.Size());

		return;
	}

	inSessionStatus_.updatedFlag = !inSessionStatus_.updatedFlag;

	auto& enemiesStatus = inSessionStatus_.enemiesStatus;

	for (size_t i = 0; i < count; ++i)
	{
		const int enemyId = _enemyIds[i];

		auto& enemyStatus = enemiesStatus[enemyId];

		enemyStatus.enemyType = _enemyTypes[i];

		enemyStatus.position = Vec3{ _positions[i * Vec3::AT_MAX + Vec3::AT_X],
			                         _positions[i * Vec3::AT_MAX + Vec3::AT_Y],
			                         _positions[i * Vec3::AT_MAX + Vec3::AT_Z] };

		enemyStatus.rotation =
		    Vec2{ _rotations[i * Vec2::AT_MAX + Vec2::AT_X], _rotations[i * Vec2::AT_MAX + Vec2::AT_Y] };

		enemyStatus.updatedFlag = inSessionStatus_.updatedFlag;

		VCU_LOG(
		    "SessionManager保存 ID={}, 座標=({}, {}, {})",
		    enemyId,
		    enemyStatus.position.x,
		    enemyStatus.position.y,
		    enemyStatus.position.z);
	}

	// 今回受信しなかった敵を削除
	std::erase_if(
	    enemiesStatus,
	    [this](const auto& idAndStatus)
	    {
		    return idAndStatus.second.updatedFlag != inSessionStatus_.updatedFlag;
	    });
}

bool SessionManager::IsExistEnemy(
    int _enemyId) const
{
	return inSessionStatus_.enemiesStatus.count(_enemyId) != 0;
}

bool SessionManager::UpdateEnemyStates(
    SizedBuffer<int32_t>& _enemyIds,
    std::string_view _stateNames,
    SizedBuffer<uint8_t>& _values)
{
	return UpdateStates(
	    _enemyIds,
	    _stateNames,
	    _values,
	    [this](int32_t enemyId) -> InSessionStatus::EnemyStatus*
	    {
		    auto it = inSessionStatus_.enemiesStatus.find(enemyId);

		    if (it == inSessionStatus_.enemiesStatus.end())
		    {
			    VCU_LOG("ステート更新失敗：敵ID={} が存在しません", enemyId);
			    return nullptr;
		    }

		    return &it->second;
	    });
}

void SessionManager::SetSelfStateRegistry(
    const StateRegistry* _registry)
{
	selfStateRegistry_ = _registry;
}

const StateRegistry* SessionManager::GetSelfStateRegistry() const
{
	return selfStateRegistry_;
}

void SessionManager::UpdateDayTime(
    float _currentTimeSec,
    int32_t _dayCount)
{
	inSessionStatus_.currentTimeSec     = _currentTimeSec;
	inSessionStatus_.dayCount           = _dayCount;
	inSessionStatus_.hasReceivedDayTime = true;
}