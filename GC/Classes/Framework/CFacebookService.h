//
//  FacebookService.h
//  GameClient
//
//  Created by qsun on 14-11-10.
//
//

#ifndef __GameClient__CFacebookService__
#define __GameClient__CFacebookService__

#include "cocos2d.h"
#include "BaseDef.h"
using namespace std;
using namespace cocos2d;
USING_NS_COC;

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
#include "platform/android/jni/JniHelper.h"
#endif

#include "FacebookMgr.h"

typedef enum apiCall_ {
    kAPIClear,
    kAPIGraphMe,
    kAPIInviteFriends,
    kAPIGraphFriends,
    kAPIPostMessage,
    kAPIPostMessageWithPhoto,
    kAPIOpenSession,
    kAPILogin,
} apiCall;

#define ScreenShotFile  "my_screen.png"

class CFacebookService {
    
public:
    
    static CFacebookService* sharedFaceBook();
    void init();
    void fbLogin();
    void fbLoginOut();
    void fbPostMessage(const string message);
    void fbPostMessageWithPhoto(const string message);
    void fbGetInvitableFriends();
    void fbSDKInviteFriends();
    void fbGetFriends();
    void fbSetFriends(vector<FBFriendInfo*> friendsList);
    void fbSetUser(string fid, string fname);
    void fbInviteFriends(vector<string> friendIds);
    
    void fbRequestFinish(int curApiCall, int state);
    bool fbBindActivity;
    
    void setFBBindActivity(bool activity);
    
    ~CFacebookService();
};
#endif /* defined(__GameClient__CFacebookService__) */
