#ifndef NETWORK_H
#define NETWORK_H


#define MSGID_CREATE_PLAYER 0x12001
#define MSGID_DESTROY_PLAYER 0x12002
#define MSGID_TERMINATE_SESSION 0x12003

//---------------------------------------------
// Network Message
//---------------------------------------------
struct NetworkMessage
{
	unsigned long msgid;
	DPNID	      dpnid;	
};

//---------------------------------------------
// Received Message
//---------------------------------------------
struct ReceivedMessage : public NetworkMessage
{
	char data[32];
};

//---------------------------------------------
// Enumerated Session Information
//---------------------------------------------
struct SessionInfo
{
	IDirectPlay8Address* address; // Session network address.
	DPN_APPLICATION_DESC description; // Application description.
};

//-----------------------------------------------------------------------------
// Player Information Structure
//-----------------------------------------------------------------------------
struct PlayerInfo
{
	DPNID		  dpnid; // DirectPlay player ID.
	char*		  name;  // Name of the player.
	void*		  data;  // Application specific player data.
	unsigned long 	  size;  // Data size.

	PlayerInfo()
	{
		dpnid = 0;
		name = NULL;
		data = NULL;
		size = 0;
	}

	~PlayerInfo()
	{
		SAFE_DELETE( name );
		SAFE_DELETE( data );
	}
};


//-----------------------------------------------------------------------------
// Network
//-----------------------------------------------------------------------------
class Network
{
public:
	Network( GUID guid, void (*HandleNetworkMessageFunction)( ReceivedMessage *msg ) );
	virtual ~Network();

	void Update();

	void EnumerateSessions();

	bool Host( char *name, char *session, int players = 0, void *playerData = NULL, unsigned long dataSize = 0 );
	bool Join( char *name, int session = 0, void *playerData = NULL, unsigned long dataSize = 0 );
	void Terminate();

	void SetReceiveAllowed( bool allowed );

	SessionInfo* GetNextSession( bool restart = false );
	PlayerInfo*  GetPlayer( DPNID dpnid );

	DPNID GetLocalID();
	DPNID GetHostID();
	bool  IsHost();

	void Send( void *data, long size, DPNID dpnid = DPNID_ALL_PLAYERS_GROUP, long flags = 0 );

private:
	static HRESULT WINAPI NetworkMessageHandler( PVOID context, DWORD msgid, PVOID data );

private:
	GUID		     m_guid;   // Application specific GUID.
	IDirectPlay8Peer*    m_dpp;    // DirectPlay peer interface.
	IDirectPlay8Address* m_device; // DirectPlay device address.

	unsigned long m_port;		// Port for network communication.
	unsigned long m_sendTimeOut;	// Timeout for sent network messages.
	unsigned long m_processingTime; // Allowed time period for processing received network messages.

	DPNID m_dpnidLocal; // DirectPlay ID of the local player.
	DPNID m_dpnidHost;  // DirectPlay ID of the host player.

	LinkedList<SessionInfo>*     m_sessions; // Linked list of enumerated sessions.
	LinkedList<PlayerInfo>*	     m_players;	 // Linked list of players.
	LinkedList<ReceivedMessage>* m_messages; // Linked list of network messages.

	CRITICAL_SECTION m_sessionCS; // Enumerated session list critical section.	
	CRITICAL_SECTION m_playerCS;  // Player list critical section.	
	CRITICAL_SECTION m_messageCS; // Network message list critical section.	

	bool m_receiveAllowed;	                              // Inidcates if the network is allowed to receive application specific messages.	
	void (*HandleNetworkMessage)( ReceivedMessage *msg ); // Pointer to an application specific network message handler.
};

#endif