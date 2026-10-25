
#include "MAP.h"
#include "pch/pch.h"

#include <Game/Common/GameManager.h>
#include <Game/Common/Sky.h>
#include <Game/Tool/DebugCamera.h>
#include <Game/Terrain/TerrainMaterial.h>
#include <Game/PlayScene/TerrainSystem.h>

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <memory>
#include <vector>
#include <cstdio>

namespace
{
	// 円周率（角度をラジアンで扱う計算に使用）
	constexpr float PI = 3.14159265358979323846f;
	// ほぼゼロかどうかを判定する誤差の許容値
	constexpr float EPSILON = 0.0001f;

	// ========================================================
	// 共通計算
	// ========================================================

	// 値を指定した最小値～最大値の範囲に収める
	float ClampValue(
	    float value,
	    float minValue,
	    float maxValue)
	{
		return std::max(minValue, std::min(value, maxValue));
	}

	// 2つの値を割合 t に応じて線形補間する
	float Lerp(
	    float a,
	    float b,
	    float t)
	{
		return a + (b - a) * t;
	}

	// 0～1の値を、始まりと終わりが滑らかになるように補間する
	float SmoothStep(
	    float t)
	{
		t = ClampValue(t, 0.0f, 1.0f);
		return t * t * (3.0f - 2.0f * t);
	}

	// XZ平面上にある2点間の距離を計算する
	float GetDistance(
	    float x1,
	    float z1,
	    float x2,
	    float z2)
	{
		const float DX = x1 - x2;
		const float DZ = z1 - z2;

		return std::sqrt(DX * DX + DZ * DZ);
	}

	// ========================================================
	// 川の中心位置
	// ========================================================

	// Z座標に応じた川の中心X座標を計算する（2つの波で蛇行させる）
	float GetRiverCenterX(
	    float z)
	{
		return MapConfig::RIVER_CENTER_X + std::sin(z * MapConfig::RIVER_FREQUENCY_A) * MapConfig::RIVER_BEND_A +
		       std::sin(z * MapConfig::RIVER_FREQUENCY_B + MapConfig::RIVER_PHASE_B) * MapConfig::RIVER_BEND_B;
	}
}  // namespace

// ============================================================
// MapData
// ============================================================

// MapData作成時に地形データを生成する
MapData::MapData()
{
	Generate();
}

// 2次元の頂点座標 (x, z) を高さ配列の1次元インデックスに変換する
int32_t MapData::GetIndex(
    int32_t x,
    int32_t z) const
{
	return z * (MapConfig::MAP_WIDTH + 1) + x;
}

// 指定したワールド座標がマップの範囲内にあるか調べる
bool MapData::IsInsideMap(
    float x,
    float z) const
{
	// マップの半分の幅（中心を原点にするために使用）
	const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

	// マップの半分の奥行き（中心を原点にするために使用）
	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	return x >= -HALF_WIDTH && x < HALF_WIDTH && z >= -HALF_DEPTH && z < HALF_DEPTH;
}

// ============================================================
// 地形の高さ計算
// ============================================================

float MapData::CalculateRawHeight(
    float x,
    float z) const
{
	// --------------------------------------------------------
	// 1. 地形全体の緩やかな起伏
	// --------------------------------------------------------

	// 地面全体に小さな波状の起伏を加える（係数を大きくすると起伏が強くなる）
	float height = MapConfig::BASE_HEIGHT + std::sin(x * 0.03f) * 1.2f + std::cos(z * 0.035f) * 1.2f;

	// --------------------------------------------------------
	// 2. 北東方向への傾斜
	// --------------------------------------------------------

	// 北東方向への傾斜がどの程度進んでいるかを0～1で求める
	const float SLOPE_RATE = ClampValue((x + z + MapConfig::SLOPE_OFFSET) / MapConfig::SLOPE_DISTANCE, 0.0f, 1.0f);

	height += SmoothStep(SLOPE_RATE) * MapConfig::NORTH_EAST_RISE;

	// --------------------------------------------------------
	// 3. 左上の渦巻き丘
	// 外周を高く、中央を低くする
	// --------------------------------------------------------

	// 現在位置から渦巻き丘の中心までのX方向の距離
	const float DX = x - MapConfig::SPIRAL_HILL_X;
	// 現在位置から渦巻き丘の中心までのZ方向の距離
	const float DZ = z - MapConfig::SPIRAL_HILL_Z;

	// 丘の中心から現在位置までの距離
	const float DIST = std::sqrt(DX * DX + DZ * DZ);
	// 渦巻き丘の半径（大きくすると丘が広くなる）
	const float RADIUS = MapConfig::SPIRAL_HILL_RADIUS;

	if (DIST < RADIUS)
	{
		// 丘の中心から外周までの割合（0が中心、1が外周）
		const float R = DIST / RADIUS;
		// 丘の中心から見た現在位置の角度
		const float ANGLE = std::atan2(DZ, DX);

		// 外周に向かって高くなる丘
		// 外周に向かうほど高くなる高さ（SPIRAL_HILL_HEIGHTで最大値を調整）
		const float OUTER_HEIGHT = SmoothStep(R) * MapConfig::SPIRAL_HILL_HEIGHT;

		// 渦巻き状の尾根
		// 距離に応じて角度をずらし、渦巻き模様を作る
		const float SPIRAL_ANGLE = ANGLE + R * MapConfig::SPIRAL_TURNS * 2.0f * PI;

		// 渦巻き状の尾根の高さ（係数2.0を大きくすると尾根が強くなる）
		const float RIDGE = std::cos(SPIRAL_ANGLE) * 2.0f * SmoothStep(R);

		// 中央のくぼみ
		// 丘の中央を低くするくぼみの深さ（係数2.0で調整）
		const float CENTER_BASIN = (1.0f - SmoothStep(R)) * 10.0f;

		height += OUTER_HEIGHT + RIDGE - CENTER_BASIN;

		// ----------------------------------------------------
		// 3-1. 丘の後ろ側に入口を作る
		// 北側（Z座標が大きい方向）から入る
		// ----------------------------------------------------

		// 入口の横幅（大きくすると通路が広くなる）
		constexpr float ENTRANCE_WIDTH = 10.0f;
		// 入口の長さ（大きくすると坂が長くなり、傾斜を緩やかにしやすい）
		constexpr float ENTRANCE_LENGTH = 40.0f;
		// 入口の奥側に加える高さ（小さくすると入口の高低差が小さくなる）
		constexpr float ENTRANCE_HEIGHT = 5.0f;

		// 入口の中心X座標（丘の中心に合わせる）
		const float ENTRANCE_CENTER_X = MapConfig::SPIRAL_HILL_X;

		// 入口の開始位置Z（丘の北側外周）
		const float ENTRANCE_START_Z = MapConfig::SPIRAL_HILL_Z + RADIUS;

		// 入口の終了位置Z（開始位置から丘の内側へ進んだ場所）
		const float ENTRANCE_END_Z = ENTRANCE_START_Z - ENTRANCE_LENGTH;

		// 現在位置が入口の横幅・長さの範囲内か判定する
		const bool IN_ENTRANCE =
		    std::abs(x - ENTRANCE_CENTER_X) <= ENTRANCE_WIDTH * 0.5f && z <= ENTRANCE_START_Z && z >= ENTRANCE_END_Z;

		if (IN_ENTRANCE)
		{
			// 入口の外側から内側に向かって上がる
			// 入口の外側から内側まで、どこまで進んだかを0～1で求める
			const float ENTRANCE_RATE = ClampValue((ENTRANCE_START_Z - z) / ENTRANCE_LENGTH, 0.0f, 1.0f);

			// 入口の目標高さ（進行率に応じて徐々に高くする）
			const float ENTRANCE_TARGET = MapConfig::BASE_HEIGHT + ENTRANCE_RATE * ENTRANCE_HEIGHT;

			// 地形を削りすぎず、通路の高さを確保する
			// 入口の中央ほど補正を強くし、左右の端は周囲の地形につなぐ
			const float ENTRANCE_BLEND =
			    SmoothStep(ClampValue(1.0f - std::abs(x - ENTRANCE_CENTER_X) / (ENTRANCE_WIDTH * 0.5f), 0.0f, 1.0f));

			// 周囲の地形から入口の高さへ補間し、地面を急に切り替えない
			height = Lerp(height, std::min(height, ENTRANCE_TARGET), ENTRANCE_BLEND);
		}

		// ----------------------------------------------------
		// 3-2. 入口の奥にある床（ここを調整すれば内部の床を変更できる）
		// ----------------------------------------------------
		// 床の横幅：大きくすると左右に広がる
		constexpr float BACK_FLOOR_WIDTH = 24.0f;
		// 床の奥行き：大きくすると入口の奥に長く広がる
		constexpr float BACK_FLOOR_LENGTH = 10.0f;
		// 床の高さ：BASE_HEIGHTを基準に上下させる
		constexpr float BACK_FLOOR_HEIGHT = 3.0f;
		// 床の中心X：渦巻き丘の中心に合わせる。左右に動かしたい場合は変更
		constexpr float BACK_FLOOR_CENTER_X = MapConfig::SPIRAL_HILL_X;
		// 床の中心Z：丘の中心より入口側／奥側へ移動したい場合は変更
		constexpr float BACK_FLOOR_CENTER_Z = MapConfig::SPIRAL_HILL_Z;
		// 床の端を周囲の地形につなぐ幅：大きいほど接続がなだらかになる
		constexpr float BACK_FLOOR_BLEND = 20.0f;

		// 床の中心からX・Z方向にどれだけ離れているか
		const float FLOOR_DX = std::abs(x - BACK_FLOOR_CENTER_X);
		const float FLOOR_DZ = std::abs(z - BACK_FLOOR_CENTER_Z);
		// 床の中央は1、外側へ行くほど0になる補正率を計算
		const float FLOOR_X_RATE =
		    ClampValue((BACK_FLOOR_WIDTH * 0.5f + BACK_FLOOR_BLEND - FLOOR_DX) / BACK_FLOOR_BLEND, 0.0f, 1.0f);
		const float FLOOR_Z_RATE =
		    ClampValue((BACK_FLOOR_LENGTH * 0.5f + BACK_FLOOR_BLEND - FLOOR_DZ) / BACK_FLOOR_BLEND, 0.0f, 1.0f);
		const float FLOOR_BLEND = SmoothStep(std::min(FLOOR_X_RATE, FLOOR_Z_RATE));

		// 指定した床の高さへ滑らかにつなぐ。床の高さを変える場合は上の定数を調整
		const float BACK_FLOOR_TARGET = MapConfig::BASE_HEIGHT + BACK_FLOOR_HEIGHT;
		height                        = Lerp(height, BACK_FLOOR_TARGET, FLOOR_BLEND);
	}

	// --------------------------------------------------------
	// 4. 丘以外の段丘
	// --------------------------------------------------------

	// 左側の段丘の中心からの距離（-12, 5が中心座標）
	const float LEFT_TERRACE_DIST = GetDistance(x, z, -12.0f, 5.0f);

	if (LEFT_TERRACE_DIST < 24.0f)
	{
		// 段丘の半径24に対する距離の割合
		const float R = LEFT_TERRACE_DIST / 24.0f;

		height += (1.0f - SmoothStep(R)) * 2.5f;
	}

	// 右側の段丘の中心からの距離（35, 22が中心座標）
	const float RIGHT_TERRACE_DIST = GetDistance(x, z, 35.0f, 22.0f);

	if (RIGHT_TERRACE_DIST < 25.0f)
	{
		// 段丘の半径25に対する距離の割合
		const float R = RIGHT_TERRACE_DIST / 25.0f;

		height += (1.0f - SmoothStep(R)) * 3.0f;
	}

	// --------------------------------------------------------
	// 5. 北東のなめらかな山（ゴール地点）
	// --------------------------------------------------------

	// ========================================================
	// 【手打ち調整エリア：北東の山】
	// ========================================================
	constexpr float MOUNTAIN_X      = 60.0f;  // 山の中心X座標（ゴール付近に合わせる）
	constexpr float MOUNTAIN_Z      = 80.0f;  // 山の中心Z座標
	constexpr float MOUNTAIN_RADIUS = 20.0f;  // 山の広さ（半径）
	constexpr float MOUNTAIN_HEIGHT = 9.0f;   // 山の高さ（全体の最高峰にするため高く設定）

	const float MX = x - MOUNTAIN_X;
	const float MZ = z - MOUNTAIN_Z;

	const float MOUNTAIN_DIST = std::sqrt(MX * MX + MZ * MZ);

	if (MOUNTAIN_DIST < MOUNTAIN_RADIUS)
	{
		const float R = MOUNTAIN_DIST / MOUNTAIN_RADIUS;  // 0.0(中心) ～ 1.0(ふもと)

		// 山の中心を高く、外周に向かってなだらかに低くなる滑らかな高さを計算
		// std::floor を外しているため、カクカクせず滑らかな斜面になります
		const float SMOOTH_MOUNTAIN = (1.0f - SmoothStep(R)) * MOUNTAIN_HEIGHT;

		height += SMOOTH_MOUNTAIN;
	}

	// --------------------------------------------------------
	// 6. 川の地形（北東の山の前を流れる川・谷筋の形成）
	// --------------------------------------------------------

	// ========================================================
	// 【手打ち調整エリア：川のルートと深さ】
	// ========================================================
	// Z座標に応じて川の曲がり具合（X座標）を計算する関数を拡張・調整します
	auto GetCustomRiverCenterX = [](float z)
	{
		// 既存の基本の蛇行に加えて、北東の山の前（特定のZ範囲）で大きく手前に引き込むカーブを作る
		float baseCurve =
		    MapConfig::RIVER_CENTER_X + std::sin(z * MapConfig::RIVER_FREQUENCY_A) * MapConfig::RIVER_BEND_A;

		// 北東の山のふもと付近（例: Zが 30.0f ～ 50.0f のエリア）で手前（西側）へ川を迂回させる
		if (z >= 25.0f && z <= 55.0f)
		{
			float t = (z - 25.0f) / 30.0f;          // 0.0 ～ 1.0
			baseCurve -= std::sin(t * PI) * 12.0f;  // 手前側にグッと引き込む
		}

		return baseCurve;
	};

	const float RIVER_CENTER = GetCustomRiverCenterX(z);
	const float RIVER_DIST   = std::abs(x - RIVER_CENTER);

	// 川岸の幅（広くすると、川に向かってなだらかな斜面になる）
	constexpr float RIVER_BANK_WIDTH = 16.0f;
	// 川の深さ（橋を架けたり歩いて渡ったりしやすいように適度な深さに調整）
	constexpr float RIVER_DEPTH = 1.2f;

	const float RIVER_RATE = ClampValue(RIVER_DIST / RIVER_BANK_WIDTH, 0.0f, 1.0f);

	// 北東の山の前を削りすぎないよう、山と交差する場所の深さを綺麗にブレンドする
	float riverDepression = (1.0f - SmoothStep(RIVER_RATE)) * RIVER_DEPTH;

	// 山のエリアと川が重なる部分で不自然にならないよう調整して高さを下げる
	height -= riverDepression;

	// --------------------------------------------------------
	// 7. 高さの制限
	// --------------------------------------------------------

	// 高さが設定した最低値・最高値を超えないように制限する
	return ClampValue(height, MapConfig::BASE_HEIGHT, MapConfig::MAX_HEIGHT);
}

// ============================================================
// 地形生成
// ============================================================

void MapData::Generate()
{
	// X方向の頂点数（マス数より1つ多い）
	const int32_t VERTEX_WIDTH = MapConfig::MAP_WIDTH + 1;

	// Z方向の頂点数（マス数より1つ多い）
	const int32_t VERTEX_DEPTH = MapConfig::MAP_DEPTH + 1;

	const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	// 全頂点の高さを保存できるよう配列の大きさを確保する
	heights.resize(static_cast<size_t>(VERTEX_WIDTH) * static_cast<size_t>(VERTEX_DEPTH));

	// 高さを連続値で保存する
	// 滝による追加の高さ減算は行わない
	for (int32_t z = 0; z < VERTEX_DEPTH; ++z)
	{
		for (int32_t x = 0; x < VERTEX_WIDTH; ++x)
		{
			// 配列のX座標を、マップ中心を原点としたワールド座標へ変換する
			const float WORLD_X = x * MapConfig::TILE_SIZE - HALF_WIDTH;

			// 配列のZ座標を、マップ中心を原点としたワールド座標へ変換する
			const float WORLD_Z = z * MapConfig::TILE_SIZE - HALF_DEPTH;

			// この頂点の地形の高さを計算して保存する
			heights[GetIndex(x, z)] = CalculateRawHeight(WORLD_X, WORLD_Z);
		}
	}

	// 道も同じ地形高度に沿わせる
	for (int32_t z = 0; z < VERTEX_DEPTH; ++z)
	{
		for (int32_t x = 0; x < VERTEX_WIDTH; ++x)
		{
			const float WORLD_X = x * MapConfig::TILE_SIZE - HALF_WIDTH;

			const float WORLD_Z = z * MapConfig::TILE_SIZE - HALF_DEPTH;

			if (IsPath(WORLD_X, WORLD_Z))
			{
				heights[GetIndex(x, z)] = CalculateRawHeight(WORLD_X, WORLD_Z);
			}
		}
	}
}

// ============================================================
// 高さ取得
// ============================================================

float MapData::GetHeight(
    float x,
    float z) const
{
	if (!IsInsideMap(x, z) || heights.empty())
	{
		return MapConfig::BASE_HEIGHT;
	}

	const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	// ワールド座標を高さ配列のX方向の格子番号へ変換する
	const int32_t GRID_X = static_cast<int32_t>(
	    ClampValue((x + HALF_WIDTH) / MapConfig::TILE_SIZE, 0.0f, static_cast<float>(MapConfig::MAP_WIDTH)));

	// ワールド座標を高さ配列のZ方向の格子番号へ変換する
	const int32_t GRID_Z = static_cast<int32_t>(
	    ClampValue((z + HALF_DEPTH) / MapConfig::TILE_SIZE, 0.0f, static_cast<float>(MapConfig::MAP_DEPTH)));

	// 指定位置に最も近い格子点の高さを返す
	return heights[GetIndex(GRID_X, GRID_Z)];
}

// ============================================================
// 道の判定
// ============================================================

bool MapData::IsPath(
    float x,
    float z) const
{
	const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

	// マップの南端から北端までの位置を0～1に変換する
	const float T = ClampValue((z + HALF_DEPTH) / (MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE), 0.0f, 1.0f);

	// スタートからゴールへ進むにつれて、左右に蛇行する道の中心X座標
	const float PATH_CENTER_X = Lerp(MapConfig::PATH_START_X, MapConfig::PATH_GOAL_X, T) +
	                            std::sin(T * 3.0f * PI) * 18.0f + std::cos(T * 1.5f * PI) * 10.0f;

	// 道の中心からの距離が道幅の半分以内なら道と判定する
	return std::abs(x - PATH_CENTER_X) <= MapConfig::PATH_WIDTH * 0.5f;
}

// ============================================================
// 川の判定
// ============================================================

bool MapData::IsRiver(
    float x,
    float z) const
{
	// 川の中心からの距離が川幅の半分以内なら川と判定する
	return std::abs(x - GetRiverCenterX(z)) <= MapConfig::RIVER_WIDTH * 0.5f;
}

// ============================================================
// スタート・ゴール位置
// ============================================================

Vec3 MapData::GetStartPosition() const
{
	// スタート地点のX座標
	const float X = MapConfig::PATH_START_X;
	// スタート地点のZ座標
	const float Z = MapConfig::PATH_START_Z;

	// 地形の高さをY座標に使い、スタート位置を返す
	return Vec3{ X, GetHeight(X, Z), Z };
}

Vec3 MapData::GetGoalPosition() const
{
	// ゴール地点のX座標
	const float X = MapConfig::PATH_GOAL_X;
	// ゴール地点のZ座標
	const float Z = MapConfig::PATH_GOAL_Z;

	return Vec3{ X, GetHeight(X, Z), Z };
}

Vec3 MapData::GetBeginPosision() const
{
	float x = MapConfig::PATH_START_X;
	float z = MapConfig::PATH_START_Z;
	float y = GetHeight(x, z);

	return Vec3();
}

Vec3 MapData::GetEndPosision() const
{
	float x = MapConfig::PATH_GOAL_X;
	float z = MapConfig::PATH_GOAL_Z;
	float y = GetHeight(x, z);

	return Vec3();
}

// ============================================================
// MAP 初期化
// ============================================================

void MAP::Start()
{
	Get<TerrainSystem>().Register(this -> ToGameObject());
	MaterialResource materialResource{};

	std::unique_ptr<TerrainMaterialProperty> property = std::make_unique<TerrainMaterialProperty>();

	property->Get().hasTexture = { TRUE, TRUE, TRUE, TRUE };

	property->Get().baseColor = { 1, 1, 1, 1 };

	materialResource.TryLoad(std::move(property));

	material = Resource<Material>{ std::move(materialResource) };

	Reload();
}

void MAP::Update() {}

// ============================================================
// 地形描画
// 水面の描画は行わない
// ============================================================

void MAP::Draw()
{
	// DirectXの描画管理機能を取得する
	Dx11Manager& DX11 = Get<Dx11Manager>();

	// オブジェクトのTransformに設定されたマテリアルを適用する
	DX11.SetMaterial(transform.GetMaterial());
	// 地形用マテリアルを適用する
	DX11.SetMaterial(material);
	// 地形用シェーダーを設定する
	DX11.SetShader(shader);
	// 生成した地形メッシュを描画対象に設定する
	DX11.SetSurface(surface);
	// 地形テクスチャを設定する
	DX11.SetTexture(texture);
	// 設定した地形を描画する
	DX11.Draw();
}

// ============================================================
// 地形メッシュ生成
// ============================================================

void MAP::Reload()
{
	Get<Dx11Manager>().Ref(
	    [this](const ComPtr<ID3D11Device>& device)
	    {
		    // エンジンで使用する頂点データ型を指定する
		    using Vertex = ModelVertex;

		    // 地形メッシュのX方向の頂点数
		    const uint32_t VERTEX_WIDTH = MapConfig::MAP_WIDTH + 1;

		    // 地形メッシュのZ方向の頂点数
		    const uint32_t VERTEX_DEPTH = MapConfig::MAP_DEPTH + 1;

		    // メッシュ全体の頂点数
		    const uint32_t VERTEX_COUNT = VERTEX_WIDTH * VERTEX_DEPTH;

		    std::vector<Vertex> vertices(VERTEX_COUNT);

		    const float HALF_WIDTH = MapConfig::MAP_WIDTH * MapConfig::TILE_SIZE * 0.5f;

		    const float HALF_DEPTH = MapConfig::MAP_DEPTH * MapConfig::TILE_SIZE * 0.5f;

		    // ------------------------------------------------
		    // 1. 頂点を作成
		    // ------------------------------------------------

		    for (uint32_t z = 0; z < VERTEX_DEPTH; ++z)
		    {
			    for (uint32_t x = 0; x < VERTEX_WIDTH; ++x)
			    {
				    // 2次元の格子座標を頂点配列の番号へ変換する
				    const uint32_t INDEX = x + z * VERTEX_WIDTH;

				    // 頂点のワールド座標を設定するための変数
				    Vec3 position{};

				    position.x = x * MapConfig::TILE_SIZE - HALF_WIDTH;

				    position.z = z * MapConfig::TILE_SIZE - HALF_DEPTH;

				    position.y = heights[GetIndex(static_cast<int32_t>(x), static_cast<int32_t>(z))];

				    // 頂点の位置をメッシュデータへ保存する
				    vertices[INDEX].position = position;

				    // タイルサイズに合わせてテクスチャ座標を設定する
				    vertices[INDEX].uv = Vec2{ x * MapConfig::TILE_SIZE, z * MapConfig::TILE_SIZE };

				    // 法線は周囲の高さから計算する
				    // 頂点の左隣の高さ（法線計算用）
				    const float LEFT_HEIGHT = GetHeight(position.x - MapConfig::TILE_SIZE, position.z);

				    // 頂点の右隣の高さ（法線計算用）
				    const float RIGHT_HEIGHT = GetHeight(position.x + MapConfig::TILE_SIZE, position.z);

				    // 頂点の後ろ側の高さ（法線計算用）
				    const float BACK_HEIGHT = GetHeight(position.x, position.z - MapConfig::TILE_SIZE);

				    // 頂点の前側の高さ（法線計算用）
				    const float FRONT_HEIGHT = GetHeight(position.x, position.z + MapConfig::TILE_SIZE);

				    // 周囲の高さの差から、地表が向いている方向（法線）を求める
				    const Vec3 NORMAL{ LEFT_HEIGHT - RIGHT_HEIGHT,
					                   2.0f * MapConfig::TILE_SIZE,
					                   BACK_HEIGHT - FRONT_HEIGHT };

				    // 法線ベクトルの長さを求め、正規化に使用する
				    const float NORMAL_LENGTH =
				        std::sqrt(NORMAL.x * NORMAL.x + NORMAL.y * NORMAL.y + NORMAL.z * NORMAL.z);

				    if (NORMAL_LENGTH > EPSILON)
				    {
					    // 法線の長さを1にそろえて頂点へ保存する
					    vertices[INDEX].normal =
					        Vec3{ NORMAL.x / NORMAL_LENGTH, NORMAL.y / NORMAL_LENGTH, NORMAL.z / NORMAL_LENGTH };
				    }
				    else
				    {
					    // 計算できない場合は上向きの法線を使用する
					    vertices[INDEX].normal = Vec3::Up();
				    }
			    }
		    }

		    // ------------------------------------------------
		    // 2. インデックスを作成
		    // ------------------------------------------------

		    const uint32_t INDEX_COUNT = MapConfig::MAP_WIDTH * MapConfig::MAP_DEPTH * 6;

		    std::vector<uint32_t> indices(INDEX_COUNT);

		    uint32_t index = 0;

		    for (uint32_t z = 0; z < MapConfig::MAP_DEPTH; ++z)
		    {
			    for (uint32_t x = 0; x < MapConfig::MAP_WIDTH; ++x)
			    {
				    // 現在のマスの左上の頂点番号
				    const uint32_t TOP_LEFT = z * VERTEX_WIDTH + x;

				    // 現在のマスの右上の頂点番号
				    const uint32_t TOP_RIGHT = TOP_LEFT + 1;

				    // 現在のマスの左下の頂点番号
				    const uint32_t BOTTOM_LEFT = (z + 1) * VERTEX_WIDTH + x;

				    // 現在のマスの右下の頂点番号
				    const uint32_t BOTTOM_RIGHT = BOTTOM_LEFT + 1;

				    indices[index++] = TOP_LEFT;
				    indices[index++] = TOP_RIGHT;
				    indices[index++] = BOTTOM_LEFT;

				    indices[index++] = TOP_RIGHT;
				    indices[index++] = BOTTOM_RIGHT;
				    indices[index++] = BOTTOM_LEFT;
			    }
		    }

		    // ------------------------------------------------
		    // 3. 頂点バッファ
		    // ------------------------------------------------

		    const D3D11_BUFFER_DESC VB_DESC{ .ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex)),
			                                 .Usage     = D3D11_USAGE_DEFAULT,
			                                 .BindFlags = D3D11_BIND_VERTEX_BUFFER };

		    ComPtr<ID3D11Buffer> vertexBuffer{};

		    const D3D11_SUBRESOURCE_DATA VB_DATA{ .pSysMem = vertices.data() };

		    if (FAILED(device->CreateBuffer(&VB_DESC, &VB_DATA, vertexBuffer.GetAddressOf())))
		    {
			    return;
		    }

		    // ------------------------------------------------
		    // 4. インデックスバッファ
		    // ------------------------------------------------

		    const D3D11_BUFFER_DESC IB_DESC{ .ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint32_t)),
			                                 .Usage     = D3D11_USAGE_DEFAULT,
			                                 .BindFlags = D3D11_BIND_INDEX_BUFFER };

		    ComPtr<ID3D11Buffer> indexBuffer{};

		    const D3D11_SUBRESOURCE_DATA IB_DATA{ .pSysMem = indices.data() };

		    if (FAILED(device->CreateBuffer(&IB_DESC, &IB_DATA, indexBuffer.GetAddressOf())))
		    {
			    return;
		    }

		    // ------------------------------------------------
		    // 5. 地形サーフェスを設定
		    // ------------------------------------------------

		    surface = Resource<Surface>{ SurfaceResource
			{ .pVertexBuffer = vertexBuffer,
			  .pIndexBuffer  = indexBuffer,
			  .size          = static_cast<UINT>(sizeof(Vertex)),
			  .offset        = 0,
			  .vertexCount   = static_cast<UINT>(vertices.size()),
			  .indexCount    = static_cast<UINT>(indices.size()),
			  .toplogy       = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST } 
			};
	    });
}


/// @brief 指定したワールド座標の地形の高さを取得する
/// @param _worldX ワールド座標のX
/// @param _worldZ ワールド座標のZ
/// @return 指定位置の地形の高さ
float MAP::GetHeight(
    const float _worldX,
    const float _worldZ)
{
	// 地形データが存在しない場合は基準の高さを返す
	if (heights.empty())
	{
		return MapConfig::BASE_HEIGHT;
	}

	// MapDataの高さ取得関数を利用する
	return MapData::GetHeight(_worldX, _worldZ);
}

/// @brief 指定したワールド座標の地形の法線を取得する
/// @param _worldX ワールド座標のX
/// @param _worldZ ワールド座標のZ
/// @return 地表面の法線ベクトル
Vec3 MAP::GetNormal(
    const float _worldX,
    const float _worldZ)
{
	// 地形データが存在しない場合は上向きの法線を返す
	if (heights.empty())
	{
		return Vec3::Up();
	}

	// 高さを調べる間隔
	// 小さくすると細かい傾斜を取得しやすくなる
	constexpr float SAMPLE_DISTANCE = MapConfig::TILE_SIZE;

	// 周囲4方向の地形の高さを取得する
	const float LEFT_HEIGHT = MapData::GetHeight(_worldX - SAMPLE_DISTANCE, _worldZ);

	const float RIGHT_HEIGHT = MapData::GetHeight(_worldX + SAMPLE_DISTANCE, _worldZ);

	const float BACK_HEIGHT = MapData::GetHeight(_worldX, _worldZ - SAMPLE_DISTANCE);

	const float FRONT_HEIGHT = MapData::GetHeight(_worldX, _worldZ + SAMPLE_DISTANCE);

	// 高さの差から法線ベクトルを計算する
	const Vec3 NORMAL{ LEFT_HEIGHT - RIGHT_HEIGHT, 2.0f * SAMPLE_DISTANCE, BACK_HEIGHT - FRONT_HEIGHT };

	// 法線の長さを計算する
	const float NORMAL_LENGTH = std::sqrt(NORMAL.x * NORMAL.x + NORMAL.y * NORMAL.y + NORMAL.z * NORMAL.z);

	// ゼロ除算を防止する
	constexpr float EPSILON = 0.0001f;

	if (NORMAL_LENGTH <= EPSILON)
	{
		return Vec3::Up();
	}

	// 法線の長さを1にそろえて返す
	return Vec3{ NORMAL.x / NORMAL_LENGTH, NORMAL.y / NORMAL_LENGTH, NORMAL.z / NORMAL_LENGTH };
}
