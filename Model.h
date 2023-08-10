///#pragma once
/////#include "Gateware.h"
/////#include<d3d11.h>
///#include "h2bParser.h"
///// Simple Vertex Shader
///const char* vertexShaderSource = R"(
///// an ultra simple hlsl vertex shader
///#pragma pack_matrix( row_major )
///struct ModelData
///{
///    float3 Kd; // diffuse reflectivity
///    float d; // dissolve (transparency) 
///    float3 Ks; // specular reflectivity
///    float Ns; // specular exponent
///    float3 Ka; // ambient reflectivity
///    float sharpness; // local reflection map sharpness
///    float3 Tf; // transmission filter
///    float Ni; // optical density (index of refraction)
///    float3 Ke; // emissive reflectivity
///	uint illum; // illumination model
///};
///cbuffer Model : register(b0)
///{
///   float4x4 worldMatrix;
///   ModelData data;
///};
///cbuffer Scene : register(b1)
///{
///  float4x4 viewMatrix;
///  float4x4 pMatrix;
///  float3 lightDirection;
///  float3 lightColor;
///  float4 SunAmbient;
///  float4 camPos;
///};
///struct OUTPUT_TO_RASTERIER
///{
///    float4 posH : SV_Position; //Homogenous projection Space  
///    float3 nrmW : NOMRAL; // normal in world space(for lighting)
///    float3 posW : WORLD; // position in world space(for lighting)
///};
///OUTPUT_TO_RASTERIER main(float4 inputVertex : POSITION, float3 inputT : TEXCOORD, float4 inputNom : NORMAL)
///{
///    OUTPUT_TO_RASTERIER output;
///    //float4 pos = float4(inputVertex, 1);
///    inputVertex= mul(inputVertex, worldMatrix);
///    output.posW = pos.xyz;
///    inputVertex = mul(inputVertex, viewMatrix);
///    inputVertex = mul(inputVertex, pMatrix);
///    output.posH = pos;
///    float4 norm = mul(inputNom, worldMatrix);
///    output.nrmW = norm.xyz;
///    return output;
///}
///)";
///// Simple Pixel Shader
///const char* pixelShaderSource = R"(
///// an ultra simple hlsl pixel shader
///struct ModelData
///{
///    float3 Kd; // diffuse reflectivity
///    float d; // dissolve (transparency) 
///    float3 Ks; // specular reflectivity
///    float Ns; // specular exponent
///    float3 Ka; // ambient reflectivity
///    float sharpness; // local reflection map sharpness
///    float3 Tf; // transmission filter
///    float Ni; // optical density (index of refraction)
///    float3 Ke; // emissive reflectivity
///	uint illum; // illumination model
///};
///cbuffer Model : register(b0)
///{
///   float4x4 worldMatrix;
///   ModelData data;
///};
///cbuffer Scene : register(b1)
///{
///  float4x4 viewMatrix;
///  float4x4 pMatrix;
///  float3 lightDirection;
///  float paddingV2;
///  float3 lightColor;
///  float padding;
///  float4 SunAmbient;
///  float4 camPos;
/// 
///};
///struct OUTPUT_TO_RASTERIER
///{
///    float4 posH : SV_Position; //Homogenous projection Space  
///    float3 nrmW : NOMRAL; // normal in world space(for lighting)
///    float3 posW : WORLD; // position in world space(for lighting)
///};
///float4 main(OUTPUT_TO_RASTERIER outer) : SV_TARGET 
///{
///    //do light math here
///   outer.nrmW = normalize(outer.nrmW);
///    /// spec formula
///   float3 viewdirection = normalize(camPos.xyz - outer.posW);
///   float3 halfVec = normalize(-lightDirection + viewdirection);
///   float intensity = max(pow(clamp(dot(outer.nrmW, halfVec), 0, 1), data.Ns), 0);
///  // Directional light math
///   float lightratio = clamp(dot(normalize(-lightDirection), outer.nrmW), 0, 1);
///   /// ambientTerm
///   lightratio = clamp(lightratio + SunAmbient, 0, 1);
///   float4 result = lightratio * float4(lightColor, 1) * float4(data.Kd, 1);
///   /// Reflectlight
///   float4 reflection = float4(lightColor, 1) * float4(data.Ks,1) * intensity ;/////float4(intensity,0,,1); //
///   return  result + reflection; ////float4(data.Kd,1);
///}
///)";
////// OOP FORM OF RENDERING														  
///struct Model_data
///{
///	GW::MATH::GMATRIXF WorldMatrix;
///	H2B::MATERIAL material;
///
///}m;
///class Model
///{
///public:
///	GW::SYSTEM::GWindow win;
///	GW::GRAPHICS::GDirectX11Surface d3d;
///	GW::MATH::GMatrix m_Create;
///
///	Microsoft::WRL::ComPtr<ID3D11Buffer>		ModelBuffer = nullptr;
///	Microsoft::WRL::ComPtr<ID3D11Buffer>		VertiBuffer = nullptr;
///	Microsoft::WRL::ComPtr<ID3D11Buffer>		IndicBuffer = nullptr;
///	Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertShader = nullptr;
///	Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixeShader = nullptr;
///	Microsoft::WRL::ComPtr<ID3D11InputLayout>	vertFormat = nullptr;
///	H2B::Parser readTheModel;
///	float aspect;
///	float zNear = 0.1;
///	float zFar = 100;
///	bool loadModel(std::string pathToH2B)
///	{
///		//transfer from the cpu to gpu aka buffers		
///		if (readTheModel.Parse(pathToH2B.c_str()))
///		{
///			return true;
///		}
///		else
///		{
///			std::cout << "nothing was found" << "\n";
///			return false;
///		}
///	}
///	Model() ///Constructor
///	{
///		GW::SYSTEM::GWindow _win;
///		GW::GRAPHICS::GDirectX11Surface _d3d;
///		win = _win;
///		d3d = _d3d;
///		ID3D11Device* creator;
///		d3d.GetDevice((void**)&creator);
///		d3d.GetAspectRatio(aspect);
///		///use the verts from the .h obj file
///		// Create Buffers
///		//for (size_t i = 0; i < readTheModel.meshCount; i++)
///		{
///
///			///VERTICES buffer
///			D3D11_SUBRESOURCE_DATA vData = { readTheModel.vertices.data(), 0, 0 };
///			CD3D11_BUFFER_DESC vertDesc(sizeof(H2B::VERTEX) * readTheModel.vertexCount, D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE); ///VERT
///			creator->CreateBuffer(&vertDesc, &vData, VertiBuffer.GetAddressOf());
///			///INDEX buffer
///			D3D11_SUBRESOURCE_DATA iData = { readTheModel.indices.data(), 0, 0 };
///			CD3D11_BUFFER_DESC verDesc(sizeof(unsigned int) * readTheModel.indexCount, D3D11_BIND_INDEX_BUFFER); ///INDEX BUFF
///			creator->CreateBuffer(&verDesc, &iData, IndicBuffer.GetAddressOf());
///			///Model Buffer
///			CD3D11_BUFFER_DESC ModelDesc(sizeof(m), D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);///Model
///			creator->CreateBuffer(&ModelDesc, nullptr, ModelBuffer.GetAddressOf());
///		}
///		UINT compilerFlags = D3DCOMPILE_ENABLE_STRICTNESS;
///#if _DEBUG
///		compilerFlags |= D3DCOMPILE_DEBUG;
///#endif
///		{
///			Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, errors;
///			if (SUCCEEDED(D3DCompile(vertexShaderSource, strlen(vertexShaderSource),///vertexShaderSource.c_str(), vertexShaderSource.length() if doing extren hlsl files.
///				nullptr, nullptr, nullptr, "main", "vs_4_0", compilerFlags, 0,
///				vsBlob.GetAddressOf(), errors.GetAddressOf())))
///			{
///				creator->CreateVertexShader(vsBlob->GetBufferPointer(),
///					vsBlob->GetBufferSize(), nullptr, vertShader.GetAddressOf());
///			}
///			else
///				std::cout << (char*)errors->GetBufferPointer() << std::endl;
///			// Create Pixel Shader
///			Microsoft::WRL::ComPtr<ID3DBlob> psBlob; errors.Reset();
///			if (SUCCEEDED(D3DCompile(pixelShaderSource, strlen(pixelShaderSource),
///				nullptr, nullptr, nullptr, "main", "ps_4_0", compilerFlags, 0,
///				psBlob.GetAddressOf(), errors.GetAddressOf())))
///			{
///				creator->CreatePixelShader(psBlob->GetBufferPointer(),
///					psBlob->GetBufferSize(), nullptr, pixeShader.GetAddressOf());
///			}
///			else
///				std::cout << (char*)errors->GetBufferPointer() << std::endl;
///			// Create Input Layout
///			D3D11_INPUT_ELEMENT_DESC format[] =
///			{
///			   {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
///				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
///			   {"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
///				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
///			   {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
///				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
///			   {"WORLD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
///				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
///			};
///			creator->CreateInputLayout(format, ARRAYSIZE(format), vsBlob->GetBufferPointer(),
///				vsBlob->GetBufferSize(), vertFormat.GetAddressOf());
///		}
///		// free temporary handle
///		creator->Release();
///	}
///	void DrawModel()
///	{
///		for (int i = 0; i < readTheModel.meshCount; i++)
///		{
///			ID3D11DeviceContext* con;
///			ID3D11RenderTargetView* view;
///			ID3D11DepthStencilView* depth;
///			d3d.GetImmediateContext((void**)&con);
///			d3d.GetRenderTargetView((void**)&view);
///			d3d.GetDepthStencilView((void**)&depth);
///			/// setup the pipeline
///			ID3D11RenderTargetView* const views[] = { view };
///			con->OMSetRenderTargets(ARRAYSIZE(views), views, depth);
///			D3D11_MAPPED_SUBRESOURCE gpuBuffer;
///			ID3D11Buffer* ModelPipleLine[] = { ModelBuffer.Get() };	   //get the addy of the buffers
///			con->VSSetConstantBuffers(0, 1, ModelPipleLine);		///set the pix and vert const buffers
///			con->PSSetConstantBuffers(0, 1, ModelPipleLine);		///set the pix and vert const buffers
///			m.material = readTheModel.materials[i];
///			con->Map(ModelBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
///			memcpy(gpuBuffer.pData, &m, sizeof(m));
///			con->Unmap(ModelBuffer.Get(), 0);
///			con->DrawIndexed(readTheModel.meshes[i].drawInfo.indexCount, readTheModel.meshes[i].drawInfo.indexOffset, 0);
///		}
///	}
///};