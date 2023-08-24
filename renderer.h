
#include <d3dcompiler.h>
#include "load_data_oriented.h"
#include "h2bParser.h"
#pragma comment(lib, "d3dcompiler.lib")
///Simple Vertex Shader
const char* vertexShaderSource = R"(
#pragma pack_matrix(row_major)
struct ModelData
{
    float3 Kd; // diffuse reflectivity
    float d; // dissolve (transparency)
    float3 Ks; // specular reflectivity
    float Ns; // specular exponent
    float3 Ka; // ambient reflectivity
    float sharpness; // local reflection map sharpness
    float3 Tf; // transmission filter
    float Ni; // optical density (index of refraction)
    float3 Ke; // emissive reflectivity
    uint illum; // illumination model
};
struct MESH_DATA
{
    uint mesh_ID;
    uint world_ID;
};
struct VertexInput
{
float3 position : POSITION;
float3 normal : NORMAL;
    // Add any other required attributes here
};

struct VertexOutput
{
    float4 posH : SV_Position;
    float3 nrmW : NORMAL;
    float3 posW : WORLD;
};
cbuffer Scene : register(b0)
{
    float4x4 viewMatrix;
    float4x4 pMatrix;
    float3 lightDirection;
    float3 lightColor;
    float4 SunAmbient;
    float4 camPos;
};
cbuffer Model : register(b1)
{
    uint vertexCount;
    uint indexCount;
    uint materialCount;
    uint meshCount;
    uint vertexStart;
    uint indexStart;
    uint materialStart;
    uint meshStart;
    uint batchStart;
    uint padding1;
    uint padding2;
    uint padding3;
};

cbuffer MeshData : register(b2)
{
    float4x4 worldMatrix;
   // ModelData materials[13]; // Assuming you have 13 materials for each instance
};

VertexOutput main(VertexInput input, uint instanceIndex : SV_InstanceID)
{
    VertexOutput output;
    float4 pos = float4(input.position, 1);

    // Fetch the transformation matrix for the current instance using MESH_DATA
    MESH_DATA meshData;
    meshData.mesh_ID = instanceIndex; // Or however you determine the mesh ID
    meshData.world_ID = instanceIndex; // Or however you determine the world ID
   // float4x4 currentWorldMatrix = worldMatrix[meshData.world_ID];

    // Apply the world matrix for the current instance
    pos = mul(pos, worldMatrix[meshData.world_ID]);
    output.posW = pos.xyz;

    // Apply the view and projection matrices
    pos = mul(pos, viewMatrix);
    pos = mul(pos, pMatrix);
    output.posH = pos;

    // Apply the normal transformation using the same instance's world matrix
    float4 norm = mul(float4(input.normal, 1),worldMatrix[meshData.world_ID]);
    output.nrmW = norm.xyz;
    return output;
}
)";
// Simple Pixel Shader
const char* pixelShaderSource = R"(
// an ultra simple hlsl pixel shader
#pragma pack_matrix( row_major )
struct ModelData
{
    float3 Kd; // diffuse reflectivity
    float d; // dissolve (transparency) 
    float3 Ks; // specular reflectivity
    float Ns; // specular exponent
    float3 Ka; // ambient reflectivity
    float sharpness; // local reflection map sharpness
    float3 Tf; // transmission filter
    float Ni; // optical density (index of refraction)
    float3 Ke; // emissive reflectivity
	uint illum; // illumination model
};
struct MESH_DATA 
{
	uint mesh_ID;
	uint world_ID;
};
cbuffer Scene : register(b0)
{
  float4x4 viewMatrix;
  float4x4 pMatrix;
  float3 lightDirection;
  float3 lightColor;
  float4 SunAmbient;
  float4 camPos;
};
cbuffer Model : register(b1)
{
   uint vertexCount;
   uint indexCount;
   uint materialCount;
   uint meshCount;
   uint vertexStart;
   uint indexStart;
   uint materialStart;
   uint meshStart;
   uint batchStart;
   uint padding1;
   uint padding2;
   uint padding3;
};
cbuffer transformBuffer : register(b2)
{
   float4x4 worldMatrix[13];
};
struct OUTPUT_TO_RASTERIER
{
    float4 posH : SV_Position;
    float3 nrmW : NORMAL;
    float3 posW : WORLD;
};

float4 main(OUTPUT_TO_RASTERIER outer) : SV_TARGET 
{
    float4 finalColor = float4(0, 0, 0, 1);

    // Loop over instances
    for (uint instanceIndex = 0; instanceIndex < vertexCount; ++instanceIndex)
    {
        // Compute instance-specific data using instanceIndex
        
        // Example: Compute a unique color based on instanceIndex
        float4 instanceColor = float4(instanceIndex / float(vertexCount), 1, 1, 1);
        
        // Calculate lighting for this instance (replace this with your lighting calculations)
        float3 normal = normalize(outer.nrmW);
        float intensity = max(dot(normal, lightDirection), 0);
        float3 lightingColor = intensity * lightColor;
        
        // Combine instance-specific color with lighting calculations
        float4 instanceResult = instanceColor * float4(lightingColor, 1);
        
        // Accumulate the instance result into the final color
        finalColor += instanceResult;
    }
    return finalColor;
}
)";
struct Scene_data///this will be mapped and unmapped once
{
	GW::MATH::GMATRIXF viewMatrix;
	GW::MATH::GMATRIXF pMatrix;
	GW::MATH::GVECTORF LightDirection;
	GW::MATH::GVECTORF LightColor;
	GW::MATH::GVECTORF SunAmbient = { 0.25f, 0.25f, 0.9f, 1 };
	GW::MATH::GVECTORF camPos;
}Scene;

struct MESH_DATA
{
	UINT mesh_ID;
	UINT world_ID;
}MeshData;
float aspect;
float zNear = 0.1;
float zFar = 100;
/// Creation, Rendering & Cleanup
class GameLevelRenderer
{
	// proxy handles
	Level_Data m_Object;
	GW::SYSTEM::GWindow win;
	GW::GRAPHICS::GDirectX11Surface d3d;
	GW::MATH::GMatrix m_Create;


	GW::MATH::GMATRIXF view = GW::MATH::GIdentityMatrixF;
	GW::MATH::GMATRIXF camera;
	float aspect;
	GW::MATH::GMATRIXF perspective;
	// what we need at a minimum to draw a triangle
	Microsoft::WRL::ComPtr<ID3D11Buffer>		SceneBuffer; /// buffer instance for the world
	Microsoft::WRL::ComPtr<ID3D11Buffer>		ModelBuffer; /// buffer instance for the models
	Microsoft::WRL::ComPtr<ID3D11Buffer>        transformBuffer; ///buffer instance for the transforms
	/// buffers for the indices and vert
	Microsoft::WRL::ComPtr<ID3D11Buffer>		VerticesBuffer;///vert buffer
	Microsoft::WRL::ComPtr<ID3D11Buffer>		IndicesBuffer;/// index buffer
	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader;/// shader
	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader;/// shader
	Microsoft::WRL::ComPtr<ID3D11InputLayout>	vertexFormat;
public:
	GameLevelRenderer(GW::SYSTEM::GWindow _win, GW::GRAPHICS::GDirectX11Surface _d3d, Level_Data object)
	{
		m_Object = object;
		win = _win;
		d3d = _d3d;
		GW::MATH::GVECTORF eye = { 0, 0, -1, 1 };
		GW::MATH::GVECTORF center = { 0, 0, 0, 1 };
		GW::MATH::GVECTORF up = { 0, 1, 0, 0 };
		m_Create.LookAtLHF(eye, center, up, camera);
		m_Create.ProjectionDirectXLHF(1.13446f, aspect, 0.1f, 100.0f, perspective);

		Scene.viewMatrix = camera;
		Scene.pMatrix = perspective;
		Scene.camPos = eye;
		Scene.LightColor = { 0.9f, 0.9f,1, 1 };
		d3d.GetAspectRatio(aspect);

		MeshData.mesh_ID = 0;
		MeshData.world_ID = 0;

		ID3D11Device* creator;
		d3d.GetDevice((void**)&creator);
		d3d.GetAspectRatio(aspect);
		{
			///vertex buffer
			/// <summary> order of parameters is  UINT byteWidth,	UINT bindFlags, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT cpuaccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0 )
			D3D11_SUBRESOURCE_DATA vData = { m_Object.levelVertices.data(), 0, 0 };	///VERT
			CD3D11_BUFFER_DESC vertDesc(sizeof(m_Object.levelVertices[0]) * m_Object.levelVertices.size(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE); ///VERT
			creator->CreateBuffer(&vertDesc, &vData, VerticesBuffer.GetAddressOf());											 ///VERT
			///INDEX buffer
			/// <summary> order of parameters is  UINT byteWidth,	UINT bindFlags, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT cpuaccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0 )
			D3D11_SUBRESOURCE_DATA iData = { m_Object.levelIndices.data(), 0, 0 };
			CD3D11_BUFFER_DESC indexDesc(sizeof(unsigned int) * m_Object.levelIndices.size(), D3D11_BIND_INDEX_BUFFER); ///INDEX BUFF
			creator->CreateBuffer(&indexDesc, &iData, IndicesBuffer.GetAddressOf());
			///scene buffer
			CD3D11_BUFFER_DESC SceneDesc(sizeof(Scene), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);///Model
			creator->CreateBuffer(&SceneDesc, nullptr, SceneBuffer.GetAddressOf());
			///Model Buffer const buffer for the level models
			CD3D11_BUFFER_DESC ModelDesc(sizeof(m_Object.levelModels[0]) * m_Object.levelModels.size(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);///Model
			creator->CreateBuffer(&ModelDesc, nullptr, ModelBuffer.GetAddressOf());
			float result = static_cast<float>(ModelDesc.ByteWidth) / 16.0f;
			if (ModelDesc.ByteWidth % 16 != 0)
			{
				std::cout << "The byte width is not a multiple of 16, it is: " << ModelDesc.ByteWidth << " bytes";
			}
			///transform buffer
			CD3D11_BUFFER_DESC transformBufferDesc(sizeof(GW::MATH::GMATRIXF) * m_Object.levelTransforms.size(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);///Model
			creator->CreateBuffer(&transformBufferDesc, nullptr, transformBuffer.GetAddressOf());

			///D3D11_BUFFER_DESC transformBufferDesc( sizeof(GW::MATH::GMATRIXF) * m_Object.levelInstances.size(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE, 0, 0);
			///ransformBufferDesc.ByteWidth = sizeof(GW::MATH::GMATRIXF) * m_Object.levelInstances.size();
			///transformBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			///transformBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; // Use D3D11_BIND_CONSTANT_BUFFER for constant buffer
			///transformBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			///transformBufferDesc.MiscFlags = 0; // No MiscFlags
			///transformBufferDesc.StructureByteStride = 0; // No structured buffer
			///creator->CreateBuffer(&transformBufferDesc, nullptr, transformBuffer.GetAddressOf());
		}
		UINT compilerFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
		compilerFlags |= D3DCOMPILE_DEBUG;
#endif
		{
			//Create Vertex Shader 
			Microsoft::WRL::ComPtr<ID3DBlob> VertexShader, errors;
			if (SUCCEEDED(D3DCompile(vertexShaderSource, strlen(vertexShaderSource),///vertexShaderSource.c_str(), vertexShaderSource.length() if doing extren hlsl files.
				nullptr, nullptr, nullptr, "main", "vs_4_0", compilerFlags, 0, VertexShader.GetAddressOf(), errors.GetAddressOf())))
			{
				creator->CreateVertexShader(VertexShader->GetBufferPointer(),
					VertexShader->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
			}
			else
				std::cout << "Vertex shader compilation error: " << reinterpret_cast<const char*>(errors->GetBufferPointer()) << std::endl;
			// Create Pixel Shader
			Microsoft::WRL::ComPtr<ID3DBlob> PixelShader; errors.Reset();
			if (SUCCEEDED(D3DCompile(pixelShaderSource, strlen(pixelShaderSource), nullptr, nullptr, nullptr, "main", "ps_4_0", compilerFlags, 0, PixelShader.GetAddressOf(), errors.GetAddressOf())))
			{
				creator->CreatePixelShader(PixelShader->GetBufferPointer(), PixelShader->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
			}
			else
				std::cout << "Vertex shader compilation error: " << reinterpret_cast<const char*>(errors->GetBufferPointer()) << std::endl;

			// Create Input Layout
			D3D11_INPUT_ELEMENT_DESC format[] =
			{
			   {
				"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			   },
			   {
				"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			   },
			   {
				"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			   },
			   {
				"WORLD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
			   }
			};
			creator->CreateInputLayout(format, ARRAYSIZE(format), VertexShader->GetBufferPointer(),
				VertexShader->GetBufferSize(), vertexFormat.GetAddressOf());
		}
		// free temporary handle
		creator->Release();
	}
	void drawGameLevel()
	{
		// grab the context & render target
		ID3D11DeviceContext* con;
		ID3D11RenderTargetView* view;
		ID3D11DepthStencilView* depth;
		d3d.GetImmediateContext((void**)&con);
		d3d.GetRenderTargetView((void**)&view);
		d3d.GetDepthStencilView((void**)&depth);
		// setup the pipeline
		ID3D11RenderTargetView* const views[] = { view };
		con->OMSetRenderTargets(ARRAYSIZE(views), views, depth);
		/// pipeline stuff
		const UINT strides[] = { sizeof(m_Object.levelVertices[0]) };
		const UINT offsets[] = { 0 };
		ID3D11Buffer* const buffs[] = { VerticesBuffer.Get() };
		con->IASetVertexBuffers(0, ARRAYSIZE(buffs), buffs, strides, offsets);///Vertex buff
		con->IASetIndexBuffer(IndicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0); ///  index buff
		con->VSSetShader(vertexShader.Get(), nullptr, 0);///Setting shaders
		con->PSSetShader(pixelShader.Get(), nullptr, 0);	///Setting shaders
		con->IASetInputLayout(vertexFormat.Get());///Setting Format
		/// now we can draw
		con->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		///THE CURRENT ISSUE IS THAT DATA FROM THE MODEL BUFFER ISNT BYTE ALIGNED WHICH MEAN 468 -4 or some other way
		///rotate over the y-axis
		Scene.LightDirection = { -1,-1,2, 1 };
		///Normalize a Vector COBO
		GW::MATH::GVector::NormalizeF(Scene.LightDirection, Scene.LightDirection);
		D3D11_MAPPED_SUBRESOURCE gpuBuffer;
		/// This updates the Scene buffer
		con->Map(SceneBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		memcpy(gpuBuffer.pData, &Scene, sizeof(Scene));
		con->Unmap(SceneBuffer.Get(), 0);
		///update the model buffer
		con->Map(ModelBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		memcpy(gpuBuffer.pData, m_Object.levelModels.data(), sizeof(m_Object.levelModels[0]) * m_Object.levelModels.size());
		con->Unmap(ModelBuffer.Get(), 0);
		///update the transform buffer
		con->Map(transformBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		memcpy(gpuBuffer.pData, m_Object.levelTransforms.data(), sizeof(GW::MATH::GMATRIXF) * m_Object.levelTransforms.size());
		con->Unmap(transformBuffer.Get(), 0);
		/// Connect PipeLine
		ID3D11Buffer* ModelPipleLine[] = { SceneBuffer.Get(),ModelBuffer.Get(),transformBuffer.Get() };
		con->VSSetConstantBuffers(0, 3, ModelPipleLine);
		con->PSSetConstantBuffers(0, 3, ModelPipleLine);
		/// Draw the MODELS
		for (size_t i = 0; i < m_Object.levelModels.size(); i++)
		{
			// Set the starting world matrix id for instances of model
			MeshData.world_ID = m_Object.levelInstances[i].transformStart;
			for (size_t j = 0; j < m_Object.levelModels[i].meshCount; j++)
			{
				auto model = m_Object.levelModels[i];
				auto meshIndex = model.meshStart + j;
				MeshData.mesh_ID = m_Object.levelMeshes[i].materialIndex + m_Object.levelMeshes[meshIndex].materialIndex;
				con->DrawIndexedInstanced(
					m_Object.levelMeshes[meshIndex].drawInfo.indexCount,
					m_Object.levelInstances[i].transformCount,
					m_Object.levelMeshes[meshIndex].drawInfo.indexOffset + model.indexStart,
					model.vertexStart, 0);
			}
		}
		// release temp handles
		depth->Release();
		view->Release();
		con->Release();
	}
	~GameLevelRenderer()
	{
		// ComPtr will auto release so nothing to do here 
	}

};