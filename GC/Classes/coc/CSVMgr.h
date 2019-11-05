#ifndef __COC_CSVMGR_H__
#define __COC_CSVMGR_H__

#include "BaseDef.h"
#include "BaseCSV.h"

NS_COC_BEGIN

const int MAX_CSVNUMS = 6;
const string csvName[MAX_CSVNUMS] = {"player.cfg", "build.cfg", "heroskill.cfg", "townhall.cfg", "configval.cfg", "pve.cfg"};

class CSVMgr
{
public:
    
    CSVMgr();
    ~CSVMgr();
public:
	static CSVMgr& getSingleton();
public:
    
    bool init();
    
	void loadAll();

	void saveAll();

	void load(const char* strname);

	void save(const char* strname, const char* pData);
    
    void checkData();
    
public:
	int		m_csvVer[MAX_CSVNUMS];
    
    BaseCSV m_csv;
};

NS_COC_END


#endif // __COC_CSVMGR_H__
