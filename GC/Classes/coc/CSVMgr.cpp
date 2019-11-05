#include "CSVMgr.h"
#include "md5.h"
#include "HttpClient.h"

NS_COC_BEGIN

CSVMgr::CSVMgr()
{
	
}

CSVMgr::~CSVMgr()
{
}

CSVMgr& CSVMgr::getSingleton()
{
	static CSVMgr s_mgr;

	return s_mgr;
}

bool CSVMgr::init()
{
    for(int i = 0; i < MAX_CSVNUMS; ++i) {
		m_csvVer[i] = 0;
    }
    
	loadAll();
    
    return true;
}

void CSVMgr::loadAll()
{
	std::string path = CCFileUtils::sharedFileUtils()->getWritablePath();

	std::string name = path + "main.dat";

	FILE* fp = fopen(name.c_str(), "rb");
	if(fp != NULL)
	{
		for(int i = 0; i < MAX_CSVNUMS; ++i)
			fread(&(m_csvVer[i]), sizeof(m_csvVer[i]), 1, fp);

		fclose(fp);
	}
}

void CSVMgr::saveAll()
{
	std::string path = CCFileUtils::sharedFileUtils()->getWritablePath();

	std::string name = path + "main.dat";

	FILE* fp = fopen(name.c_str(), "wb");
	if(fp != NULL)
	{
		for(int i = 0; i < MAX_CSVNUMS; ++i)
			fwrite(&(m_csvVer[i]), sizeof(m_csvVer[i]), 1, fp);

		fclose(fp);
	}
}

void CSVMgr::load(const char* strname)
{
}

void CSVMgr::checkData()
{
	unsigned long size;
	char* pBuff = NULL;

    string key[MAX_CSVNUMS];
    for (int i = 0; i < MAX_CSVNUMS; i++) {
        string fullPath = CCFileUtils::sharedFileUtils()->getWritablePath() + csvName[i];
        FILE* fp = fopen(fullPath.c_str(), "rb");
        
        if(fp != NULL)
        {
            fseek(fp, 0, SEEK_END);
            size = ftell(fp);
            fseek(fp, 0, SEEK_SET);
            pBuff = new char[size];
            fread(pBuff, size, 1, fp);
            fclose(fp);
        }
        
        if(pBuff != NULL)
        {
            string verify = MD5(pBuff, size).toString();
            key[i] = verify;
        }
    }
    
    HttpClient::getSingleton().checkCSV(key);

}

void CSVMgr::save(const char* strname, const char* pData)
{
	std::string path = CCFileUtils::sharedFileUtils()->getWritablePath();

	std::string name = path + strname;

	FILE* fp = fopen(name.c_str(), "wb");
	if(fp != NULL)
	{
		fwrite(pData, strlen(pData), 1, fp);

		fclose(fp);
	}
}

NS_COC_END
