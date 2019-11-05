//
//  SoundMgr.h
//  GameClient
//
//  Created by chowray on 13-5-7.
//
//

#ifndef __GameClient__SoundMgr__
#define __GameClient__SoundMgr__

#include "cocos-ext.h"

USING_NS_CC_EXT;

#define SOUND_DELAY  180

class SoundMgr
{
public:
    SoundMgr();
    ~SoundMgr();
public:
	static SoundMgr& getSingleton();
    
    void init();
public:
    void playBackgroundMusic(const char* pszFilePath);
    void playEffect(const char* pszFilePath, int delay = 300);
    
    void pauseBackgroundMusic();
    void stopBackgroundMusic();
    void resumeBackgroundMusic();
    
    bool isBackgroundMusicPlaying();
    bool isBackgroundMusicPaused();
    
    void playButtonSound(CCControlButton* pButton);
public:
    std::map<std::string, long long>    m_mapSound;
    
    bool m_musicOn;
    bool m_soundOn;
    
    bool m_backgroundMusicPlayed;
    bool m_backgroundMusicPaused;
};

#endif /* defined(__GameClient__SoundMgr__) */
