
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
	virtual void			ServerActivate(edict_t *pEdictList, int edictCount, int clientMax);
	virtual void			GameFrame(bool simulating);
	virtual void			LevelShutdown(void);
	virtual void			ClientActive(edict_t *pEntity);
	virtual void			ClientDisconnect(edict_t *pEntity);
	virtual void			ClientPutInServer(edict_t *pEntity, char const *playername);
	virtual void			SetCommandClient(int index);
	virtual void			ClientSettingsChanged(edict_t *pEdict);
	virtual PLUGIN_RESULT	ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen);
	virtual PLUGIN_RESULT	ClientCommand(edict_t *pEntity, const CCommand &args);
	virtual PLUGIN_RESULT	NetworkIDValidated(const char *pszUserName, const char *pszNetworkID);
	virtual void			OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue);
	virtual void			OnEdictAllocated(edict_t *edict);
	virtual void			OnEdictFreed(const edict_t *edict);

	// IGameEventListener Interface
	virtual void FireGameEvent(KeyValues * event);

	virtual int GetCommandIndex() { return m_iClientCommandIndex; }
private:
	int m_iClientCommandIndex;
};


CShityPlugin g_ShityPlugin;
EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CShityPlugin, IServerPluginCallbacks, INTERFACEVERSION_ISERVERPLUGINCALLBACKS, g_ShityPlugin);

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

void CShityPlugin::ServerActivate(edict_t *pEdictList, int edictCount, int clientMax)
{
}

void CShityPlugin::GameFrame(bool simulating)
{
}

void CShityPlugin::LevelShutdown(void)
{
}

void CShityPlugin::ClientActive(edict_t *pEntity)
{
}

void CShityPlugin::ClientDisconnect(edict_t *pEntity)
{
}
void CShityPlugin::ClientPutInServer(edict_t *pEntity, char const *playername)
{
}
void CShityPlugin::SetCommandClient(int index)
{
	m_iClientCommandIndex = index;
}

void CShityPlugin::ClientSettingsChanged(edict_t *pEdict)
{
}
PLUGIN_RESULT CShityPlugin::ClientConnect(bool *bAllowConnect, edict_t *pEntity, const char *pszName, const char *pszAddress, char *reject, int maxrejectlen)
{
	return PLUGIN_CONTINUE;
}


PLUGIN_RESULT CShityPlugin::ClientCommand(edict_t *pEntity, const CCommand &args)
{
	return PLUGIN_CONTINUE;
}

PLUGIN_RESULT CShityPlugin::NetworkIDValidated(const char *pszUserName, const char *pszNetworkID)
{
	return PLUGIN_CONTINUE;
}

void CShityPlugin::OnQueryCvarValueFinished(QueryCvarCookie_t iCookie, edict_t *pPlayerEntity, EQueryCvarValueStatus eStatus, const char *pCvarName, const char *pCvarValue)
{
}
void CShityPlugin::OnEdictAllocated(edict_t *edict)
{
}
void CShityPlugin::OnEdictFreed(const edict_t *edict)
{
}

void CShityPlugin::FireGameEvent(KeyValues * event)
{
}