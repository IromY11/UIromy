

#include<d3dx9.h>
#include<iostream>

#include<windows.h>
#include <SDL.h>
#include<CEGUI/CEGUI.h>
#include<CEGUI/RendererModules/Direct3D9/Renderer.h>
#undef main

/*#ifdef _MSC_VER
# if defined(DEBUG) || defined (_DEBUG)
#   pragma comment (lib, "CEGUIDirect3D9Renderer-0_d.lib")
#   pragma comment (lib, "CEGUIBase-0_d.lib")
# else
#   pragma comment (lib, "CEGUIDirect3D9Renderer-0.lib")
#   pragma comment (lib, "CEGUIBase-0.lib")
# endif
#endif*/


using namespace std;
using namespace CEGUI;


string root = "C:/Users/Iromys/Documents/GitHub/UIromy/cegui-ceed/data/samples/datafiles0_8/";
void initialiseResourceGroupDirectories(){
	CEGUI::DefaultResourceProvider* rp = static_cast<CEGUI::DefaultResourceProvider*>
		(CEGUI::System::getSingleton().getResourceProvider());
	rp->setResourceGroupDirectory("schemes", root+"schemes/");
	rp->setResourceGroupDirectory("imagesets", root+"imagesets/");
	rp->setResourceGroupDirectory("fonts", root+"fonts/");
	rp->setResourceGroupDirectory("layouts", root+"layouts/");
	rp->setResourceGroupDirectory("looknfeels", root+"looknfeel/");
	rp->setResourceGroupDirectory("lua_scripts", root+"lua_scripts/");
	rp->setResourceGroupDirectory("animations", root+"animations/");
	rp->setResourceGroupDirectory("schemas", root+"xml_schemas/");
}

void initialiseDefaultResourceGroups(){
	
	CEGUI::ImageManager::setImagesetDefaultResourceGroup("imagesets");
	CEGUI::Font::setDefaultResourceGroup("fonts");
	CEGUI::Scheme::setDefaultResourceGroup("schemes");
	CEGUI::WidgetLookManager::setDefaultResourceGroup("looknfeels");
	CEGUI::WindowManager::setDefaultResourceGroup("layouts");
	CEGUI::ScriptModule::setDefaultResourceGroup("lua_scripts");
	CEGUI::AnimationManager::setDefaultResourceGroup("animations");
	CEGUI::XMLParser* parser = CEGUI::System::getSingleton().getXMLParser();
    if (parser->isPropertyPresent("SchemaDefaultResourceGroup"))
        parser->setProperty("SchemaDefaultResourceGroup", "schemas");
}



// this is the type used to describe a single vertex
// each vertex has x,y,z coords and a color associated with it
struct D3DVERTEX
{
	float fX,fY,fZ;
	DWORD dwColor;
};

SDL_Event               event;              //used to poll for events and handle input
LPDIRECT3D9             Direct3D_object = NULL;    //used to create a direct 3d device
LPDIRECT3DDEVICE9       Direct3D_device = NULL;    //basic rendering object
D3DPRESENT_PARAMETERS   present_parameters; //stores the important attributes and 
D3DXMATRIX              projection_matrix;  //   properties your Direct3D_device will have
LPDIRECT3DVERTEXBUFFER9 tri_buffer = NULL;  //data buffer which the Direct3D_device can draw from
VOID* pData;                                //pointer to beginning of vertex buffer
//actual data to be fed to the vertex buffer
D3DVERTEX aTriangle[ ] = {{-2.0f,1.0f,10.0f,0xffff0000},
{-3.0f,-1.0f,10.0f,0xff00ff00},
{-1.0f,-1.0f,10.0f,0xff0000ff}};

int main(int argc, char *args[])
{
	SDL_Surface * screen;
	if( SDL_Init( SDL_INIT_VIDEO  ) < 0 || !SDL_GetVideoInfo() )
		return 0;
	screen = SDL_SetVideoMode( 800, 600, SDL_GetVideoInfo()->vfmt->BitsPerPixel, SDL_RESIZABLE );
	Direct3D_object = Direct3DCreate9(D3D_SDK_VERSION);
	if( Direct3D_object == NULL )
	{
		MessageBox(GetActiveWindow(),L"Could not create Direct3D Object",L"D3D_OBJ ERR",MB_OK);
		return 0;
	}
	SDL_WM_SetCaption("Iromy",NULL);
	ZeroMemory(&present_parameters, sizeof(present_parameters));
	present_parameters.Windowed = true;
	present_parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
	present_parameters.EnableAutoDepthStencil = true;
	present_parameters.AutoDepthStencilFormat = D3DFMT_D16;
	present_parameters.hDeviceWindow = GetActiveWindow();
	present_parameters.BackBufferWidth = 800;
	present_parameters.BackBufferHeight = 600;
	present_parameters.BackBufferFormat = D3DFMT_R5G6B5;
	present_parameters.MultiSampleType = D3DMULTISAMPLE_NONE;
	if( FAILED(Direct3D_object->CreateDevice(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,
		GetActiveWindow(), D3DCREATE_SOFTWARE_VERTEXPROCESSING,
		&present_parameters,&Direct3D_device)))
	{
		MessageBox(GetActiveWindow(),L"Could not create Direct3D Device",L"D3D_DEV ERR",MB_OK);
		return 0;
	}
	D3DXMatrixPerspectiveFovLH(&projection_matrix, D3DX_PI / 4.0f,(float) 800/600,1, 1000);
	Direct3D_device->SetTransform(D3DTS_PROJECTION,&projection_matrix);
	Direct3D_device->SetRenderState(D3DRS_AMBIENT,RGB(255,255,255));

	Direct3D_device->SetRenderState(D3DRS_LIGHTING,false);

	Direct3D_device->SetRenderState(D3DRS_CULLMODE,D3DCULL_NONE);

	Direct3D_device->SetRenderState(D3DRS_ZENABLE,D3DZB_TRUE);
	Direct3D_device->SetFVF((D3DFVF_XYZ | D3DFVF_DIFFUSE));
	Direct3D_device->CreateVertexBuffer(sizeof(aTriangle),D3DUSAGE_WRITEONLY,
		(D3DFVF_XYZ | D3DFVF_DIFFUSE),
		D3DPOOL_MANAGED,&tri_buffer,NULL);
	tri_buffer->Lock(0,sizeof(pData),(void**)&pData,0);
	memcpy(pData,aTriangle,sizeof(aTriangle));
	tri_buffer->Unlock();

	CEGUI::Direct3D9Renderer& myRenderer = CEGUI::Direct3D9Renderer::bootstrapSystem( Direct3D_device );


	initialiseResourceGroupDirectories();
	initialiseDefaultResourceGroups();

	
	//cout<<WindowManager::getDefaultResourceGroup()<<endl;
	//cout<<System::getSingleton().getDefaultGUIContext().getDefaultFont()<<endl;
	
	SchemeManager::getSingleton().createFromFile( "vhdemo.scheme" );
	//FontManager::getSingleton().createFromFile( "vhdemo.font" );
	
 
	System::getSingleton().getDefaultGUIContext().setDefaultFont( "vhdemo" );
	//System::getSingleton().getDefaultGUIContext().getMouseCursor().setDefaultImage( "TaharezLook/MouseArrow" );
	//System::getSingleton().getDefaultGUIContext().setDefaultTooltipType( "TaharezLook/Tooltip" );
 
	WindowManager& wmgr = WindowManager::getSingleton();
	Window* myRoot = wmgr.createWindow( "DefaultWindow", "root" );
	System::getSingleton().getDefaultGUIContext().setRootWindow( myRoot );
	
	Window *nw = WindowManager::getSingleton().loadLayoutFromFile("vhdemo.layout","layouts");
	myRoot->addChild(nw);


	while( 1 )
	{
		Direct3D_device->Clear(0,NULL,D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER,
			D3DCOLOR_XRGB(0,150,125),1.0f,0);
		Direct3D_device->BeginScene();

		Direct3D_device->SetStreamSource(0,tri_buffer,0,sizeof(D3DVERTEX));
		Direct3D_device->DrawPrimitive(D3DPT_TRIANGLELIST,0,1);
		CEGUI::System::getSingleton().renderAllGUIContexts();
		Direct3D_device->EndScene();

		Direct3D_device->Present(NULL,NULL,NULL,NULL);

		while( SDL_PollEvent( &event ) )
			switch( event.type )
		{
			case SDL_KEYDOWN:
				if ( event.key.keysym.sym == SDLK_ESCAPE )
				{
					Direct3D_device->Release();
					Direct3D_device = NULL;
					Direct3D_object->Release();
					Direct3D_object = NULL;
					return 0;
				}
				break;
			case SDL_QUIT:
				Direct3D_device->Release();
				Direct3D_device = NULL;
				Direct3D_object->Release();
				Direct3D_object = NULL;
				SDL_Quit();
				return 0;
				break;
		}
	}

	return 0;
}