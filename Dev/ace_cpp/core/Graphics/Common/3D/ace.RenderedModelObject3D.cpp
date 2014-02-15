﻿
#include "ace.RenderedModelObject3D.h"
#include "ace.Mesh_Imp.h"
#include "ace.Deformer_Imp.h"
#include "ace.Model_Imp.h"

#include "../Animation/ace.AnimationClip_Imp.h"
#include "../Animation/ace.AnimationSource_Imp.h"
#include "../Animation/ace.KeyframeAnimation_Imp.h"

#include "../ace.Graphics_Imp.h"
#include "../Resource/ace.NativeShader_Imp.h"
#include "../Resource/ace.RenderState_Imp.h"
#include "../Resource/ace.IndexBuffer_Imp.h"

namespace ace
{
	static const char* dx_vs = R"(

float4x4	matMCP[32]					: register( c0 );
float3		directionalLightDirection	: register( c128 );
float3		directionalLightColor		: register( c129 );

struct VS_Input
{
	float3 Position		: Position0;
	float3 Normal		: Normal0;
	float3 Binormal		: Binormal0;
	float2 UV			: UV0;
	float2 UVSub		: UVSub0;
	float4 Color		: Color0;
	float4 BoneWeights	: BoneWeights0;
	uint4 BoneIndexes	: BoneIndexes0;
	uint4 BoneIndexesOriginal	: BoneIndexesOriginal0;
};

struct VS_Output
{
	float4 Pos		: SV_POSITION;
	float4 Color	: Color0;
	float2 UV		: TEXCOORD0;
};

float4x4 calcMatrix(float4 weights, uint4 indexes)
{
	return matMCP[indexes.x] * weights.x +
	matMCP[indexes.y] * weights.y +
	matMCP[indexes.z] * weights.z +
	matMCP[indexes.w] * weights.w;
}

VS_Output main( const VS_Input Input )
{
	VS_Output Output = (VS_Output)0;
	//Output.Pos = mul( matMCP[0], float4( Input.Position.x, Input.Position.y, Input.Position.z, 1.0 ) );
	Output.Pos = mul( calcMatrix(Input.BoneWeights,Input.BoneIndexes), float4( Input.Position.x, Input.Position.y, Input.Position.z, 1.0 ) );
	
	Output.UV = Input.UV;
	Output.Color.xyz = directionalLightColor * max( dot(directionalLightDirection,Input.Normal), 0.0 );
	Output.Color.w = 1.0;
	return Output;
}

)";

	static const char* dx_ps = R"(

struct PS_Input
{
	float4 Position	: SV_POSITION;
	float4 Color	: Color0;
	float2 UV		: TEXCOORD0;
};

float3		hasTextures	: register( c0 );

Texture2D		g_colorTexture		: register( t0 );
SamplerState	g_colorSampler		: register( s0 );

Texture2D		g_normalTexture		: register( t1 );
SamplerState	g_normalSampler		: register( s1 );

Texture2D		g_specularTexture		: register( t2 );
SamplerState	g_specularSampler		: register( s2 );

float4 main( const PS_Input Input ) : SV_Target
{
	float4 Output = Input.Color;
	if(Output.a == 0.0f) discard;

	if(hasTextures.x != 0.0)
	{
		Output = Output * g_colorTexture.Sample(g_colorSampler, Input.UV);
	}

	return Output;
}

)";

	static const char* gl_vs = R"(

attribute vec3 Position;
attribute vec3 Normal;
attribute vec3 Binormal;
attribute vec2 UV;
attribute vec2 UVSub;
attribute vec4 Color;
attribute vec4 BoneWeights;
attribute vec4 BoneIndexes;
attribute vec4 BoneIndexesOriginal;

uniform mat4		matMCP[32];
uniform vec3		directionalLightDirection;
uniform vec3		directionalLightColor;

varying vec4 vaTexCoord;
varying vec4 vaColor;

mat4 calcMatrix(vec4 weights, vec4 indexes)
{
	return matMCP[int(indexes.x)] * weights.x +
	matMCP[int(indexes.y)] * weights.y +
	matMCP[int(indexes.z)] * weights.z +
	matMCP[int(indexes.w)] * weights.w;
}

void main()
{
//	gl_Position = matMCP[0] * vec4(Position.x,Position.y,Position.z,1.0);
	gl_Position = calcMatrix(BoneWeights,BoneIndexes) * vec4(Position.x,Position.y,Position.z,1.0);
	vaTexCoord = vec4(UV.x,UV.y,0.0,0.0);
	vaColor.xyz = directionalLightColor * max( dot(directionalLightDirection,Normal), 0.0 );
	vaColor.w = 1.0;
}

)";

	static const char* gl_ps = R"(

varying vec4 vaTexCoord;
varying vec4 vaColor;

uniform vec3 hasTextures;

uniform sampler2D g_colorTexture;
uniform sampler2D g_normalTexture;
uniform sampler2D g_specularTexture;

void main() 
{
	gl_FragColor = vaColor;

	// gl only
	vec4 vaTexCoord_ = vaTexCoord;
	vaTexCoord_.y = 1.0 - vaTexCoord_.y;

	if(hasTextures.x != 0.0)
	{
		gl_FragColor = gl_FragColor * texture2D(g_colorTexture, vaTexCoord_.xy);
	}
}

)";


	RenderedModelObject3D::BoneProperty::BoneProperty()
	{
		Position[0] = 0.0f;
		Position[1] = 0.0f;
		Position[2] = 0.0f;

		Rotation[0] = 0.0f;
		Rotation[1] = 0.0f;
		Rotation[2] = 0.0f;
		Rotation[3] = 0.0f;

		Scale[0] = 1.0f;
		Scale[1] = 1.0f;
		Scale[2] = 1.0f;
	}


	Matrix44 RenderedModelObject3D::BoneProperty::CalcMatrix(eRotationOrder rotationType)
	{
		return ModelUtils::CalcMatrix(
			Position,
			Rotation,
			Scale,
			rotationType);
	}

	RenderedModelObject3D::MeshGroup::MeshGroup(Mesh_Imp* mesh)
		: m_mesh(nullptr)
		, m_deformer(nullptr)
	{
		SafeAddRef(mesh);
		m_mesh = mesh;
		CheckDeformer();
	}

	RenderedModelObject3D::MeshGroup::~MeshGroup()
	{
		SetInternalDeformer(nullptr);
		SafeRelease(m_mesh);
	}

	void RenderedModelObject3D::MeshGroup::Flip(AnimationClip* animationClip, int32_t time)
	{
		CheckDeformer();

		CalculateAnimation(animationClip, time);

		CalclateBoneMatrices();

		// コピー
		if (m_matrixes_fr.size() != m_matrixes.size())
		{
			m_matrixes_fr.resize(m_matrixes.size());
		}

		std::copy(m_matrixes.begin(), m_matrixes.end(), m_matrixes_fr.begin());
	}


	void RenderedModelObject3D::MeshGroup::CalculateAnimation(AnimationClip* animationClip, int32_t time)
	{
		if (animationClip == nullptr) return;

		auto source = (AnimationSource_Imp*) animationClip->GetSource().get();
		auto& animations = source->GetAnimations();

		for (auto& a : animations)
		{
			auto a_ = (KeyframeAnimation_Imp*) a;

			auto type = a_->GetTargetType();
			auto axis = a_->GetTargetAxis();
			auto bi = m_deformer->GetBoneIndex(a_->GetTargetName());

			if (type == eAnimationCurveTargetType::ANIMATION_CURVE_TARGET_TYPE_NONE) continue;
			if (axis == eAnimationCurveTargetAxis::ANIMATION_CURVE_TARGET_AXIS_NONE) continue;
			if (bi < 0) continue;

			auto value = a_->GetValue(time);

			if (type == eAnimationCurveTargetType::ANIMATION_CURVE_TARGET_TYPE_POSITON)
			{
				m_boneProps[bi].Position[axis] = value;
			}
			else if (type == eAnimationCurveTargetType::ANIMATION_CURVE_TARGET_TYPE_ROTATION)
			{
				m_boneProps[bi].Rotation[axis] = value / 180.0f * 3.141592f;
			}
			else if (type == eAnimationCurveTargetType::ANIMATION_CURVE_TARGET_TYPE_SCALE)
			{
				m_boneProps[bi].Scale[axis] = value;
			}
		}
	}

	void RenderedModelObject3D::MeshGroup::CalclateBoneMatrices()
	{
		if (m_deformer == nullptr) return;

		for (auto i = 0; i < m_deformer->GetBones().size(); i++)
		{
			auto& b = m_deformer->GetBones()[i];
			m_matrixes[i] = m_boneProps[i].CalcMatrix(b.RotationType);
		}

		ModelUtils::CalculateBoneMatrixes(
			m_matrixes,
			m_deformer->GetBones(),
			m_matrixes);
	}

	void RenderedModelObject3D::MeshGroup::CheckDeformer()
	{
		auto m = (Mesh_Imp*) m_mesh;

		if (m != nullptr)
		{
			if (m->GetDeformer_() != m_deformer)
			{
				SetInternalDeformer(m->GetDeformer_());
			}
		}
		else
		{
			SetInternalDeformer(nullptr);
		}
	}

	void RenderedModelObject3D::MeshGroup::SetInternalDeformer(Deformer* deformer)
	{
		auto d = (Deformer_Imp*) deformer;
		SafeSubstitute(m_deformer, d);

		if (deformer != nullptr)
		{
			m_matrixes.resize(m_deformer->GetBones().size());
			m_boneProps.resize(m_deformer->GetBones().size());

			for (int32_t i = 0; i < m_boneProps.size(); i++)
			{
				m_boneProps[i] = BoneProperty();
			}
		}
		else
		{
			m_matrixes.resize(0);
			m_boneProps.resize(0);
		}
	}

	RenderedModelObject3D::RenderedModelObject3D(Graphics* graphics)
		: RenderedObject3D(graphics)
		, m_animationPlaying(nullptr)
		, m_animationTime(0)
	{
		std::vector<ace::VertexLayout> vl;
		vl.push_back(ace::VertexLayout("Position", ace::LAYOUT_FORMAT_R32G32B32_FLOAT));
		vl.push_back(ace::VertexLayout("Normal", ace::LAYOUT_FORMAT_R32G32B32_FLOAT));
		vl.push_back(ace::VertexLayout("Binormal", ace::LAYOUT_FORMAT_R32G32B32_FLOAT));
		vl.push_back(ace::VertexLayout("UV", ace::LAYOUT_FORMAT_R32G32_FLOAT));
		vl.push_back(ace::VertexLayout("UVSub", ace::LAYOUT_FORMAT_R32G32_FLOAT));
		vl.push_back(ace::VertexLayout("Color", ace::LAYOUT_FORMAT_R8G8B8A8_UNORM));
		vl.push_back(ace::VertexLayout("BoneWeights", ace::LAYOUT_FORMAT_R8G8B8A8_UNORM));
		vl.push_back(ace::VertexLayout("BoneIndexes", ace::LAYOUT_FORMAT_R8G8B8A8_UINT));
		vl.push_back(ace::VertexLayout("BoneIndexesOriginal", ace::LAYOUT_FORMAT_R8G8B8A8_UINT));

		std::shared_ptr<ace::NativeShader_Imp> shader;
		std::vector<ace::Macro> macro;
		if (GetGraphics()->GetGraphicsType() == GRAPHICS_TYPE_GL)
		{
			m_shader = GetGraphics()->CreateShader_Imp(
				gl_vs,
				"vs",
				gl_ps,
				"ps",
				vl,
				macro);
		}
		else
		{
			m_shader = GetGraphics()->CreateShader_Imp(
				dx_vs,
				"vs",
				dx_ps,
				"ps",
				vl,
				macro);
		}

		assert(m_shader != nullptr);

		std::vector<ace::ConstantBufferInformation> constantBuffers_vs;
		constantBuffers_vs.resize(3);
		constantBuffers_vs[0].Format = ace::CONSTANT_BUFFER_FORMAT_MATRIX44_ARRAY;
		constantBuffers_vs[0].Name = std::string("matMCP");
		constantBuffers_vs[0].Offset = 0;
		constantBuffers_vs[0].Count = 32;

		constantBuffers_vs[1].Format = ace::CONSTANT_BUFFER_FORMAT_FLOAT3;
		constantBuffers_vs[1].Name = std::string("directionalLightDirection");
		constantBuffers_vs[1].Offset = sizeof(Matrix44) * 32;

		constantBuffers_vs[2].Format = ace::CONSTANT_BUFFER_FORMAT_FLOAT3;
		constantBuffers_vs[2].Name = std::string("directionalLightColor");
		constantBuffers_vs[2].Offset = sizeof(Matrix44) * 32 + sizeof(float) * 4;

		m_shader->CreateVertexConstantBuffer<VertexConstantBuffer>(constantBuffers_vs);

		std::vector<ace::ConstantBufferInformation> constantBuffers_ps;
		constantBuffers_ps.resize(1);
		constantBuffers_ps[0].Format = ace::CONSTANT_BUFFER_FORMAT_FLOAT3;
		constantBuffers_ps[0].Name = std::string("hasTextures");
		constantBuffers_ps[0].Offset = 0;

		m_shader->CreatePixelConstantBuffer<PixelConstantBuffer>(constantBuffers_ps);
	}

	RenderedModelObject3D::~RenderedModelObject3D()
	{
		m_meshGroups_fr.clear();

		m_meshGroups.clear();

		if (m_model != nullptr)
		{
			m_model->Detach(this);
		}

		SafeRelease(m_model);

		for (auto& a : m_animationClips)
		{
			a.second->Release();
		}
		m_animationClips.clear();
	}

	void RenderedModelObject3D::SetModel(Model* model)
	{
		UnloadModel();

		if (m_model != nullptr)
		{
			m_model->Detach(this);
		}

		SafeRelease(m_model);

		if (model == nullptr) return;
		auto model_ = (Model_Imp*) model;
		m_model = model_;
		SafeAddRef(m_model);

		m_model->Attach(this);

		LoadModel();
	}

	void RenderedModelObject3D::AddMesh(Mesh* mesh)
	{
		m_meshGroups.push_back(std::make_shared<MeshGroup>((Mesh_Imp*)mesh));
	}

	void RenderedModelObject3D::UnloadModel()
	{
		// 描画中以外のオブジェクトをリセット

		m_meshGroups.clear();

		for (auto& a : m_animationClips)
		{
			a.second->Release();
		}
		m_animationClips.clear();
	}

	void RenderedModelObject3D::LoadModel()
	{
		if (m_model == nullptr) return;

		for (auto& m : m_model->GetMeshes())
		{
			AddMesh(m);
		}

		for (int32_t i = 0; i < m_model->GetAnimationClips().size(); i++)
		{
			AddAnimationClip(m_model->GetAnimationClipNames()[i].c_str(), m_model->GetAnimationClips()[i]);
		}
	}

	void RenderedModelObject3D::ReloadModel()
	{
		UnloadModel();
		LoadModel();
	}

	void RenderedModelObject3D::AddAnimationClip(const achar* name, AnimationClip* animationClip)
	{
		if (animationClip == nullptr) return;

		if (m_animationClips.find(name) == m_animationClips.end())
		{
			SafeAddRef(animationClip);
			m_animationClips[name] = animationClip;
		}
	}

	void RenderedModelObject3D::PlayAnimation(const achar* name)
	{
		auto it = m_animationClips.find(name);
		if (it == m_animationClips.end()) return;

		m_animationPlaying = (*it).second;
		m_animationTime = 0;
	}

	void RenderedModelObject3D::Flip()
	{
		RenderedObject3D::Flip();

		for (auto& g : m_meshGroups)
		{
			g->Flip(m_animationPlaying, m_animationTime);
		}

		// アニメーションの適用
		if (m_animationPlaying != nullptr)
		{
			m_animationTime++;
		}

		m_meshGroups_fr.clear();
		m_meshGroups_fr.resize(m_meshGroups.size());
		std::copy(m_meshGroups.begin(), m_meshGroups.end(), m_meshGroups_fr.begin());
	}

	void RenderedModelObject3D::Rendering(RenderingProperty& prop)
	{
		auto& vbuf = m_shader->GetVertexConstantBuffer<VertexConstantBuffer>();
		auto& pbuf = m_shader->GetPixelConstantBuffer<PixelConstantBuffer>();

		for (auto& g : m_meshGroups_fr)
		{
			auto& matrices = g->m_matrixes_fr;
			auto mesh = g->GetMesh();

			// 行列計算
			if (matrices.size() > 0)
			{
				// ボーンあり
				for (int32_t i = 0; i < Min(32, matrices.size()); i++)
				{
					vbuf.MCPMatrices[i].Indentity();
					Matrix44::Mul(vbuf.MCPMatrices[i], prop.CameraProjectionMatrix, GetLocalMatrix_FR());
					Matrix44::Mul(vbuf.MCPMatrices[i], vbuf.MCPMatrices[i], matrices[i]);
				}
			}
			else
			{
				// ボーンなし
				vbuf.MCPMatrices[0].Indentity();
				Matrix44::Mul(vbuf.MCPMatrices[0], prop.CameraProjectionMatrix, GetLocalMatrix_FR());
				for (int32_t i = 1; i < 32; i++)
				{
					vbuf.MCPMatrices[i] = vbuf.MCPMatrices[0];
				}
			}

			{
				vbuf.DirectionalLightDirection = prop.DirectionalLightDirection;
				vbuf.DirectionalLightColor.X = prop.DirectionalLightColor.R / 255.0f;
				vbuf.DirectionalLightColor.Y = prop.DirectionalLightColor.G / 255.0f;
				vbuf.DirectionalLightColor.Z = prop.DirectionalLightColor.B / 255.0f;
			}

			auto& boneOffsets = mesh->GetBoneOffsets();
			auto& materialOffsets = mesh->GetMaterialOffsets();

			if (boneOffsets.size() == 0 || materialOffsets.size() == 0)
			{
				// ボーン、もしくはマテリアルの設定がない場合

				pbuf.HasTextures.X = 0.0f;
				pbuf.HasTextures.Y = 0.0f;
				pbuf.HasTextures.Z = 0.0f;

				GetGraphics()->SetVertexBuffer(mesh->GetVertexBuffer().get());
				GetGraphics()->SetIndexBuffer(mesh->GetIndexBuffer().get());
				GetGraphics()->SetShader(m_shader.get());

				auto& state = GetGraphics()->GetRenderState()->Push();
				state.DepthTest = true;
				state.DepthWrite = true;
				state.CullingType = CULLING_DOUBLE;
				GetGraphics()->GetRenderState()->Update(false);

				GetGraphics()->DrawPolygon(mesh->GetIndexBuffer()->GetCount() / 3);

				GetGraphics()->GetRenderState()->Pop();
			}
			else
			{
				// 設定がある場合
				auto mIndex = 0;
				auto bIndex = 0;

				auto fOffset = 0;
				auto fCount = 0;

				auto bFCount = boneOffsets[bIndex].FaceOffset;
				auto mFCount = materialOffsets[mIndex].FaceOffset;

				while (fCount < mesh->GetIndexBuffer()->GetCount() / 3)
				{
					fCount = Min(bFCount, mFCount) - fOffset;
					if (fCount == 0) break;

					auto material = mesh->GetMaterial(materialOffsets[mIndex].MaterialIndex);

					if (material != nullptr)
					{
						pbuf.HasTextures.X = material->ColorTexture != nullptr ? 1.0f : 0.0f;
						pbuf.HasTextures.Y = material->NormalTexture != nullptr ? 1.0f : 0.0f;
						pbuf.HasTextures.Z = material->SpecularTexture != nullptr ? 1.0f : 0.0f;

						if (material->ColorTexture != nullptr)
						{
							m_shader->SetTexture("g_colorTexture", material->ColorTexture, 0);
						}

						if (material->NormalTexture != nullptr)
						{
							m_shader->SetTexture("g_normalTexture", material->NormalTexture, 1);
						}

						if (material->SpecularTexture != nullptr)
						{
							m_shader->SetTexture("g_specularTexture", material->SpecularTexture, 2);
						}
					}
					else
					{
						pbuf.HasTextures.X = 0.0f;
						pbuf.HasTextures.Y = 0.0f;
						pbuf.HasTextures.Z = 0.0f;
					}
					
					GetGraphics()->SetVertexBuffer(mesh->GetVertexBuffer().get());
					GetGraphics()->SetIndexBuffer(mesh->GetIndexBuffer().get());
					GetGraphics()->SetShader(m_shader.get());

					auto& state = GetGraphics()->GetRenderState()->Push();
					state.DepthTest = true;
					state.DepthWrite = true;
					state.CullingType = CULLING_DOUBLE;
					GetGraphics()->GetRenderState()->Update(false);

					GetGraphics()->DrawPolygon(mesh->GetIndexBuffer()->GetCount() / 3);

					GetGraphics()->GetRenderState()->Pop();


					if (fCount + fOffset == bFCount && boneOffsets.size() > bIndex)
					{
						bFCount += boneOffsets[bIndex].FaceOffset;
						bIndex++;
					}

					if (fCount + fOffset == mFCount && materialOffsets.size() > mIndex)
					{
						mFCount += materialOffsets[mIndex].FaceOffset;
						mIndex++;
					}

					fOffset += fCount;
				}
			}

			
		}
	}
}