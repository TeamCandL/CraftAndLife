#include "MAP.h"
#include "pch/pch.h"

#include <algorithm>
#include <cmath>

namespace
{
	constexpr float PI = 3.14159265358979323846f;

	// 道の中心位置
	constexpr float PATH_START_X = -70.0f;
	constexpr float PATH_END_X = 70.0f;

	// 川の位置
	constexpr float RIVER_CENTER_Z = 0.0f;

	float ClampValue(float value, float minValue, float maxValue)
	{
		return std::max(minValue, std::min(value, maxValue));
	}

	float Lerp(float a, float b, float t)
	{
		return a + (b - a) * t;
	}

	float SmoothStep(float t)
	{
		t = ClampValue(t, 0.0f, 1.0f);
		return t * t * (3.0f - 2.0f * t);
	}
} // namespace

// ============================================================
// MapData
// ============================================================

MapData::MapData()
{
	Generate();
}

int32_t MapData::GetIndex(int32_t x, int32_t z) const
{
	return z * MapConfig::MAP_WIDTH + x;
}

bool MapData::IsInsideMap(float x, float z) const
{
	const float halfWidth = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

	const float halfDepth = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	return x >= -halfWidth && x < halfWidth && z >= -halfDepth && z < halfDepth;
}

// ============================================================
// 地形の高さ計算
// ============================================================

float MapData::CalculateHeight(float x, float z) const
{
	// 大きな丘
	const float hillA = std::sin(x * 0.035f) * 3.0f;

	const float hillB = std::cos(z * 0.045f) * 2.0f;

	const float hillC = std::sin((x + z) * 0.025f) * 2.0f;

	float height = MapConfig::BASE_HEIGHT + hillA + hillB + hillC;

	// マップ中央付近に高低差を作る
	const float distanceFromCenter = std::sqrt(x * x + z * z);

	height += std::max(0.0f, 1.0f - distanceFromCenter / 70.0f) * 3.0f;

	return ClampValue(height, MapConfig::BASE_HEIGHT, MapConfig::MAX_HEIGHT);
}

void MapData::Generate()
{
	heights.resize(MapConfig::MAP_WIDTH * MapConfig::MAP_DEPTH);

	const float halfWidth = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

	const float halfDepth = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	for (int32_t z = 0; z < MapConfig::MAP_DEPTH; ++z)
	{
		for (int32_t x = 0; x < MapConfig::MAP_WIDTH; ++x)
		{
			const float worldX = x * MapConfig::TILE_SIZE - halfWidth;

			const float worldZ = z * MapConfig::TILE_SIZE - halfDepth;

			heights[GetIndex(x, z)] = CalculateHeight(worldX, worldZ);
		}
	}

	SmoothHeights();

	// 道の部分は歩きやすい高さに近づける
	for (int32_t z = 0; z < MapConfig::MAP_DEPTH; ++z)
	{
		for (int32_t x = 0; x < MapConfig::MAP_WIDTH; ++x)
		{
			const float worldX = x * MapConfig::TILE_SIZE - halfWidth;

			const float worldZ = z * MapConfig::TILE_SIZE - halfDepth;

			if (IsPath(worldX, worldZ))
			{
				heights[GetIndex(x, z)] = MapConfig::BASE_HEIGHT;
			}
		}
	}
}

// ============================================================
// 高さの平滑化
// ============================================================

void MapData::SmoothHeights()
{
	std::vector<float> temporary(heights.size());

	for (int32_t iteration = 0; iteration < MapConfig::SMOOTHING_ITERATIONS; ++iteration)
	{
		temporary = heights;

		for (int32_t z = 1; z < MapConfig::MAP_DEPTH - 1; ++z)
		{
			for (int32_t x = 1; x < MapConfig::MAP_WIDTH - 1; ++x)
			{
				const int32_t index = GetIndex(x, z);

				const float neighborAverage = (heights[GetIndex(x - 1, z)] + heights[GetIndex(x + 1, z)] +
				                               heights[GetIndex(x, z - 1)] + heights[GetIndex(x, z + 1)]) *
				                              0.25f;

				temporary[index] = Lerp(heights[index], neighborAverage, MapConfig::SMOOTHING_RATE);
			}
		}

		heights.swap(temporary);
	}
}

// ============================================================
// 高さ取得：頂点間を補間する
// ============================================================

float MapData::GetHeight(float x, float z) const
{
	if (!IsInsideMap(x, z) || heights.empty())
	{
		return MapConfig::BASE_HEIGHT;
	}

	const float halfWidth = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

	const float halfDepth = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	const float gridX = (x + halfWidth) / MapConfig::TILE_SIZE;

	const float gridZ = (z + halfDepth) / MapConfig::TILE_SIZE;

	const int32_t x0 = static_cast<int32_t>(gridX);
	const int32_t z0 = static_cast<int32_t>(gridZ);

	const int32_t x1 = std::min(x0 + 1, MapConfig::MAP_WIDTH - 1);

	const int32_t z1 = std::min(z0 + 1, MapConfig::MAP_DEPTH - 1);

	const float tx = gridX - x0;
	const float tz = gridZ - z0;

	const float h00 = heights[GetIndex(x0, z0)];
	const float h10 = heights[GetIndex(x1, z0)];
	const float h01 = heights[GetIndex(x0, z1)];
	const float h11 = heights[GetIndex(x1, z1)];

	const float top = Lerp(h00, h10, tx);
	const float bottom = Lerp(h01, h11, tx);

	return Lerp(top, bottom, tz);
}

// ============================================================
// 道・川
// ============================================================

bool MapData::IsPath(float x, float z) const
{
	// スタートからゴールまでの直線的な道
	const float pathCenterX = Lerp(
	    PATH_START_X, PATH_END_X, ClampValue((z + MapConfig::MAP_DEPTH * 0.5f) / MapConfig::MAP_DEPTH, 0.0f, 1.0f));

	return std::abs(x - pathCenterX) <= MapConfig::PATH_WIDTH * 0.5f;
}

bool MapData::IsRiver(float x, float z) const
{
	(void)x;

	return std::abs(z - RIVER_CENTER_Z) <= MapConfig::RIVER_WIDTH * 0.5f;
}

// ============================================================
// スタート・ゴール位置
// ============================================================

Vec3 MapData::GetStartPosition() const
{
	const float x = PATH_START_X;
	const float z = -MapConfig::MAP_DEPTH * 0.4f;

	return Vec3{x, GetHeight(x, z), z};
}

Vec3 MapData::GetGoalPosition() const
{
	const float x = PATH_END_X;
	const float z = MapConfig::MAP_DEPTH * 0.4f;

	return Vec3{x, GetHeight(x, z), z};
}

// ============================================================
// MAP
// ============================================================

void MAP::Start()
{
	// マップデータは MapData の生成時に作成済み
}

void MAP::Update()
{
	// 必要に応じてマップのイベント処理を追加
}

void MAP::Draw()
{
	/*
	    描画処理は、現在の情報だけでは確定できません。

	    Enemy.cpp ではモデル描画に次のAPIを使っています。

	    Get<Dx11Manager>().SetShader(shader);
	    Get<Dx11Manager>().SetSurface(parts.mesh);
	    Get<Dx11Manager>().SetMaterial(parts.material);
	    Get<Dx11Manager>().Draw();

	    地形を動的な三角形メッシュとして生成するための
	    メッシュ作成APIが必要です。
	*/
}