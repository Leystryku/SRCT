#ifdef _WIN32
#define DLL_IMPORT				extern "C" __declspec( dllimport )
#else
#define DLL_IMPORT   extern "C"
#endif

#define DBG_INTERFACE	DLL_IMPORT

enum SpewType_t
{
	SPEW_MESSAGE = 0,
	SPEW_WARNING,
	SPEW_ASSERT,
	SPEW_ERROR,
	SPEW_LOG,

	SPEW_TYPE_COUNT
};

enum SpewRetval_t
{
	SPEW_DEBUGGER = 0,
	SPEW_CONTINUE,
	SPEW_ABORT
};
class Color;

typedef SpewRetval_t(*SpewOutputFunc_t)(SpewType_t spewType, const char *pMsg);
typedef void* (*CreateInterfaceFn)(const char *pName, int *pReturnCode);
typedef void* (*InstantiateInterfaceFn)();

DBG_INTERFACE void   SpewOutputFunc(SpewOutputFunc_t func);

DBG_INTERFACE SpewOutputFunc_t GetSpewOutputFunc(void);

DBG_INTERFACE const Color* GetSpewOutputColor(void);