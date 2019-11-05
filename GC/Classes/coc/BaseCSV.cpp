/****************************************************************
	cocos2d x zephyr
*****************************************************************/

#include "BaseCSV.h"
#include "../coc/HttpClient.h"
#include "../Utils/DataDecrypt.h"
#include "md5.h"

NS_COC_BEGIN

BaseCSV::BaseCSV() 
	: m_pBuff(NULL)
	, m_pArray(NULL)
	, m_iWidth(0)
	, m_iHeight(0)
	, m_isRemote(false)
{
}

BaseCSV::~BaseCSV() 
{
	release();
}

void BaseCSV::load(const char* name)
{
	std::string fullPath = name;
	unsigned long size;
	char* pBuff = NULL;
	bool isNeedDecode = false;

	if(m_isRemote)
	{
		fullPath = CCFileUtils::sharedFileUtils()->getWritablePath() + name;
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
	}
	else
	{
		pBuff = (char*)CCFileUtils::sharedFileUtils()->getFileData(fullPath.c_str(), "rb", &size);
	}

	if(pBuff != NULL)
	{
        if (DATA_ENCRYPT && string(name).find(".cfg") != -1) {
            isNeedDecode = true;
		}
        
		loadData(pBuff, size, isNeedDecode);
		
		delete[] pBuff;
	}
}

void BaseCSV::release()
{
	if(m_pBuff != NULL)
	{
		free(m_pBuff);

		m_pBuff = NULL;
	}

	if(m_pArray != NULL)
	{
		free(m_pArray);

		m_pArray = NULL;
	}

	m_mapHead.clear();
}

const char* BaseCSV::get(int x, int y) const
{
	if(x >= 0 && x < m_iWidth && y >= 0 && y < m_iHeight)
	{
		if(m_pArray != NULL)
			return m_pArray[y][x];
	}

	return NULL;
}

const char* BaseCSV::get(const char* headname, int y) const
{
	_MapHead::const_iterator it = m_mapHead.find(headname);
	if(it != m_mapHead.end())
		return get(it->second, y);

	return NULL;
}

void BaseCSV::loadData(const char* pBuff1, unsigned long size, bool isNeedDecode)
{
	const char* pBuff = pBuff1;

	if(isNeedDecode)
	{
		unsigned long outSize = 0;
		pBuff = (char*)decryptData(pBuff1, size, &outSize);
		size = outSize;
	}

	onRelease();
	release();

	{
		m_pBuff = (char*)malloc(size + 1);

		memcpy(m_pBuff, pBuff, size);

		m_pBuff[size] = '\n';

		m_iWidth = 0;
		m_iHeight = 0;

        bool newLine = false;
        
		for(unsigned long i = 0; i <= size; ++i)
		{
            if (m_pBuff[i] != ' ' && m_pBuff[i] != '\t' && m_pBuff[i] != '\r' && m_pBuff[i] != '\n') {
                newLine = true;
            }
            
            if(m_iHeight == 0 && m_pBuff[i] == ',')
            {
                ++m_iWidth;
            }
            else if(m_pBuff[i] == '\r' && m_pBuff[i + 1] == '\n')
            {
                if (newLine) {
                    ++i;
                    ++m_iHeight;
                    newLine = false;
                }
            }
            else if(m_pBuff[i] == '\r' || m_pBuff[i] == '\n')
            {
                if (newLine) {
                    ++m_iHeight;
                    newLine = false;
                }
            }
		}
        
		++m_iWidth;
        
		m_pArray = (char***)malloc(sizeof(char*) * m_iWidth * m_iHeight + sizeof(char**) * m_iHeight);

		for(int i = 0; i < m_iHeight; ++i)
		{
			m_pArray[i] = (char**)((char*)m_pArray + m_iHeight * sizeof(char**) + m_iWidth * i * sizeof(char*));
		}

		int x = 0;
		int y = 0;
		char* last = m_pBuff;

		for(unsigned long i = 0; i <= size; ++i)
		{
			if(m_pBuff[i] == ',')
			{
				m_pBuff[i] = '\0';

				CC_ASSERT(y < m_iHeight);
				CC_ASSERT(x < m_iWidth);
				m_pArray[y][x] = last;

				++x;
				last = m_pBuff + i + 1;
			}	
			else if(m_pBuff[i] == '\r' && m_pBuff[i + 1] == '\n')
			{
				m_pBuff[i] = '\0';
				m_pBuff[i + 1] = '\0';

				CC_ASSERT(y < m_iHeight);
				CC_ASSERT(x < m_iWidth);
				m_pArray[y][x] = last;

				++i;
				++y;
				x = 0;

				last = m_pBuff + i + 1;
			}
			else if(m_pBuff[i] == '\r' || m_pBuff[i] == '\n')
			{
				m_pBuff[i] = '\0';

				CC_ASSERT(y < m_iHeight);
				CC_ASSERT(x < m_iWidth);
				m_pArray[y][x] = last;

				++y;
				x = 0;

				last = m_pBuff + i + 1;
			}
            
            if (y >= m_iHeight) {
                break;
            }
		}

		for(int i = 0; i < m_iWidth; ++i)
		{
			_PairHead p(m_pArray[0][i], i);

			m_mapHead.insert(p);
		}

		onLoadEnd();

		if(m_pBuff != NULL)
		{
			free(m_pBuff);

			m_pBuff = NULL;
		}

		if(m_pArray != NULL)
		{
			free(m_pArray);

			m_pArray = NULL;
		}
	}

	if(isNeedDecode)
		delete[] pBuff;
}

NS_COC_END