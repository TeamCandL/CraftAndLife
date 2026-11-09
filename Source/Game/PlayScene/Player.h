#pragma once
#include "IPlayer.h"
#include "Player/IPlayerController.h"
#include "PlayerRoleType.h"
#include <Game/Game.h>
#include <Game/PlayScene/ItemSystem.h>
#include "Player/PlayerAnimType.h"
#include "Player/PlayerJobType.h"
#include "Player/HelthState.h"

struct StateRegistry;

/// @brief プレイヤーのデータ
struct PlayerData
{
	PlayerData(
	    const GameObject _objMainCamera) :
	    objMainCamera{ _objMainCamera }
	{
	}

	Resource<Shader> shaderFront{ "Shader/Unlit.hlsl", RasterizerConfig{ .cullMode = D3D11_CULL_FRONT } };
	Resource<Shader> shaderBack{ "Shader/Unlit.hlsl", RasterizerConfig{ .cullMode = D3D11_CULL_FRONT } };
	Resource<Shader> shader{ "Shader/Unlit.hlsl" };

	std::array<Resource<Model>, PlayerJobType_Max> models{
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-female-b.fbx",  // Healer
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-male-b.fbx",    // Tank
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-male-f.fbx",    // Fighter
		"Model/kenny/kenney_mini-characters/Models/FBX format/character-female-f.fbx",  // Archer
	};
	// Resource<Model> model{ "Model/kenny/kenney_mini-characters/Models/FBX format/character-female-a.fbx" };

	Component<Transform> transform{};
	Component<TransformAnimator> transformAnimator{};  ///< FBXではなくTransformコンポーネントのアニメーションするやつ
	bool isAnimPreviewingDebug{};                      ///< アニメーションテスト用ふらぐ
	Component<RigidBody> rigidBody{};
	Component<StaticOBB> obb{};
	Component<OBBRenderer> obbRenderer{};
	Component<Name> name{ "Player" };

	PlayerJobType jobType{ PlayerJobType_Healer };  ///< プレイヤーの役職
	HelthState helthState{ HelthState_Live };       ///< プレイヤーの生存ステータス

	GameObject objMainCamera{};        ///< プレイヤーのカメラ
	GameObject objPlayerController{};  ///< プレイヤーの操作
	GameObject objJobController{};     ///< 役職固有の操作
	GameObject objPlayerHand{};        ///< プレイヤーの手持ち
	GameObject objPlayerBag{};
	GameObject objHotbar{};
	PlayerRoleType role{ PlayerRoleType_Max };

	float angleX{};
	float angleY{};

	PlayerAnimType animIndex{};  ///< アニメーション
	float animSpeed{ 1.0f };     ///< アニメーションの再生速度 通常が1.0f

	// パラメータ
	int32_t maxHP{ 100 };         ///< 最大HP
	int32_t hp{ 100 };            ///< 現在HP
	int32_t damage{ 10 };         ///< 攻撃力
	int32_t attentionLevel{ 5 };  ///< 敵からの注目度 基本値：5 数値が高いほど狙われやすい

	const float ATTACK_COOL_TIME{ 5.0f };
	float nowAttackCoolTime{ 0.0f };

	const float ATTACK_COLL_LIFE_TIME{ 1.0f };
	const float DAMAGED_COOL_TIME{ 1.0f };
	float nowDamagedCoolTime{ 0.0f };

	const Vec3 OBB_OFFSET{ 0.0f, 0.003f, 0.0f };  ///< OBBのオフセット(位置)
	const float OBB_SCALE{ 0.005f };              ///< OBBのスケール

	// ジャンプ用パラメータ
	float jumpVelocity{ 0.0f };         ///< 上下方向の速度
	const float JUMP_POWER{ 12.0f };    ///< ジャンプ初速
	const float GRAVITY{ 15.0f };       ///< 重力
	const float GROUND_HEIGHT{ 0.0f };  ///< 地面の高さ
	const float JUMP_CUT_RATE{ 0.5f };  ///< ボタンを離したときの速度倍率
	bool isJumping{ false };            ///< ジャンプ中かどうか

	// 足音SE
	Resource<Audio> footstepSE{ "BGM_SE/footsteps.mp3" };

	// ジャンプ音
	Resource<Audio> jumpSE{ "BGM_SE/Jump.mp3" };

	// 盾で攻撃を防いだときのSE
	Resource<ResourceType::Audio> shieldGuardSE{ "BGM_SE/ShieldGuard.mp3" };

	// 足音を鳴らす間隔
	float footstepTimer{ 0.0f };

	// 足音の間隔（秒）
	const float FOOTSTEP_INTERVAL{ 0.4f };

	// HP減少テスト用
	float hpTestTimer{ 0.0f };

	struct JobStatus
	{
		float moveSpeed{ 0.1f };
		Vec3 moveVelocity{};
		float maxHpBoost{ 0.0f };
	} jobStatus;  ///< プレイヤーの役職固有のステータス

	// inline static const float DIE_ANIM_TIME_SEC_MAX{ 21.0f / 60.0f };  ///< 死亡アニメーションの時間
	inline static const float DIE_ANIM_TIME_SEC_MAX{ 20.0f / 60.0f };  ///< 死亡アニメーションの時間

	struct LiveStatus
	{
		float dieAnimTimeSec{ DIE_ANIM_TIME_SEC_MAX };
	} liveStatus;  ///< 生存中のステータス

	// クラフトUIを開いているか(クラフト機能側がUIを表示するか判定するのに使う)
	bool isOpenCraftUI{ false };

	bool isHoldShield{ false };  ///< シールドを構えているか true / false

	float shieldGurdAngleRad{ DirectX::XM_PI / 180.0f * 90.0f };  ///< シールドで防げる正面に対する攻撃の角度
};

/// @brief プレイヤー
struct Player : ObjectBase<PlayerData>, IPlayer
{
	using ObjectBase<PlayerData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;

	/// @brief 描画処理
	void Draw() override;

	void AttackCollider();

	void Damaged(int32_t _damage, Vec3 _worldPos);

	/// @brief 生存ステータスを設定する
	/// @param _state 生存ステータス
	void SetHelthState(const HelthState _state);

	/// @brief プレイヤーの役職を設定する
	/// @param _jobType 役職の種類
	void SetJob(const PlayerJobType _jobType);

	/// @brief 移動処理
	void MoveBody();

	/// @brief 死亡アニメーション
	void DeadAnim();
	/// @brief 死亡処理
	void Dead()
	{
		attentionLevel = 0;
	}

	/// @brief アイテムを与える(ホットバーで回収できるアイテムとは限らない)
	/// @param _itemId アイテムId
	/// @param _fromPosition どこからアイテムを取得したかの座標
	void GiveItem(const ItemId _itemId, const Vec3 _fromPosition);

	void RegisterStates(StateRegistry& registry) const;

	Vec3 GetPosition() const override;
	float GetAngleY() const override;

	void UpdatePickNearestItem();
	void TryAddItem(ItemId _id);
	void SpawnItem(ItemId _id);
};