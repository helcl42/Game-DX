#ifndef ENGINE_H
#define ENGINE_H


#define DIRECTINPUT_VERSION 0x0800


#include <stdio.h>
#include <tchar.h>
#include <windowsx.h>
#include <iostream>

#include <d3dx9.h>
#include <dinput.h>
#include <dplay8.h>
#include <dmusici.h>


#define SAFE_DELETE( p )       { if( p ) { delete ( p );     ( p ) = NULL; } }
#define SAFE_DELETE_ARRAY( p ) { if( p ) { delete[] ( p );   ( p ) = NULL; } }
#define SAFE_RELEASE( p )      { if( p ) { ( p )->Release(); ( p ) = NULL; } }


#include "resource.h"
#include "LinkedList.h"
#include "ResourceManagement.h"
#include "Geometry.h"
#include "Font.h"
#include "Scripting.h"
#include "DeviceEnumeration.h"
#include "Input.h"
#include "Network.h"
#include "SoundSystem.h"
#include "BoundingVolume.h"
#include "Material.h"
#include "Mesh.h"
#include "SceneObject.h"
#include "AnimatedObject.h"
#include "SpawnerObject.h"
#include "ViewFrustum.h"
#include "RenderCache.h"
#include "SceneManager.h"
#include "CollisionDetection.h"
#include "State.h"



//-----------------------------------------------------------------------------
// Engine Setup Structure
//-----------------------------------------------------------------------------
struct EngineSetup
{
	HINSTANCE	  instance; 	    // Application instance handle.
	GUID		  guid;		    // Application GUID.
	char*		  name;		    // Name of the application.
	float		  scale;            // Unit scale in meters/unit.
	unsigned char 	  totalBackBuffers; // Number of back buffers to use.
	char*		  spawnerPath;	    // Path for locating the spawner object scripts.

	void (*HandleNetworkMessage)( ReceivedMessage *msg ); // Network message handler.
	void (*StateSetup)();								  // State setup function.
	void (*CreateMaterialResource)( Material **resource, char *name, char *path ); // Material resource creation function.	

	//-------------------------------------------------------------------------
	// The engine setup structure constructor.
	//-------------------------------------------------------------------------
	EngineSetup()
	{
		GUID defaultGUID = { 0x24215591, 0x24c0, 0x4316, { 0xb5, 0xb2, 0x67, 0x98, 0x2c, 0xb3, 0x82, 0x54 } };

		instance = NULL;
		guid = defaultGUID;
		name = "Application";
		scale = 1.0f;
		totalBackBuffers = 1;
		HandleNetworkMessage = NULL;
		StateSetup = NULL;
		CreateMaterialResource = NULL;
		spawnerPath = "./";
	}
};

//-----------------------------------------------------------------------------
// Engine Class
//-----------------------------------------------------------------------------
class Engine
{
private:
	static Engine* m_instance;
	char	          m_fpsText[16];  	// Frame rate character string.
	Font*		  m_fpsFont;	  	// Font for rendering the frame rate.

	bool	          m_loaded;		// Indicates if the engine is loading.
	HWND		  m_window;		// Main window handle.
	bool	          m_deactive;    	// Indicates if the application is active or not.

	EngineSetup*      m_setup;		// Copy of the engine setup structure.
	IDirect3DDevice9* m_device;		// Direct3D device interface.
	D3DDISPLAYMODE	  m_displayMode; 	// Display mode of the current Direct3D device.
	ID3DXSprite*	  m_sprite;		// Sprite interface for rendering 2D textured primitives.
	unsigned char	  m_currentBackBuffer;  // Keeps track of which back buffer is at the front of the swap chain.	

	LinkedList<State>* m_states;		 // Linked list of states.
	State*		   m_currentState; 	 // Pointer to the current state.
	bool		   m_stateChanged; 	 // Indicates if the state changed in the current frame.

	ResourceManager<Script>*   m_scriptManager;   // Script manager.
	ResourceManager<Material>* m_materialManager; // Material manager.
	ResourceManager<Mesh>*	   m_meshManager;     // Mesh manager.

	Input*	      m_input;	      // Input object.
	Network*      m_network;      // Network object.
	SoundSystem*  m_soundSystem;  // Sound system.
	SceneManager* m_sceneManager; // Scene manager.

public:
	Engine( EngineSetup *setup = NULL );
	virtual ~Engine();

	void	       Run();
	static Engine* GetInstance();

	HWND GetWindow();
	void SetDeactiveFlag( bool deactive );

	float		  GetScale();
	IDirect3DDevice9* GetDevice();
	D3DDISPLAYMODE*   GetDisplayMode();
	ID3DXSprite*	  GetSprite();

	void   AddState( State *state, bool change = true );
	void   RemoveState( State *state );
	void   ChangeState( unsigned long id );
	State* GetCurrentState();

	ResourceManager<Script>*   GetScriptManager();
	ResourceManager<Material>* GetMaterialManager();
	ResourceManager<Mesh>*	   GetMeshManager();

	Input*	      GetInput();
	Network*      GetNetwork();
	SoundSystem*  GetSoundSystem();
	SceneManager* GetSceneManager();
};


//-----------------------------------------------------------------------------
// Externals
//-----------------------------------------------------------------------------
extern Engine *g_engine;

#endif
