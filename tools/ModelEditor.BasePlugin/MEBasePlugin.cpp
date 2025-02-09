#include "main.h"

MEBasePlugin::MEBasePlugin(bqME* sdk)
	:
	m_sdk(sdk)
{
}

MEBasePlugin::~MEBasePlugin()
{
}

const bqUID& MEBasePlugin::GetUID()
{
	return MEUID_BASE_PLUGIN;
}

const wchar_t* MEBasePlugin::GetName()
{
	return L"Base plugin";
}

const wchar_t* MEBasePlugin::GetDescription()
{
	return L"Base functionality";
}

const wchar_t* MEBasePlugin::GetAuthor()
{
	return L"BADCOIQ";
}

uint32_t MEBasePlugin::GetNumOfImporters()
{
	return 0;
}

uint32_t MEBasePlugin::GetNumOfExporters()
{
	return 0;
}

bqMEExporter* MEBasePlugin::GetExporter(uint32_t)
{
	return 0;
}

bqMEImporter* MEBasePlugin::GetImporter(uint32_t)
{
	return 0;
}

