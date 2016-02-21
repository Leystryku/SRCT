
class IGameEventListener
{
public:
	virtual	~IGameEventListener(void) {};

	// FireEvent is called by EventManager if event just occured
	// KeyValue memory will be freed by manager if not needed anymore
	virtual void FireGameEvent(void *shit) = 0;
};


class  IServerPluginCallbacks
{
public:
	// Initialize the plugin to run
	// Return false if there is an error during startup.
	virtual bool			Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory) = 0;

	// Called when the plugin should be shutdown
	virtual void			Unload(void) = 0;

	// called when a plugins execution is stopped but the plugin is not unloaded
	virtual void			Pause(void) = 0;

	// called when a plugin should start executing again (sometime after a Pause() call)
	virtual void			UnPause(void) = 0;

	// Returns string describing current plugin.  e.g., Admin-Mod.  
	virtual const char     *GetPluginDescription(void) = 0;

	// Called any time a new level is started (after GameInit() also on level transitions within a game)
	virtual void			LevelInit(char const *pMapName) = 0;

	// The server is about to activate
	virtual void			ServerActivate(void *pEdictList, int edictCount, int clientMax) = 0;

	// The server should run physics/think on all edicts
	virtual void			GameFrame(bool simulating) = 0;

	// Called when a level is shutdown (including changing levels)
	virtual void			LevelShutdown(void) = 0;

	// Client is going active
	virtual void			ClientActive(void *pEntity) = 0;

	// Client is disconnecting from server
	virtual void			ClientDisconnect(void *pEntity) = 0;

	// Client is connected and should be put in the game
	virtual void			ClientPutInServer(void *pEntity, char const *playername) = 0;

	// Sets the client index for the client who typed the command into their console
	virtual void			SetCommandClient(int index) = 0;

	// A player changed one/several replicated cvars (name etc)
	virtual void			ClientSettingsChanged(void *pEdict) = 0;

	// Client is connecting to server ( set retVal to false to reject the connection )
	//	You can specify a rejection message by writing it into reject
	virtual int	ClientConnect(bool *bAllowConnect, void *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen) = 0;

	// The client has typed a command at the console
	virtual int	ClientCommand(void *pEntity, const CCommand &args) = 0;

	// A user has had their network id setup and validated 
	virtual int	NetworkIDValidated(const char *pszUserName, const char *pszNetworkID) = 0;

	// This is called when a query from IServerPluginHelpers::StartQueryCvarValue is finished.
	// iCookie is the value returned by IServerPluginHelpers::StartQueryCvarValue.
	// Added with version 2 of the interface.
	virtual void			OnQueryCvarValueFinished(void*shit) = 0;

};

//---------------------------------------------------------------------------------
// Purpose: a sample 3rd party plugin class
//---------------------------------------------------------------------------------
class CShityPlugin : public IServerPluginCallbacks, public IGameEventListener
{
public:


	CShityPlugin();
	~CShityPlugin();

	// IServerPluginCallbacks methods
	virtual bool			Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory);
	virtual void			Unload(void);
	virtual void			Pause(void);
	virtual void			UnPause(void);
	virtual const char     *GetPluginDescription(void);
	virtual void			LevelInit(char const *pMapName);
	virtual void			ServerActivate(void *pEdictList, int edictCount, int clientMax);
	virtual void			GameFrame(bool simulating);
	virtual void			LevelShutdown(void);
	virtual void			ClientActive(void *pEntity);
	virtual void			ClientDisconnect(void *pEntity);
	virtual void			ClientPutInServer(void *pEntity, char const *playername);
	virtual void			SetCommandClient(int index);
	virtual void			ClientSettingsChanged(void *pEdict);
	virtual int	ClientConnect(bool *bAllowConnect, void *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen);
	virtual int	ClientCommand(void *pEntity, const CCommand &args);
	virtual int	NetworkIDValidated(const char *pszUserName, const char *pszNetworkID);
	virtual void			OnQueryCvarValueFinished(void*shit);
	virtual void			OnEdictAllocated(void *edict);
	virtual void			OnEdictFreed(const void *edict);

	// IGameEventListener Interface
	virtual void FireGameEvent(void * shit);

	virtual int GetCommandIndex() { return m_iClientCommandIndex; }
private:
	int m_iClientCommandIndex;
};

// Used internally to register classes.
class InterfaceReg
{
public:
	InterfaceReg(InstantiateInterfaceFn fn, const char *pName);

public:
	InstantiateInterfaceFn	m_CreateFn;
	const char				*m_pName;

	InterfaceReg			*m_pNext; // For the global list.
	static InterfaceReg		*s_pInterfaceRegs;
};


// Use this to expose an interface that can have multiple instances.
// e.g.:
// EXPOSE_INTERFACE( CInterfaceImp, IInterface, "MyInterface001" )
// This will expose a class called CInterfaceImp that implements IInterface (a pure class)
// clients can receive a pointer to this class by calling CreateInterface( "MyInterface001" )
//
// In practice, the shared header file defines the interface (IInterface) and version name ("MyInterface001")
// so that each component can use these names/vtables to communicate
//
// A single class can support multiple interfaces through multiple inheritance
//
// Use this if you want to write the factory function.
#if !defined(_STATIC_LINKED) || !defined(_SUBSYSTEM)
#define EXPOSE_INTERFACE_FN(functionName, interfaceName, versionName) \
	static InterfaceReg __g_Create##interfaceName##_reg(functionName, versionName);
#else
#define EXPOSE_INTERFACE_FN(functionName, interfaceName, versionName) \
	namespace _SUBSYSTEM \
	{	\
		static InterfaceReg __g_Create##interfaceName##_reg(functionName, versionName); \
	}
#endif

#if !defined(_STATIC_LINKED) || !defined(_SUBSYSTEM)
#define EXPOSE_INTERFACE(className, interfaceName, versionName) \
	static void* __Create##className##_interface() {return static_cast<interfaceName *>( new className );} \
	static InterfaceReg __g_Create##className##_reg(__Create##className##_interface, versionName );
#else
#define EXPOSE_INTERFACE(className, interfaceName, versionName) \
	namespace _SUBSYSTEM \
	{	\
		static void* __Create##className##_interface() {return static_cast<interfaceName *>( new className );} \
		static InterfaceReg __g_Create##className##_reg(__Create##className##_interface, versionName ); \
	}
#endif

// Use this to expose a singleton interface with a global variable you've created.
#if !defined(_STATIC_LINKED) || !defined(_SUBSYSTEM)
#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR_WITH_NAMESPACE(className, interfaceNamespace, interfaceName, versionName, globalVarName) \
	static void* __Create##className##interfaceName##_interface() {return static_cast<interfaceNamespace interfaceName *>( &globalVarName );} \
	static InterfaceReg __g_Create##className##interfaceName##_reg(__Create##className##interfaceName##_interface, versionName);
#else
#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR_WITH_NAMESPACE(className, interfaceNamespace, interfaceName, versionName, globalVarName) \
	namespace _SUBSYSTEM \
	{ \
		static void* __Create##className##interfaceName##_interface() {return static_cast<interfaceNamespace interfaceName *>( &globalVarName );} \
		static InterfaceReg __g_Create##className##interfaceName##_reg(__Create##className##interfaceName##_interface, versionName); \
	}
#endif

#define EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, versionName, globalVarName) \
	EXPOSE_SINGLE_INTERFACE_GLOBALVAR_WITH_NAMESPACE(className, , interfaceName, versionName, globalVarName)

// Use this to expose a singleton interface. This creates the global variable for you automatically.
#if !defined(_STATIC_LINKED) || !defined(_SUBSYSTEM)
#define EXPOSE_SINGLE_INTERFACE(className, interfaceName, versionName) \
	static className __g_##className##_singleton; \
	EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, versionName, __g_##className##_singleton)
#else
#define EXPOSE_SINGLE_INTERFACE(className, interfaceName, versionName) \
	namespace _SUBSYSTEM \
	{	\
		static className __g_##className##_singleton; \
	}	\
	EXPOSE_SINGLE_INTERFACE_GLOBALVAR(className, interfaceName, versionName, __g_##className##_singleton)
#endif


CShityPlugin g_ShityPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShityPlugin, IServerPluginCallbacks, "ISERVERPLUGINCALLBACKS003", g_ShityPlugin);

//---------------------------------------------------------------------------------
// Purpose: constructor/destructor
//---------------------------------------------------------------------------------
CShityPlugin::CShityPlugin()
{
	m_iClientCommandIndex = 0;
}

CShityPlugin::~CShityPlugin()
{
}

bool CShityPlugin::Load(CreateInterfaceFn interfaceFactory, CreateInterfaceFn gameServerFactory)
{
	return true;
}

void CShityPlugin::Unload(void)
{
}

void CShityPlugin::Pause(void)
{
}

void CShityPlugin::UnPause(void)
{
}

const char *CShityPlugin::GetPluginDescription(void)
{
	return "SRCT Server V1.0 - Leystryku";
}

void CShityPlugin::LevelInit(char const *pMapName)
{
}

void CShityPlugin::ServerActivate(void *pEdictList, int edictCount, int clientMax)
{
}

void CShityPlugin::GameFrame(bool simulating)
{
}

void CShityPlugin::LevelShutdown(void)
{
}

void CShityPlugin::ClientActive(void *pEntity)
{
}

void CShityPlugin::ClientDisconnect(void *pEntity)
{
}
void CShityPlugin::ClientPutInServer(void *pEntity, char const *playername)
{
}
void CShityPlugin::SetCommandClient(int index)
{
	m_iClientCommandIndex = index;
}

void CShityPlugin::ClientSettingsChanged(void *pEdict)
{
}

int CShityPlugin::ClientConnect(bool *bAllowConnect, void *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen)
{
	return 0;
}


int CShityPlugin::ClientCommand(void *pEntity, const CCommand &args)
{
	return 0;
}

int CShityPlugin::NetworkIDValidated(const char *pszUserName, const char *pszNetworkID)
{
	return 0;
}

void CShityPlugin::OnQueryCvarValueFinished(void*shit)
{
}
void CShityPlugin::OnEdictAllocated(void *edict)
{
}
void CShityPlugin::OnEdictFreed(const void *edict)
{
}

void CShityPlugin::FireGameEvent(void * event)
{
}