#pragma once
#include <Game/Game.h>

/// @brief 敵のパラメータ初期値。Skeleton, BossSkeleton, Spider, Bat, Dragonの順番でパラメータを格納
namespace AnimalParam
{
	const std::vector<int32_t> Hp{ 50, 200, 30, 20, 400 };                              ///< 体力
	const std::vector<float> Speed{ 4.5f, 3.5f, 10.5f, 7.5f, 6.0f };                    ///< 移動速度
	const std::vector<float> WanderingRange{ 50.0f, 50.0f, 50.0f, 50.0f, 0.0f };        ///< 徘徊範囲
	const std::vector<float> RotationAnglePerSec{ 22.5f, 18.0f, 45.0f, 30.5f, 25.5f };  ///< 回転速度
	const std::vector<float> Scale{ 0.7f, 1.1f, 0.4f, 0.3f, 1.2f };                     ///< モデルの大きさ
	const std::vector<int32_t> DropItemId{ 9, 25, 10, 11, 12 };                         ///< ドロップアイテムID
	const std::vector<int32_t> DropMinCount{ 1, 1, 1, 1, 2 };                           ///< ドロップアイテム最小個数
	const std::vector<int32_t> DropMaxCount{ 1, 1, 3, 2, 3 };                           ///< ドロップアイテム最大個数
}  // namespace AnimalParam

struct StateRegistry;

struct AnimalData
{
	/// @brief 指定なし生成
	AnimalData(GameObject objEnemyController_) {};

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

	Resource<Shader> shader{ "Shader/Unlit.hlsl", RasterizerConfig{ .cullMode = D3D11_CULL_FRONT } };  ///< シェーダ
	// Resource<Model> model{
	//     "Model/kenny/kenney_mini-characters/Models/FBX format/aid-defibrillator-green.fbx"}; ///< モデル

	// Resource<Model> model{"Model/Enemy/Bat.fbx"};

	Resource<Audio> flameSE{ "BGM_SE/Flame.mp3" };

	Resource<Model> model{};
	Resource<Shader> shadowShader{ "Shader/ShadowMap.hlsl" };
	int32_t modelIndex{};

	Component<Transform> transform{ Vec3{},
		                            Quaternion::FromEuler(Vec3{ -DirectX::XM_PIDIV2, 0.0f, 0.0f }) };  ///< 座標系
	Component<RigidBody> rigidBody{};                                                                  ///< 剛体
	Component<StaticOBB> obb{};            ///< OBB当たり判定
	Component<OBBRenderer> obbRenderer{};  ///< OBB描画
	Component<Name> name{ "Animal" };      ///< 名前

	float angleX{};
	float angleY{};

	int animIndex{};  ///< アニメーションのインデクス

	GameObject objOtherPlayerController{};  ///< 他プレイヤーのオブジェクト
	GameObject objEnemyController{};        ///< 敵一覧

	//-----------------------------------------//
	// パラメータ//----------------------------//
	//-----------------------------------------//
	int32_t id{};                            ///< ID
	int32_t hp{};                            ///< 体力
	int32_t damage{};                        ///< 攻撃力
	float speed{};                           ///< 移動速度
	const float DAMAGED_COOL_TIME{ 0.35f };  ///< 被ダメージクールタイム
	float nowDamagedCoolTime{ 0.0f };        ///< 現在の被ダメージクールタイム
	float scale{ 1.0f };                     ///< モデルの大きさ

	bool alive{ true };  ///< 生きているか

	const float WAN_RANGE{};  ///< 徘徊範囲
	const float
	    WAN_REFERENCE{};  ///< 徘徊基準点,これを現在地(座標)から引いてからランダム値を足すことで、徘徊範囲を決定する

	GameObject objWanderingCheckPoint{};  ///< 徘徊時のチェックポイント
	float rotationAnglePerSec{};          ///< 回転速度
	float torque{};                       ///< 回転力

	int32_t dropItemID{};    ///< ドロップアイテムのID
	int32_t dropMinCount{};  ///< ドロップアイテムの最小個数
	int32_t dropMaxCount{};  ///< ドロップアイテムの最大個数

	// 死亡演出関連
	bool isDead{ false };                      // 死亡演出中かどうか
	float deathElapsedSec{ 0.0f };             // 死亡演出の経過時間
	float fallDurationSec{ 0.6f };             // 横たわるまでの時間
	float shrinkDurationSec{ 0.8f };           // 縮小して消滅するまでの時間
	Vec3 deathStartScale{ 1.0f, 1.0f, 1.0f };  // 死亡演出開始時にスケール
	Quaternion deathStartRotation{};           // 死亡演出開始時の回転
};

/// @brief 敵
struct Animal : ObjectBase<AnimalData>
{
	using ObjectBase<AnimalData>::ObjectBase;

	/// @brief 開始処理
	void Start() override;

	/// @brief 更新処理
	void Update() override;
	/// @brief 移動処理
	/// @param vecX_ X軸の移動量
	/// @param vecZ_ Z軸の移動量
	/// @param spd_ 速度
	/// @param angleY_	Y軸の回転角度
	void Move(Vec3 target_, Vec3 pos_, Vec3 Fwd_, float spd_);

	/// @brief 描画処理
	void Draw() override;

	void DrawShadowMap() override;

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
	/// @brief
	/// @param damage 受けるダメージ量
	/// @brief ダメージを受ける（通常攻撃・弓など）
	/// @param damage 受けるダメージ量
	void Damaged(int32_t damage);
	/// @brief 剣・大剣によるダメージを受ける
	/// @param damage 受けるダメージ量
	/// @param isGreatsword 大剣による攻撃かどうか
	void Damaged(int32_t damage, bool isGreatsword);

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

	/// @brief 現在体力取得（ホスト用）
	/// @return
	int32_t GetHp() const
	{
		return hp;
	}

	void DropItem();

	void RegisterStates(StateRegistry& registry) const;

	void StartDeathAnimation();

	void UpdateDeathAnimation();
};