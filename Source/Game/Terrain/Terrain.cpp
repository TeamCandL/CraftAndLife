#include "pch\pch.h"
#include "Terrain.h"
#include "TerrainMaterial.h"

void Terrain::Start()
{
	//transform.position = Vec3{ -worldScale * 0.5f, 0.0f, -worldScale * 0.5f };

	widthSizeWorld = worldScale;

	// デフォルトで幅や高さが未設定だと距離計算で0除算になるため
	// transform のスケールをデフォルトのワールドサイズ・最大高さとして設定する
	if (widthSizeWorld == 0.0f)
	{
		widthSizeWorld = transform.scale.x;
	}

	if (maxHeight == 0.0f)
	{
		maxHeight = transform.scale.y;
	}

	// オフセットが未設定ならトランスフォーム位置を使う
	if (offset == Vec3{})
	{
		offset = transform.position;
	}

#pragma region マテリアル作成
		MaterialResource material{};

		std::unique_ptr<TerrainMaterialProperty> pModelProperty = std::make_unique<TerrainMaterialProperty>();
		pModelProperty->Get().hasTexture = { TRUE, TRUE, TRUE, TRUE };
		pModelProperty->Get().baseColor = { 1, 1, 1, 1 };

		material.TryLoad(std::move(pModelProperty));

		this->material = Resource<Material>{ std::move(material) };
#pragma endregion

	FillRaw(200, 0);
	Reload();
}

void Terrain::Update()
{
}

void Terrain::Draw()
{
	Dx11Manager& dx11 = Get<Dx11Manager>();

	dx11.SetMaterial(transform.GetMaterial());
	dx11.SetMaterial(material);

	dx11.SetShader(shader);
	dx11.SetSurface(surface);
	dx11.SetTexture(texture);

	dx11.Draw();
}

Vec3 Terrain::GetNormal(const float _worldX, const float _worldZ) const
{
	const float DISTANCE = GetDistance();

	Vec2 d{};
	auto [h00, h10, h01, h11] = GetTileHeights(_worldX, _worldZ, &d);

	Vec3 normal{};
	// 三角形の前後どちらにいるかで判定分け
	if (d.x + d.y <= 1.0f)
	{
		Vec3 edgeX{ DISTANCE, h10 - h00, 0.0f };  // 左上から右下へのベクトル
		Vec3 edgeZ{ 0.0f, h01 - h00, DISTANCE };  // 左上から右下へのベクトル

		normal = DirectX::XMVector3Cross(edgeX, edgeZ);  // 外積で法線を求める
	}
	else
	{
		Vec3 edgeX{ -DISTANCE, h01, 0.0f };        // 左上から右下へのベクトル
		Vec3 edgeZ{ 0.0f, h10 - h11, -DISTANCE };  // 左上から右下へのベクトル

		normal = DirectX::XMVector3Cross(edgeX, edgeZ);  // 外積で法線を求める
	}

	// 正規化して返すー！
	return DirectX::XMVector3Normalize(normal);
}

float Terrain::GetWorldPositionY(const float _worldX, const float _worldZ) const
{
	Vec2 d{};
	auto [h00, h10, h01, h11] = GetTileHeights(_worldX, _worldZ, &d);

	float finalHeight = 0.0f;
	// 三角形の前後どちらにいるかで判定分け
	if (d.x + d.y <= 1.0f)
	{
		finalHeight = h00 + (h10 - h00) * d.x + (h01 - h00) * d.y;
	}
	else
	{
		finalHeight = h11 + (h01 - h11) * (1.0f - d.x) + (h10 - h11) * (1.0f - d.y);
	}

	finalHeight = finalHeight / UINT16_MAX * maxHeight;
	finalHeight = finalHeight + offset.y;

	return finalHeight;
}

Vec3 Terrain::GetWorldOffset() const
{
	return offset;
}

void Terrain::FillRaw(const uint32_t _resolutionWidthSize, const uint8_t _height)
{
	terrainMapResolution = _resolutionWidthSize;

	rawData.clear();  // 一度きれいに消してから
	rawData.resize((_resolutionWidthSize + 1) * (_resolutionWidthSize + 1), _height);  // リサイズ
}

TerrainData::TileHeights Terrain::GetTileHeights(const float _worldX, const float _worldZ, Vec2* _pOutD) const
{
	// 頂点間の距離
	const float DISTANCE = GetDistance();

	// ローカルのインデクス
	Vec2 localIndex{ Vec2{ _worldX - offset.x, _worldZ - offset.z } / DISTANCE };

	// 範囲外はクランプする
	localIndex.x = std::clamp(localIndex.x, 0.0f, static_cast<float>(terrainMapResolution));
	localIndex.y = std::clamp(localIndex.y, 0.0f, static_cast<float>(terrainMapResolution));

	// 左上の格子点インデクスを取得する
	Vec2Int gridIndex
	{
		static_cast<int32_t>(std::floor(localIndex.x)),
		static_cast<int32_t>(std::floor(localIndex.y))
	};

	// 小数点以下の細かい位置
	Vec2 d = localIndex - Vec2{ static_cast<float>(gridIndex.x), static_cast<float>(gridIndex.y) };

	if (_pOutD)
	{
		*_pOutD = d;  // dが要求されているなら出力用にコピー
	}

	// 実際のワールドY座標へのレート
	const float ratio = static_cast<float>(maxHeight) / static_cast<float>(UINT8_MAX);
	return TileHeights
	{
		.h00 = static_cast<float>(RawDataAt(gridIndex.x, gridIndex.y)) * ratio,
		.h10 = static_cast<float>(RawDataAt(gridIndex.x, gridIndex.y + 1)) * ratio,
		.h01 = static_cast<float>(RawDataAt(gridIndex.x + 1, gridIndex.y)) * ratio,
		.h11 = static_cast<float>(RawDataAt(gridIndex.x + 1, gridIndex.y + 1)) * ratio,
	};
}

const uint8_t& Terrain::RawDataAt(const uint32_t _indexX, const uint32_t _indexY) const
{
	const size_t INDEX{ static_cast<size_t>(_indexX + _indexY * GetTerrainMapVerticesWidth()) };
	return rawData.at(INDEX);
}

uint8_t& Terrain::RawDataAt(const uint32_t _indexX, const uint32_t _indexY)
{
	const size_t INDEX{ static_cast<size_t>(_indexX + _indexY * GetTerrainMapVerticesWidth()) };
	return rawData[INDEX];
}

float Terrain::GetDistance() const
{
	return widthSizeWorld / static_cast<float>(terrainMapResolution);
}

void Terrain::Reload()
{
	Get<Dx11Manager>().Ref([this](const ComPtr<ID3D11Device>& _pDevice)
	{
		using Vertex = ModelVertex;

		const uint32_t n = GetTerrainMapVerticesWidth();

#pragma region 頂点作成
		std::vector<Vertex> vertices(n * n);

		const float DISTANCE = GetDistance();

		for (uint32_t z = 0; z < n; z++)
		{
			for (uint32_t x = 0; x < n; x++)
			{
				uint32_t index = x + z * n;

				Vec3 position{};

				position.x = x * DISTANCE + offset.x;
				position.z = z * DISTANCE + offset.z;

				position.y = RawDataAt(x, z) * (maxHeight / UINT8_MAX);

				vertices[index].position = position;
				vertices[index].uv = Vec2{ x * DISTANCE, z * DISTANCE };
				vertices[index].normal = Vec3::Up();  // 後で計算する
			}
		}
#pragma endregion

#pragma region インデクスの作成
		// 1タイルあたり三角形2つのため、6index
		std::vector<uint32_t> indices((n - 1) * (n - 1) * 6);

		int32_t indicesCount = 0;
		for (uint32_t z = 0; z < n - 1; z++)
		{
			for (uint32_t x = 0; x < n - 1; x++)
			{
				uint32_t topLeft = z * n + x;
				uint32_t topRight = z * n + (x + 1);
				uint32_t bottomLeft = (z + 1) * n + x;
				uint32_t bottomRight = (z + 1) * n + (x + 1);

				// 左上, 右上, 左下
				indices[indicesCount++] = topLeft;
				indices[indicesCount++] = topRight;
				indices[indicesCount++] = bottomLeft;

				// 右上, 右下, 左下
				indices[indicesCount++] = topRight;
				indices[indicesCount++] = bottomRight;
				indices[indicesCount++] = bottomLeft;
			}
		}
#pragma endregion

#pragma region 頂点の法線を整備
		// とりあえず全ての法線を追加していく
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			uint32_t i0 = indices[i];
			uint32_t i1 = indices[i + 1];
			uint32_t i2 = indices[i + 2];

			Vec3 v0 = vertices[i0].position.v;
			Vec3 v1 = vertices[i1].position.v;
			Vec3 v2 = vertices[i2].position.v;

			Vec3 edge1 = v1 - v0;
			Vec3 edge2 = v2 - v0;

			// 外積で法線を求める
			Vec3 faceNormal = DirectX::XMVector3Cross(edge1, edge2);

			// 求めた法線を蓄積する
			vertices[i0].normal = Vec3{ vertices[i0].normal.v } + faceNormal;
			vertices[i1].normal = Vec3{ vertices[i1].normal.v } + faceNormal;
			vertices[i2].normal = Vec3{ vertices[i2].normal.v } + faceNormal;
		}

		// 蓄積した分を正規化していく
		for (size_t i = 0; i < vertices.size(); i++)
		{
			Vec3 normal{ vertices[i].normal.v };
			normal = DirectX::XMVector3Normalize(normal);
			vertices[i].normal = normal;
		}
#pragma endregion

#pragma region 頂点バッファの作成
		const D3D11_BUFFER_DESC VERTEX_BUFFER_DESC
		{
			.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(Vertex)),
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_VERTEX_BUFFER,
			.CPUAccessFlags = NULL,
			.MiscFlags = NULL,
			.StructureByteStride = 0,
		};

		ComPtr<ID3D11Buffer> pVertexBuffer{};

		const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA_VERTEX
		{
			.pSysMem = vertices.data(),
			.SysMemPitch = static_cast<UINT>(vertices.size() * sizeof(Vertex)),
			.SysMemSlicePitch = 0,
		};

		_pDevice->CreateBuffer(&VERTEX_BUFFER_DESC, &INITIALIZE_DATA_VERTEX, pVertexBuffer.GetAddressOf());
#pragma endregion

#pragma region インデクスバッファの作成
		const D3D11_BUFFER_DESC INDEX_BUFFER_DESC
		{
			.ByteWidth = static_cast<UINT>(indices.size() * sizeof(uint32_t)),
			.Usage = D3D11_USAGE_DEFAULT,
			.BindFlags = D3D11_BIND_INDEX_BUFFER,
			.CPUAccessFlags = NULL,
			.MiscFlags = NULL,
			.StructureByteStride = 0,
		};

		ComPtr<ID3D11Buffer> pIndexBuffer{};

		const D3D11_SUBRESOURCE_DATA INITIALIZE_DATA_INDEX
		{
			.pSysMem = indices.data(),
			.SysMemPitch = static_cast<UINT>(indices.size() * sizeof(uint32_t)),
			.SysMemSlicePitch = 0,
		};

		_pDevice->CreateBuffer(&INDEX_BUFFER_DESC, &INITIALIZE_DATA_INDEX, pIndexBuffer.GetAddressOf());
#pragma endregion

#pragma region サーフェイスとして作成
		SurfaceResource surface
		{
			.pVertexBuffer = pVertexBuffer,
			.pIndexBuffer = pIndexBuffer,
			.size = static_cast<UINT>(sizeof(Vertex)),
			.offset = 0,
			.vertexCount = static_cast<UINT>(vertices.size()),
			.indexCount = static_cast<UINT>(indices.size()),
			.toplogy = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
		};

		this->surface = { std::move(surface) };
#pragma endregion
	});
}
