#include "main.h"

static ClassDescExporter _descriptionExporter;
static ClassDescMod_collisionMesh _descriptionMod_collisionMesh;

HINSTANCE hInstance = 0;

BOOL WINAPI DllMain(HINSTANCE hinstDLL, ULONG fdwReason, LPVOID /*lpvReserved*/)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
	{
		// Hang on to this DLL's instance handle.
		hInstance = hinstDLL;
		DisableThreadLibraryCalls(hInstance);
	}
	return(TRUE);
}

void GUI_GetNumbers(const char* str, const char* format, ...)
{
	va_list args;
	va_start(args, format);
	vsscanf(str, format, args);
	va_end(args);
}

extern "C"
{
	__declspec(dllexport) const TCHAR* LibDescription()
	{
		// Retrieve astring from the resource string table
		static TCHAR buf[256];
		if (hInstance)
			return L"Badcoiq Engine Plugin for 3DS Max";
		return NULL;
	}

	__declspec(dllexport) int LibNumberClasses()
	{
		return 2;
	}

	__declspec(dllexport) ClassDesc* LibClassDesc(int i)
	{
		switch (i)
		{
		case 0:
			return &_descriptionExporter;
		case 1:
			return &_descriptionMod_collisionMesh;
		}

		return 0;
	}

	__declspec(dllexport) ULONG LibVersion()
	{
		return Get3DSMAXVersion();
	}

	__declspec(dllexport) int LibInitialize()
	{
		// TODO: Perform initialization here.
		return TRUE;
	}

	__declspec(dllexport) int LibShutdown()
	{
		// TODO: Perform uninitialization here.
		return TRUE;
	}
}
