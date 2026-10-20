#include "pch\pch.h"

#include "ParticleSystem.h"

#ifdef _DEBUG
#pragma comment(lib, "Effekseerd.lib")
#pragma comment(lib, "EffekseerRendererDX11d.lib")
#else
#pragma comment(lib, "Effekseer.lib")
#pragma comment(lib, "EffekseerRendererDX11.lib")
#endif

bool ParticleRegistry::TryLoad(fs::path&& _file, ParticleResource& _out)
{
	if (fs::is_regular_file(_file))
	{
		auto particleData = std::make_shared<ParticleResource>();
		particleData->ref = Effekseer::Effect::Create(managerRef_, _file.u16string().c_str());
		particleData->filePath = _file;
	}
}
