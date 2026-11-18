
#include "Model.h"
#include "../../Manager/Dx11Manager.h"
#include "../../ManagerRegistry.h"
#include "../Mathf.h"
#include "Model/ModelMaterial.h"
#include "Surface.h"
#include <functional>
#include <cmath>
#include <cstdint>
#include <cstring>

namespace
{
	/// <summary>
	/// ufbx_matrixをMat4x4に変換
	/// </summary>
	/// <param name="_from">ufbx_matrix</param>
	/// <returns>Mat4x4</returns>
	static inline cha::Mat4x4 ConvertToMat4x4(
	    const ufbx_matrix& _from)
	{
		return cha::Mat4x4{
			static_cast<float>(_from.m00), static_cast<float>(_from.m10),
			static_cast<float>(_from.m20), 0.0f,  // 1行目: X軸
			static_cast<float>(_from.m01), static_cast<float>(_from.m11),
			static_cast<float>(_from.m21), 0.0f,  // 2行目: Y軸
			static_cast<float>(_from.m02), static_cast<float>(_from.m12),
			static_cast<float>(_from.m22), 0.0f,  // 3行目: Z軸
			static_cast<float>(_from.m03), static_cast<float>(_from.m13),
			static_cast<float>(_from.m23), 1.0f,  // 4行目: 移動(Translation)
		};
	}

	/// @brief ufbx_vec4をfloat4に変換
	/// @param _from ufbx_vec4
	/// @return cha::Float4
	static inline cha::Float4 ConvertToFloat4(
	    const ufbx_vec4& _from)
	{
		return cha::Float4{ static_cast<float>(_from.x),
			                static_cast<float>(_from.y),
			                static_cast<float>(_from.z),
			                static_cast<float>(_from.w) };
	}
}  // namespace

bool cha::ModelRegistry::TryLoad(
    fs::path&& _file,
    ModelResource& _out)
{
	if (_file.extension() == ".fbx")
	{
		return TryLoadFbx(_file, _out);
	}

	VCU_ASSERT(false && "未対応のモデルファイル拡張子");
	return false;
}

bool cha::ModelRegistry::TryLoadFbx(
    const fs::path& _file,
    ModelResource& _out)
{
	ufbx_error error{};
	ufbx_load_opts options{};

	options.target_axes = ufbx_axes_left_handed_y_up;  // 左手座標系

	options.target_unit_meters = 1.0f;

	ufbx_scene* pScene{ ufbx_load_file(_file.string().c_str(), &options, &error) };
	VCU_ASSERT(pScene && "FBXシーンの読み込みに失敗");
	if (!pScene)
	{
		return false;
	}

	_out.file = _file;

	for (ufbx_node* pNode : pScene->nodes)
	{
		if (ufbx_mesh * pMesh{ pNode->mesh }; pMesh)
		{
			for (auto& part : pMesh->material_parts)
			{
				ModelParts parts{};
				parts.name = pNode->name.data;

				if (bool succeed = TryLoadParts(pNode->mesh, &part, parts); !succeed)
				{
					VCU_ASSERT(false && "パーツ読み込みに失敗");
					ufbx_free_scene(pScene);
					return false;
				}

				{  // マテリアル作成
					MaterialResource material{};

					std::unique_ptr<ModelMaterialProperty> pModelProperty = std::make_unique<ModelMaterialProperty>();
					pModelProperty->Get().toWorldMatrix                   = ConvertToMat4x4(pNode->node_to_parent);

					// TODO: もしかしたら part.index が不適正かも

					if (pNode->materials.count == 0)
					{
						continue;  // マテリアル 0 なら無視
					}

					if (part.index >= pNode->materials.count)
					{
						VCU_ASSERT(false && "マテリアルインデックスが範囲外です");
						continue;
					}
					ufbx_material* pUfbxMaterial{ pNode->materials[part.index] };

					pModelProperty->Get().hasTextureSkinAnim.x = FALSE;
					pModelProperty->Get().hasTextureSkinAnim.y = pMesh->skin_deformers.count > 0;
					pModelProperty->Get().hasTextureSkinAnim.z = pScene->anim_stacks.count > 0;

					// マテリアルのプロパティを出力する
					/*for (auto& prop : pUfbxMaterial->props.props)
					{
					    VCU_LOG("prop name :{}", prop.name.data);
					}*/

					{  // ディフューズカラー
						ufbx_prop* pProp = ufbx_find_prop(&pUfbxMaterial->props, "DiffuseColor");
						if (pProp)
						{
							pModelProperty->Get().baseColor = ConvertToFloat4(pProp->value_vec4);
						}
					}

					// テクスチャがあるなら探す
					if (pUfbxMaterial->textures.count > 0)
					{
						ufbx_material_texture textureInMaterial{ pUfbxMaterial->textures[0] };
						ufbx_texture* pTexture = textureInMaterial.texture;

						if (pTexture && pTexture->filename.length > 0)
						{
							pModelProperty->Get().hasTextureSkinAnim.x = TRUE;

							std::string_view textureFileName{ pTexture->filename.data };

							// 埋め込みテクスチャ（Embedded）データが存在するかチェック
							if (pTexture->content.size > 0 && pTexture->content.data != nullptr)
							{
								// 埋め込みデータの場合はメモリからテクスチャを作成する
								// ※必要に応じて、独自マテリアルクラスにメモリからのロード機能を追加してください
								material.texture = { TextureLoadConfig{
									pTexture->content.data,
									pTexture->content.size,
									textureFileName  // 拡張子を判別するために元のファイル名も渡す
								} };
							}
							else
							{
								// 【既存の処理】外部ファイル名からロードする
								material.texture = { textureFileName };
							}
						}
					}
					else
					{
					}

					material.TryLoad(std::move(pModelProperty));

					parts.material = Resource<ResourceType::Material>{ std::move(material) };
				}

				MaterialResource& transformMaterial = parts.transformMaterial.Get();
				transformMaterial.slot              = 3;
				ModelTransformMaterialProperty* pProperty =
				    static_cast<ModelTransformMaterialProperty*>(transformMaterial.pProperty.get());

				(*pProperty)->toWorldMatrix =
				    // cha::Mathf::RemoveScale(
				    // ConvertToMat4x4(pNode->geometry_to_world)
				    ConvertToMat4x4(pNode->geometry_to_world)
				    //)
				    ;

				_out.subParts.emplace_back(std::move(parts));
			}
		}
	}

	// NOTE: ufbxのシーンをそのままモデルで使うため、解放はモデル構造体の解放タイミングで行う
	//     : ufbx_free_scene(pScene);
	_out.pRawScene = pScene;

	// FBXのEuler回転補間による短時間の大回転を避けるため、
	// 回転カーブを細かくベイクし、実行時はクォータニオンSLERPで評価する。
	_out.bakedAnimations.resize(pScene->anim_stacks.count, nullptr);
	for (size_t i = 0; i < pScene->anim_stacks.count; ++i)
	{
		ufbx_anim_stack* animStack = pScene->anim_stacks.data[i];
		if (!animStack || !animStack->anim)
			continue;

		ufbx_bake_opts bakeOpts{};
		bakeOpts.resample_rate         = 240.0;
		bakeOpts.minimum_sample_rate   = 1000.0;  // 1ms未満の区間だけを除外
		bakeOpts.no_resample_rotation  = false;
		bakeOpts.key_reduction_enabled = false;

		ufbx_error bakeError{};
		_out.bakedAnimations[i] = ufbx_bake_anim(pScene, animStack->anim, &bakeOpts, &bakeError);
		// ベイク失敗時はnullptrのままにし、Draw()で通常評価へフォールバックする。
	}

	if (pScene->skin_deformers.count > 0)
	{
		_out.isSkinnedModel = true;
		if (!TryCreateBoneBuffer(_out.pBoneConstantBuffer))
		{
			return false;  // 作成失敗
		}
	}

	return true;
}

bool cha::ModelRegistry::TryLoadParts(
    ufbx_mesh* _pMesh,
    ufbx_mesh_part* _pPart,
    ModelParts& _out) const
{
	std::vector<ModelVertex> vertices{};
	std::vector<uint32_t> polyIndices;
	polyIndices.resize(_pMesh->max_face_triangles * 3);

	// 各マテリアルごとにポリゴンを処理
	for (uint32_t polyIndex : _pPart->face_indices)
	{
		ufbx_face poly{ _pMesh->faces[polyIndex] };

		// 面を三角化する
		uint32_t polyCount{ ufbx_triangulate_face(polyIndices.data(), polyIndices.size(), _pMesh, poly) };

		for (uint32_t i = 0; i < polyCount * 3; i++)
		{
			uint32_t index{ polyIndices[i] };

			ModelVertex v{};

			// 頂点の座標を取得
			v.position = Vec3{ _pMesh->vertex_position[index].v };

			// 頂点の法線を取得（存在しない場合はゼロベクトルを使用）
			if (_pMesh->vertex_normal.indices.count > index && _pMesh->vertex_normal.values.count > 0)
			{
				v.normal = Vec3{ ufbx_get_vertex_vec3(&_pMesh->vertex_normal, index).v };
			}
			else
			{
				v.normal = Vec3{ 0.0f, 0.0f, 0.0f };
			}

			// 頂点のUVを取得（存在しない場合はデフォルト(0,0)を使用）
			if (_pMesh->vertex_uv.indices.count > index && _pMesh->vertex_uv.values.count > 0)
			{
				v.uv   = Vec2{ ufbx_get_vertex_vec2(&_pMesh->vertex_uv, index).v };
				v.uv.y = 1.0f - v.uv.y;
			}
			else
			{
				v.uv = Vec2{ 0.0f, 0.0f };
			}

			// 頂点にスキンメッシュアニメーション用のウェイトインデックスとウェイトを割り当てる
			for (int b = 0; b < 4; b++)
			{
				v.boneIndices[b] = 0;
				v.boneWeights[b] = 0.0f;
			}

			// この頂点が属している元のメッシュ頂点インデックスを取得
			uint32_t originalVertexIndex = _pMesh->vertex_indices[index];

			// ufbxの正しいスキンデフォーマーへのアクセス
			if (_pMesh->skin_deformers.count > 0)
			{
				// TODO: 理論頂点ってなんぞや
				ufbx_skin_deformer* pSkin = _pMesh->skin_deformers.data[0];  // でフォーマーは(理論頂点?)を0として取得

				if (pSkin)
				{
					// 各頂点に対してどのweightがついているかの範囲
					ufbx_skin_vertex skinVertex = pSkin->vertices[originalVertexIndex];

					uint32_t numWeights = skinVertex.num_weights;    // 頂点が影響するウェイトの数
					uint32_t useWeights = std::min(numWeights, 4u);  // 最大4つに制限

					float weightSum = 0.0f;  // ウェイトの影響度の合計
					for (uint32_t b = 0; b < useWeights; b++)
					{
						uint32_t weightIdx = skinVertex.weight_begin + b;  // ウェイトのインデクス
						if (weightIdx >= pSkin->weights.count)
							continue;
						ufbx_skin_weight skinWeight = pSkin->weights.data[weightIdx];
						// HLSL側のBoneMatricesは256本。範囲外のクラスタは使わない。
						if (skinWeight.cluster_index >= 256 || skinWeight.cluster_index >= pSkin->clusters.count)
							continue;
						v.boneIndices[b] = skinWeight.cluster_index;
						v.boneWeights[b] = static_cast<float>(skinWeight.weight);
						weightSum += v.boneWeights[b];
					}

					// ウェイトの影響度合計を1に正規化する
					if (weightSum > 0.0f)
					{
						for (uint32_t b = 0; b < 4; b++)
						{
							v.boneWeights[b] /= weightSum;  // 正規化！
						}
					}
					else
					{
						// ウェイト0の頂点はシェーダ側でスキニングせず元の位置を使う。
						for (int b = 0; b < 4; ++b)
						{
							v.boneIndices[b] = 0;
							v.boneWeights[b] = 0.0f;
						}
					}
				}
				else  // もしスキン情報がないなら 1.0f をべた張り
				{
					v.boneWeights[0] = 1.0f;
				}
			}

			vertices.push_back(v);
		}
	}

	VCU_ASSERT(vertices.size() == _pPart->num_triangles * 3 && "メッシュの三角化に失敗");

	// 頂点ストリーム
	ufbx_vertex_stream streams[]{
		{ vertices.data(), vertices.size(), sizeof(ModelVertex) },
	};

	std::vector<uint32_t> indices{};
	indices.resize(_pPart->num_triangles * 3);

	// 頂点重複を削除し、インデックスを作成
	size_t verticesCount{ ufbx_generate_indices(streams, 1, indices.data(), indices.size(), nullptr, nullptr) };

	// 前詰めされたからリサイズする
	vertices.resize(verticesCount);

	SurfaceResource mesh{};

	if (TryCreateVertexBuffer(mesh.pVertexBuffer, vertices) && TryCreateIndexBuffer(mesh.pIndexBuffer, indices))
	{
		mesh.size        = sizeof(ModelVertex);
		mesh.vertexCount = static_cast<UINT>(vertices.size());
		mesh.indexCount  = static_cast<UINT>(indices.size());
		_out.mesh        = { std::move(mesh) };
		return true;  // 成功
	}

	return false;  // どちらかバッファ作成失敗
}

bool cha::ModelRegistry::TryCreateVertexBuffer(
    ComPtr<ID3D11Buffer>& _p,
    const std::vector<ModelVertex> _vertices) const
{
	const D3D11_BUFFER_DESC DESC{
		.ByteWidth           = static_cast<UINT>(sizeof(ModelVertex) * _vertices.size()),
		.Usage               = D3D11_USAGE_DYNAMIC,
		.BindFlags           = D3D11_BIND_VERTEX_BUFFER,
		.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE,
		.MiscFlags           = NULL,
		.StructureByteStride = 0,
	};

	D3D11_SUBRESOURCE_DATA data{
		.pSysMem = _vertices.data(),
	};

	HRESULT hResult{};
	ManagerRegistry::Ref<Dx11Manager>().Ref(
	    [&hResult, &_p, DESC, data](const ComPtr<ID3D11Device> _pDevice)
	    {
		    hResult = _pDevice->CreateBuffer(&DESC, &data, _p.GetAddressOf());
	    });
	VCU_ASSERT(SUCCEEDED(hResult) && "FbxLoadで頂点バッファの作成に失敗");
	if (FAILED(hResult))
	{
		return false;
	}

	return true;
}

bool cha::ModelRegistry::TryCreateIndexBuffer(
    ComPtr<ID3D11Buffer>& _p,
    const std::vector<uint32_t> _indices) const
{
	const D3D11_BUFFER_DESC DESC{
		.ByteWidth           = static_cast<UINT>(sizeof(uint32_t) * _indices.size()),
		.Usage               = D3D11_USAGE_DEFAULT,
		.BindFlags           = D3D11_BIND_INDEX_BUFFER,
		.CPUAccessFlags      = 0,
		.MiscFlags           = NULL,
		.StructureByteStride = 0,
	};

	D3D11_SUBRESOURCE_DATA data{
		.pSysMem = _indices.data(),
	};

	HRESULT hResult{};
	ManagerRegistry::Ref<Dx11Manager>().Ref(
	    [&hResult, &_p, DESC, data](const ComPtr<ID3D11Device> _pDevice)
	    {
		    hResult = _pDevice->CreateBuffer(&DESC, &data, _p.GetAddressOf());
	    });
	VCU_ASSERT(SUCCEEDED(hResult) && "FbxLoadでインデックスバッファの作成に失敗");
	if (FAILED(hResult))
	{
		return false;
	}

	return true;
}

bool cha::ModelRegistry::TryCreateBoneBuffer(
    ComPtr<ID3D11Buffer>& _p) const
{
	// シェーダー側の MAX_BONES (256) と完全にサイズを合わせる
	const UINT bufferSize = sizeof(Mat4x4) * 256;

	const D3D11_BUFFER_DESC DESC{
		.ByteWidth           = bufferSize,
		.Usage               = D3D11_USAGE_DYNAMIC,         // 毎フレーム書き換える
		.BindFlags           = D3D11_BIND_CONSTANT_BUFFER,  // 定数バッファとしてバインド
		.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE,      // CPUからの書き込みを許可
		.MiscFlags           = NULL,
		.StructureByteStride = 0,
	};

	HRESULT hResult{};
	ManagerRegistry::Ref<Dx11Manager>().Ref(
	    [&hResult, &_p, DESC](const ComPtr<ID3D11Device> _pDevice)
	    {
		    // 初期データは無し（nullptr）で空のバッファを作る
		    hResult = _pDevice->CreateBuffer(&DESC, nullptr, _p.GetAddressOf());
	    });

	VCU_ASSERT(SUCCEEDED(hResult) && "ボーン用定数バッファの作成に失敗");
	if (FAILED(hResult))
	{
		return false;
	}

	return true;
}

void cha::ModelResource::Draw(
    const float _deltaTime,
    const size_t _animIndex)
{
	ufbx_scene* scene = pRawScene;
	if (!scene || scene->anim_stacks.count == 0 || pBoneConstantBuffer.Get() == nullptr)
		return;

	const size_t animIndex = std::min(_animIndex, scene->anim_stacks.count - 1);
	if (playingAnimIndex != animIndex)
	{
		playingAnimIndex = animIndex;
		currentAnimTime  = 0.0;
	}

	ufbx_anim_stack* stack = scene->anim_stacks.data[playingAnimIndex];
	if (!stack || !stack->anim)
		return;

	// ufbxのstack範囲で時間をループさせる。
	// キー範囲外の値は NO_EXTRAPOLATION により最初/最後のキー値に固定する。
	const double begin    = stack->time_begin;
	const double end      = stack->time_end;
	const double duration = end - begin;
	if (!std::isfinite(begin) || !std::isfinite(end) || duration <= 0.0)
		return;

	if (std::isfinite(_deltaTime))
		currentAnimTime += static_cast<double>(_deltaTime);

	double elapsed = std::fmod(currentAnimTime, duration);
	if (elapsed < 0.0)
		elapsed += duration;

	double evalTime = begin + elapsed;
	if (evalTime >= end)
		evalTime = std::nextafter(end, begin);
	if (evalTime < begin)
		evalTime = begin;

	const ufbx_anim* anim = stack->anim;
	std::vector<DirectX::XMMATRIX> worldMatrices(scene->elements.count, DirectX::XMMatrixIdentity());
	std::vector<uint8_t> evaluated(scene->elements.count, 0);
	std::vector<uint8_t> evaluating(scene->elements.count, 0);

	// ノード配列が親子順に並んでいる保証に依存せず、親を先に評価する。
	std::function<bool(ufbx_node*)> evaluateNode = [&](ufbx_node* node) -> bool
	{
		if (!node || node->element_id >= worldMatrices.size())
			return false;
		const size_t id = node->element_id;
		if (evaluated[id])
			return true;
		if (evaluating[id])
			return false;  // 不正な循環階層を検出
		evaluating[id] = 1;

		if (node->parent && !evaluateNode(node->parent))
			return false;

		ufbx_transform localTRS =
		    ufbx_evaluate_transform_flags(anim, node, evalTime, UFBX_TRANSFORM_FLAG_NO_EXTRAPOLATION);

		/*VCU_LOG(
		    "ANIMLOG {}, time={}, quat=({}, {}, {}, {})", node->name.data,
		    evalTime,
		    static_cast<float>(localTRS.rotation.x),
		    static_cast<float>(localTRS.rotation.y),
		    static_cast<float>(localTRS.rotation.z),
		    static_cast<float>(localTRS.rotation.w));*/

		// ベイク済みチャンネルがある場合はSLERP/線形補間で評価する。
		ufbx_baked_anim* baked = animIndex < bakedAnimations.size() ? bakedAnimations[animIndex] : nullptr;
		if (baked)
		{
			ufbx_baked_node* bakedNode = ufbx_find_baked_node(baked, node);
			if (bakedNode)
			{
				if (bakedNode->translation_keys.count > 0)
					localTRS.translation = ufbx_evaluate_baked_vec3(bakedNode->translation_keys, evalTime);
				if (bakedNode->rotation_keys.count > 0)
					localTRS.rotation = ufbx_evaluate_baked_quat(bakedNode->rotation_keys, evalTime);
				if (bakedNode->scale_keys.count > 0)
					localTRS.scale = ufbx_evaluate_baked_vec3(bakedNode->scale_keys, evalTime);
			}
		}

		/*VCU_LOG(
		    "FINAL ANIMLOG {}, time={}, quat=({}, {}, {}, {})",
		    node->name.data,
		    evalTime,
		    static_cast<float>(localTRS.rotation.x),
		    static_cast<float>(localTRS.rotation.y),
		    static_cast<float>(localTRS.rotation.z),
		    static_cast<float>(localTRS.rotation.w));*/

		DirectX::XMVECTOR translation = DirectX::XMVectorSet(
		    static_cast<float>(localTRS.translation.x),
		    static_cast<float>(localTRS.translation.y),
		    static_cast<float>(localTRS.translation.z),
		    1.0f);
		DirectX::XMVECTOR rotation = DirectX::XMVectorSet(
		    static_cast<float>(localTRS.rotation.x),
		    static_cast<float>(localTRS.rotation.y),
		    static_cast<float>(localTRS.rotation.z),
		    static_cast<float>(localTRS.rotation.w));
		DirectX::XMVECTOR scale = DirectX::XMVectorSet(
		    static_cast<float>(localTRS.scale.x),
		    static_cast<float>(localTRS.scale.y),
		    static_cast<float>(localTRS.scale.z),
		    0.0f);

		// クォータニオンがゼロ/非有限の場合のNaN伝播を防ぐ。
		const float rotationLengthSq = DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(rotation));
		if (!std::isfinite(rotationLengthSq) || rotationLengthSq < 1.0e-12f)
			rotation = DirectX::XMQuaternionIdentity();
		else
			rotation = DirectX::XMQuaternionNormalize(rotation);

		const DirectX::XMMATRIX localMatrix =
		    DirectX::XMMatrixAffineTransformation(scale, DirectX::XMVectorZero(), rotation, translation);
		worldMatrices[id] = node->parent
		                        ? DirectX::XMMatrixMultiply(localMatrix, worldMatrices[node->parent->element_id])
		                        : localMatrix;

		evaluating[id] = 0;
		evaluated[id]  = 1;
		return true;
	};

	for (size_t i = 0; i < scene->nodes.count; ++i)
	{
		if (!evaluateNode(scene->nodes.data[i]))
		{
			VCU_ASSERT(false && "ノードの親子行列評価に失敗しました");
			return;
		}
	}

	if (scene->skin_deformers.count == 0)
		return;

	ufbx_skin_deformer* skin = scene->skin_deformers.data[0];
	if (!skin)
		return;

	constexpr size_t MAX_BONES = 256;
	std::vector<cha::Mat4x4> skinningMatrices(MAX_BONES, cha::Mat4x4::Identity());
	const size_t numBones = std::min(skin->clusters.count, MAX_BONES);

	for (size_t i = 0; i < numBones; ++i)
	{
		ufbx_skin_cluster* cluster = skin->clusters.data[i];
		if (!cluster)
			continue;

		// 元コードのufbx_matrix.vの並びと行列規約を維持する。
		// ここを転置/並べ替えすると逆バインド行列の解釈が変わるため変更しない。
		const ufbx_matrix& m      = cluster->geometry_to_bone;
		DirectX::XMMATRIX invBind = DirectX::XMMatrixSet(
		    static_cast<float>(m.v[0]),
		    static_cast<float>(m.v[1]),
		    static_cast<float>(m.v[2]),
		    0.0f,
		    static_cast<float>(m.v[3]),
		    static_cast<float>(m.v[4]),
		    static_cast<float>(m.v[5]),
		    0.0f,
		    static_cast<float>(m.v[6]),
		    static_cast<float>(m.v[7]),
		    static_cast<float>(m.v[8]),
		    0.0f,
		    static_cast<float>(m.v[9]),
		    static_cast<float>(m.v[10]),
		    static_cast<float>(m.v[11]),
		    1.0f);

		DirectX::XMMATRIX currentBoneWorld = DirectX::XMMatrixIdentity();
		if (cluster->bone_node)
		{
			const size_t boneId = cluster->bone_node->element_id;
			if (boneId >= worldMatrices.size() || !evaluated[boneId])
				continue;
			currentBoneWorld = worldMatrices[boneId];
		}

		// 既存の行列規約（invBind * boneWorld）を維持する。
		DirectX::XMMATRIX finalSkinMatrix = DirectX::XMMatrixMultiply(invBind, currentBoneWorld);
		finalSkinMatrix                   = DirectX::XMMatrixTranspose(finalSkinMatrix);
		DirectX::XMStoreFloat4x4(reinterpret_cast<DirectX::XMFLOAT4X4*>(&skinningMatrices[i]), finalSkinMatrix);
	}

	ManagerRegistry::Ref<Dx11Manager>().Ref(
	    [this, &skinningMatrices](const ComPtr<ID3D11DeviceContext> context)
	    {
		    if (context.Get() == nullptr || pBoneConstantBuffer.Get() == nullptr)
			    return;
		    D3D11_MAPPED_SUBRESOURCE mappedResource{};
		    const HRESULT hr = context->Map(pBoneConstantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		    if (FAILED(hr))
		    {
			    VCU_ASSERT(false && "ボーン定数バッファのMapに失敗しました");
			    return;
		    }
		    std::memcpy(mappedResource.pData, skinningMatrices.data(), sizeof(cha::Mat4x4) * MAX_BONES);
		    context->Unmap(pBoneConstantBuffer.Get(), 0);
		    context->VSSetConstantBuffers(4, 1, pBoneConstantBuffer.GetAddressOf());
	    });
}

void cha::ModelResource::UpdateAnim(
    const float _deltaTime,
    const int _animIndex)
{
	// スキンモデルの再生時間はDraw()だけが更新する。
	if (isSkinnedModel)
		return;

	playingAnimIndex = static_cast<size_t>(std::max(_animIndex, 0));

	ModelResource& _model = *this;

	ufbx_scene* pScene = _model.pRawScene;
	// アニメーションスタックがないなら無視
	if (!pScene || pScene->anim_stacks.count == 0)
	{
		return;
	}

	// 現在再生中のアニメーションを取得
	if (playingAnimIndex >= pScene->anim_stacks.count)
		return;
	ufbx_anim_stack* pAnimStack = pScene->anim_stacks[playingAnimIndex];

	// 再生時間進める
	_model.currentAnimTime += _deltaTime;

	// アニメーションの長さに応じて時間をループ処理
	float duration = static_cast<float>(pAnimStack->time_end - pAnimStack->time_begin);
	float evalTime = static_cast<float>(pAnimStack->time_begin);

	if (duration > 0.0f)
	{
		evalTime += std::fmodf(currentAnimTime, duration);
	}

	// 指定時間に置けるアニメーション状態をサンプリング
	ufbx_error error{};
	ufbx_scene* pEvaluatedScene = ufbx_evaluate_scene(pRawScene, pAnimStack->anim, evalTime, nullptr, &error);
	if (!pEvaluatedScene)
	{
		// エラー処理（必要に応じてログ出力など）
		return;
	}

	if (!isSkinnedModel)
	{
		// 評価されたシーンの各ノードをループ
		for (size_t i = 0; i < pEvaluatedScene->nodes.count; ++i)
		{
			ufbx_node* pEvalNode = pEvaluatedScene->nodes[i];
			if (!pEvalNode->mesh)
				continue;

			// ufbxのノード名と一致する subParts を探す
			// (Setup時の name の入れ方に応じて、pEvalNode->name.data または pEvalNode->mesh->name.data
			// と比較してください)
			for (auto& part : subParts)
			{
				if (part.name == pEvalNode->name.data)
				{
					// 親子関係が計算済みのワールド行列を取得
					ufbx_matrix rawWorldMat = pEvalNode->node_to_world;

					// 既存の ConvertToMat4x4 ヘルパーを使用して独自の行列型に変換
					cha::Mat4x4 localWorldMatrix = ConvertToMat4x4(rawWorldMat);

					// transformMaterial のプロパティを書き換える
					ModelTransformMaterialProperty* pProp =
					    static_cast<ModelTransformMaterialProperty*>(part.transformMaterial.Get().pProperty.get());

					pProp->Get().toWorldMatrix = localWorldMatrix;
					break;
				}
			}
		}
	}
	else
	{
		// スキンメッシュモデルの場合は、ボーン行列配列 (worldMatrices) を計算して
		// ここで定数バッファへ転送する処理を実装してください。
	}

	// 評価用の一時的なシーンを必ず解放（メモリリーク防止）
	ufbx_free_scene(pEvaluatedScene);
}

void cha::ModelResource::SetCurrentAnimTime(
    const float _time)
{
	currentAnimTime = _time;
}
