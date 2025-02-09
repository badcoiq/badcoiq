#define BQ_EXPORT
#include "main.h"

extern "C"
{
	BQ_API bqMEPlugin* BQ_CDECL MECreatePlugin(bqME* sdk)
	{
		return new MEBasePlugin(sdk);
	}

	BQ_API void BQ_CDECL MEDestroyPlugin(bqMEPlugin* plugin)
	{
		if (plugin)
		{
			delete plugin;
		}
	}
}

