//
//  LoadingScene.hpp
//  MyGame
//
//  Created by 林 on 2019/4/11.
//

#ifndef LoadingScene_h
#define LoadingScene_h

#include "cocos2d.h"

USING_NS_CC;

class LoadingScene : public Scene
{
public:
    static Scene* createScene();
    
    virtual bool init();
    void onEnter();
    // implement the "static create()" method manually
    CREATE_FUNC(LoadingScene);
    
private:
    void loadingCallBack(cocos2d::Texture2D *texture);
};


#endif /* LoadingScene_h */
