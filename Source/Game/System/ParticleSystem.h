#pragma once
#include <Game/Game.h>
// Effekseer
#include <Effekseer/Effekseer.h>
#include <Effekseer/EffekseerRendererDX11.h>
struct ParticleParameter
{
	// パーティクルの位置、回転、スケール
	Vec3 position;
	Quaternion rotation;
	Vec3 scale;
	// ループするか
	bool isLoop;
	// 破棄するか
	bool destroyMe;
	// パーティクルの再生速度
	float playSpeed;
};

struct ParticleSystemData
{
};

struct ParticleResource
{
	fs::path filePath;
	Effekseer::EffectRef ref;
};
struct ParticleData
{
	// パーティクルの位置、回転、スケール
	Vec3 position;
	Quaternion rotation;
	Vec3 scale;
	// ループするか
	bool isLoop;
	// 破棄するか
	bool destroyMe;
	// パーティクルの再生速度
	float playSpeed;

private:
	Effekseer::Handle handle;
};
struct ParticleRegistry : public ResourceRegistryBase<ParticleResource>
{
public:
	ParticleRegistry() = default;
	~ParticleRegistry() = default;

	bool TryLoad(fs::path&& _file, ParticleResource& _out) override;

private:
	// 描画オブジェクトの参照
	EffekseerRendererDX11::RendererRef rendererRef_;
	// エフェクトのマネージャーの参照
	Effekseer::ManagerRef managerRef_;
};

struct ParticleSystem : SystemObjectBase<ParticleSystem>
{
	/// @brief 開始処理
	void Start() override;
	/// @brief 更新処理
	void Update() override;
	std::unordered_map<std::string, std::shared_ptr<ParticleData>> effectDataMap;
};