//
//  FacebookService.h
//  GameClient
//
//  Created by qsun on 14-11-10.
//
//

//#import "CAppPlatform.h"
//#import <Foundation/Foundation.h>
//#import <UIKit/UIKit.h>
//#import <GameKit/GameKit.h>
//#import <GameKit/GKLocalPlayer.h>
//#import "AppController.h"
//
#import <FacebookSDK/FacebookSDK.h>

#import "CFacebookService.h"
#import "FacebookService_ios.h"

#include "LogicInfo.h"


FBSession *fbsession;
FacebookService *facebookLoader;

static CFacebookService* s_pCFacebookService = NULL;



CFacebookService* CFacebookService::sharedFaceBook()
{
    if (s_pCFacebookService == NULL) {
        s_pCFacebookService = new CFacebookService();        
    }
    return s_pCFacebookService;
}
CFacebookService::~CFacebookService()
{
    //    [flurryAdDelegate release];
}

void CFacebookService::init()
{
    this->setFBBindActivity(false);
    
    facebookLoader = [[FacebookService alloc] init];
}

void CFacebookService::setFBBindActivity(bool activity)
{
    if(!FacebookMgr::getSingleton().isOpenState())
    {
        fbBindActivity = false;
        LogicInfo::getSingleton().m_bindType = BIND_SYSTEM_TYPE;
        return;
    }
    fbBindActivity = activity;
    if(fbBindActivity)
        LogicInfo::getSingleton().m_bindType = BIND_FACEBOOK_TYPE;
    else
        LogicInfo::getSingleton().m_bindType = BIND_SYSTEM_TYPE;
}

void CFacebookService::fbInviteFriends(vector<string> friendIds)
{
    NSMutableArray *inviteIds = [NSMutableArray array];
    for(int i = 0; i < friendIds.size(); i++)
    {
        [inviteIds addObject:[NSString stringWithUTF8String:friendIds[i].c_str()]];
    }
    [facebookLoader startInvite:inviteIds];
}

void CFacebookService::fbSetFriends(vector<coc::FBFriendInfo*> friendsList)
{
    coc::FacebookMgr::getSingleton().fbSetFriends(friendsList);
    coc::FacebookMgr::getSingleton().fbGetFBFriendsFinish();
}

void CFacebookService::fbSetUser(string fid, string fname)
{
    CCLOG("fbSetUser %s---%s",fid.c_str(),fname.c_str());
    coc::FBUserInfo fbUser;
    fbUser.fid = fid;
    fbUser.fname = fname;
    coc::FacebookMgr::getSingleton().fbSetUser(fbUser);
}

void CFacebookService::fbGetInvitableFriends()
{
    [facebookLoader apiGetInvitabelFriends];
}

void CFacebookService::fbGetFriends()
{
    [facebookLoader apiGraphFriends];
}

void CFacebookService::fbSDKInviteFriends()
{
    [facebookLoader inviteFriendsPlayGame];
}

void CFacebookService::fbLogin()
{
    [facebookLoader login:YES];
}

void CFacebookService::fbLoginOut()
{
    [facebookLoader logOut];
}

void CFacebookService::fbPostMessage(const string message)
{
    [facebookLoader postMessage:[NSString stringWithUTF8String:message.c_str()]];
}

void CFacebookService::fbPostMessageWithPhoto(const string message)
{
    [facebookLoader postMessageWithPhoto:[NSString stringWithUTF8String:message.c_str()]];
}

void CFacebookService::fbRequestFinish(int curApiCall, int state)
{
    FacebookMgr::getSingleton().fbRequestFinish(curApiCall, state);
    
}
