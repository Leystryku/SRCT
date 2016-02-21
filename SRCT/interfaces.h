class IVEngineServer
{
	template <typename T> T inline GetFunction(void* instance, unsigned int uiIndex)
	{
		void **vInstance = *(void***)instance;
		return (T)vInstance[uiIndex];
	}

public:

	inline void ServerCommand(const char *str)
	{
		typedef void(__thiscall* OriginalFn) (void*thisptr, const char*cmd);
		this->GetFunction<OriginalFn>(this, 36)(this, str);

	}


};


class ICvar
{
	template <typename T> T inline GetFunction(void* instance, unsigned int uiIndex)
	{
		void **vInstance = *(void***)instance;
		return (T)vInstance[uiIndex];
	}

public:
	inline void RegisterConCommand(void *cvar)
	{
		typedef void(__thiscall* OriginalFn) (void*thisptr, void*cvar);
		this->GetFunction<OriginalFn>(this, 6)(this, cvar);
	}

};