//
//  FacebookMgr.h
//  GameClient
//
//  Created by chowray on 13-5-7.
//
//

#ifndef __GameClient__FacebookMgr__
#define __GameClient__FacebookMgr__

#include "cocos-ext.h"

#include "BaseDef.h"
#include "BaseCSV.h"

NS_COC_BEGIN


#define FBPostLinkCaption "I am playing this great game Battle Glory and I thought that you might like it. Check it out."

#define FBPostShareMessage "Cool things I did in Battle Glory! \n (iPhone: https://itunes.apple.com/us/app/a-battle-of-glory/id909539738?l=zh&ls=1&mt=8  iPad:https://itunes.apple.com/us/app/a-battle-of-glory-hd/id909539860?l=zh&ls=1&mt=8  Android: https://play.google.com/store/apps/details?id=com.mgstudio.bg&hl=en)  \n"

struct FBFriendInfo : CCObject{
    std::string fid;
	std::string fname;
    std::string fphoto;
    CCSpriteFrame *fSpriteFrame;
    bool isSelect;
    bool isInvite;//是否被邀请过
    bool isRequesting;
    FBFriendInfo()
    {
        fSpriteFrame = NULL;
        isRequesting = false;
        isInvite = false;
        isSelect = false;
    }
    ~FBFriendInfo()
    {
        fSpriteFrame = NULL;
    }
    
    void removeSpriteFrame()
    {
        if(fSpriteFrame)
        {
            fSpriteFrame->release();
            fSpriteFrame = NULL;
            isRequesting = false;
            isSelect = false;
            isInvite = false;
        }
    }
    
    void getFBUserImage(int w , int h);
    
    void onHttpRequestCompleted(cocos2d::CCNode *sender,CCHttpResponse *response);
    
};

struct FBUserInfo : CCObject{
	std::string fid;
	std::string fname;
};

class FacebookMgr
{
public:
    FacebookMgr();
    ~FacebookMgr();
public:
	static FacebookMgr& getSingleton();
    
    void init();
    void fbGetTestFriends();
    void fbLoginOut();
    void fbSetUser(FBUserInfo fbUser);
    void fbGetFBFriendsFinish();
    void fbSetFriends(vector<coc::FBFriendInfo*> friendsList);
    void fbClearFriends();
    bool isLogin();
    bool isOpenState();
    void fbRequestFinish(int curApiCall, int state);
    void fbShare(string message);
    bool isShare();
    bool isInvite();
    void deleteFriendById(const string fid);
    void deleteFriendByName(const string fname);
    FBFriendInfo* getFriendById(const string fid);
    FBFriendInfo* getFriendByName(const string fname);
public:
    std::vector<FBFriendInfo *>       m_fbFriendsList;
    
    FBUserInfo                      m_fbUser;
    
    std::vector<string>       m_pendingInviteFriendIds;
    
    int                       m_inviteCount;//每次邀请成功好友的个数
    
    int                       m_state;//0 屏蔽facebook 功能 1 打开
    
    int                       m_shareState;//0分享功能关闭 1 打开
    
    int                       m_inviteState;//0邀请功能关闭 1 打开
    
    bool                      m_isTipLogin;
    
    string                    m_shareMessaage;
    
    bool                      m_shareFreeGem;
    
    bool                      m_isFirstShare;
    
    int                       m_shareType;
    
    bool                      m_isGetFreeGemLogin;
    
    int 					  m_inviteType;
    
    CCControlButton*          m_pCurBtn;

public:
   
};
NS_COC_END
#endif /* defined(__GameClient__FacebookMgr__) */
