#include "SessionManager.h"
#include <ranges>

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

	// 文字列を改行で分解してく
	for (auto&& line_range : _names | std::views::split('\n'))
	{
		std::string_view line{ line_range.begin(), line_range.end() };

		// CRLF 対策
		if (!line.empty() && line.back() == '\r')
		{
			line.remove_suffix(1);
		}

		inSessionStatus_.memberNames.push_back(std::string{ line });
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

void SessionManager::UpdateEnemies(
    SizedBuffer<int>& _enemyIds,
    SizedBuffer<int>& _enemyTypes,
    SizedBuffer<float>& _positions,
    SizedBuffer<float>& _rotations)
{
	inSessionStatus_.updatedFlag = !inSessionStatus_.updatedFlag;
	auto& enemiesStatus          = inSessionStatus_.enemiesStatus;

	size_t count = _enemyIds.Size();

	for (size_t i = 0; i < count; i++)
	{
		int enemyId = _enemyIds[i];

		auto& enemyStatus = enemiesStatus[enemyId];

		enemyStatus.enemyType = _enemyTypes[i];

		if (_enemyTypes.Size() < count)
		{
			VCU_LOG("敵の種類データが不足しています。ID数: {}, 種類数: {}", count, _enemyTypes.Size());
			return;
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

		enemyStatus.position = Vec3{
			_positions[i * Vec3::AT_MAX + Vec3::AT_X],
			_positions[i * Vec3::AT_MAX + Vec3::AT_Y],
			_positions[i * Vec3::AT_MAX + Vec3::AT_Z],
		};

		enemyStatus.rotation = Vec2{
			_rotations[i * Vec2::AT_MAX + Vec2::AT_X],
			_rotations[i * Vec2::AT_MAX + Vec2::AT_Y],
		};

		enemyStatus.updatedFlag = inSessionStatus_.updatedFlag;

		// VCU_LOG(
		//     "Update position[{}] = ({}, {}, {})",
		//     enemyId,
		//     enemyStatus.position.x,
		//     enemyStatus.position.y,
		//     enemyStatus.position.z);

		// VCU_LOG("Update rotation[{}] = ({}, {})", enemyId, enemyStatus.rotation.x, enemyStatus.rotation.y);
	}

	// 今回の受信データに存在しなかった敵を削除
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
	const size_t count = _enemyIds.Size();

	if (count != _values.Size())
	{
		VCU_LOG("敵ステートのID数と値の数が一致しません");
		return false;
	}

	size_t i     = 0;
	bool updated = false;

	for (auto&& lineRange : _stateNames | std::views::split('\n'))
	{
		std::string_view stateName{ lineRange.begin(), lineRange.end() };

		// CRLF対策
		if (!stateName.empty() && stateName.back() == '\r')
		{
			stateName.remove_suffix(1);
		}

		if (stateName.empty() || i >= count)
		{
			VCU_LOG("敵ステート名が不正、またはデータ数が一致しません");
			return false;
		}

		const int enemyId = _enemyIds[i];
		const bool value  = _values[i] != 0;

		auto enemyIt = inSessionStatus_.enemiesStatus.find(enemyId);

		if (enemyIt == inSessionStatus_.enemiesStatus.end())
		{
			VCU_LOG("敵ステート更新失敗：敵ID={} が存在しません", enemyId);
			++i;
			continue;
		}

		auto& states = enemyIt->second.states;
		const std::string key{ stateName };

		auto stateIt = states.find(key);

		if (stateIt == states.end())
		{
			states.emplace(key, value);
			updated = true;
		}
		else if (stateIt->second != value)
		{
			stateIt->second = value;
			updated         = true;
		}

		++i;
	}

	if (i != count)
	{
		VCU_LOG("敵ステート名と敵IDの数が一致しません");
		return false;
	}

	return updated;
}

void SessionManager::UpdateDayTime(
    float _currentTimeSec)
{
	inSessionStatus_.currentTimeSec     = _currentTimeSec;
	inSessionStatus_.hasReceivedDayTime = true;
}
