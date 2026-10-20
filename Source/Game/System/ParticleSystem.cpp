#include "pch\pch.h"

#include "ParticleSystem.h"

#ifdef _DEBUG
#pragma comment(lib, "Effekseerd.lib")
#pragma comment(lib, "EffekseerRendererDX11d.lib")
#else
#pragma comment(lib, "Effekseer.lib")
#pragma comment(lib, "EffekseerRendererDX11.lib")
#endif

void ParticleSystem::Start()
{
	Get<Dx11Manager>().Ref(
	    [](ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	    {
		    // pDevice->
	    });
}

void ParticleSystem::Update()
{
}
