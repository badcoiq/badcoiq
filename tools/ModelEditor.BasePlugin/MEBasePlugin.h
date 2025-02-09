#ifndef _MEBASEPLUGIN_H_
#define _MEBASEPLUGIN_H_

class MEBasePlugin : public bqMEPlugin
{
	bqME* m_sdk = 0;
public:
	MEBasePlugin(bqME* sdk);
	virtual ~MEBasePlugin();

	virtual const bqUID& GetUID() override;
	virtual const wchar_t* GetName() override;
	virtual const wchar_t* GetDescription() override;
	virtual const wchar_t* GetAuthor() override;
	virtual uint32_t GetNumOfImporters() override;
	virtual uint32_t GetNumOfExporters() override;
	virtual bqMEExporter* GetExporter(uint32_t) override;
	virtual bqMEImporter* GetImporter(uint32_t) override;
};

#endif
