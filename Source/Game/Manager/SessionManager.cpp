#include "SessionManager.h"
#include <ranges>
#include <type_traits>

namespace
{
	/// @brief プレイヤー・敵共通のステート更新処理
	template <typename TIds, typename TResolver>
	bool UpdateStates(
	    TIds& _ids,
	    std::string_view _stateNames,
	    SizedBuffer<uint8_t>& _values,
	    TResolver&& _resolver)
	{
		// 単体IDなら1件、複数IDならバッファのサイズ
		constexpr bool isSingleId = std::is_integral_v<std::remove_cvref_t<TIds>>;

		const size_t count = [&]()
		{
			if constexpr (isSingleId)
				return size_t{ 1 };
			else
				return _ids.Size();
		}();

		if (count != _values.Size())
		{
			VCU_LOG("ステート更新失敗：ID数と値の数が一致しません");
			return false;
		}

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

		if (stateNames.size() != count)
		{
			VCU_LOG("ステート名とIDの数が一致しません");
			return false;
		}

		bool updated = false;

		for (size_t i = 0; i < count; ++i)
		{
			// 単体ID・複数IDのどちらにも対応
			const int32_t id = [&]()
			{
				if constexpr (isSingleId)
					return _ids;
				else
					return _ids[i];
			}();

			// IDから対象のステータスを取得
			auto* status = _resolver(id);

			if (status == nullptr)
				continue;

			const std::string& name = stateNames[i];
			const bool value        = _values[i] != 0;

			auto& states = status->states;
			auto stateIt = states.find(name);

			if (stateIt == states.end())
			{
				states.emplace(name, value);
				updated = true;
			}
			else if (stateIt->second != value)
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

void SessionManager::SetMemberNames(
    const std::string_view _names)
{
	inSessionStatus_.memberNames.clear();
	inSessionStatus_.memberId.clear();

	int32_t id = 0;

	// 文字列を改行で分解してく
	for (auto&& line_range : _names | std::views::split('\n'))
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

		inSessionStatus_.memberNames.push_back(std::string{ line });
		inSessionStatus_.memberId.push_back(id++);
	}
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
    SizedBuffer<float>& _rotations)
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
		playerStatus.updatedFlag = !playerStatus.updatedFlag;  // 更新フラグを反転

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
		    // プレイヤーIDからID配列内の位置を検索
		    auto idIt = std::ranges::find(inSessionStatus_.memberId, playerId);

		    if (idIt == inSessionStatus_.memberId.end())
		    {
			    VCU_LOG("プレイヤーステート更新失敗：プレイヤーID={} が存在しません", playerId);
			    return nullptr;
		    }

		    // IDと同じインデックスの名前を取得
		    const size_t index     = std::distance(inSessionStatus_.memberId.begin(), idIt);
		    const auto& playerName = inSessionStatus_.memberNames[index];

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
			    VCU_LOG("敵ステート更新失敗：敵ID={} が存在しません", enemyId);
			    return nullptr;
		    }

		    return &it->second;
	    });
}

void SessionManager::UpdateDayTime(
    float _currentTimeSec)
{
	inSessionStatus_.currentTimeSec     = _currentTimeSec;
	inSessionStatus_.hasReceivedDayTime = true;
}
