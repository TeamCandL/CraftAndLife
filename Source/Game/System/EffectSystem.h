#pragma once

#include <Base/Base.h>

struct EffectResource
{
	fs::path filePath{};
	Effekseer::EffectRef ref{ nullptr };
};
struct EffectInstance
{
};

struct EffectRegistry : public ResourceRegistryBase<EffectResource>
{
  public:
	EffectRegistry();
	~EffectRegistry() = default;

	bool TryLoad(fs::path&& _file, EffectResource& _out) override;

  private:
};

struct EffectSystem : SystemObjectBase<EffectSystem>
{
	void Start() override;
	void Update() override;
	void Draw();
	Effekseer::Handle Play(cha::Resource<ResourceType::Effect>& _resource);

  private:
	// 描画オブジェクトの参照
	EffekseerRendererDX11::RendererRef rendererRef_;
	// エフェクトのマネージャーの参照
	Effekseer::ManagerRef managerRef_;
};

template <> struct ResourceTraits<ResourceType::Effect>
{
	using DataT       = EffectResource;
	using RegistryT   = EffectRegistry;
	using LoadConfigT = fs::path;
};