#include "EffectSystem.h"
#include <Game/Game.h>
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
constexpr int MAX_SQUARE{ 8192 };
constexpr size_t PARTICLE_POOL_CAPACITY{ 256 };

Effekseer::Matrix43 CnvMat43(
    DirectX::XMFLOAT4X4 _mat)
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

Effekseer::Matrix43 CnvMat43(
    const Mat4x4& _mat)
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

Effekseer::Matrix44 CnvMat44(
    const Mat4x4& _mat)
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

Effekseer::Matrix44 CnvMat(
    DirectX::XMFLOAT4X4 _mat)
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

bool EffectRegistry::TryLoad(
    fs::path&& _file,
    EffectResource& _out)
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
}

void EffectSystem::Update()
{
	const double deltaTime = Get<DeltaTime>();
	const double fps       = 60.0;
	auto view              = GetView<Transform, EffectPlayer>();
	view.ForEach(
	    [this, deltaTime, fps](IObject& _object, Transform& _transform, EffectPlayer& _effect)
	    {
		    // 未使用のデータは更新しない
		    if (_effect.ref == nullptr || _effect.handle == -1)
		    {
			    return;
		    }

		    Effekseer::Handle& handle = _effect.handle;
		    // 破棄フラグが立っているか
		    if (_effect.destroyMe)
		    {
			    // エフェクトの再生を停止
			    if (handle >= 0)
			    {
				    managerRef_->StopEffect(handle);
			    }
			    // インスタンスを解放
			    return;
		    }

		    // 再生時間が0の場合、再生を始める
		    if (_effect.elapsedTime == 0)
		    {
			    handle = managerRef_->Play(_effect.ref, 0, 0, 0);
		    }

		    // エフェクトの最大フレームを取得
		    int32_t maxFrame = _effect.ref->CalculateTerm().TermMax;

		    // 再生終了
		    double duration = maxFrame / fps;
		    if (_effect.elapsedTime > duration)
		    {
			    if (handle >= 0)
			    {
				    // エフェクトの再生を停止
				    managerRef_->StopEffect(handle);
			    }

			    // ループ再生するかどうか
			    if (_effect.isLoop)
			    {
				    // 再生時間を0にして、再び再生できるように設定
				    _effect.elapsedTime = 0;
			    }
			    else
			    {
				    // ループ再生しない場合、未使用状態に戻す
				    _effect.Reset();
			    }
		    }
		    // エフェクトの更新を行う
		    else
		    {
			    // フレーム数を設定
			    rendererRef_->SetTime(static_cast<float>(_effect.elapsedTime));
			    Effekseer::Matrix43 mat = CnvMat43(_transform.world);
			    // 180度回転させる
			    Quaternion quat           = Quaternion::FromEuler({ 0.0f, DirectX::XMConvertToRadians(180.0f), 0.0f });
			    DirectX::XMMATRIX quatMat = DirectX::XMMatrixRotationQuaternion(quat);
			    Mat4x4 finalMat           = DirectX::XMMatrixMultiply(quatMat, _transform.world);

			    // エフェクトのインスタンスに対して処理をし、更新速度を設定
			    managerRef_->SetMatrix(handle, CnvMat43(finalMat));
			    managerRef_->SetSpeed(handle, _effect.playSpeed);
			    // 再生時間を増やす
			    _effect.elapsedTime += deltaTime;
		    }
	    });

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
		    drawParameter.ZNear                = 0.0f;
		    drawParameter.ZFar                 = data.farClipDistance;
		    drawParameter.ViewProjectionMatrix = rendererRef_->GetCameraProjectionMatrix();
		    managerRef_->Draw(drawParameter);
		    rendererRef_->EndRendering();
	    });
}

Effekseer::EffectRef EffectSystem::Play(
    cha::Resource<ResourceType::Effect>& _resource)
{
	EffectResource& particle = _resource.Get();
	// 該当するエフェクトが作成されていないなら、新規作成
	if (particle.ref == nullptr)
	{
		if (fs::exists(particle.filePath))
		{
			fs::path filePath(particle.filePath);
			auto str     = filePath.u16string();
			auto u16Char = str.c_str();
			particle.ref = Effekseer::Effect::Create(managerRef_, u16Char);
		}
	}

	// エフェクトの元データへの参照
	return particle.ref;
}