#include "Player.h"
#include "PlayerAttackColl.h"
#include "IMainCamera.h"
#include "Player/PlayerControllerNormal.h"
#include "pch\pch.h"
#include <Game/System/AbstructInputSystem.h>
#include "Entity.h"
#include <Game/PlayScene/TerrainSystem.h>
#include <Game/PlayScene/EntitySystem.h>
#include <Game/Manager/SessionManager.h>
#include <Game/PlayScene/PlayerBag.h>
#include <Game/PlayScene/CraftUI.h>
#include <Game/PlayScene/UI/HotBar.h>
#include <string>

void Player::Start()
{
	obb.isTrigger = true;

	transform.scale = Vec3::One() * 200.0f;
	obb.offset      = OBB_OFFSET;
	obb.scale       = Vec3::One() * OBB_SCALE;
	obb.isTrigger   = true;  ///< 当たり判定を貫通させる(押し出さなくなる)

	objPlayerController = GetScene().Instantiate<PlayerControllerNormal>();

	// ジャンプの初期化
	jumpVelocity = 0.0f;
	isJumping    = false;

	// 初期位置の地面の高さを取得
	transform.position.y = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);

	// バッグ作成
	objPlayerBag = GetScene().Instantiate<PlayerBag>();

	objCraftUI = GetScene().Instantiate<CraftUI>();
	objHotbar  = GetScene().Instantiate<HotBar>();
}

void Player::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();
	std::string playerName         = sessionManager.RefStatus().name;

	ImGui::Begin("Player");
	ImGui::DragInt("anim", &animIndex, 1, 0);
	ImGui::DragFloat("rotationX", &transform.rotation.x, 1, 0);
	ImGui::DragFloat("rotationY", &transform.rotation.y, 1, 0);
	ImGui::Text("プレイヤー名：%s", playerName.c_str());
	ImGui::End();

	nowAttackCoolTime -= Get<DeltaTime>();
	nowDamagedCoolTime -= Get<DeltaTime>();

	// プレイヤーの更新処理
	Get<IPlayerController>(objPlayerController).Update(Data());

	// カメラの更新処理
	Get<IMainCamera>(objMainCamera).UpdatePositionFromTarget(this->ToGameObject());

	transform.rotation = Quaternion::FromEuler(Vec3{
	    0.0f,
	    Get<IMainCamera>(objMainCamera).GetAngleY(),
	    0.0f,
	});

	// 入力取得
	AbstructInputSystem& inputSystem = Get<AbstructInputSystem>();

	auto pPlayerInput = inputSystem.GetPlayerInput();

	Vec2 bodyMove = pPlayerInput->GetMoveBody();

	float moveSpeed = 0.1f;

	// カメラの向きを取得
	float angleY = Get<IMainCamera>(objMainCamera).GetAngleY();

	// カメラの向きから前方向を計算
	Vec3 forward = Vec3{ sinf(angleY), 0.0f, cosf(angleY) };

	// カメラの向きから右方向を計算
	Vec3 right = Vec3{ cosf(angleY), 0.0f, -sinf(angleY) };

	// 入力に応じて移動方向を計算
	Vec3 moveDirection = right * bodyMove.x + forward * bodyMove.y;

	// 地形に沿った移動
	Vec3 nextPosition = transform.position;
	nextPosition += moveDirection * moveSpeed;

	// 移動前と移動先の地面の高さを取得
	float currentHeight = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);

	float nextHeight = Get<TerrainSystem>().GetHeight(nextPosition.x, nextPosition.z);

	// 登れる段差の高さ
	const float MAX_STEP_HEIGHT = 0.1f;

	// 高低差が大きすぎなければ移動する
	if (nextHeight - currentHeight <= MAX_STEP_HEIGHT)
	{
		transform.position.x = nextPosition.x;
		transform.position.z = nextPosition.z;
	}

	// ジャンプしていないときは地面に合わせる
	if (!isJumping)
	{
		float groundHeight = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);

		transform.position.y = groundHeight;
	}

	// 現在位置の地面の高さを取得
	float groundHeight = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);

	// ジャンプしていないときは地面の高さに合わせる
	if (!isJumping)
	{
		transform.position.y = groundHeight;
	}

	// ジャンプ処理
	float deltaTime = Get<DeltaTime>();

	// ジャンプボタンを押した瞬間
	if (pPlayerInput->IsJumpBegin() && !isJumping)
	{
		jumpVelocity = JUMP_POWER;
		isJumping    = true;
	}

	// ボタンを離した瞬間
	// 上昇中なら上向きの速度を弱める
	if (pPlayerInput->IsJumpEnd() && jumpVelocity > 0.0f)
	{
		jumpVelocity *= JUMP_CUT_RATE;
	}

	// 上昇・下降処理
	if (isJumping)
	{
		// 重力を適用
		jumpVelocity -= GRAVITY * deltaTime;

		// Y座標を更新
		transform.position.y += jumpVelocity * deltaTime;

		// 地面に着地したら停止
		float groundHeight = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);

		if (transform.position.y <= groundHeight)
		{
			transform.position.y = groundHeight;
			jumpVelocity         = 0.0f;
			isJumping            = false;
		}
	}

	// お試しで出してるエンティティ
	if (pPlayerInput->IsInteractBegin())
	{
		Get<EntitySystem>().Spawn(
		    8,
		    transform.position + Vec3::Up() * 2.0f,
		    transform.Forward() * 3.0f + Vec3::Up() * 2.0f);
	}
	if (Get<InputSystem>().GetKeyDown(KeyCode::T))
	{
		Get<EntitySystem>().Spawn(
		    7,
		    transform.position + Vec3::Up() * 2.0f,
		    transform.Forward() * 3.0f + Vec3::Up() * 2.0f);
		VCU_LOG("objPawn");
	}
	UpdatePickNearestItem();

	PlayerBag& bag   = Get<PlayerBag>(objPlayerBag);
	HotBar& hotbar   = Get<HotBar>(objHotbar);
	CraftUI& craftUI = Get<CraftUI>(objCraftUI);
	bool isAction    = pPlayerInput->IsActionBegin();

	static bool open = false;
	if (isAction)
	{
		open = !open;
	}
	if (open)
	{
		craftUI.SetActiveAll(true);
		craftUI.SetActive(true);
		if (ItemId id = craftUI.Update(bag); id != EMPTY_ITEM_ID)
		{
			// クラフトを試みて、成功した場合
			if (bag.TryCraft(id))
			{
				if (hotbar.TrySetItem(id))
					return;

				// バッグに追加できる場合、追加
				if (bag.CanAddItem(id))
				{
					bag.AddItem(id);
				}
				// 追加できないなら、エンティティとして前方に放り出す
				else
				{
					Get<EntitySystem>().Spawn(
					    id,
					    transform.position + Vec3::Up() * 2.0f,
					    transform.Forward() * 3.0f + Vec3::Up() * 2.0f);
					VCU_LOG("objPawn");
				}
			}
		}
	}
	else
	{
		craftUI.SetActiveAll(false);
		craftUI.SetActive(false);
	}

	// HPゲージ動作確認用：1秒ごとに10ダメージ
	hpTestTimer += Get<DeltaTime>();

	if (hpTestTimer >= 1.0f)
	{
		Damaged(10);
		hpTestTimer = 0.0f;
	}
}

void Player::Draw()
{
	Get<Dx11Manager>().SetShader(shaderFront);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	model.Get().Draw(Get<DeltaTime>(), animIndex);

	const auto& subParts{ model.Get().subParts };

	for (const auto& parts : subParts)
	{
		Get<Dx11Manager>().SetSurface(parts.mesh);
		Get<Dx11Manager>().SetMaterial(parts.material);
		Get<Dx11Manager>().SetMaterial(parts.transformMaterial);
		Get<Dx11Manager>().Draw();
	}
}

void Player::AttackCollider()
{
	if (nowAttackCoolTime <= 0.0f)
	{
		GetScene().Instantiate<PlayerAttackColl>(transform.position, damage, ATTACK_COLL_LIFE_TIME);

		nowAttackCoolTime = ATTACK_COOL_TIME;
	}
}

void Player::Damaged(
    int32_t damage)
{
	if (nowDamagedCoolTime <= 0.0f && hp > 0)
	{
		hp -= damage;

		if (hp < 0)
		{
			hp = 0;
		}

		nowDamagedCoolTime = DAMAGED_COOL_TIME;
	}
}

Vec3 Player::GetPosition() const
{
	return transform.position;
}

float Player::GetAngleY() const
{
	return Vec3{ transform.rotation.ToEuler() }.y;
}

void Player::UpdatePickNearestItem()
{
	GameObject objPick = Get<EntitySystem>().PickNearest(transform.position, 3.0f);
	// エンティティを拾うことができた場合
	PlayerBag& bag = Get<PlayerBag>(objPlayerBag);
	if (objPick)
	{
		ItemId pickedItemId = Get<IEntity>(objPick).GetItemId();
		// バッグに追加できるなら、追加
		if (bag.CanAddItem(pickedItemId))
		{
			bag.AddItem(pickedItemId);
		}

		VCU_LOG("objPick");
	}
}
