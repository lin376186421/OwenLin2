//
//  SoundMgr.cpp
//  GameClient
//
//  Created by chowray on 13-5-7.
//
//

#include "SoundMgr.h"
#include "SimpleAudioEngine.h"
#include "BaseDef.h"
#include "CUserData.h"

SoundMgr::SoundMgr()
{
    
}

SoundMgr::~SoundMgr()
{
    
}

SoundMgr& SoundMgr::getSingleton()
{
	static SoundMgr s_mgr;
    
	return s_mgr;
}

void SoundMgr::init()
{
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("Sound/game_loading.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("Sound/back.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadBackgroundMusic("Sound/fight_back.mp3");
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/logo.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/select_barrack.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/select_gold.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/select_oil.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/select_build.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/select_attack.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/get_gold.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/get_oil.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/att_gold.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/att_oil.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/btn.m4a");
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/fight_start.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/building_uplevel.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/building_destroy.mp3");

	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/getexp.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/levelup.mp3");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/build_move.m4a");
	CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/build_ok.m4a");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/btn.m4a");
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/clanwar_win.mp3");
    CocosDenshion::SimpleAudioEngine::sharedEngine()->preloadEffect("Sound/clanwar_lose.mp3");
    
    m_mapSound["logo.mp3"] = 0;
    m_mapSound["select_barrack.mp3"] = 0;
    m_mapSound["select_gold.mp3"] = 0;
    m_mapSound["select_oil.mp3"] = 0;
    m_mapSound["select_build.mp3"] = 0;
    m_mapSound["select_attack.mp3"] = 0;
    m_mapSound["get_gold.mp3"] = 0;
    m_mapSound["get_oil.mp3"] = 0;
    m_mapSound["att_gold.mp3"] = 0;
    m_mapSound["att_oil.mp3"] = 0;
    m_mapSound["btn.m4a"] = 0;
    
    m_mapSound["fight_start.mp3"] = 0;
    m_mapSound["building_uplevel.mp3"] = 0;
    m_mapSound["building_destroy.mp3"] = 0;

	m_mapSound["getexp.mp3"] = 0;
	m_mapSound["levelup.mp3"] = 0;
	m_mapSound["build_move.m4a"] = 0;
	m_mapSound["build_ok.m4a"] = 0;
    
    m_mapSound["clanwar_win.mp3"] = 0;
    m_mapSound["clanwar_lose.mp3"] = 0;
    
    m_musicOn = CCUserDefault::sharedUserDefault()->getBoolForKey(GAME_SETTING_MUSIC, true);
    m_soundOn = CCUserDefault::sharedUserDefault()->getBoolForKey(GAME_SETTING_SOUND, true);
    
    m_backgroundMusicPlayed = false;
    m_backgroundMusicPaused = false;
}

void SoundMgr::playBackgroundMusic(const char *pszFilePath)
{
    if (m_musicOn) {
        std::string filePath(pszFilePath);
        filePath.insert(0, "Sound/");
        CocosDenshion::SimpleAudioEngine::sharedEngine()->playBackgroundMusic(filePath.c_str(), true);
        
        m_backgroundMusicPlayed = true;
    }
}

void SoundMgr::pauseBackgroundMusic()
{
    if (m_backgroundMusicPlayed) {
        m_backgroundMusicPaused = true;
    }
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->pauseBackgroundMusic();
}

void SoundMgr::stopBackgroundMusic()
{
    m_backgroundMusicPlayed = false;
    m_backgroundMusicPaused = false;
    
    CocosDenshion::SimpleAudioEngine::sharedEngine()->stopBackgroundMusic();
}

void SoundMgr::resumeBackgroundMusic()
{
    if (m_musicOn) {
    
        CocosDenshion::SimpleAudioEngine::sharedEngine()->resumeBackgroundMusic();
    }
}

bool SoundMgr::isBackgroundMusicPlaying()
{
    return CocosDenshion::SimpleAudioEngine::sharedEngine()->isBackgroundMusicPlaying();
}

bool SoundMgr::isBackgroundMusicPaused()
{
    return m_backgroundMusicPaused;
}

void SoundMgr::playEffect(const char *pszFilePath, int delay)
{
    if (m_soundOn) {
        std::string filePath(pszFilePath);
        filePath.insert(0, "Sound/");
        if(getCurrentTimeMs() - m_mapSound[pszFilePath] >= delay)
        {
            CocosDenshion::SimpleAudioEngine::sharedEngine()->playEffect(filePath.c_str());
            m_mapSound[pszFilePath] = getCurrentTimeMs();
        }
    }
}

void SoundMgr::playButtonSound(CCControlButton* pButton)
{
    playEffect("btn.m4a", 0);
}