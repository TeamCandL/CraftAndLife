#include "pch\pch.h"
#include "ParticleSystem.h"

void ParticleSystem::Start()
{
	Get<Dx11Manager>().Ref([](ComPtr<ID3D11Device> pDevice, ComPtr<ID3D11DeviceContext> pContext)
	{
		//pDevice->
	});
}

void ParticleSystem::Update()
{
}
