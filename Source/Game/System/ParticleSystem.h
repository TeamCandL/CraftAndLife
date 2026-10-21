// #pragma once
//// Effekseer
// #include <Effekseer/Effekseer.h>
// #include <Effekseer/EffekseerRendererDX11.h>
// #include <Game/Game.h>
// struct ParticleParameter
//{
//	// パーティクルの位置、回転、スケール
//	Vec3 position;
//	Quaternion rotation;
//	Vec3 scale;
//	// ループするか
//	bool isLoop;
//	// 破棄するか
//	bool destroyMe;
//	// パーティクルの再生速度
//	float playSpeed;
// };
//
// struct ParticleSystemData
//{
// };
//
// struct ParticleResource
//{
//	fs::path filePath;
//	Effekseer::EffectRef ref;
// };
// struct ParticleInstance
//{
//	friend class ParticleSystem;
//	// パーティクルの位置、回転、スケール
//	Vec3 position;
//	Quaternion rotation;
//	Vec3 scale;
//	// ループするか
//	bool isLoop;
//	// 破棄するか
//	bool destroyMe;
//	// パーティクルの再生速度
//	float playSpeed;
//
//	private:
//	Effekseer::Handle handle;
//	Effekseer::EffectRef ref;
//	double elapsedTime;
// };
// struct ParticleRegistry : public ResourceRegistryBase<ParticleResource>
//{
// public:
//	ParticleRegistry();
//	~ParticleRegistry() = default;
//
//	bool TryLoad(fs::path&& _file, ParticleResource& _out) override;
//
// private:
//
// };
//
// struct ParticleSystem : SystemObjectBase<ParticleSystem>
//{
//
//	void Start() override;
//
//	void Update() override;
//	void Draw();
//	std::shared_ptr<ParticleInstance> Play(cha::Resource<ResourceType::Particle>& _resource);
//
// private:
//	std::shared_ptr<ParticleInstance> CreateInstance();
//	std::vector<std::shared_ptr<ParticleInstance>> particleInstanceList_;
//	// 描画オブジェクトの参照
//	EffekseerRendererDX11::RendererRef rendererRef_;
//	// エフェクトのマネージャーの参照
//	Effekseer::ManagerRef managerRef_;
// };
//
// template <> struct ResourceTraits<ResourceType::Particle>
//{
//	using DataT = ParticleResource;
//	using RegistryT = ParticleRegistry;
//	using LoadConfigT = fs::path;
// };