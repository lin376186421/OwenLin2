#include "NGNKit.h"
#include <stdlib.h>
#include "cocos2d.h"
#include "CAppPlatform.h"

using namespace std;
using namespace cocos2d;

int64_t m_currentTime = -1;
int64_t m_currentMilliSecond;

//static pthread_t s_netTimeThread;
//static void* NetTimeThread(void *data);

static NGNKit* s_pSharedKit = NULL;
NGNKit* NGNKit::sharedKit(){
	if (s_pSharedKit == NULL) {
		s_pSharedKit = new NGNKit();
	}

	return s_pSharedKit;
}

void NGNKit::init()
{
    sessionId = "";
    
    m_currentTime = -1;
    
    m_currentMilliSecond = millisecondNow();
    
    srand(time(NULL));
    
//    pthread_create(&s_netTimeThread, NULL, NetTimeThread, NULL);
//    pthread_detach(s_netTimeThread);
    
    string version = CCUserDefault::sharedUserDefault()->getStringForKey(DATA_APP_VERSION);
    string appVersion = CAppPlatform::sharedAppPlatform()->getBundleVersion();
    
    if (version != appVersion) { //new version
        
    }
    else
    {
        
    }
    
    CCUserDefault::sharedUserDefault()->setStringForKey(DATA_APP_VERSION, appVersion);
    CCUserDefault::sharedUserDefault()->flush();
}

bool NGNKit::connectedToNetwork()
{
    return true;
}

void NGNKit::getServerInfo()
{
//    CHttpService::sharedHttpService()->serverConfig();
//    CHttpService::sharedHttpService()->saleLogs();
}

static void* NetTimeThread(void *data)
{
    while (true) {
        
        if(m_currentTime != -1)
        {
            int64_t curMilliSecond = millisecondNow();
            int64_t milliSecond = curMilliSecond - m_currentMilliSecond;
            
            m_currentTime += milliSecond;
            
            m_currentMilliSecond = curMilliSecond;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
			Sleep(500);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WP8)

#else
			usleep(500);
#endif // _MSC_VER
        }
    }
    
    return NULL;
}

