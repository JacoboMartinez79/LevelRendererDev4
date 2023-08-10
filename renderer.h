#include <d3dcompiler.h>
#include "load_data_oriented.h"
#include "h2bParser.h"
#pragma comment(lib, "d3dcompiler.lib")
// Simple Vertex Shader
const char* vertexShaderSource = R"(
// an ultra simple hlsl vertex shader
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
cbuffer Model : register(b0)
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
   float4x4 worldMatrix;
   ModelData data;
};
//cbuffer ModelDataBuffer : register(b0)
//{
//   float4x4 worldMatrix;
//   ModelData data;
//};
cbuffer Scene : register(b1)
{
  float4x4 viewMatrix;
  float4x4 pMatrix;
  float3 lightDirection;
  float3 lightColor;
  float4 SunAmbient;
  float4 camPos;
};

struct OUTPUT_TO_RASTERIER
{
    float4 posH : SV_Position; //Homogenous projection Space  
    float3 nrmW : NORMAL; // normal in world space(for lighting)
    float3 posW : WORLD; // position in world space(for lighting)
};
struct Vertex
{
    float3 position : Position; //positions
    float3 uvw : UVW; // UVWs
    float3 normals : NORMAL; // Normals
};
OUTPUT_TO_RASTERIER main(float3 inputVertex : POSITION, float3 inputT : TEXCOORD, float3 inputNom : NORMAL)
{
    OUTPUT_TO_RASTERIER output;
    float4 pos = float4(inputVertex, 1);
    pos = mul(pos, worldMatrix);
    output.posW = pos.xyz;
    pos = mul(pos, viewMatrix);
    pos = mul(pos, pMatrix);
    output.posH = pos;
    float4 norm = mul(float4(inputNom,1), worldMatrix);
    output.nrmW = norm.xyz;
    return output;
}
)";
// Simple Pixel Shader
const char* pixelShaderSource = R"(
// an ultra simple hlsl pixel shader
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
cbuffer Model : register(b0)
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
   float4x4 worldMatrix;
   ModelData data;
};
//cbuffer ModelDataBuffer : register(b0)
//{
//};
cbuffer Scene : register(b1)
{
  float4x4 viewMatrix;
  float4x4 pMatrix;
  float3 lightDirection;
  float3 lightColor;
  float4 SunAmbient;
  float4 camPos;
  // float4x4 worldMatrix;
  // ModelData data;
};
struct OUTPUT_TO_RASTERIER
{
    float4 posH : SV_Position; //Homogenous projection Space  
    float3 nrmW : NORMAL; // normal in world space(for lighting)
    float3 posW : WORLD; // position in world space(for lighting)
};
float4 main(OUTPUT_TO_RASTERIER outer) : SV_TARGET 
{
    outer.nrmW = normalize(outer.nrmW);

    // Initialize the final color
    float4 finalColor = float4(0, 0, 0, 1);

    // Loop over instances
    for (uint instanceIndex = 0; instanceIndex < vertexCount; ++instanceIndex)
    {
        // Compute instance-specific data using instanceIndex

        // Example: Compute a unique color based on instanceIndex
        float4 instanceColor = float4(instanceIndex / float(vertexCount), 1, 1, 1);

        // Calculate lighting for this instance (replace this with your lighting calculations)
        float3 normal = normalize(outer.nrmW);
        float3 viewDirection = normalize(camPos.xyz - outer.posW);
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
	// what we need at a minimum to draw a triangle
	Microsoft::WRL::ComPtr<ID3D11Buffer>		SceneBuffer; /// buffer instance for the world
	Microsoft::WRL::ComPtr<ID3D11Buffer>		ModelBuffer; /// buffer instance for the models
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
		ID3D11Device* creator;
		d3d.GetDevice((void**)&creator);
		d3d.GetAspectRatio(aspect);
		{
			D3D11_SUBRESOURCE_DATA vData = { m_Object.levelVertices.data(), 0, 0 };	///VERT
			/// <summary> order of parameters is  UINT byteWidth,	UINT bindFlags, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT cpuaccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0 )
			CD3D11_BUFFER_DESC vertDesc(sizeof(m_Object.levelVertices[0]) * m_Object.levelVertices.size(), D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE); ///VERT
			creator->CreateBuffer(&vertDesc, &vData, VerticesBuffer.GetAddressOf());											 ///VERT
			///INDEX buffer
			D3D11_SUBRESOURCE_DATA iData = { m_Object.levelIndices.data(), 0, 0 };
			/// <summary> order of parameters is  UINT byteWidth,	UINT bindFlags, D3D11_USAGE usage = D3D11_USAGE_DEFAULT, UINT cpuaccessFlags = 0, UINT miscFlags = 0, UINT structureByteStride = 0 )
			CD3D11_BUFFER_DESC indexDesc(sizeof(m_Object.levelIndices[0]) * m_Object.levelIndices.size(), D3D11_BIND_INDEX_BUFFER); ///INDEX BUFF
			creator->CreateBuffer(&indexDesc, &iData, IndicesBuffer.GetAddressOf());
			///Model Buffer const buffer for the level models
			CD3D11_BUFFER_DESC ModelDesc(sizeof(m_Object.levelModels) * m_Object.levelModels.size(), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);///Model
			creator->CreateBuffer(&ModelDesc, nullptr, ModelBuffer.GetAddressOf());
			/// <summary> this is gonna the const buffer for the camera 
			///scene buffer
			CD3D11_BUFFER_DESC SceneDesc(sizeof(Scene), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);///Model
			creator->CreateBuffer(&SceneDesc, nullptr, SceneBuffer.GetAddressOf());
		}
		UINT compilerFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
		compilerFlags |= D3DCOMPILE_DEBUG;
#endif
		{
			//Create Vertex Shader 
			Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, errors;
			if (SUCCEEDED(D3DCompile(vertexShaderSource, strlen(vertexShaderSource),///vertexShaderSource.c_str(), vertexShaderSource.length() if doing extren hlsl files.
				nullptr, nullptr, nullptr, "main", "vs_4_0", compilerFlags, 0,
				vsBlob.GetAddressOf(), errors.GetAddressOf())))
			{
				creator->CreateVertexShader(vsBlob->GetBufferPointer(),
					vsBlob->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
			}
			else
				std::cout << "Vertex shader compilation error: " << reinterpret_cast<const char*>(errors->GetBufferPointer()) << std::endl;
			// Create Pixel Shader
			Microsoft::WRL::ComPtr<ID3DBlob> psBlob; errors.Reset();
			if (SUCCEEDED(D3DCompile(pixelShaderSource, strlen(pixelShaderSource),
				nullptr, nullptr, nullptr, "main", "ps_4_0", compilerFlags, 0,
				psBlob.GetAddressOf(), errors.GetAddressOf())))
			{
				creator->CreatePixelShader(psBlob->GetBufferPointer(),
					psBlob->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
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
			creator->CreateInputLayout(format, ARRAYSIZE(format), vsBlob->GetBufferPointer(),
				vsBlob->GetBufferSize(), vertexFormat.GetAddressOf());
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
		/// MAKE 3D CAMERA!!!!!!!!!!!!!!!!!!!!!
		GW::MATH::GVECTORF center = { 0.15,0.75f,0,1 };// this is my view
		GW::MATH::GVECTORF eye = { 0.75f, 0.25f,-1.50f, 1 };//this the eye
		GW::MATH::GVECTORF up = { 0,5,0,0 };// moving camera up
		///rotate over the y-axis
		Scene.camPos = eye;
		Scene.LightDirection = { -1,-1,2, 1 };
		///Normalize a Vector COBO
		GW::MATH::GVector::NormalizeF(Scene.LightDirection, Scene.LightDirection);
		///                  R     G    B  A
		Scene.LightColor = {0.9f, 0.9f,1, 1 };
		m_Create.LookAtLHF(eye, center, up, Scene.viewMatrix);///view matrix is setup
		m_Create.ProjectionDirectXLHF(G_DEGREE_TO_RADIAN_F(65), aspect, zNear, zFar, Scene.pMatrix);

		/// This updates the Scene buffer
		D3D11_MAPPED_SUBRESOURCE gpuBuffer;
		con->Map(SceneBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		memcpy(gpuBuffer.pData, &Scene, sizeof(Scene));
		con->Unmap(SceneBuffer.Get(), 0);
		///update the model buffer
		con->Map(ModelBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
		memcpy(gpuBuffer.pData, &m_Object.LM, sizeof(m_Object.LM));
		con->Unmap(ModelBuffer.Get(), 0);
		///////// Connect PipeLine
		///ID3D11Buffer* ModelPipleLine[] = { ModelBuffer.Get() };
		///ID3D11Buffer* scenePipeLine[] = { SceneBuffer.Get() };
		///con->VSSetConstantBuffers(0, 1, ModelPipleLine);
		///con->VSSetConstantBuffers(1, 1, scenePipeLine);
		///con->PSSetConstantBuffers(0, 1, ModelPipleLine);
		///con->PSSetConstantBuffers(1, 1, scenePipeLine);
		//maybe useful to condonse this down
		ID3D11Buffer* buffers[] = { ModelBuffer.Get(), SceneBuffer.Get() };
		con->VSSetConstantBuffers(0, 2, buffers);
		con->PSSetConstantBuffers(0, 2, buffers);
		/// Draw the MODELS
		///for (size_t i = 0; i < m_Object.levelModels.size(); ++i)
		///{
		///	auto mi = m_Object.levelInstances[i].transformStart;
		///	for (int j = 0; j < m_Object.levelModels[i].meshCount; j++)
		///	{
		///		auto model = m_Object.levelModels[i];
		///		auto meshIndex = model.meshStart + j;
		///		con->DrawIndexedInstanced(
		///			m_Object.levelMeshes[meshIndex].drawInfo.indexCount,
		///			m_Object.levelInstances[i].transformCount,
		///			m_Object.levelMeshes[meshIndex].drawInfo.indexOffset + model.indexStart, // Ensure proper alignment
		///			model.vertexStart, 0);
		///	}
		///}
		// Draw the MODELS
		for (size_t i = 0; i < m_Object.levelModels.size(); ++i)
		{
			auto model = m_Object.levelModels[i];
			auto mi = m_Object.levelInstances[i].transformStart;
			// Loop over instances of the current model
			for (int instanceIndex = 0; instanceIndex < m_Object.levelInstances[i].transformCount; ++instanceIndex)
			{
				// Get the index for the instance's transformations
				int transformIndex = mi + instanceIndex;

				// Loop over meshes of the current model
				for (int j = 0; j < model.meshCount; j++)
				{
					auto meshIndex = model.meshStart + j;
					con->DrawIndexedInstanced(
						m_Object.levelMeshes[meshIndex].drawInfo.indexCount,
						1, // Draw a single instance
						m_Object.levelMeshes[meshIndex].drawInfo.indexOffset + model.indexStart,
						model.vertexStart, transformIndex); // Use the specific transform index
				}
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