#pragma once

#include <Base/Base.h>

#include <Game/Game.h>

struct EffectResource
{
	fs::path filePath{};
	Effekseer::EffectRef ref{nullptr};
};
struct EffectInstance
{
	friend class EffectSystem;
	// エフェクトの位置、回転、スケール
	Vec3 position{Vec3::Zero()};
	Quaternion rotation{Quaternion::Identity()};
	Vec3 scale{Vec3::One()};
	// ループするか
	bool isLoop{false};
	// 破棄するか
	bool destroyMe{false};
	// エフェクトの再生速度
	float playSpeed{1.0f};

private:
	Effekseer::Handle handle{};
	Effekseer::EffectRef ref{nullptr};
	double elapsedTime{0.0};
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
	std::shared_ptr<EffectInstance> Play(cha::Resource<ResourceType::Effect>& _resource);

private:
	std::shared_ptr<EffectInstance> CreateInstance();
	std::vector<std::shared_ptr<EffectInstance>> effectInstanceList_;
	// 描画オブジェクトの参照
	EffekseerRendererDX11::RendererRef rendererRef_;
	// エフェクトのマネージャーの参照
	Effekseer::ManagerRef managerRef_;
};

template <> struct ResourceTraits<ResourceType::Effect>
{
	using DataT = EffectResource;
	using RegistryT = EffectRegistry;
	using LoadConfigT = fs::path;
};