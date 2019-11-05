//
//  AppPlatform_ios.m
//  GameClient
//
//  Created by qsun on 13-7-31.
//
//

#import "AppPlatform_ios.h"
#import "AppConfig.h"
#include "CAppPlatform.h"

@implementation AppPlatform_ios

@synthesize appNewVersionAlert;
@synthesize appNewVersionURL;

static AppPlatform_ios* s_pAppPlatform = nil;
+(AppPlatform_ios*) sharedAppPlatform
{
    if (s_pAppPlatform == nil) {
        s_pAppPlatform = [[AppPlatform_ios alloc] init];
    }
    return s_pAppPlatform;
}

-(void)tapjoyConnectSuccess:(NSNotification*)notifyObj
{
    CAppPlatform::sharedAppPlatform()->getTapJopPoint();
    [Tapjoy setVideoAdDelegate:self];
}

-(void)tapjoyViewClosed:(NSNotification*)notifyObj
{
    CAppPlatform::sharedAppPlatform()->getTapJopPoint();
}
- (void)showDirectPlayVideoAd:(NSNotification*)notification
{
	CAppPlatform::sharedAppPlatform()->showDirectPlayVideoAd();
}

- (void)showDirectPlayVideoAdError:(NSNotification*)notification
{
	CAppPlatform::sharedAppPlatform()->showDirectPlayVideoAdError();
}

- (void)videoAdBegan
{
}
- (void)videoAdClosed
{
}
- (void)videoAdError:(NSString*)errorMsg
{
}

-(void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex
{
    if (alertView == appNewVersionAlert) {
        [[UIApplication sharedApplication] openURL:[NSURL URLWithString:appNewVersionURL]];
    }
}


- (void)didExit
{
    CAppPlatform::sharedAppPlatform()->getAarkiPoint();
}

#pragma mark - Brand Connect delegate

- (void)brandConnectDidInitWithCampaignInfo:(NSDictionary *)campaignInfo
{
    if([[campaignInfo objectForKey:@"numberAvailableCampaigns"] intValue] > 0)
    {
        NSString *massage = [NSString stringWithFormat:@"brandConnect initialized: \n ads:%@ \n total credits: %@ \n first campaign credits: %@ \n",[campaignInfo objectForKey:@"numberAvailableCampaigns"],[campaignInfo objectForKey:@"totalNumberCredits"],[campaignInfo objectForKey:@"firstCampaignCredits"]];
        NSLog(@"%@",massage);
    }
    else
    {
        NSLog(@"brand connect did init with campaign info");
    }

}

- (void)brandConnectDidFailInitWithError:(NSDictionary *)error
{
    NSLog(@"brand connect did fail with error");

}

- (void)brandConnectWindowWillOpen
{
    NSLog(@"brand connect window will open");
}

- (void)brandConnectWindowDidClose
{
    NSLog(@"brand connect window will close");
    CAppPlatform::sharedAppPlatform()->getSSAPoint();
}

- (void)brandConnectDidFinishAd:(NSDictionary *)campaignInfo;
{
    NSLog(@"brand connect did finish ad with credits: %d", [[campaignInfo objectForKey:@"credits"] integerValue]);
}

- (void)brandConnectNoMoreOffers
{
    NSLog(@"brandConnectNoMoreOffers");
}

- (void)offerWallDidClose
{
    NSLog(@"offerWallDidClose");
    CAppPlatform::sharedAppPlatform()->getSSAPoint();
}
@end
