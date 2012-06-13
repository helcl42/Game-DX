#include "Engine.h"


Engine * Engine::m_instance = NULL;


//-----------------------------------------------------------------------------
// Handles Windows messages.
//-----------------------------------------------------------------------------
LRESULT CALLBACK WindowProc( HWND wnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
	switch( msg )
	{
		case WM_ACTIVATEAPP:
			Engine::GetInstance()->SetDeactiveFlag( !wparam );
			return 0;

		case WM_DESTROY:
			PostQuitMessage( 0 );
			return 0;

		default:
			return DefWindowProc( wnd, msg, wparam, lparam );
	}
}

//-----------------------------------------------------------------------------
// The engine class constructor.
//-----------------------------------------------------------------------------
Engine::Engine( EngineSetup *setup )
{
	// Indicate that the engine is not yet loaded.
	m_loaded = false;

	// If no setup structure was passed in, then create a default one.
	// Otehrwise, make a copy of the passed in structure.
	m_setup = new EngineSetup;
	if( setup != NULL )
		memcpy( m_setup, setup, sizeof( EngineSetup ) );

	// Store a pointer to the engine in a global variable for easy access.
	m_instance = this;

	// Prepare and register the window class.
	WNDCLASSEX wcex;
	wcex.cbSize        = sizeof( WNDCLASSEX );
	wcex.style         = CS_CLASSDC;
	wcex.lpfnWndProc   = WindowProc;
	wcex.cbClsExtra    = 0;
	wcex.cbWndExtra    = 0;
	wcex.hInstance     = m_setup->instance;
	wcex.hIcon         = LoadIcon( NULL, IDI_APPLICATION );
	wcex.hCursor       = LoadCursor( NULL, IDC_ARROW );
	wcex.hbrBackground = NULL;
	wcex.lpszMenuName  = NULL;
	wcex.lpszClassName = "WindowClass";
	wcex.hIconSm       = LoadIcon( NULL, IDI_APPLICATION );
	RegisterClassEx( &wcex );

	// Initialise the COM using multithreaded concurrency.
	CoInitializeEx( NULL, COINIT_MULTITHREADED );

	// Create the Direct3D interface.
	IDirect3D9 *d3d = Direct3DCreate9( D3D_SDK_VERSION );

	// Enumerate Direct3D device configurations on the default adapter.
	DeviceEnumeration* deviceEnumeration = DeviceEnumeration::GetInstance();
	if( deviceEnumeration->Enumerate( d3d ) != IDOK )
	{
		SAFE_RELEASE( d3d );
		return;
	}

	// Create the window and retrieve a handle to it.
	m_window = CreateWindow( "WindowClass", m_setup->name, deviceEnumeration->IsWindowed() ? WS_OVERLAPPED : WS_POPUP, 0, 0, 800, 600, NULL, NULL, m_setup->instance, NULL );

	// Prepare the device presentation parameters.
	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory( &d3dpp, sizeof( D3DPRESENT_PARAMETERS ) );
	d3dpp.BackBufferWidth = deviceEnumeration->GetSelectedDisplayMode()->Width;
	d3dpp.BackBufferHeight = deviceEnumeration->GetSelectedDisplayMode()->Height;
	d3dpp.BackBufferFormat = deviceEnumeration->GetSelectedDisplayMode()->Format;
	d3dpp.BackBufferCount = m_setup->totalBackBuffers;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.hDeviceWindow = m_window;
	d3dpp.Windowed = deviceEnumeration->IsWindowed();
	d3dpp.EnableAutoDepthStencil = true;
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
	d3dpp.FullScreen_RefreshRateInHz = deviceEnumeration->GetSelectedDisplayMode()->RefreshRate;
	if( deviceEnumeration->IsVSynced() == true )
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	else
		d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	// Destroy the device enumeration object.
	SAFE_DELETE( deviceEnumeration );

	// Create the Direct3D device with hardware vertex processing. 
	if( FAILED( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &m_device ) ) )
	{ 
		// Create the Direct3D device with mixed vertex processing. 
		if( FAILED( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, D3DCREATE_MIXED_VERTEXPROCESSING, &d3dpp, &m_device ) ) ) 
		{ 
			// Create the Direct3D device with software vertex processing. 
			if( FAILED( d3d->CreateDevice( D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_window, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &m_device ) ) ) 
				return; 
			} 
	}

	// Release the Direct3D interface as it is no longer needed.
	SAFE_RELEASE( d3d );

	// Switch lighting off by default.
	m_device->SetRenderState( D3DRS_LIGHTING, false );

	// Set the texture filters to use anisotropic texture filtering.
	m_device->SetSamplerState ( 0, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC );
	m_device->SetSamplerState ( 0, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC );
	m_device->SetSamplerState( 0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR );

	// Set the projection matrix.
	D3DXMATRIX projMatrix;
	D3DXMatrixPerspectiveFovLH( &projMatrix, D3DX_PI / 4, (float)d3dpp.BackBufferWidth / (float)d3dpp.BackBufferHeight, 0.1f / m_setup->scale, 1000.0f / m_setup->scale );
	m_device->SetTransform( D3DTS_PROJECTION, &projMatrix );

	// Store the display mode details.
	m_displayMode.Width = d3dpp.BackBufferWidth;
	m_displayMode.Height = d3dpp.BackBufferHeight;
	m_displayMode.RefreshRate = d3dpp.FullScreen_RefreshRateInHz;
	m_displayMode.Format = d3dpp.BackBufferFormat;

	// The swap chain always starts on the first back buffer.
	m_currentBackBuffer = 0;

	// Create the sprite interface.
	D3DXCreateSprite( m_device, &m_sprite );

	// Create the font to display the frame rate.
	m_fpsFont = new Font();

	// Create the linked lists of states.
	m_states = new LinkedList< State >;
	m_currentState = NULL;

	// Create the resource managers.
	m_scriptManager = new ResourceManager< Script >;
	m_materialManager = new ResourceManager< Material >( m_setup->CreateMaterialResource );
	m_meshManager = new ResourceManager< Mesh >;

	// Create the input object.
	m_input = new Input( m_window );

	// Create the network object.
	m_network = new Network( m_setup->guid, m_setup->HandleNetworkMessage );

	// Create the sound system.
	m_soundSystem = new SoundSystem( m_setup->scale );

	// Create the scene manager.
	m_sceneManager = new SceneManager( m_setup->scale, m_setup->spawnerPath );

	// Seed the random number generator with the current time.
	srand( timeGetTime() );

	// Allow the application to perform any state setup now.
	if( m_setup->StateSetup != NULL )
		m_setup->StateSetup();

	// The engine is fully loaded and ready to go.
	m_loaded = true;
}

//-----------------------------------------------------------------------------
// The engine class destructor.
//-----------------------------------------------------------------------------
Engine::~Engine()
{
	// Ensure the engine is loaded.
	if( m_loaded == true )
	{
		// Destroy the states linked lists.
		if( m_currentState != NULL )
			m_currentState->Close();
		SAFE_DELETE( m_states );

		// Destroy everything.
		SAFE_DELETE( m_sceneManager );
		SAFE_DELETE( m_soundSystem );
		SAFE_DELETE( m_network );
		SAFE_DELETE( m_input );
		SAFE_DELETE( m_meshManager );
		SAFE_DELETE( m_materialManager );
		SAFE_DELETE( m_scriptManager );

		// Destroy the font used for the frame rate.
		SAFE_DELETE( m_fpsFont );

		// Release the sprite interface.
		SAFE_RELEASE( m_sprite );

		// Release the device.
		SAFE_RELEASE( m_device );
	}

	// Uninitialise the COM.
	CoUninitialize();

	// Unregister the window class.
	UnregisterClass( "WindowClass", m_setup->instance );

	// Destroy the engine setup structure.
	SAFE_DELETE( m_setup );
}


Engine* Engine::GetInstance()
{
	if(!m_instance)
		m_instance = new Engine();
	return m_instance;
}


//-----------------------------------------------------------------------------
// Enters the engine into the main processing loop.
//-----------------------------------------------------------------------------
void Engine::Run()
{
	// Ensure the engine is loaded.
	if( m_loaded == true )
	{
		// Show the window.
		ShowWindow( m_window, SW_NORMAL );

		// Used to retrieve details about the viewer from the application.
		ViewerSetup viewer;

		// Enter the message loop.
		MSG msg;
		ZeroMemory( &msg, sizeof( MSG ) );
		while( msg.message != WM_QUIT )
		{
			if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
			{
				TranslateMessage( &msg );
				DispatchMessage( &msg );
			}
			else if( !m_deactive )
			{
				// Calculate the elapsed time.
				unsigned long currentTime = timeGetTime();
				static unsigned long lastTime = currentTime;
				float elapsed = ( currentTime - lastTime ) / 1000.0f;
				lastTime = currentTime;

				// Calculate the frame rate.
				static float frameTime = 1.0f;
				static int frameCount = 0;
				frameTime += elapsed;
				frameCount++;

				// Update the fps every half a second.
				if( frameTime > 1.0f )
				{
					sprintf( m_fpsText, "%d", frameCount );

					frameTime = 0.0f;
					frameCount = 0;
				}

				// Update the network object, processing any pending messages.
				m_network->Update();

				// Update the input object, reading the keyboard and mouse.
				m_input->Update();

				// Check if the user wants to make a forced exit.
				if( m_input->GetKeyPress( DIK_F1 ) )
					PostQuitMessage( 0 );

				// Request the viewer from the current state, if there is one.
				if( m_currentState != NULL )
					m_currentState->RequestViewer( &viewer );

				// Ensure the viewer is valid.
				if( viewer.viewer != NULL )
				{
					// Update the scene.
					m_sceneManager->Update( elapsed, viewer.viewer->GetViewMatrix() );

					// Set the view transformation.
					m_device->SetTransform( D3DTS_VIEW, viewer.viewer->GetViewMatrix() );

					// Update the 3D sound listener.
					m_soundSystem->UpdateListener( viewer.viewer->GetForwardVector(), viewer.viewer->GetTranslation(), viewer.viewer->GetVelocity() );
				}

				// Update the current state (if there is one), taking state
				// changes into account.
				m_stateChanged = false;
				if( m_currentState != NULL )
					m_currentState->Update( elapsed );
				if( m_stateChanged == true )
					continue;

				// Begin the scene.
				m_device->Clear( 0, NULL, viewer.viewClearFlags, 0, 1.0f, 0 );
				if( SUCCEEDED( m_device->BeginScene() ) )
				{
					// Render the scene, if there is a valid viewer.
					if( viewer.viewer != NULL )
						m_sceneManager->Render( elapsed, viewer.viewer->GetTranslation() );

					// Render the current state, if there is one.
					if( m_currentState != NULL )
						m_currentState->Render();

					// Render the frame rate.
					m_fpsFont->Render( m_fpsText, 0, 0 );

					// End the scene and present it.
					m_device->EndScene();
					m_device->Present( NULL, NULL, NULL, NULL );

					// Keep track of the index of the current back buffer.
					if( ++m_currentBackBuffer == m_setup->totalBackBuffers + 1 )
						m_currentBackBuffer = 0;
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Returns the window handle.
//-----------------------------------------------------------------------------
HWND Engine::GetWindow()
{
	return m_window;
}

//-----------------------------------------------------------------------------
// Sets the deactive flag.
//-----------------------------------------------------------------------------
void Engine::SetDeactiveFlag( bool deactive )
{
	m_deactive = deactive;
}

//-----------------------------------------------------------------------------
// Returns the scale that the engine is currently running in.
//-----------------------------------------------------------------------------
float Engine::GetScale()
{
	return m_setup->scale;
}

//-----------------------------------------------------------------------------
// Returns a pointer to the Direct3D device.
//-----------------------------------------------------------------------------
IDirect3DDevice9 *Engine::GetDevice()
{
	return m_device;
}

//-----------------------------------------------------------------------------
// Returns a pointer to the display mode of the current Direct3D device.
//-----------------------------------------------------------------------------
D3DDISPLAYMODE *Engine::GetDisplayMode()
{
	return &m_displayMode;
}

//-----------------------------------------------------------------------------
// Returns a pointer to the sprite interface.
//-----------------------------------------------------------------------------
ID3DXSprite *Engine::GetSprite()
{
	return m_sprite;
}

//-----------------------------------------------------------------------------
// Adds a state to the engine.
//-----------------------------------------------------------------------------
void Engine::AddState( State *state, bool change )
{
	m_states->Add( state );

	if( change == false )
		return;

	if( m_currentState != NULL )
		m_currentState->Close();

	m_currentState = m_states->GetLast();
	m_currentState->Load();
}

//-----------------------------------------------------------------------------
// Removes a state from the engine
//-----------------------------------------------------------------------------
void Engine::RemoveState( State *state )
{
	m_states->Remove( &state );
}

//-----------------------------------------------------------------------------
// Changes processing to the state with the specified ID.
//-----------------------------------------------------------------------------
void Engine::ChangeState( unsigned long id )
{
	// Iterate through the list of states and find the new state to change to.
	m_states->Iterate( true );
	while( m_states->Iterate() != NULL )
	{
		if( m_states->GetCurrent()->GetID() == id )
		{
			// Close the old state.
			if( m_currentState != NULL )
				m_currentState->Close();

			// Let the sound system perform garbage collection.
			m_soundSystem->GarbageCollection();

			// Set the new current state and load it.
			m_currentState = m_states->GetCurrent();
			m_currentState->Load();

			// Swap the back buffers until the first one is in the front.
			while( m_currentBackBuffer != 0 )
			{
				m_device->Present( NULL, NULL, NULL, NULL );

				if( ++m_currentBackBuffer == m_setup->totalBackBuffers + 1 )
					m_currentBackBuffer = 0;
			}

			// Indicate that the state has changed.
			m_stateChanged = true;

			break;
		}
	}
}

//-----------------------------------------------------------------------------
// Returns a pointer to the current state.
//-----------------------------------------------------------------------------
State *Engine::GetCurrentState()
{
	return m_currentState;
}

//-----------------------------------------------------------------------------
// Returns a pointer to the script manager.
//-----------------------------------------------------------------------------
ResourceManager< Script > *Engine::GetScriptManager()
{
	return m_scriptManager;
}

//-----------------------------------------------------------------------------
// Returns a pointer to the material manager.
//-----------------------------------------------------------------------------
ResourceManager< Material > *Engine::GetMaterialManager()
{
	return m_materialManager;
}

//-----------------------------------------------------------------------------
// Returns a pointer to the mesh manager.
//-----------------------------------------------------------------------------
ResourceManager< Mesh > *Engine::GetMeshManager()
{
	return m_meshManager;
}

//-----------------------------------------------------------------------------
// Returns a pointer to the input object.
//-----------------------------------------------------------------------------
Input *Engine::GetInput()
{
	return m_input;
}

//-----------------------------------------------------------------------------
// Returns a pointer to the network object.
//-----------------------------------------------------------------------------
Network *Engine::GetNetwork()
{
	return m_network;
}

//-----------------------------------------------------------------------------
// Returns a pointer to the sound system.
//-----------------------------------------------------------------------------
SoundSystem *Engine::GetSoundSystem()
{
	return m_soundSystem;
}

//-----------------------------------------------------------------------------
// Returns a pointer to the scene manager.
//-----------------------------------------------------------------------------
SceneManager *Engine::GetSceneManager()
{
	return m_sceneManager;
}