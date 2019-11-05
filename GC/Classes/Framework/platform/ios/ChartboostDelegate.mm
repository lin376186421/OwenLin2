//
//  ChartboostDelegate.m
//  GameClient
//
//  Created by Lee  on 14-2-17.
//
//

#import "ChartboostDelegate.h"
#include "CUserData.h"
#include "cocos2d.h"
#include "HttpClient.h"
#include "LogicInfo.h"
#include "PlayerScene.h"
#include "Shop.h"
#include "GameWorldScene.h"
#include "CAppPlatform.h"
USING_NS_CC;
USING_NS_COC;

@implementation ChartboostDelegate

- (void)didClickInterstitial:(CBLocation)location
{
    CAppPlatform::sharedAppPlatform()->getChartBoostClickGems();
    NSLog(@"didClickInterstitial");
}
- (void)didClickMoreApps:(CBLocation)location
{
    CAppPlatform::sharedAppPlatform()->getChartBoostClickGems();
    NSLog(@"didClickMoreApps");
}

- (void)didCloseMoreApps
{
    NSLog(@"didCloseMoreApps");
}

@end
