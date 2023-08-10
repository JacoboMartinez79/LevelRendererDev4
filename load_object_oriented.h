// This is a sample of how to load a level in a object oriented fashion.
// Feel free to use this code as a base and tweak it for your needs.

// This reads .h2b files which are optimized binary .obj+.mtl files
#include "h2bParser.h"
#include "Gateware.h"
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")
class Model {
	// Loads and stores CPU model data from .h2b file
	H2B::Parser cpuModel; // reads the .h2b format
	// Shader variables needed by this model. 
	GW::MATH::GMATRIXF world;
	// TODO: Add matrix/light/etc vars..
	//struct Scene_data///this will be mapped and unmapped once
	//{
	//	GW::MATH::GMATRIXF viewMatrix;
	//	GW::MATH::GMATRIXF pMatrix;
	//	//GW::MATH::GVECTORF LightDirection;
	//	GW::MATH::GVECTORF LightColor;
	//	GW::MATH::GVECTORF SunAmbient = { 0.25f, 0.25f, 0.35f, 1 };
	//	GW::MATH::GVECTORF camPos = { { 0.75f, 0.25f,-1.50f, 1 } };
	//}Scene;
	//struct Model_data///this will be mapped and unmapped multiple times
	//{
	//	GW::MATH::GMATRIXF WorldMatrix;
	//	H2B::ATTRIBUTES attributes;
	//}m;
	//float aspect;
	//GW::MATH::GVECTORF center = { 0.15,0.75f,0,1 };// this is my view
	//GW::MATH::GVECTORF eye = { 0.75f, 0.25f,-1.50f, 1 };//this the eye
	//GW::MATH::GVECTORF up = { 0,1,0,0 };// moving camera up
	//// TODO: API Rendering vars here (unique to this model)
	//Microsoft::WRL::ComPtr<ID3D11Buffer>		SceneBuffer; /// buffer instance
	//Microsoft::WRL::ComPtr<ID3D11Buffer>		ModelBuffer; /// buffer instance
	///// buffers for the indices and vert
	//Microsoft::WRL::ComPtr<ID3D11VertexShader>	vertexShader;/// shader
	//Microsoft::WRL::ComPtr<ID3D11PixelShader>	pixelShader;/// shader
	//Microsoft::WRL::ComPtr<ID3D11InputLayout>	vertexFormat;
	//// Vertex Buffer
	//Microsoft::WRL::ComPtr<ID3D11Buffer>		VerticesBuffer;///vert buffer
	//// Index Buffer
	//Microsoft::WRL::ComPtr<ID3D11Buffer>		IndicesBuffer;/// index buffer
	//
	//GW::SYSTEM::GWindow win;
	//GW::MATH::GMatrix m_Create;
	//GW::GRAPHICS::GDirectX11Surface d3d;
	//// Pipeline/State Objects
	//ID3D11Device* creator;
	//ID3D11DeviceContext* con;
	//ID3D11RenderTargetView* view;
	//ID3D11DepthStencilView* depth;
	// Uniform/ShaderVariable Buffer
	
	// Vertex/Pixel Shaders

public:
	inline void SetWorldMatrix(GW::MATH::GMATRIXF worldMatrix) {
		world = worldMatrix;
	}

	bool LoadModelDataFromDisk(const char* h2bPath) {
		// if this succeeds "cpuModel" should now contain all the model's info

		return cpuModel.Parse(h2bPath);
	}
	bool UploadModelData2GPU(GW::SYSTEM::GWindow window, GW::GRAPHICS::GDirectX11Surface _d3d)
	{

		//ID3D11Device* creator;
		//d3d.GetDevice((void**)&creator);
		//d3d.GetAspectRatio(aspect);
////		// TODO: Use chosen API to upload this model's graphics data to GPU
////
////
////		UINT compilerFlags = D3DCOMPILE_ENABLE_STRICTNESS;
////#if _DEBUG
////		compilerFlags |= D3DCOMPILE_DEBUG;
////#endif
///	{
///		//Create Vertex Shader 
///		Microsoft::WRL::ComPtr<ID3DBlob> vsBlob, errors;
///		if (SUCCEEDED(D3DCompile(vertexShaderSource, strlen(vertexShaderSource),///vertexShaderSource.c_str(), vertexShaderSource.length() if doing extren hlsl files.
///			nullptr, nullptr, nullptr, "main", "vs_4_0", compilerFlags, 0,
///			vsBlob.GetAddressOf(), errors.GetAddressOf())))
///		{
///			creator->CreateVertexShader(vsBlob->GetBufferPointer(),
///				vsBlob->GetBufferSize(), nullptr, vertexShader.GetAddressOf());
///		}
///		else
///			std::cout << (char*)errors->GetBufferPointer() << std::endl;
///		// Create Pixel Shader
///		Microsoft::WRL::ComPtr<ID3DBlob> psBlob; errors.Reset();
///		if (SUCCEEDED(D3DCompile(pixelShaderSource, strlen(pixelShaderSource),
///			nullptr, nullptr, nullptr, "main", "ps_4_0", compilerFlags, 0,
///			psBlob.GetAddressOf(), errors.GetAddressOf())))
///		{
///			creator->CreatePixelShader(psBlob->GetBufferPointer(),
///				psBlob->GetBufferSize(), nullptr, pixelShader.GetAddressOf());
///		}
///		else
///			std::cout << (char*)errors->GetBufferPointer() << std::endl;
///
///		// Create Input Layout
///		D3D11_INPUT_ELEMENT_DESC format[] =
///		{
///		   {
///			"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
///			D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
///			   },
///			   {
///				"TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
///				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
///			   },
///			   {
///				"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
///				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
///			   },
///			   {
///				"WORLD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
///				D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0
///			   }
///			};
///			creator->CreateInputLayout(format, ARRAYSIZE(format), vsBlob->GetBufferPointer(),
///				vsBlob->GetBufferSize(), vertexFormat.GetAddressOf());
///		}
///		// free temporary handle
///		creator->Release();
///	
///		const UINT strides[] = { sizeof(float) };
///		const UINT offsets[] = { 0 };
///		///Connect the buffers
///		ID3D11Buffer* const buffs[] = { VerticesBuffer.Get() };
///		con->IASetVertexBuffers(0, ARRAYSIZE(buffs), buffs, strides, offsets);///Vertex buff
///		con->IASetIndexBuffer(IndicesBuffer.Get(), DXGI_FORMAT_R32_UINT, 0); ///  index buff
///		con->VSSetShader(vertexShader.Get(), nullptr, 0);///Setting shaders
///		con->PSSetShader(pixelShader.Get(), nullptr, 0);	///Setting shaders
///		con->IASetInputLayout(vertexFormat.Get());///Setting Format
///		con->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
///
///		D3D11_MAPPED_SUBRESOURCE gpuBuffer;
///		con->Map(SceneBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gpuBuffer);
///		memcpy(gpuBuffer.pData, &Scene, sizeof(Scene));
///		con->Unmap(SceneBuffer.Get(), 0);

		return true;
	}
	bool DrawModel(   /*specific API command list or context*/) {
		// TODO: Use chosen API to setup the pipeline for this model and draw it
		//for (size_t i = 0; i < ; i++)
		//{
		//	con->Draw();
		//
		//}
		
	}
	bool FreeResources() {
		// TODO: Use chosen API to free all GPU resources used by this model
		//creator->Release();
		//depth->Release();
		//view->Release();
		//con->Release();
	}
};


class Level_Objects {

	// store all our models
	std::list<Model> allObjectsInLevel;
	// TODO: This could be a good spot for any global data like cameras or lights
	///GW::MATH::GVECTORF center = { 0.15,0.75f,0,1 };// this is my view
	///GW::MATH::GVECTORF eye = { 0.75f, 0.25f,-1.50f, 1 };//this the eye
	///GW::MATH::GVECTORF up = { 0,1,0,0 };// moving camera up
	///GW::MATH::GVECTORF camPos = {};
	///Scene.LightColor = { 0.9f,0.9f,1,1 };
	///m_Create.LookAtLHF(eye, center, up, Scene.viewMatrix);///view matrix is setup
	///m_Create.ProjectionDirectXLHF(G_DEGREE_TO_RADIAN_F(65), aspect, zNear, zFar, Scene.pMatrix);
public:
	
	// Imports the default level txt format and creates a Model from each .h2b
	bool LoadLevel(	const char* gameLevelPath, const char* h2bFolderPath, GW::SYSTEM::GLog log) {
		
		// What this does:
		// Parse GameLevel.txt 
		// For each model found in the file...
			// Create a new Model class on the stack.
				// Read matrix transform and add to this model.
				// Load all CPU rendering data for this model from .h2b
			// Add the newly found Model to our list of total models for the level 

		log.LogCategorized("EVENT", "LOADING GAME LEVEL [OBJECT ORIENTED]");
		log.LogCategorized("MESSAGE", "Begin Reading Game Level Text File.");

		UnloadLevel();// clear previous level data if there is any
		GW::SYSTEM::GFile file;
		file.Create();
		if (-file.OpenTextRead(gameLevelPath)) {
			log.LogCategorized(
				"ERROR", (std::string("Game level not found: ") + gameLevelPath).c_str());
			return false;
		}
		char linebuffer[1024];
		while (+file.ReadLine(linebuffer, 1024, '\n'))
		{
			// having to have this is a bug, need to have Read/ReadLine return failure at EOF
			if (linebuffer[0] == '\0')
				break;
			if (std::strcmp(linebuffer, "MESH") == 0)
			{
				file.ReadLine(linebuffer, 1024, '\n');
				log.LogCategorized("INFO", (std::string("Model Detected: ") + linebuffer).c_str());
				// create the model file name from this (strip the .001)
				std::string modelFile = linebuffer;
				modelFile = modelFile.substr(0, modelFile.find_last_of("."));
				modelFile += ".h2b";

				// now read the transform data as we will need that regardless
				GW::MATH::GMATRIXF transform;
				for (int i = 0; i < 4; ++i) {
					file.ReadLine(linebuffer, 1024, '\n');
					// read floats
					std::sscanf(linebuffer + 13, "%f, %f, %f, %f",
						&transform.data[0 + i * 4], &transform.data[1 + i * 4],
						&transform.data[2 + i * 4], &transform.data[3 + i * 4]);
				}
				std::string loc = "Location: X ";
				loc += std::to_string(transform.row4.x) + " Y " +
					std::to_string(transform.row4.y) + " Z " + std::to_string(transform.row4.z);
				log.LogCategorized("INFO", loc.c_str());

				// Add new model to list of all Models
				log.LogCategorized("MESSAGE", "Begin Importing .H2B File Data.");
				Model newModel;
				modelFile = std::string(h2bFolderPath) + "/" + modelFile;
				newModel.SetWorldMatrix(transform);
				// If we find and load it add it to the level
				if (newModel.LoadModelDataFromDisk(modelFile.c_str())) {
					// add to our level objects
					allObjectsInLevel.push_back(newModel);
					log.LogCategorized("INFO", (std::string("H2B Imported: ") + modelFile).c_str());
				}
				else {
					// notify user that a model file is missing but continue loading
					log.LogCategorized("ERROR",
						(std::string("H2B Not Found: ") + modelFile).c_str());
					log.LogCategorized("WARNING", "Loading will continue but model(s) are missing.");
				}
				log.LogCategorized("MESSAGE", "Importing of .H2B File Data Complete.");
			}
		}
		log.LogCategorized("MESSAGE", "Game Level File Reading Complete.");
		// level loaded into CPU ram
		log.LogCategorized("EVENT", "GAME LEVEL WAS LOADED TO CPU [OBJECT ORIENTED]");
		return true;
	}
	// Draws all objects in the level
	void RenderLevel() 
	{
		// iterate over each model and tell it to draw itself
		for (auto e : allObjectsInLevel) 
		{
			e.DrawModel(/*pass any needed global info.(ex:camera)*/);
		}
	}
	// used to wipe CPU & GPU level data between levels
	void UnloadLevel() {
		allObjectsInLevel.clear();
	}
	// *THIS APPROACH COMBINES DATA & LOGIC* 
	// *WITH THIS APPROACH THE CURRENT RENDERER SHOULD BE JUST AN API MANAGER CLASS*
	// *ALL ACTUAL GPU LOADING AND RENDERING SHOULD BE HANDLED BY THE MODEL CLASS* 
	// For example: anything that is not a global API object should be encapsulated.
};

