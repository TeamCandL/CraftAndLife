#include "EffectSystem.h"
#include "pch\pch.h"

#ifdef _DEBUG
#pragma comment(lib, "Effekseerd.lib")
#pragma comment(lib, "EffekseerRendererDX11d.lib")
#else
#pragma comment(lib, "Effekseer.lib")
#pragma comment(lib, "EffekseerRendererDX11.lib")
#endif
using namespace Effekseer;
using namespace EffekseerRendererDX11;
constexpr int MAX_SQUARE{8192};
constexpr size_t PARTICLE_POOL_CAPACITY{256};

Effekseer::Matrix43 CnvMat43(DirectX::XMFLOAT4X4 _mat)
{
	Effekseer::Matrix43 mat43{};
	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < 3; ++j)
		{
			mat43.Value[i][j] = _mat.m[i][j];
		}
	}
	return mat43;
}

Effekseer::Matrix43 CnvMat43(const Mat4x4& _mat)
{
	Effekseer::Matrix43 out;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			out.Value[i][j] = DirectX::XMVectorGetByIndex(_mat.r[i], j);
		}
	}
	return out;
}

Effekseer::Matrix44 CnvMat44(const Mat4x4& _mat)
{
	Effekseer::Matrix44 out;
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			out.Values[i][j] = DirectX::XMVectorGetByIndex(_mat.r[i], j);
		}
	}
	return out;
}

Mat4x4 CalculateWorldMat(const EffectInstance& _instance)
{
	using namespace DirectX;

	Mat4x4 out = XMMatrixIdentity() * XMMatrixScalingFromVector(_instance.scale) *
	             XMMatrixRotationQuaternion(_instance.rotation) * XMMatrixTranslationFromVector(_instance.position);
	return out;
}

Effekseer::Matrix43 CalculateMat43(std::shared_ptr<EffectInstance> _pInstance)
{
	return CnvMat43(CalculateWorldMat(*_pInstance.get()));
}

Effekseer::Matrix44 CnvMat(DirectX::XMFLOAT4X4 _mat)
{
	Effekseer::Matrix44 out;

	out.Values[0][0] = _mat._11;
	out.Values[1][0] = _mat._12;
	out.Values[2][0] = _mat._13;
	out.Values[3][0] = _mat._14;
	out.Values[0][1] = _mat._21;
	out.Values[1][1] = _mat._22;
	out.Values[2][1] = _mat._23;
	out.Values[3][1] = _mat._24;
	out.Values[0][2] = _mat._31;
	out.Values[1][2] = _mat._32;
	out.Values[2][2] = _mat._33;
	out.Values[3][2] = _mat._34;
	out.Values[0][3] = _mat._41;
	out.Values[1][3] = _mat._42;
	out.Values[2][3] = _mat._43;
	out.Values[3][3] = _mat._44;
	return out;
}

EffectRegistry::EffectRegistry() :
    ResourceRegistryBase()
{
}

bool EffectRegistry::TryLoad(fs::path&& _file, EffectResource& _out)
{
	if (_file.extension() != ".efkefc")
	{
		VCU_ASSERT(false && "不正な拡張子です。.efkefcを使ってください");
		return false;
	}
	_out.filePath = _file;
	return true;
}

void EffectSystem::Start()
{

	Get<Dx11Manager>().Ref(
	    [this](ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	    {
		    // 描画に使用するレンダラー
		    rendererRef_ = Renderer::Create(pDevice.Get(), pContext.Get(), MAX_SQUARE);
		    // エフェクトを管理するマネージャー
		    managerRef_ = Effekseer::Manager::Create(MAX_SQUARE);
		    managerRef_->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

		    // マネージャーに各種レンダラー、ローダーをセット
		    managerRef_->SetSpriteRenderer(rendererRef_->CreateSpriteRenderer());
		    managerRef_->SetRibbonRenderer(rendererRef_->CreateRibbonRenderer());
		    managerRef_->SetRingRenderer(rendererRef_->CreateRingRenderer());
		    managerRef_->SetTrackRenderer(rendererRef_->CreateTrackRenderer());
		    managerRef_->SetModelRenderer(rendererRef_->CreateModelRenderer());

		    managerRef_->SetTextureLoader(rendererRef_->CreateTextureLoader());
		    managerRef_->SetModelLoader(rendererRef_->CreateModelLoader());
		    managerRef_->SetMaterialLoader(rendererRef_->CreateMaterialLoader());
		    managerRef_->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());
	    });

	// インスタンスプールを確保
	effectInstanceList_.reserve(PARTICLE_POOL_CAPACITY);
}

void EffectSystem::Update()
{
	const double deltaTime = Get<DeltaTime>();
	const double fps = 60.0;
	for (auto& instance : effectInstanceList_)
	{
		// 未使用のデータは更新しない
		if (instance == nullptr)
		{
			continue;
		}

		Effekseer::Handle& handle = instance->handle;
		// 破棄フラグが立っているか
		if (instance->destroyMe)
		{
			// エフェクトの再生を停止
			if (handle >= 0)
			{
				managerRef_->StopEffect(handle);
			}
			// インスタンスを解放
			instance.reset();
			continue;
		}

		// 再生時間が0の場合、再生を始める
		if (instance->elapsedTime == 0)
		{
			handle = managerRef_->Play(instance->ref, 0, 0, 0);
		}

		// エフェクトの最大フレームを取得
		int32_t maxFrame = instance->ref->CalculateTerm().TermMax;

		// 再生終了
		double duration = maxFrame / fps;
		if (instance->elapsedTime > duration)
		{
			if (handle >= 0)
			{
				// エフェクトの再生を停止
				managerRef_->StopEffect(handle);
			}

			// ループ再生するかどうか
			if (instance->isLoop)
			{
				// 再生時間を0にして、再び再生できるように設定
				instance->elapsedTime = 0;
			}
			else
			{
				// ループ再生しない場合、未使用状態に戻す
				instance.reset();
			}
		}
		// エフェクトの更新を行う
		else
		{
			// フレーム数を設定
			rendererRef_->SetTime(static_cast<float>(instance->elapsedTime));
			Effekseer::Matrix43 mat = CalculateMat43(instance);
			// エフェクトのインスタンスに対して処理をし、更新速度を設定

			managerRef_->SetMatrix(handle, mat);
			managerRef_->SetSpeed(handle, instance->playSpeed);
			// 再生時間を増やす
			instance->elapsedTime += deltaTime;
		}
	}

	managerRef_->Update(static_cast<float>(deltaTime * fps));
}

void EffectSystem::Draw()
{
	Get<Dx11Manager>().EnqueueTransparent(
	    [this]()
	    {
		    CameraSystemData& data = Get<CameraSystem>().Data();
		    using namespace DirectX;
		    XMFLOAT4X4 fProjMat;
		    XMStoreFloat4x4(&fProjMat, XMMatrixTranspose(data.projectionMatrix));
		    XMFLOAT4X4 fViewMat;
		    XMStoreFloat4x4(&fViewMat, XMMatrixTranspose(data.viewMatrix));

		    rendererRef_->SetProjectionMatrix(CnvMat(fProjMat));
		    rendererRef_->SetCameraMatrix(CnvMat(fViewMat));
		    // Effekseerの描画パラメーターを作成
		    // Effekseerによる描画
		    rendererRef_->BeginRendering();
		    Effekseer::Manager::DrawParameter drawParameter;
		    drawParameter.ZNear = 0.0f;
		    drawParameter.ZFar = data.farClipDistance;
		    drawParameter.ViewProjectionMatrix = rendererRef_->GetCameraProjectionMatrix();
		    managerRef_->Draw(drawParameter);
		    rendererRef_->EndRendering();
	    });
}

std::shared_ptr<EffectInstance> EffectSystem::Play(cha::Resource<ResourceType::Effect>& _resource)
{
	// インスタンス作成
	std::shared_ptr<EffectInstance> instance = CreateInstance();
	if (instance == nullptr)
	{
		return nullptr;
	}

	EffectResource& particle = _resource.Get();
	// 該当するエフェクトが作成されていないなら、新規作成
	if (particle.ref == nullptr)
	{
		if (fs::exists(particle.filePath))
		{
			fs::path filePath(particle.filePath);
			auto str = filePath.u16string();
			auto u16Char = str.c_str();
			particle.ref = Effekseer::Effect::Create(managerRef_, u16Char);
		}
	}

	// インスタンスに、エフェクトの元データへの参照を渡す
	instance->ref = particle.ref;
	return instance;
}

std::shared_ptr<EffectInstance> EffectSystem::CreateInstance()
{
	// 空きスロットを探す
	for (auto& slot : effectInstanceList_)
	{
		// 空きスロット発見、再利用!
		if (slot == nullptr)
		{
			slot = std::make_shared<EffectInstance>();
			return slot;
		}
	}
	// 空きスロットがなかった

	// 容量に余裕があるなら、新規追加
	if (effectInstanceList_.size() < PARTICLE_POOL_CAPACITY)
	{
		return effectInstanceList_.emplace_back(std::make_shared<EffectInstance>());
	}

	// 空きスロットなし!容量に余裕もなし!作れません!
	return nullptr;
}
