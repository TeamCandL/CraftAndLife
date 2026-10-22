#pragma once

#include <Base/Base.h>

#include <Game/Game.h>

struct ParticleSystemData
{
};

struct ParticleResource
{
	fs::path filePath{};
	Effekseer::EffectRef ref{nullptr};
};
struct ParticleInstance
{
	friend class ParticleSystem;
	// パーティクルの位置、回転、スケール
	Vec3 position{Vec3::Zero()};
	Quaternion rotation{Quaternion::Identity()};
	Vec3 scale{Vec3::One()};
	// ループするか
	bool isLoop{false};
	// 破棄するか
	bool destroyMe{false};
	// パーティクルの再生速度
	float playSpeed{1.0f};

private:
	Effekseer::Handle handle{};
	Effekseer::EffectRef ref{nullptr};
	double elapsedTime{0.0};
};
struct ParticleRegistry : public ResourceRegistryBase<ParticleResource>
{
public:
	ParticleRegistry();
	~ParticleRegistry() = default;

	bool TryLoad(fs::path&& _file, ParticleResource& _out) override;

private:
};

struct ParticleSystem : SystemObjectBase<ParticleSystem>
{

	void Start() override;

	void Update() override;
	void Draw();
	std::shared_ptr<ParticleInstance> Play(cha::Resource<ResourceType::Particle>& _resource);

private:
	std::shared_ptr<ParticleInstance> CreateInstance();
	std::vector<std::shared_ptr<ParticleInstance>> particleInstanceList_;
	// 描画オブジェクトの参照
	EffekseerRendererDX11::RendererRef rendererRef_;
	// エフェクトのマネージャーの参照
	Effekseer::ManagerRef managerRef_;
};

template <> struct ResourceTraits<ResourceType::Particle>
{
	using DataT = ParticleResource;
	using RegistryT = ParticleRegistry;
	using LoadConfigT = fs::path;
};