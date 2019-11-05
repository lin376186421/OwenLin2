//
//  FacebookMgr.cpp
//  GameClient
//
//  Created by chowray on 13-5-7.
//
//

#include "FacebookMgr.h"
#include "LogicInfo.h"
#include "CAppPlatform.h"
#include "PlayerScene.h"
#include "GameWorldScene.h"
#include "../form/FBInviteUI.h"
#include "CFacebookService.h"
NS_COC_BEGIN


void FBFriendInfo::getFBUserImage(int w , int h)
{
    if(isRequesting)
        return;
    isRequesting = true;
    char url[512];
    sprintf(url, "https://graph.facebook.com/%s/picture?width=%d&height=%d",fid.c_str(),w,h);
    
    CCHttpRequest* request = new CCHttpRequest();
//    request->setUrl(url);
    request->setUrl(fphoto.c_str());
    request->setRequestType(CCHttpRequest::kHttpGet);
    request->setResponseCallback(this, httpresponse_selector(FBFriendInfo::onHttpRequestCompleted));
    request->setTag("GET PIC");
    CCHttpClient::getInstance()->send(request);
    
}
void FBFriendInfo::onHttpRequestCompleted(cocos2d::CCNode *sender,CCHttpResponse *response)
{
    if (!response)
    {
        return;
    }
    
    std::vector<char> *buffer = response->getResponseData();
    
    int length = (*buffer).size();
    if(length == 0)
        return;
    
    CCImage* img = new CCImage;
    img->initWithImageData((unsigned char*)buffer->data(),buffer->size());
    
    CCTexture2D* texture = new CCTexture2D();
    bool isImg = texture->initWithImage(img);
    if(isImg == false)
        return;
    CCRect rect = CCRectZero;
    rect.size = texture->getContentSize();
    fSpriteFrame = CCSpriteFrame::createWithTexture(texture, rect);
    fSpriteFrame->retain();
    LogicInfo::getSingleton().m_asyncViewList.insert(pair<int, int>(ASYNC_FB_GET_PIC, 0));
//    LogicInfo::getSingleton().onNotify(NOTIFY_FB_GET_PIC_OK);
    img->release();
    buffer->clear();
    isRequesting = false;
    
    
}

FacebookMgr::FacebookMgr()
{
    m_state = FACEBOOK_ON;
    m_isTipLogin = false;
    m_shareState = 1;
    m_inviteState = 1;
    m_shareMessaage = FBPostShareMessage;
    m_shareType = 0;
    m_shareFreeGem = false;
    m_isFirstShare = false;
    m_isGetFreeGemLogin = false;
    m_inviteType = 0;
}

FacebookMgr::~FacebookMgr()
{
    
}

FacebookMgr& FacebookMgr::getSingleton()
{
	static FacebookMgr s_mgr;
    
	return s_mgr;
}

void FacebookMgr::init()
{
    
}
bool FacebookMgr::isOpenState()
{
    return m_state == 1;
}

bool FacebookMgr::isLogin()
{
    if(m_fbUser.fid.length() > 0)
    {
        return true;
    }
    return false;
}
void FacebookMgr::fbShare(string message)
{
    if(isShare() == false)
    {
        return;
    }
    if(FacebookMgr::getSingleton().isLogin())
    {
        CFacebookService::sharedFaceBook()->fbPostMessageWithPhoto(message);
        
        PlayerScene::getSingleton().addNote(ViewString::getSingleton().getValue("fb_share_info").c_str());
    }
    else
        PlayerScene::getSingleton().m_pGameWorld->showFBLoginUI(0);
}
bool FacebookMgr::isShare()
{
    if(isOpenState() && m_shareState == 1)
    {
        return true;
    }
    return false;
}

bool FacebookMgr::isInvite()
{
    if(isOpenState() && m_inviteState == 1)
    {
        return true;
    }
    return false;
}
void FacebookMgr::fbRequestFinish(int curApiCall, int state)
{
    CCLOG("fbRequestFinish calltype:%d state:%d",curApiCall,state);
    if(curApiCall == kAPIPostMessageWithPhoto || curApiCall == kAPIPostMessage)
    {
        if(state == 1)
        {
            LogicInfo::getSingleton().m_asyncViewList.insert(pair<int, int>(ASYNC_FB_POST, 0));
            HttpClient::getSingleton().requestFBShareLog();
            if(FacebookMgr::getSingleton().m_shareFreeGem && !FacebookMgr::getSingleton().m_isFirstShare)
            {
                LogicInfo::getSingleton().m_addGemType = ADD_GEM_FB_SHARE;
                LogicInfo::getSingleton().requestAdsGem(5);
            }
        }
        FacebookMgr::getSingleton().m_shareFreeGem = false;
        FacebookMgr::getSingleton().m_isFirstShare = false;
    }
    else if (curApiCall == kAPIInviteFriends)
    {
        if(state == 1)
        {
            HttpClient::getSingleton().requestFBInviteFriends(m_pendingInviteFriendIds);
            for(int i = 0;  i < m_pendingInviteFriendIds.size(); i++)
            {
                string fid = m_pendingInviteFriendIds[i];
                deleteFriendById(fid);
            }
            LogicInfo::getSingleton().onNotify(NOTIFY_FB_INVITE_OK);
        }
        m_pendingInviteFriendIds.clear();
    }
}

void FacebookMgr::fbGetTestFriends()
{
    vector<string> fids;
    fids.push_back("1505566313019104");
    fids.push_back("100007655640318");
    fids.push_back("100004665163519");
    fids.push_back("100002051958798");
    fids.push_back("1280593103");
    fids.push_back("100003380705512");
    fids.push_back("100003589201162");
    fids.push_back("730727385");
    fids.push_back("100002495045665");
    fids.push_back("100000587856611");
    fids.push_back("100004100954029");
    fids.push_back("100001784706426");
    fids.push_back("100006829181629");
    fids.push_back("100007615475491");
    fids.push_back("100007822689976");
    fids.push_back("100007622013804");
    
    fbClearFriends();
    for(int i = 0; i < fids.size(); i++)
    {
        FBFriendInfo *finfo = new FBFriendInfo();
        finfo->fid = fids[i];
        finfo->fname = "test";
        finfo->fSpriteFrame = NULL;
        finfo->isSelect = false;
        
        m_fbFriendsList.push_back(finfo);
    }
}

void FacebookMgr::fbSetUser(FBUserInfo fbUser)
{
    m_fbUser = fbUser;
    
    LogicInfo::getSingleton().m_asyncViewList.insert(pair<int, int>(ASYNC_FB_LOGIN, 0));
    
    if(CCUserDefault::sharedUserDefault()->getBoolForKey("FB_LOGIN") == false)
    {
        CCUserDefault::sharedUserDefault()->setBoolForKey("FB_LOGIN", true);
        CCUserDefault::sharedUserDefault()->flush();
    }
    CFacebookService::sharedFaceBook()->setFBBindActivity(true);

    CAppPlatform::sharedAppPlatform()->verifyLocalPlayer(m_fbUser.fid, m_fbUser.fname);
}
void FacebookMgr::fbGetFBFriendsFinish()
{
    LogicInfo::getSingleton().onNotify(NOTIFY_FB_FRIENDS_OK);
}

void FacebookMgr::fbSetFriends(vector<FBFriendInfo*> friendsList)
{
//    fbClearFriends();
//	m_fbFriendsList = friendsList;
    for(int i = 0 ; i < friendsList.size(); i++)
    {
        m_fbFriendsList.push_back(friendsList[i]);
    }
}
void FacebookMgr::fbClearFriends()
{
    for(int i = 0; i < m_fbFriendsList.size(); i++)
    {
        FBFriendInfo *finfo = m_fbFriendsList[i];
        finfo->removeSpriteFrame();
        if(finfo)
            delete finfo;
    }
    m_fbFriendsList.clear();
}

void FacebookMgr::fbLoginOut()
{
    m_fbUser.fid = "";
    m_fbUser.fname = "";
    fbClearFriends();
    LogicInfo::getSingleton().onNotify(NOTIFY_FB_LOGIN_OUT_OK);
    CFacebookService::sharedFaceBook()->setFBBindActivity(false);
}

void FacebookMgr::deleteFriendById(const string fid)
{
    FBFriendInfo *finfo = getFriendById(fid);
    if(finfo != NULL)
    {
        std::vector<FBFriendInfo*>::iterator it = std::find(m_fbFriendsList.begin(), m_fbFriendsList.end(), finfo);
        if(it != m_fbFriendsList.end())
        {
            FBFriendInfo *info = *it;
            info->removeSpriteFrame();
            m_fbFriendsList.erase(it);
            if(info)
                delete info;
        }
    }
}

void FacebookMgr::deleteFriendByName(const string fname)
{
    FBFriendInfo *finfo = getFriendByName(fname);
    if(finfo != NULL)
    {
        std::vector<FBFriendInfo*>::iterator it = std::find(m_fbFriendsList.begin(), m_fbFriendsList.end(), finfo);
        if(it != m_fbFriendsList.end())
        {
            FBFriendInfo *info = *it;
            info->removeSpriteFrame();
            m_fbFriendsList.erase(it);
            if(info)
                delete info;
        }
    }
}

FBFriendInfo* FacebookMgr::getFriendById(const string fid)
{
    FBFriendInfo *finfo = NULL;
    for(int i = 0; i < m_fbFriendsList.size(); i++)
    {
        FBFriendInfo *info = m_fbFriendsList[i];
        if(info->fid == fid)
            finfo = info;
    }
    return finfo;
}
FBFriendInfo* FacebookMgr::getFriendByName(const string fname)
{
    FBFriendInfo *finfo = NULL;
    for(int i = 0; i < m_fbFriendsList.size(); i++)
    {
        FBFriendInfo *info = m_fbFriendsList[i];
        if(info->fname == fname)
            finfo = info;
    }
    return finfo;
}



NS_COC_END
