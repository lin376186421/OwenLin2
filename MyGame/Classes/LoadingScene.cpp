//
//  LoadingScene.cpp
//  MyGame
//
//  Created by 林 on 2019/4/11.
//

#include "LoadingScene.h"
#include "SimpleAudioEngine.h"
#include "GameScene.h"

//USING_NS_CC;//using namespace cocos2d;
using namespace CocosDenshion;

bool LoadingScene::init()
{
    if (Scene::init())
        return true;
    else
        return false;
}

Scene* LoadingScene::createScene()
{
    return LoadingScene::create();
}

void LoadingScene::onEnter()
{
    Scene::onEnter();
//    cocos2d::log("onEnter");
    
//    Size visibleSize = Director::getInstance()->getVisibleSize();
//    visibleSize.width;
//
//    Vec2 origin = Director::getInstance()->getVisibleOrigin();
//    origin.x;
    
    auto splashBg = Sprite::create("splash.png");
    splashBg->setPosition(Vec2(Screen_Visible_Origin.x + Screen_Visible_Size.width/2, Screen_Visible_Origin.y+Screen_Visible_Size.height/2));
    addChild(splashBg);
    
    auto logo = Label::createWithTTF("by OwenLin", "Marker Felt.ttf", 20);
    logo->setColor(Color3B(255,255,255));
    logo->setPosition(Vec2(Screen_Visible_Origin.x+Screen_Visible_Size.width/2, Screen_Visible_Origin.y+Screen_Visible_Size.height/2 - 50));
//    logo->setPosition(Vec2(Screen_Visible_Origin.x+Screen_Visible_Size.width, Screen_Visible_Origin.y+Screen_Visible_Size.height));
    addChild(logo);
    
    Director::getInstance()->getTextureCache()->addImageAsync("game.png", CC_CALLBACK_1(LoadingScene::loadingCallBack, this));
}

void LoadingScene::loadingCallBack(Texture2D *texture)
{
    log("资源已缓存");
    
    SpriteFrameCache::getInstance()->addSpriteFramesWithFile("game.plist");
    auto birdAnimation = Animation::create();
    birdAnimation->setDelayPerUnit(0.2);
    birdAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("bird1.png"));
    birdAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("bird2.png"));
    birdAnimation->addSpriteFrame(SpriteFrameCache::getInstance()->getSpriteFrameByName("bird3.png"));
    AnimationCache::getInstance()->addAnimation(birdAnimation, "birdAnimation");
    
    SimpleAudioEngine::getInstance()->preloadEffect("die.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("hit.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("point.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("swooshing.mp3");
    SimpleAudioEngine::getInstance()->preloadEffect("wing.mp3");
    
    auto gameScene = GameScene::createScene();
    TransitionScene *transition = TransitionFade::create(0.5f, gameScene);
    Director::getInstance()->replaceScene(transition);
}
