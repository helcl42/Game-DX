#include "Main.h"


PlayerManager::PlayerManager()
{
	// Create the list of player objects.
	m_players = new LinkedList< PlayerObject >();
	m_viewingPlayer = NULL;

	// Clear all the local player's movement details.
	m_localMovement = false;
	m_localDrive = 0.0f;
	m_localStrafe = 0.0f;
	m_localFire = false;

	// Indicate that the local player needs to be spawned.
	m_spawnLocalPlayer = true;
	m_requestedSpawnPoint = false;
}


PlayerManager::~PlayerManager()
{
	// Destroy the list of player objects.
	m_players->ClearPointers();
	SAFE_DELETE( m_players );
}


void PlayerManager::Update( float elapsed )
{
	// Ensure the scene is loaded.
	if( Engine::GetInstance()->GetSceneManager()->IsLoaded() == false )
		return;

	// Get a pointer to the local player.
	PlayerObject *localPlayer = m_players->GetFirst();
	if( localPlayer == NULL )
		return;

	// Check if the local player is dead.
	if( localPlayer->GetHealth() <= 0.0f && m_spawnLocalPlayer == false )
	{
		// Spawn the local player on a left mouse button press.
		if( Engine::GetInstance()->GetInput()->GetButtonPress( 0 ) == true )
			m_spawnLocalPlayer = true;

		return;
	}

	// If the local player needs to be spawned and a spawn point hasn't already
	// been requested, then request a spawn point from the host.
	if( m_spawnLocalPlayer == true && m_requestedSpawnPoint == false )
	{
		// Send a request spawn point message to the host.
		NetworkMessage rspm;
		rspm.msgid = MSGID_SPAWN_POINT_REQUEST;
		rspm.dpnid = Engine::GetInstance()->GetNetwork()->GetLocalID();
		Engine::GetInstance()->GetNetwork()->Send( &rspm, sizeof( NetworkMessage ), Engine::GetInstance()->GetNetwork()->GetHostID() );

		// Indicate that a spawn point has been requested.
		m_requestedSpawnPoint = true;

		return;
	}
	else if( m_spawnLocalPlayer == true )
	{
		// The local player needs to be spawned, but a spawn point has already
		// been requested. So just return and wait for the request response.
		return;
	}

	// Calculate a delayed elapsed time, used for smoothing out view movement.
	static float delayedElapsed = 0.0f;
	delayedElapsed = delayedElapsed * 0.99f + elapsed * 0.01f;

	// Skip any further input if the local player is dying.
	if( localPlayer->GetDying() == true )
		return;

	// It is impossible to send network messages for every mouse movement,
	// therefore the player's looking direction will be updated directly.
	localPlayer->MouseLook( delayedElapsed, (float)Engine::GetInstance()->GetInput()->GetDeltaY(), (float)Engine::GetInstance()->GetInput()->GetDeltaX() );

	// Send a periodic player look update to sync the other players.
	static unsigned long lookUpdate = timeGetTime();
	if( lookUpdate + 100 < timeGetTime() && localPlayer->GetEnabled() == true )
	{
		PlayerLookUpdateMsg plum;
		plum.msgid = MSGID_PLAYER_LOOK_UPDATE;
		plum.dpnid = localPlayer->GetID();
		plum.viewTilt = localPlayer->GetViewTilt();
		plum.rotationY = localPlayer->GetRotation().y;
		Engine::GetInstance()->GetNetwork()->Send( &plum, sizeof( PlayerLookUpdateMsg ), DPNID_ALL_PLAYERS_GROUP, DPNSEND_NOLOOPBACK );

		lookUpdate = timeGetTime();
	}

	// Used for storing the local player's desired movement.
	float desiredDrive = 0.0f;
	float desiredStrafe = 0.0f;
	bool desiredFire = false;

	// Check if the local player is trying to drive fowards or backwards.
	if( Engine::GetInstance()->GetInput()->GetKeyPress( DIK_W, true ) )
		desiredDrive = 1.0;
	else if( Engine::GetInstance()->GetInput()->GetKeyPress( DIK_S, true ) )
		desiredDrive = -1.0;

	// Check if the local player is trying to strafe left or right.
	if( Engine::GetInstance()->GetInput()->GetKeyPress( DIK_D, true ) )
		desiredStrafe = 1.0;
	else if( Engine::GetInstance()->GetInput()->GetKeyPress( DIK_A, true ) )
		desiredStrafe = -1.0;

	// Check if the local player is trying to fire their weapon.
	desiredFire = Engine::GetInstance()->GetInput()->GetButtonPress( 0, true );

	// Check if we need to drive the local player.
	if( m_localDrive != desiredDrive )
	{
		m_localDrive = desiredDrive;
		m_localMovement = true;
	}

	// Check if we need to strafe the local player.
	if( m_localStrafe != desiredStrafe )
	{
		m_localStrafe = desiredStrafe;
		m_localMovement = true;
	}

	// Check if we need to fire the local player's weapon.
	if( m_localFire != desiredFire )
	{
		m_localFire = desiredFire;
		m_localMovement = true;
	}

	// If the local player's movement changed or the move update timer expires,
	// update the local player across the network. Since the network message is
	// sent using DPNSEND_NOLOOPBACK, the local player will need to update too.
	static unsigned long moveUpdate = timeGetTime();
	if( ( m_localMovement == true || moveUpdate + 200 < timeGetTime() ) && localPlayer->GetEnabled() == true )
	{
		PlayerMoveUpdateMsg pmum;
		pmum.msgid = MSGID_PLAYER_MOVE_UPDATE;
		pmum.dpnid = localPlayer->GetID();
		pmum.translation = localPlayer->GetTranslation();
		pmum.drive = m_localDrive;
		pmum.strafe = m_localStrafe;
		pmum.fire = m_localFire;
		Engine::GetInstance()->GetNetwork()->Send( &pmum, sizeof( PlayerMoveUpdateMsg ), DPNID_ALL_PLAYERS_GROUP, DPNSEND_NOLOOPBACK );
		m_localMovement = false;

		localPlayer->SetDrive( m_localDrive );
		localPlayer->SetStrafe( m_localStrafe );
		localPlayer->SetFire( m_localFire );

		moveUpdate = timeGetTime();
	}

	char changeWeapon = 0;
	if( Engine::GetInstance()->GetInput()->GetDeltaWheel() > 0 )
		changeWeapon = 1;
	else if( Engine::GetInstance()->GetInput()->GetDeltaWheel() < 0 )
		changeWeapon = -1;

	// Change the local player's weapon if the scroll wheel moved.
	if( changeWeapon != 0 )
		localPlayer->ChangeWeapon( changeWeapon );

	// Check if the local player is trying to change to a specific weapon.
	if( Engine::GetInstance()->GetInput()->GetKeyPress( DIK_1 ) == true )
		localPlayer->ChangeWeapon( 0, 0 );
	if( Engine::GetInstance()->GetInput()->GetKeyPress( DIK_2 ) == true )
		localPlayer->ChangeWeapon( 0, 1 );
	if( Engine::GetInstance()->GetInput()->GetKeyPress( DIK_3 ) == true )
		localPlayer->ChangeWeapon( 0, 2 );
	if( Engine::GetInstance()->GetInput()->GetKeyPress( DIK_4 ) == true )
		localPlayer->ChangeWeapon( 0, 3 );
	if( Engine::GetInstance()->GetInput()->GetKeyPress( DIK_5 ) == true )
		localPlayer->ChangeWeapon( 0, 4 );
	if( Engine::GetInstance()->GetInput()->GetKeyPress( DIK_6 ) == true )
		localPlayer->ChangeWeapon( 0, 5 );
	if( Engine::GetInstance()->GetInput()->GetKeyPress( DIK_7 ) == true )
		localPlayer->ChangeWeapon( 0, 6 );
	if( Engine::GetInstance()->GetInput()->GetKeyPress( DIK_8 ) == true )
		localPlayer->ChangeWeapon( 0, 7 );
	if( Engine::GetInstance()->GetInput()->GetKeyPress( DIK_9 ) == true )
		localPlayer->ChangeWeapon( 0, 8 );
	if( Engine::GetInstance()->GetInput()->GetKeyPress( DIK_0 ) == true )
		localPlayer->ChangeWeapon( 0, 9 );

}


void PlayerManager::SpawnLocalPlayer( long spawnPoint )
{
	// Ensure the spawn point request was successful.
	if( spawnPoint == -1 )
	{
		// End the spawn point request so a new one can be attempted.
		m_requestedSpawnPoint = false;

		// Indicate that the local player needs to be spawned.
		m_spawnLocalPlayer = true;

		return;
	}

	// Send a message to spawn the local player at the given spawn point.
	SpawnPlayerMsg spm;
	spm.msgid = MSGID_SPAWN_PLAYER;
	spm.dpnid = Engine::GetInstance()->GetNetwork()->GetLocalID();
	spm.translation = Engine::GetInstance()->GetSceneManager()->GetSpawnPointByID( spawnPoint )->GetTranslation();
	Engine::GetInstance()->GetNetwork()->Send( &spm, sizeof( SpawnPlayerMsg ), DPNID_ALL_PLAYERS_GROUP );
}


void PlayerManager::SpawnPlayer( DPNID dpnid, D3DXVECTOR3 translation )
{
	// Find the player to spawn.
	m_players->Iterate( true );
	while( m_players->Iterate() )
		if( m_players->GetCurrent()->GetID() == dpnid )
			break;
	if( m_players->GetCurrent() == NULL )
		return;

	// Enable the player object.
	m_players->GetCurrent()->SetEnabled( true );
	m_players->GetCurrent()->SetVisible( true );

	// Restore the player's health.
	m_players->GetCurrent()->SetHealth( 100.0f );
	m_players->GetCurrent()->SetDying( false );

	// Play the idle animation twice to ensure that both animation tracks 
	// contain the idle animation data. This will prevent players from 
	// transitioning from the death animation when respawning. 
	m_players->GetCurrent()->PlayAnimation( 0, 0.0f ); 
	m_players->GetCurrent()->PlayAnimation( 0, 0.0f );

	// Set the player's translation and rotation.
	m_players->GetCurrent()->SetTranslation( translation );
	m_players->GetCurrent()->MouseLook( 0.0f, 0.0f, 0.0f, true );

	// Check if the local player was spawned.
	if( m_players->GetCurrent()->GetID() == Engine::GetInstance()->GetNetwork()->GetLocalID() )
	{
		// Set local player as the viewing player.
		m_players->GetCurrent()->SetIsViewing( true );
		m_viewingPlayer = m_players->GetCurrent();

		// The request for a local player spawn point was successful.
		m_requestedSpawnPoint = false;

		// Indicate that the local player has been spawned.
		m_spawnLocalPlayer = false;
	}
	else
		m_players->GetCurrent()->SetIsViewing( false );
}


PlayerObject *PlayerManager::AddPlayer( PlayerInfo *player )
{
	// Create the script for the player's character.
	Script *script = new Script( ( (PlayerData*)player->data )->character, "./Assets/Characters/" );

	// Create the player object.
	PlayerObject *object = m_players->Add( new PlayerObject( player, script ) );

	// Destroy the character script.
	SAFE_DELETE( script );

	return object;
}


void PlayerManager::RemovePlayer( DPNID dpnid )
{
	// Find the player to remove.
	m_players->Iterate( true );
	while( m_players->Iterate() )
		if( m_players->GetCurrent()->GetID() == dpnid )
			break;
	if( m_players->GetCurrent() == NULL )
		return;

	// Remove the player object.
	PlayerObject *player = m_players->GetCurrent();
	m_players->Remove( &player );
}


PlayerObject *PlayerManager::GetLocalPlayer()
{
	return m_players->GetFirst();
}


PlayerObject *PlayerManager::GetPlayer( DPNID dpnid )
{
	m_players->Iterate( true );
	while( m_players->Iterate() )
		if( m_players->GetCurrent()->GetID() == dpnid )
			return m_players->GetCurrent();

	return NULL;
}


PlayerObject *PlayerManager::GetNextPlayer( bool restart )
{
	m_players->Iterate( restart );
	if( restart == true )
		m_players->Iterate();

	return m_players->GetCurrent();
}


PlayerObject *PlayerManager::GetViewingPlayer()
{
	return m_viewingPlayer;
}

