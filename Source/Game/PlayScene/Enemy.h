#pragma once
#include "EnemyAiCheckPoint.h"
#include <Game/Game.h>
#include "EnemyType.h"
#include "EnemyStatus.h"

struct TargetInfo
{
	Vec3 _pos{};
	int32_t _attentionLevel{ 5 };
};

/// @brief 敵のパラメータ初期値。Skeleton, BossSkeleton, Spider, Bat, Dragonの順番でパラメータを格納
namespace EnemyParam
{
	const std::vector<int32_t> Hp{ 50, 300, 30, 20, 500 };           ///< 体力
	const std::vector<int32_t> Damage{ 10, 25, 8, 5, 40 };           ///< 攻撃力
	const std::vector<float> Speed{ 5.0f, 3.5f, 6.5f, 7.5f, 4.5f };  ///< 移動速度
	const std::vector<bool> HasCloseAttack{ true, true, true, true, true };
	const std::vector<bool> HasLongAttack{ false, false, false, false, true };
	const std::vector<float> VisibilityDist{ 50.0f, 100.0f, 50.0f, 40.0f, 140.0f };
	const std::vector<float> VisibilityAngle{ 90.0f, 90.0f, 90.0f, 90.0f, 90.0f };
	const std::vector<float> AttackCoolTime{ 2.0f, 1.5f, 1.5f, 1.0f, 3.0f };
	const std::vector<float> RotationAnglePerSec{ 30.0f, 20.0f, 45.0f, 40.0f, 35.0f };
	const std::vector<float> Scale{ 0.7f, 1.1f, 0.4f, 0.3f, 1.2f };
	const std::vector<int32_t> DropItemId{ 9, 9, 10, 11, 12 };
}  // namespace EnemyParam

struct StateRegistry;

struct EnemyData
{
	Component<Name> nameDEB{};

	/// @brief 指定なし生成
	EnemyData(
	    GameObject objEnemyController_,
	    const std::string& _name = "AAA") :
	    ATTACK_COOL_TIME{ EnemyParam::AttackCoolTime[static_cast<int>(EnemyType::Skeleton)] }
	{
		nameDEB.content    = _name;
		objEnemyController = objEnemyController_;

		transform.position = Vec3{ 0.0f, 15.0f, 0.0f };
		enemyType          = EnemyType::Skeleton;

		hp                  = EnemyParam::Hp[static_cast<int>(EnemyType::Skeleton)];
		damage              = EnemyParam::Damage[static_cast<int>(EnemyType::Skeleton)];
		speed               = EnemyParam::Speed[static_cast<int>(EnemyType::Skeleton)];
		hasCloseAttack      = EnemyParam::HasCloseAttack[static_cast<int>(EnemyType::Skeleton)];
		hasLongAttack       = EnemyParam::HasLongAttack[static_cast<int>(EnemyType::Skeleton)];
		visibilityDist      = EnemyParam::VisibilityDist[static_cast<int>(EnemyType::Skeleton)];
		visibilityAngle     = EnemyParam::VisibilityAngle[static_cast<int>(EnemyType::Skeleton)];
		rotationAnglePerSec = EnemyParam::RotationAnglePerSec[static_cast<int>(EnemyType::Skeleton)];
		scale               = EnemyParam::Scale[static_cast<int>(EnemyType::Skeleton)];
		dropItemID          = EnemyParam::DropItemId[static_cast<int>(EnemyType::Skeleton)];
		transform.scale     = Vec3::One() * scale;
	}
	/// @brief 位置指定生成
	/// @param position_ 位置
	EnemyData(
	    GameObject objEnemyController_,
	    const Vec3 position_) :
	    transform{ position_ },
	    ATTACK_COOL_TIME{ EnemyParam::AttackCoolTime[static_cast<int>(EnemyType::Skeleton)] }
	{
		objEnemyController = objEnemyController_;

		enemyType = EnemyType::Skeleton;

		hp                  = EnemyParam::Hp[static_cast<int>(EnemyType::Skeleton)];
		damage              = EnemyParam::Damage[static_cast<int>(EnemyType::Skeleton)];
		speed               = EnemyParam::Speed[static_cast<int>(EnemyType::Skeleton)];
		hasCloseAttack      = EnemyParam::HasCloseAttack[static_cast<int>(EnemyType::Skeleton)];
		hasLongAttack       = EnemyParam::HasLongAttack[static_cast<int>(EnemyType::Skeleton)];
		visibilityDist      = EnemyParam::VisibilityDist[static_cast<int>(EnemyType::Skeleton)];
		visibilityAngle     = EnemyParam::VisibilityAngle[static_cast<int>(EnemyType::Skeleton)];
		rotationAnglePerSec = EnemyParam::RotationAnglePerSec[static_cast<int>(EnemyType::Skeleton)];
		scale               = EnemyParam::Scale[static_cast<int>(EnemyType::Skeleton)];
		dropItemID          = EnemyParam::DropItemId[static_cast<int>(EnemyType::Skeleton)];
		transform.scale     = Vec3::One() * scale;
	}
	/// @brief 種類指定生成
	/// @param enemyType_ 敵の種類(Skeleton, Spider, Bat, Dragon)
	EnemyData(
	    GameObject objEnemyController_,
	    const int enemyType_) :
	    enemyType{ static_cast<EnemyType>(enemyType_) },
	    ATTACK_COOL_TIME{ EnemyParam::AttackCoolTime[enemyType_] }
	{
		objEnemyController = objEnemyController_;

		modelIndex = enemyType_;

		transform.position = Vec3{ 0.0f, 0.0f, 0.0f };

		hp                  = EnemyParam::Hp[enemyType_];
		damage              = EnemyParam::Damage[enemyType_];
		speed               = EnemyParam::Speed[enemyType_];
		hasCloseAttack      = EnemyParam::HasCloseAttack[enemyType_];
		hasLongAttack       = EnemyParam::HasLongAttack[enemyType_];
		visibilityDist      = EnemyParam::VisibilityDist[enemyType_];
		visibilityAngle     = EnemyParam::VisibilityAngle[enemyType_];
		rotationAnglePerSec = EnemyParam::RotationAnglePerSec[enemyType_];
		scale               = EnemyParam::Scale[enemyType_];
		dropItemID          = EnemyParam::DropItemId[enemyType_];
		transform.scale     = Vec3::One() * scale;
	}
	/// @brief 位置、種類指定生成
	/// @param position_ 位置
	/// @param enemyType_ 種類
	EnemyData(
	    GameObject objEnemyController_,
	    const Vec3 position_,
	    int enemyType_) :
	    transform{ position_ },
	    enemyType{ static_cast<EnemyType>(enemyType_) },
	    ATTACK_COOL_TIME{ EnemyParam::AttackCoolTime[enemyType_] }
	{
		objEnemyController = objEnemyController_;

		modelIndex = enemyType_;

		hp                  = EnemyParam::Hp[enemyType_];
		damage              = EnemyParam::Damage[enemyType_];
		speed               = EnemyParam::Speed[enemyType_];
		hasCloseAttack      = EnemyParam::HasCloseAttack[enemyType_];
		hasLongAttack       = EnemyParam::HasLongAttack[enemyType_];
		visibilityDist      = EnemyParam::VisibilityDist[enemyType_];
		visibilityAngle     = EnemyParam::VisibilityAngle[enemyType_];
		rotationAnglePerSec = EnemyParam::RotationAnglePerSec[enemyType_];
		scale               = EnemyParam::Scale[enemyType_];
		dropItemID          = EnemyParam::DropItemId[enemyType_];
		transform.scale     = Vec3::One() * scale;
	}
	/// @brief
	/// @param position_	生成位置
	/// @param enemyType_	敵の種類
	/// @param isBoss_	ボスかどうか
	EnemyData(
	    GameObject objEnemyController_,
	    const Vec3 position_,
	    int enemyType_,
	    bool isBoss_) :
	    transform{ position_ },
	    enemyType{ static_cast<EnemyType>(enemyType_) },
	    ATTACK_COOL_TIME{ EnemyParam::AttackCoolTime[enemyType_] }
	{
		objEnemyController = objEnemyController_;
		isBoss             = isBoss_;

		modelIndex = enemyType_;

		hp                  = EnemyParam::Hp[enemyType_];
		damage              = EnemyParam::Damage[enemyType_];
		speed               = EnemyParam::Speed[enemyType_];
		hasCloseAttack      = EnemyParam::HasCloseAttack[enemyType_];
		hasLongAttack       = EnemyParam::HasLongAttack[enemyType_];
		visibilityDist      = EnemyParam::VisibilityDist[enemyType_];
		visibilityAngle     = EnemyParam::VisibilityAngle[enemyType_];
		rotationAnglePerSec = EnemyParam::RotationAnglePerSec[enemyType_];
		scale               = EnemyParam::Scale[enemyType_];
		dropItemID          = EnemyParam::DropItemId[enemyType_];
		transform.scale     = Vec3::One() * scale;
	}

	/// @brief アニメーションの状態
	///	@param Start アニメーション開始
	/// @param Play アニメーション再生中
	/// @param End アニメーション終了
	enum AnimState
	{
		Start,
		Play,
		End,
	};

	/*Resource<Shader> shaderFront  ///< モデル描画用シェーダー(表面)
	{
	    "Shader/Unlit.hlsl",
	    RasterizerConfig{ .cullMode = D3D11_CULL_FRONT }
	};
	Resource<Shader> shaderBack  ///< モデル描画用シェーダー(裏面)
	{
	    "Shader/Unlit.hlsl",
	    RasterizerConfig{ .cullMode = D3D11_CULL_FRONT }
	};*/
	Resource<Shader> shader{ "Shader/Unlit.hlsl", RasterizerConfig{ .cullMode = D3D11_CULL_FRONT } };  ///< シェーダ
	// Resource<Model> model{
	//     "Model/kenny/kenney_mini-characters/Models/FBX format/aid-defibrillator-green.fbx"}; ///< モデル

	// Resource<Model> model{"Model/Enemy/Bat.fbx"};

	//----------------------//
	//---------/SE/---------//
	//----------------------//
	Resource<Audio> attackSE{ "BGM_SE/Striking the enemy.mp3" };  ///< 攻撃SE
	// Resource<Audio> damagedSE{ "Sound/Damaged.wav" };            ///< 被ダメージSE

	std::array<Resource<Model>, EnemyType_Max> models{
		Resource<Model>{ "Model/Enemy/Skeleton.fbx" }, Resource<Model>{ "Model/Enemy/Skeleton.fbx" },
		Resource<Model>{ "Model/Enemy/Spider.fbx" },   Resource<Model>{ "Model/Enemy/Bat.fbx" },
		Resource<Model>{ "Model/Enemy/Dragon.fbx" },
	};
	int32_t modelIndex{};

	Component<Transform> transform{ Vec3{},
		                            Quaternion::FromEuler(Vec3{ -DirectX::XM_PIDIV2, 0.0f, 0.0f }) };  ///< 座標系
	Component<RigidBody> rigidBody{};                                                                  ///< 剛体
	Component<StaticOBB> obb{};            ///< OBB当たり判定
	Component<OBBRenderer> obbRenderer{};  ///< OBB描画
	Component<Name> name{ "Enemy" };       ///< 名前

	float angleX{};
	float angleY{};

	int animIndex{};  ///< アニメーションのインデクス

	GameObject objOtherPlayerController{};  ///< 他プレイヤーのオブジェクト
	GameObject objEnemyController{};        ///< 敵一覧

	//-----------------------------------------//
	// パラメータ//----------------------------//
	//-----------------------------------------//
	int32_t id{};                               ///< ID
	int32_t hp{};                               ///< 体力
	int32_t damage{};                           ///< 攻撃力
	float speed{};                              ///< 移動速度
	bool hasCloseAttack{};                      ///< 近距離攻撃の有無
	bool hasLongAttack{};                       ///< 遠距離攻撃の有無
	float visibilityDist{};                     ///< 視認距離
	float visibilityAngle{};                    ///< 視認角度
	EnemyType enemyType{};                      ///< 敵の種類
	const float ATTACK_COOL_TIME{ 5.0f };       ///< 攻撃クールタイム
	float nowAttackCoolTime{ 0.0f };            ///< 現在の攻撃クールタイム
	const float ATTACK_COLL_LIFE_TIME{ 0.5f };  ///< 攻撃判定の寿命
	const float DAMAGED_COOL_TIME{ 0.5f };      ///< 被ダメージクールタイム
	float nowDamagedCoolTime{ 0.0f };           ///< 現在の被ダメージクールタイム
	float scale{ 1.0f };                        ///< モデルの大きさ

	bool isBoss{ false };  ///< ボスかどうか

	bool followed{ false };  ///< 追跡中か
	bool alive{ true };      ///< 生きているか
	bool attacked{ false };  ///< 攻撃中か

	const Vec3 DEFAULT_TARGET_POS{ 0.0f, -100.0f, 0.0f };

	const float WAN_RANGE{ 50 };  ///< 徘徊範囲
	const float WAN_REFERENCE{
		WAN_RANGE / 2
	};  ///< 徘徊基準点,これを現在地(座標)から引いてからランダム値を足すことで、徘徊範囲を決定する

	Vec3 targetPos{};                     ///< 追跡地点
	GameObject objWanderingCheckPoint{};  ///< 徘徊時のチェックポイント
	float rotationAnglePerSec{};          ///< 回転速度
	float torque{};                       ///< 回転力

	int32_t dropItemID{};  ///< ドロップアイテムのID
};

/// @brief 敵
struct Enemy : ObjectBase<EnemyData>
{
	using ObjectBase<EnemyData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;
	/// @brief 移動処理
	/// @param vecX_ X軸の移動量
	/// @param vecZ_ Z軸の移動量
	/// @param spd_ 速度
	/// @param angleY_	Y軸の回転角度
	void Move(Vec3 target_, Vec3 pos_, Vec3 Fwd, float spd_);
	/// @brief プレイヤーの視認
	void Sensor();

	/// @brief 描画処理
	void Draw() override;

	void OnDestroy() override;

	/// @brief 現在地の座標を返すよ
	/// @return
	Vec3 GetPosition() const
	{
		return transform.position;
	}
	float GetAngleY() const
	{
		return angleY;
	}
	int32_t GetID() const
	{
		return id;
	}
	EnemyStatus GetStatus() const
	{
		return { id, enemyType, transform.position, angleY };
	}

	/// @brief 攻撃処理
	/// @param targetPos_
	void Attack(Vec3 targetPos_);
	/// @brief 遠距離攻撃
	void LongAttack();
	/// @brief 近距離攻撃
	void CloseAttack();
	/// @brief 攻撃判定の生成
	void AttackCollider();
	/// @brief 攻撃判定の生成(遠距離)
	/// @param forward_ 自身から見たターゲットの方向
	void AttackCollider(const Vec3& forward_);
	/// @brief
	/// @param damage 受けるダメージ量
	void Damaged(int32_t damage);
	/// @brief 生死を設定
	/// @param alive_ 生死の状態
	void SetAlive(
	    bool alive_)
	{
		alive = alive_;
	}
	/// @brief アニメーション再生(仮)
	/// @param animIndex_ アニメーション番号
	/// @param state_ アニメーションの状態
	void PlayAnim(int32_t animIndex_, AnimState state_);

	/// @brief 徘徊時のチェックポイント設定
	void SetWanderingCheckPoint(Vec3 position_);

	void SetID(
	    int32_t id_)
	{
		id = id_;
	};

	void DropItem();

	void RegisterStates(StateRegistry& registry) const;
};