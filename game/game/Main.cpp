#include "Main.h"


void HandleNetworkMessage( ReceivedMessage *msg )
{
	// Only allow network messages to be processed while in the game state.
	if( Engine::GetInstance()->GetCurrentState()->GetID() != STATE_GAME )
		return;

	// Pass control over to the game state's network message handler.
	( (Game*)Engine::GetInstance()->GetCurrentState() )->HandleNetworkMessage( msg );
}


void StateSetup()
{
	Engine::GetInstance()->AddState( new Menu, true );
	Engine::GetInstance()->AddState( new Game, false );
}


void CreateMaterialResource( Material **resource, char *name, char *path = "./" )
{
	if( strcmp( name, "defaultMaterial" ) == 0 )
		*resource = new GameMaterial();
	else
		*resource = new GameMaterial( name, path );
}


int WINAPI WinMain( HINSTANCE instance, HINSTANCE prev, LPSTR cmdLine, int cmdShow )
{
	// A unique id for the application so it can be identified on a network.
	GUID guid = { 0xd6c55c78, 0x5030, 0x43b7, { 0x85, 0xa9, 0xc, 0x8b, 0xbe, 0x77, 0x5a, 0x62 } };

	// Create the engine setup structure.
	EngineSetup setup;
	setup.instance = instance;
	setup.guid = guid;
	setup.name = "Cityscape";
	setup.scale = 0.01f;
	setup.totalBackBuffers = 1;
	setup.HandleNetworkMessage = HandleNetworkMessage;
	setup.StateSetup = StateSetup;
	setup.CreateMaterialResource = CreateMaterialResource;
	setup.spawnerPath = "./Assets/Objects/";

	// Create the engine (using the setup structure), then run it.
	Engine* engine = new Engine( &setup );
	engine->Run();
	SAFE_DELETE(engine);

	return true;
}