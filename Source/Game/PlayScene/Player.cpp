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
#include <Game/PlayScene/UI/HotBar.h>
#include <string>
#include "PlayerHand.h"
#include "Player/IPlayerJobController.h"
#include "Player/PlayerJobControllerHealer.h"
#include "Player/PlayerJobControllerTank.h"
#include "Player/PlayerJobControllerFighter.h"
#include "Player/PlayerJobControllerArcher.h"
#include "StateRegistry.h"
#include <Game/Manager/AudioManager.h>
#include <Game/Manager/SessionManager/SessionStateReader.h>

void Player::Start()
{
	obb.isTrigger = true;

	transform.scale = Vec3::One() * 200.0f;
	obb.offset      = OBB_OFFSET;
	obb.scale       = Vec3::One() * OBB_SCALE;
	obb.isTrigger   = true;

	// プレイヤーコントローラー作成
	objPlayerController = GetScene().Instantiate<PlayerControllerNormal>();
	objPlayerHand       = GetScene().Instantiate<PlayerHand>(this->ToGameObject());

	// ジャンプの初期化
	jumpVelocity = 0.0f;
	isJumping    = false;

	// 初期位置の地面の高さを取得
	transform.position.y = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);

	// バッグ作成
	objPlayerBag = GetScene().Instantiate<PlayerBag>();

	// ホットバー作成
	objHotbar = GetScene().Instantiate<HotBar>();

	// ホットバーが選択するアイテムが切り替わった際に呼ばれるコールバックをセット
	Get<HotBar>(objHotbar).SetOnChangedSelectedItemCallback(
	    [this]()
	    {
		    // 選択中のアイテムをプレイヤーの手にセット
		    HotBar& hotbar = Get<HotBar>(objHotbar);
		    Get<PlayerHand>(objPlayerHand).SetItem(hotbar.GetSelectedItemId());
		    VCU_LOG("change selected item");
	    });

	SetJob(PlayerJobType_Archer);
}

void Player::Update()
{
	SessionManager& sessionManager = Get<SessionManager>();
	std::string playerName         = sessionManager.RefStatus().name;

	ImGui::Begin("Player");
	ImGui::DragInt("anim", &animIndex, 1, 0);
	ImGui::DragInt("jobType", &jobType, 1, 0, 3);
	ImGui::DragFloat("animSpeed", &animSpeed, 1.0f, 0.0f);
	ImGui::DragFloat("rotationX", &transform.rotation.x, 1, 0);
	ImGui::DragFloat("rotationY", &transform.rotation.y, 1, 0);
	ImGui::Text("プレイヤー名：%s", playerName.c_str());
	ImGui::Separator();
	ImGui::DragFloat3("offset", transform.position.v, 0.0001f);

	Vec3 angles = transform.rotation.ToEuler();
	ImGui::DragFloat3("angles", angles.v, DirectX::XM_PI / 180.0f);
	transform.rotation = Quaternion::FromEuler(angles);

	if (ImGui::Button("出力"))
	{
		fs::path filePath{ "./animDumpForPlayer.txt" };
		std::ofstream ofs{ filePath, std::ios::app };

		if (ofs)
		{
			ofs << std::format(
			           "TransformAnimationKey{{ 0.0f, Vec3{{ {:f}f, {:f}f, {:f}f }}, Quaternion::FromEuler(Vec3{{ "
			           "{:f}f, {:f}f, "
			           "{:f}f }}), Vec3{{ "
			           "{:f}f, {:f}f, "
			           "{:f}f }} }}",
			           transform.position.x,
			           transform.position.y,
			           transform.position.z,
			           angles.x,
			           angles.y,
			           angles.z,
			           transform.scale.x,
			           transform.scale.y,
			           transform.scale.z)
			    << std::endl;
		}

		ofs.close();
	}

	if (ImGui::Button(isAnimPreviewingDebug ? "アニメーション設定終了" : "アニメーション設定する"))
	{
		isAnimPreviewingDebug = !isAnimPreviewingDebug;

		if (isAnimPreviewingDebug)
		{
			transformAnimator.playAnimation = "";
		}
		else
		{
			transformAnimator.playAnimation = "idle";
		}
	}

	ImGui::End();

	if (isAnimPreviewingDebug)

	{
		transformAnimator.playAnimation = "";
		return;
	}

	if (Get<InputSystem>().GetKeyDown(KeyCode::J))
	{
		SetJob((jobType + 1) % PlayerJobType_Max);
	}

	nowAttackCoolTime -= Get<DeltaTime>();
	nowDamagedCoolTime -= Get<DeltaTime>();

	// プレイヤーの更新処理
	Get<IPlayerController>(objPlayerController).Update(Data());

	// カメラの更新処理
	Get<IMainCamera>(objMainCamera).UpdatePositionFromTarget(this->ToGameObject());

	if (helthState == HelthState_Live)
	{
		MoveBody();
	}
	else if (helthState == HelthState_Dead)
	{
		DeadAnim();
	}

	// お試しで出してるエンティティ
	if (Get<AbstructInputSystem>().GetPlayerInput()->IsInteractBegin())
	{
		SpawnItem(9);
	}

	if (Get<InputSystem>().GetKeyDown(KeyCode::T))
	{
		SpawnItem(8);
	}

	// 近くのアイテムを拾う
	UpdatePickNearestItem();

	// クラフト画面を開く
	if (Get<InputSystem>().GetKeyDown(KeyCode::C) ||
	    Get<AbstructInputSystem>().GetPadInputSystem().IsButtonTriggered(GameInputGamepadY))
	{
		isOpenCraftUI = !isOpenCraftUI;
	}

	// 選択スロットのアイテムを投げ捨てる
	if (Get<AbstructInputSystem>().GetPlayerInput()->IsInteractBegin() && isOpenCraftUI == false)
	{
		HotBar& hotbar = Get<HotBar>(objHotbar);

		if (hotbar.CanRemoveSelectedItem())
		{
			ItemId selectedItemId = hotbar.GetSelectedItemId();
			hotbar.RemoveSelectedItem();
			SpawnItem(selectedItemId);
		}
	}

	/*
	//テスト用
	 // HPゲージ動作確認用：1秒ごとに10ダメージ
	 hpTestTimer += Get<DeltaTime>();

	 if (hpTestTimer >= 1.0f)
	 {
	     Damaged(10);
	     hpTestTimer = 0.0f;
	 }
	*/

	if (Get<InputSystem>().GetKeyDown(KeyCode::K))
	{
		Damaged(10);
	}

	Get<IPlayerJobController>(objJobController).Update(Data());
}

void Player::Draw()
{
	Get<Dx11Manager>().SetShader(shaderFront);
	Get<Dx11Manager>().SetMaterial(transform.GetMaterial());

	models[jobType].Get().Draw(Get<DeltaTime>() * animSpeed, animIndex);

	const auto& subParts{ models[jobType].Get().subParts };

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
		if (jobStatus.maxHpBoost > 0.0f)
		{
			jobStatus.maxHpBoost -= damage;

			if (jobStatus.maxHpBoost >= 0.0f)
			{
				return;
			}
			else
			{
				hp += jobStatus.maxHpBoost;
			}
		}
		else
		{
			hp -= damage;
		}

		if (hp < 0)
		{
			hp = 0;
		}

		nowDamagedCoolTime = DAMAGED_COOL_TIME;
	}

	if (hp <= 0)
	{
		SetHelthState(HelthState_Dead);
	}
}

void Player::SetHelthState(
    const HelthState _state)
{
	if (helthState == _state)
	{
		return;
	}

	switch (_state)
	{
	case HelthState_Live:
		Get<IMainCamera>(objMainCamera).SetCameraType(MainCameraType_TPS);
		break;

	case HelthState_Dead:
		Get<IMainCamera>(objMainCamera).SetCameraType(MainCameraType_Ghost);
		break;

	default:
		VCU_ASSERT("不正な生存ステータスですよ。");
		return;
	}

	helthState = _state;
}

void Player::SetJob(
    const PlayerJobType _jobType)
{
	// 今のjobコンを消す
	GetScene().Destroy(objJobController);

	jobType = _jobType;

	switch (jobType)
	{
	case PlayerJobType_Healer:
		objJobController = GetScene().Instantiate<PlayerJobControllerHealer>();
		break;

	case PlayerJobType_Tank:
		objJobController = GetScene().Instantiate<PlayerJobControllerTank>();
		break;

	case PlayerJobType_Fighter:
		objJobController = GetScene().Instantiate<PlayerJobControllerFighter>();
		break;

	case PlayerJobType_Archer:
		objJobController = GetScene().Instantiate<PlayerJobControllerArcher>();
		break;

	default:
		VCU_ASSERT(false && "選択不可能な役職です。");
		return;
	}

	Get<IPlayerJobController>(objJobController).Start(Data());
}

void Player::MoveBody()
{
	transform.rotation = Quaternion::FromEuler(Vec3{
	    0.0f,
	    Get<IMainCamera>(objMainCamera).GetAngleY(),
	    0.0f,
	});

	// 入力取得
	AbstructInputSystem& inputSystem = Get<AbstructInputSystem>();
	auto pPlayerInput                = inputSystem.GetPlayerInput();

	Vec2 bodyMove = pPlayerInput->GetMoveBody();

	float moveSpeed = jobStatus.moveSpeed;

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
	nextPosition += moveDirection * moveSpeed + (jobStatus.moveVelocity * Get<DeltaTime>());

	animSpeed = (moveDirection * moveSpeed + (jobStatus.moveVelocity * Get<DeltaTime>())).Length() / 0.1f;

	if (animSpeed > 0.01f)
	{
		animIndex = PlayerAnimType_Walk;

		// 反対方向への入力があったらアニメーションを反転する
		if (bodyMove.y < 0)
		{
			animSpeed *= -1.0f;
		}
	}
	else
	{
		animIndex = PlayerAnimType_Idle;
		animSpeed = 1.0f;
	}

	// --------------------------------
	// 足音SE
	// --------------------------------
	if (animIndex == PlayerAnimType_Walk && !isJumping)
	{
		footstepTimer += Get<DeltaTime>();

		if (footstepTimer >= FOOTSTEP_INTERVAL)
		{
			Get<AudioManager>().PlayFootstep(footstepSE);
			footstepTimer = 0.0f;
		}
	}
	else
	{
		// 足音を停止
		Get<AudioManager>().StopFootstep();

		// タイマーをリセット
		footstepTimer = 0.0f;
	}

	// TerrainSystemを取得
	auto& terrain = Get<TerrainSystem>();

	// 許容する傾斜の上限
	// 数値を大きくすると急な坂も登れる
	const float MAX_SLOPE = 0.7f;

	// 移動前の位置
	Vec3 currentPosition = transform.position;

	// ----------------------------------------
	// X方向の移動判定
	// ----------------------------------------
	Vec3 candidateX = transform.position;
	candidateX.x    = nextPosition.x;

	float currentHeightX    = terrain.GetHeight(currentPosition.x, currentPosition.z);
	float nextHeightX       = terrain.GetHeight(candidateX.x, candidateX.z);
	float moveDistanceX     = fabsf(candidateX.x - currentPosition.x);
	float heightDifferenceX = nextHeightX - currentHeightX;

	// 高さの変化が移動距離に対して大きすぎる場合は移動しない
	if (heightDifferenceX <= 0.0f || (moveDistanceX > 0.0001f && heightDifferenceX / moveDistanceX <= MAX_SLOPE))
	{
		transform.position.x = candidateX.x;
	}

	// ----------------------------------------
	// Z方向の移動判定
	// ----------------------------------------
	Vec3 candidateZ = transform.position;
	candidateZ.z    = nextPosition.z;

	float currentHeightZ    = terrain.GetHeight(transform.position.x, transform.position.z);
	float nextHeightZ       = terrain.GetHeight(candidateZ.x, candidateZ.z);
	float moveDistanceZ     = fabsf(candidateZ.z - transform.position.z);
	float heightDifferenceZ = nextHeightZ - currentHeightZ;

	if (heightDifferenceZ <= 0.0f || (moveDistanceZ > 0.0001f && heightDifferenceZ / moveDistanceZ <= MAX_SLOPE))
	{
		transform.position.z = candidateZ.z;
	}

	// 現在位置の地面の高さを取得
	float groundHeight = Get<TerrainSystem>().GetHeight(transform.position.x, transform.position.z);

	// ジャンプしていないときは地面に合わせる
	if (!isJumping)
	{
		if (transform.position.y - groundHeight > 0.1f)
		{
			isJumping = true;
		}
		else if (transform.position.y - groundHeight > 0.0f)
		{
			transform.position.y = groundHeight;
		}
	}

	// ジャンプしていないときは地面の高さに合わせる
	if (!isJumping)
	{
		transform.position.y = groundHeight;
	}

	// --------------------------------
	// ジャンプ処理
	// --------------------------------
	float deltaTime = Get<DeltaTime>();

	// ジャンプボタンを押した瞬間
	if (pPlayerInput->IsJumpBegin() && !isJumping)
	{
		// ジャンプ音を再生
		Get<AudioManager>().PlayJump(jumpSE);

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
		if (transform.position.y <= groundHeight)
		{
			transform.position.y = groundHeight;
			jumpVelocity         = 0.0f;
			isJumping            = false;
		}

		if (jumpVelocity < 0.0f)
		{
			animIndex = PlayerAnimType_FallBig;
			animSpeed = 1.0f;
		}
		else
		{
			animIndex = PlayerAnimType_Fall;
			animSpeed = 1.0f;
		}
	}
	else
	{
	}

	// MEMO: PlayerHand側に処理を移動しました。
	// 手の更新をする
	Get<PlayerHand>(objPlayerHand).Update(Data());
}

void Player::DeadAnim()
{
	if (liveStatus.dieAnimTimeSec > 0.0f)
	{
		liveStatus.dieAnimTimeSec -= Get<DeltaTime>();

		animIndex = PlayerAnimType_Die;
		animSpeed = 1.0f;

		if (liveStatus.dieAnimTimeSec <= 0.0f)
		{
			liveStatus.dieAnimTimeSec = 0.0f;

			animIndex = PlayerAnimType_Die;
			animSpeed = 0.0f;

			Get<PlayerHand>(objPlayerHand).SetItem(0);
		}
	}
	else
	{
		animIndex = PlayerAnimType_Die;
		animSpeed = 0.0f;
	}
}

void Player::RegisterStates(
    StateRegistry& registry) const
{
	registry.Register({
	    { "hp", Data().hp },
	    { "helthState", Data().helthState },
	    { "animIndex", Data().animIndex },
	});
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
	if (objPick)
	{
		ItemId pickedItemId = Get<IEntity>(objPick).GetItemId();
		TryAddItem(pickedItemId);
	}
}

void Player::TryAddItem(
    ItemId _id)
{
	/// ----------------------------------------------
	/// 今のjobで、引数のアイテムを追加できるか確認
	/// ----------------------------------------------

	// ジョブによって追加可能か切り替えるかどうか。falseにすれば、どのジョブも弓拾ったりできる。デバッグ用。
	bool filterAddItemOnJob{ false };

	ItemInfo info     = Get<ItemSystem>().GetInfo(_id);
	bool playerCanUse = (info.canUseJobType == PlayerJobType_Max || info.canUseJobType == jobType);

	if (playerCanUse == false && filterAddItemOnJob)
	{
		SpawnItem(_id);
		return;
	}

	/// ---------------------------
	/// ホットバーに追加できるか確認
	/// ---------------------------
	ItemStack itemStack{};
	HotBar& hotbar = Get<HotBar>(objHotbar);

	if (hotbar.TrySetItem(_id))
	{
		return;
	}
	else if (hotbar.SwapItem(itemStack, _id))
	{
		if (itemStack.id != EMPTY_ITEM_ID)
		{
			for (int i = 0; i < itemStack.count; i++)
			{
				SpawnItem(itemStack.id);
			}
		}

		return;
	}

	/// ---------------------------
	/// バッグに追加できるか確認
	/// ---------------------------
	PlayerBag& bag = Get<PlayerBag>(objPlayerBag);

	if (bag.CanAddItem(_id))
	{
		bag.AddItem(_id);
	}
	// 追加できないなら、エンティティとして前方に放り出す
	else
	{
		SpawnItem(_id);
	}
}

void Player::SpawnItem(
    ItemId _id)
{
	Get<EntitySystem>().Spawn(
	    _id,
	    transform.position + Vec3::Up() * 2.0f,
	    transform.Forward() * 3.0f + Vec3::Up() * 2.0f);

	VCU_LOG("objPawn");
}
