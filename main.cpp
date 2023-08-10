// Simple basecode showing how to create a window and attatch a d3d11surface
#define GATEWARE_ENABLE_CORE // All libraries need this
#define GATEWARE_ENABLE_SYSTEM // Graphics libs require system level libraries
#define GATEWARE_ENABLE_GRAPHICS // Enables all Graphics Libraries
#define GATEWARE_ENABLE_MATH
#define GATEWARE_ENABLE_GDIRECTX11SURFACE 
//Ignore some GRAPHICS libraries we aren't going to use
#define GATEWARE_DISABLE_GRASTERSURFACE // we have another template for this
#define GATEWARE_DISABLE_GOPENGLSURFACE // we have another template for this
#define GATEWARE_DISABLE_GVULKANSURFACE // we have another template for this
// With what we want & what we don't defined we can include the API
#include "Gateware.h"
#include "renderer.h" // example rendering code (not Gateware code!)

// open some namespaces to compact the code a bit
using namespace GW;
using namespace CORE;
using namespace SYSTEM;
using namespace GRAPHICS;
// lets pop a window and use D3D11 to clear to a green screen
int main()
{
	///FileIO();
	GWindow win;
	GEventResponder msgs;
	GDirectX11Surface d3d11;
	GW::SYSTEM::GLog logger;
	logger.Create("../GameLevelThird.txt");
	logger.EnableConsoleLogging(true); // mirror output to the console
	logger.Log("Start Program.");
	Level_Data object;
	if (+win.Create(0, 0, 800, 600, GWindowStyle::WINDOWEDBORDERED))
	{
		float clr[] = { 100 / 255.0f, 0 / 255.0f, 0 / 255.0f, 1 };
		msgs.Create([&](const GW::GEvent& e) {GW::SYSTEM::GWindow::Events q;
		if (+e.Read(q) && q == GWindow::Events::RESIZE)
			clr[2] += 0.01f; });
		win.Register(msgs);
		if (+d3d11.Create(win, GW::GRAPHICS::DEPTH_BUFFER_SUPPORT))
		{
			object.LoadLevel("../GameLevelThird.txt", "../Models", logger);
			GameLevelRenderer renderer(win, d3d11, object);
			while (+win.ProcessWindowEvents())
			{
				IDXGISwapChain* swap;
				ID3D11DeviceContext* con;
				ID3D11RenderTargetView* view;
				ID3D11DepthStencilView* depth;
				if (+d3d11.GetImmediateContext((void**)&con) && +d3d11.GetRenderTargetView((void**)&view) && +d3d11.GetDepthStencilView((void**)&depth) && +d3d11.GetSwapchain((void**)&swap))
				{
					con->ClearRenderTargetView(view, clr);
					con->ClearDepthStencilView(depth, D3D11_CLEAR_DEPTH, 1, 0);
					swap->Present(1, 0);
					///Level Render through data 
					renderer.drawGameLevel();
					swap->Release();
					view->Release();
					depth->Release();
					con->Release();
				}
			}
		}
	}
	return 0;
}