#include "ModelFbx.h"
#include "../../Manager/Dx11Manager.h"
#include "../../ManagerRegistry.h"
#include "../Mathf.h"
#include "Model/ModelMaterial.h"
#include "Surface.h"
#include <functional>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <vector>

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

	// 回転行列部分を直交化する。
	// 平行移動成分は変更しない。
	static void NormalizeRotationMatrix(
	    ufbx_matrix& m)
	{
		// X軸
		double x0 = m.m00;
		double x1 = m.m01;
		double x2 = m.m02;

		// Y軸
		double y0 = m.m10;
		double y1 = m.m11;
		double y2 = m.m12;

		// X軸を正規化
		double xLength = std::sqrt(x0 * x0 + x1 * x1 + x2 * x2);

		if (xLength < 1.0e-12)
			return;

		x0 /= xLength;
		x1 /= xLength;
		x2 /= xLength;

		// Y軸からX軸方向の成分を除去
		const double dot = x0 * y0 + x1 * y1 + x2 * y2;

		y0 -= dot * x0;
		y1 -= dot * x1;
		y2 -= dot * x2;

		double yLength = std::sqrt(y0 * y0 + y1 * y1 + y2 * y2);

		if (yLength < 1.0e-12)
			return;

		y0 /= yLength;
		y1 /= yLength;
		y2 /= yLength;

		// Z軸を外積で再生成
		const double z0 = x1 * y2 - x2 * y1;
		const double z1 = x2 * y0 - x0 * y2;
		const double z2 = x0 * y1 - x1 * y0;

		m.m00 = x0;
		m.m01 = x1;
		m.m02 = x2;

		m.m10 = y0;
		m.m11 = y1;
		m.m12 = y2;

		m.m20 = z0;
		m.m21 = z1;
		m.m22 = z2;
	}

	// Quaternionの符号を統一する
	static inline void NormalizeQuaternion(
	    ufbx_quat& q)
	{
		const double lengthSq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;

		if (lengthSq <= 1.0e-12)
		{
			q = ufbx_identity_quat;
			return;
		}

		const double invLength = 1.0 / std::sqrt(lengthSq);

		q.x *= invLength;
		q.y *= invLength;
		q.z *= invLength;
		q.w *= invLength;

		// 同じ回転を表すQuaternionの符号を統一
		if (q.w < 0.0)
		{
			q.x = -q.x;
			q.y = -q.y;
			q.z = -q.z;
			q.w = -q.w;
		}
	}

	/// @brief ufbx_vec4をfloat4に変換
	/// @param _from ufbx_vec4
	/// @return cha::Float4
	/// <summary>
	/// ufbxのワールド行列をDirectXMathの行ベクトル形式へ変換する。
	/// ConvertToMat4x4()と同じ転置規約を使用する。
	/// </summary>
	static inline DirectX::XMMATRIX ConvertToXMMATRIX(
	    const ufbx_matrix& _from)
	{
		return DirectX::XMMatrixSet(
		    static_cast<float>(_from.m00),
		    static_cast<float>(_from.m10),
		    static_cast<float>(_from.m20),
		    0.0f,
		    static_cast<float>(_from.m01),
		    static_cast<float>(_from.m11),
		    static_cast<float>(_from.m21),
		    0.0f,
		    static_cast<float>(_from.m02),
		    static_cast<float>(_from.m12),
		    static_cast<float>(_from.m22),
		    0.0f,
		    static_cast<float>(_from.m03),
		    static_cast<float>(_from.m13),
		    static_cast<float>(_from.m23),
		    1.0f);
	}

	static inline cha::Float4 ConvertToFloat4(
	    const ufbx_vec4& _from)
	{
		return cha::Float4{ static_cast<float>(_from.x),
			                static_cast<float>(_from.y),
			                static_cast<float>(_from.z),
			                static_cast<float>(_from.w) };
	}
	// ------------------------------------------------------------
	// CPUアニメーション評価用ヘルパー
	// ------------------------------------------------------------

	/// @brief ベイク結果からノードごとのトラックを作る
	/// 回転は実キーフレーム(UFBX_BAKED_KEY_KEYFRAME)だけを残し、
	/// 隣り合うキーのクォータニオンが同じ半球になるよう符号を揃える。
	static void BuildNodeTracks(
	    const ufbx_scene* _pScene,
	    const ufbx_baked_anim* _pBaked,
	    std::vector<cha::ModelNodeTrack>& _out)
	{
		_out.clear();
		_out.resize(_pScene->nodes.count);
		if (!_pBaked)
			return;

		for (const ufbx_baked_node& bn : _pBaked->nodes)
		{
			if (bn.typed_id >= _out.size())
				continue;
			cha::ModelNodeTrack& track = _out[bn.typed_id];

			track.translation.assign(bn.translation_keys.data, bn.translation_keys.data + bn.translation_keys.count);
			track.scale.assign(bn.scale_keys.data, bn.scale_keys.data + bn.scale_keys.count);

			// 実キーフレームだけを抽出
			for (const ufbx_baked_quat& key : bn.rotation_keys)
			{
				if (key.flags & UFBX_BAKED_KEY_KEYFRAME)
					track.rotation.push_back(key);
			}
			// 実キーが1つも無い場合 (定数回転など) は全キーを使う
			if (track.rotation.empty())
				track.rotation.assign(bn.rotation_keys.data, bn.rotation_keys.data + bn.rotation_keys.count);

			// 符号を揃える (q と -q は同じ回転。最短経路で補間するため)
			for (size_t k = 1; k < track.rotation.size(); ++k)
			{
				const ufbx_quat& a = track.rotation[k - 1].value;
				ufbx_quat& b       = track.rotation[k].value;
				if (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w < 0.0)
				{
					b.x = -b.x;
					b.y = -b.y;
					b.z = -b.z;
					b.w = -b.w;
				}
			}
		}
	}

	/// @brief 回転キーを時間で評価 (クォータニオンSLERP)
	static ufbx_quat EvaluateRotation(
	    const std::vector<ufbx_baked_quat>& _keys,
	    const double _time)
	{
		if (_keys.size() == 1 || _time <= _keys.front().time)
			return _keys.front().value;
		if (_time >= _keys.back().time)
			return _keys.back().value;

		// _time を超える最初のキー
		auto it = std::upper_bound(
		    _keys.begin(),
		    _keys.end(),
		    _time,
		    [](double t, const ufbx_baked_quat& k)
		    {
			    return t < k.time;
		    });

		const ufbx_baked_quat& next = *it;
		const ufbx_baked_quat& prev = *(it - 1);

		const double span = next.time - prev.time;
		if (span <= 0.0)
			return next.value;

		const double t = (_time - prev.time) / span;
		return ufbx_quat_slerp(prev.value, next.value, static_cast<ufbx_real>(t));
	}

	static ufbx_vec3 EvaluateVec3(
	    const std::vector<ufbx_baked_vec3>& _keys,
	    const double _time)
	{
		ufbx_baked_vec3_list list{};
		list.data  = const_cast<ufbx_baked_vec3*>(_keys.data());
		list.count = _keys.size();
		return ufbx_evaluate_baked_vec3(list, _time);
	}

	/// @brief ufbx_matrix(アフィン3x4)で点を変換
	static inline void TransformPoint(
	    const ufbx_matrix& m,
	    const float* p,
	    float* out)
	{
		out[0] = static_cast<float>(m.m00 * p[0] + m.m01 * p[1] + m.m02 * p[2] + m.m03);
		out[1] = static_cast<float>(m.m10 * p[0] + m.m11 * p[1] + m.m12 * p[2] + m.m13);
		out[2] = static_cast<float>(m.m20 * p[0] + m.m21 * p[1] + m.m22 * p[2] + m.m23);
	}

	/// @brief ufbx_matrix(アフィン3x4)で方向ベクトルを変換 (平行移動なし)
	static inline void TransformDirection(
	    const ufbx_matrix& m,
	    const float* p,
	    float* out)
	{
		out[0] = static_cast<float>(m.m00 * p[0] + m.m01 * p[1] + m.m02 * p[2]);
		out[1] = static_cast<float>(m.m10 * p[0] + m.m11 * p[1] + m.m12 * p[2]);
		out[2] = static_cast<float>(m.m20 * p[0] + m.m21 * p[1] + m.m22 * p[2]);
	}
}  // namespace

bool cha::ModelFbxRegistry::TryLoad(
    fs::path&& _file,
    ModelFbxResource& _out)
{
	if (_file.extension() == ".fbx")
	{
		return TryLoadFbx(_file, _out);
	}

	VCU_ASSERT(false && "未対応のモデルファイル拡張子");
	return false;
}

bool cha::ModelFbxRegistry::TryLoadFbx(
    const fs::path& _file,
    ModelFbxResource& _out)
{
	ufbx_error error{};
	ufbx_load_opts options{};

	options.target_axes                = ufbx_axes_right_handed_y_up;  // 左手座標系
	options.target_unit_meters         = 1.0f;
	options.handedness_conversion_axis = UFBX_MIRROR_AXIS_Z;
	options.space_conversion           = UFBX_SPACE_CONVERSION_ADJUST_TRANSFORMS;
	options.generate_missing_normals   = true;

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

				// スキンメッシュならCPUスキニング用の情報も受け取る
				ModelSkinPart skinPart{};
				const bool isSkinPart = pMesh->skin_deformers.count > 0 && pMesh->skin_deformers.data[0];

				if (bool succeed = TryLoadParts(pNode->mesh, &part, parts, isSkinPart ? &skinPart : nullptr); !succeed)
				{
					VCU_ASSERT(false && "パーツ読み込みに失敗");
					ufbx_free_scene(pScene);
					return false;
				}

				if (isSkinPart && skinPart.pVertexBuffer)
				{
					skinPart.pSkin           = pMesh->skin_deformers.data[0];
					skinPart.skinnedVertices = skinPart.bindVertices;
					_out.skinParts.emplace_back(std::move(skinPart));
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

	// FBXの回転はオイラー角でキー間を補間するため、359° -> 1° のようなキー間で
	// 逆回りに1回転してしまう。ufbxのリサンプル結果もこの経路をなぞるので、
	// 回転は「実キーフレームのみ」(no_resample_rotation) を取り出し、
	// 実行時にクォータニオンSLERP(最短経路)で補間する。
	_out.bakedAnimations.resize(pScene->anim_stacks.count, nullptr);
	_out.nodeTracks.resize(pScene->anim_stacks.count);
	for (size_t i = 0; i < pScene->anim_stacks.count; ++i)
	{
		ufbx_anim_stack* animStack = pScene->anim_stacks.data[i];
		if (!animStack || !animStack->anim)
			continue;

		ufbx_bake_opts bakeOpts{};
		bakeOpts.resample_rate         = 60.0;  // 平行移動/スケールの非線形区間用
		bakeOpts.minimum_sample_rate   = 19.5;  // 20Hz以上のキーは再サンプルしない
		bakeOpts.no_resample_rotation  = true;  // 回転を再サンプルしない (重要)
		bakeOpts.key_reduction_enabled = false;

		ufbx_error bakeError{};
		ufbx_baked_anim* pBaked = ufbx_bake_anim(pScene, animStack->anim, &bakeOpts, &bakeError);

		// ベイク失敗時はトラックが空のまま -> Draw()でufbx_evaluate_transformにフォールバックする。
		if (pBaked)
		{
			BuildNodeTracks(pScene, pBaked, _out.nodeTracks[i]);
			ufbx_free_baked_anim(pBaked);  // 必要な分はトラックにコピー済み
		}
	}

	if (pScene->skin_deformers.count > 0)
	{
		// ボーン行列はCPUで頂点に適用するため、定数バッファは作らない。
		_out.isSkinnedModel = true;
	}

	return true;
}

bool cha::ModelFbxRegistry::TryLoadParts(
    ufbx_mesh* _pMesh,
    ufbx_mesh_part* _pPart,
    ModelParts& _out,
    ModelSkinPart* _pSkinPart) const
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

		// CPUスキニング用にバインド姿勢の頂点と頂点バッファを保持する
		if (_pSkinPart)
		{
			_pSkinPart->bindVertices  = vertices;
			_pSkinPart->pVertexBuffer = mesh.pVertexBuffer;
		}

		_out.mesh = { std::move(mesh) };
		return true;  // 成功
	}

	return false;  // どちらかバッファ作成失敗
}

bool cha::ModelFbxRegistry::TryCreateVertexBuffer(
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

bool cha::ModelFbxRegistry::TryCreateIndexBuffer(
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

bool cha::ModelFbxRegistry::TryCreateBoneBuffer(
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

void cha::ModelFbxResource::Draw(
    const float _deltaTime,
    const size_t _animIndex)
{
	ufbx_scene* scene = pRawScene;

	// スキンモデル以外は何もしない
	if (!scene || !isSkinnedModel || skinParts.empty())
		return;

	if (scene->anim_stacks.count == 0)
		return;

	// アニメーション番号の決定
	const size_t animIndex = std::min(_animIndex, scene->anim_stacks.count - 1);

	if (playingAnimIndex != animIndex)
	{
		playingAnimIndex = animIndex;
		currentAnimTime  = 0.0;
	}

	const ufbx_anim_stack* stack = scene->anim_stacks.data[playingAnimIndex];

	if (!stack || !stack->anim)
		return;

	// アニメーション時間の計算
	const double begin    = stack->time_begin;
	const double end      = stack->time_end;
	const double duration = end - begin;

	if (!std::isfinite(begin) || !std::isfinite(end) || !std::isfinite(duration) || duration <= 0.0)
	{
		return;
	}

	if (std::isfinite(_deltaTime))
	{
		currentAnimTime += static_cast<double>(_deltaTime);
	}

	// 時間が異常値になった場合の対策
	if (!std::isfinite(currentAnimTime))
	{
		currentAnimTime = 0.0;
	}

	// アニメーション時間をループさせる
	double elapsed = std::fmod(currentAnimTime, duration);

	if (elapsed < 0.0)
	{
		elapsed += duration;
	}

	currentAnimTime = elapsed;

	double evalTime = begin + elapsed;

	// 終端を超えないようにする
	if (evalTime >= end)
	{
		evalTime = std::nextafter(end, begin);
	}

	if (evalTime < begin)
	{
		evalTime = begin;
	}

	// ------------------------------------------------------------
	// 1. 全ノードのローカル変換を評価し、ワールド行列を作る (CPU)
	//    回転はクォータニオンSLERPなので、オイラー角の逆回り補間は起きない。
	// ------------------------------------------------------------

	const size_t nodeCount = scene->nodes.count;

	const std::vector<ModelNodeTrack>* pTracks = nullptr;
	if (playingAnimIndex < nodeTracks.size() && nodeTracks[playingAnimIndex].size() == nodeCount)
	{
		pTracks = &nodeTracks[playingAnimIndex];
	}

	std::vector<ufbx_matrix> worldMatrices(nodeCount);
	std::vector<uint8_t> worldDone(nodeCount, 0);

	// 親を先に計算する (ノード順に依存しないよう親チェーンを辿る)
	auto computeWorld = [&](auto&& self, const size_t _id) -> void
	{
		if (worldDone[_id])
			return;

		const ufbx_node* node = scene->nodes.data[_id];

		ufbx_transform transform = node->local_transform;  // ベースはバインド(静止)姿勢

		if (pTracks)
		{
			const ModelNodeTrack& track = (*pTracks)[_id];
			if (!track.translation.empty())
				transform.translation = EvaluateVec3(track.translation, evalTime);
			if (!track.rotation.empty())
				transform.rotation = EvaluateRotation(track.rotation, evalTime);
			if (!track.scale.empty())
				transform.scale = EvaluateVec3(track.scale, evalTime);
		}
		else
		{
			// ベイクに失敗した場合のフォールバック (従来の評価)
			transform = ufbx_evaluate_transform(stack->anim, node, evalTime);
		}

		const ufbx_matrix local = ufbx_transform_to_matrix(&transform);

		if (node->parent)
		{
			const size_t parentId = node->parent->typed_id;
			self(self, parentId);
			worldMatrices[_id] = ufbx_matrix_mul(&worldMatrices[parentId], &local);
		}
		else
		{
			worldMatrices[_id] = local;
		}
		worldDone[_id] = 1;
	};

	for (size_t n = 0; n < nodeCount; ++n)
	{
		computeWorld(computeWorld, n);
	}

	// ------------------------------------------------------------
	// 2. スキンデフォーマーごとのスキニング行列 (geometry -> world)
	// ------------------------------------------------------------

	std::vector<std::vector<ufbx_matrix>> skinMatrices(scene->skin_deformers.count);

	auto getSkinMatrices = [&](const ufbx_skin_deformer* _pSkin) -> const std::vector<ufbx_matrix>&
	{
		std::vector<ufbx_matrix>& mats = skinMatrices[_pSkin->typed_id];
		if (mats.empty() && _pSkin->clusters.count > 0)
		{
			mats.resize(_pSkin->clusters.count, ufbx_identity_matrix);
			for (size_t i = 0; i < _pSkin->clusters.count; ++i)
			{
				const ufbx_skin_cluster* cluster = _pSkin->clusters.data[i];
				if (!cluster || !cluster->bone_node)
					continue;

				// geometry_to_bone : ジオメトリ空間 -> ボーン空間 (逆バインド行列)
				// worldMatrices    : ボーン空間 -> ワールド空間 (アニメーション適用後)
				mats[i] = ufbx_matrix_mul(&worldMatrices[cluster->bone_node->typed_id], &cluster->geometry_to_bone);
			}
		}
		return mats;
	};

	// ------------------------------------------------------------
	// 3. CPUで頂点をスキニング
	// ------------------------------------------------------------

	for (ModelSkinPart& part : skinParts)
	{
		if (!part.pSkin || part.pSkin->typed_id >= skinMatrices.size())
			continue;

		const std::vector<ufbx_matrix>& mats = getSkinMatrices(part.pSkin);

		const size_t vertexCount = part.bindVertices.size();
		part.skinnedVertices.resize(vertexCount);

		for (size_t v = 0; v < vertexCount; ++v)
		{
			const ModelVertex& src = part.bindVertices[v];
			ModelVertex& dst       = part.skinnedVertices[v];

			dst = src;  // UV・ボーン情報などはそのままコピー

			float weightSum = 0.0f;
			for (int b = 0; b < 4; ++b)
			{
				weightSum += std::max(src.boneWeights[b], 0.0f);
			}

			// ウェイトの無い頂点はスキニングせず、バインド姿勢のまま使う
			if (weightSum <= 1.0e-6f)
				continue;

			const float srcPos[3]{ src.position.x, src.position.y, src.position.z };
			const float srcNrm[3]{ src.normal.x, src.normal.y, src.normal.z };

			float pos[3]{ 0.0f, 0.0f, 0.0f };
			float nrm[3]{ 0.0f, 0.0f, 0.0f };

			for (int b = 0; b < 4; ++b)
			{
				const float w = std::max(src.boneWeights[b], 0.0f) / weightSum;
				if (w <= 0.0f)
					continue;

				const size_t boneIndex = static_cast<size_t>(src.boneIndices[b]);
				if (boneIndex >= mats.size())
					continue;

				float p[3]{};
				float n[3]{};
				TransformPoint(mats[boneIndex], srcPos, p);
				TransformDirection(mats[boneIndex], srcNrm, n);

				for (int k = 0; k < 3; ++k)
				{
					pos[k] += p[k] * w;
					nrm[k] += n[k] * w;
				}
			}

			dst.position.x = pos[0];
			dst.position.y = pos[1];
			dst.position.z = pos[2];
			dst.normal.x   = nrm[0];
			dst.normal.y   = nrm[1];
			dst.normal.z   = nrm[2];
		}
	}

	// ------------------------------------------------------------
	// 4. スキニング後の頂点をGPUの動的頂点バッファへ転送
	// ------------------------------------------------------------

	ManagerRegistry::Ref<Dx11Manager>().Ref(
	    [this](const ComPtr<ID3D11DeviceContext> context)
	    {
		    if (context.Get() == nullptr)
		    {
			    return;
		    }

		    for (ModelSkinPart& part : skinParts)
		    {
			    if (!part.pVertexBuffer || part.skinnedVertices.empty())
				    continue;

			    D3D11_MAPPED_SUBRESOURCE mappedResource{};

			    const HRESULT hr =
			        context->Map(part.pVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

			    if (FAILED(hr))
			    {
				    VCU_ASSERT(false && "頂点バッファのMapに失敗しました");
				    continue;
			    }

			    std::memcpy(
			        mappedResource.pData,
			        part.skinnedVertices.data(),
			        sizeof(ModelVertex) * part.skinnedVertices.size());

			    context->Unmap(part.pVertexBuffer.Get(), 0);
		    }
	    });
}

void cha::ModelFbxResource::UpdateAnim(
    const float _deltaTime,
    const int _animIndex)
{
	// スキンモデルの再生時間はDraw()だけが更新する。
	if (isSkinnedModel)
		return;

	playingAnimIndex = static_cast<size_t>(std::max(_animIndex, 0));

	ModelFbxResource& _model = *this;

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
		evalTime += std::fmodf(static_cast<float>(currentAnimTime), duration);
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

void cha::ModelFbxResource::SetCurrentAnimTime(
    const float _time)
{
	currentAnimTime = _time;
}