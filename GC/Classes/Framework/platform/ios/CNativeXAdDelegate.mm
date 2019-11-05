//
//  FlurryAdsDelegate.m
//  RobinHood
//
//  Created by qsun on 13-2-1.
//
//

#import "CNativeXAdDelegate.h"
#import "CAppPlatform.h"
#import "AppController.h"
#include "PlayerScene.h"
#include "GameWorldScene.h"

@implementation CNativeXAdDelegate
- (id) init
{
    if(self = [super init]){
        isNativeCreated = NO;
    }
    return self;
}

//used to set a specific parent view controller for the AdView
- (UIViewController *)presentingViewControllerForAdView:(NativeXAdView *) adView
{
    //Return the UIViewController you would like us to use when presenting the NativeX ad view
    UIViewController *viewController = (UIViewController*)[AppController sharedAppController].viewController;
    
    return viewController;
}

- (void)nativeXAdView:(NativeXAdView *)adView didLoadWithPlacement:(NSString *)placement
{
    NSLog(@"[%@-- didLoadWithPlacement] didLoadWithPlacement", adView.placement);
    if([placement isEqualToString:[NSString stringWithUTF8String:NATIVE_X_GAMELAUNCH]] == NO)
    {
        if(PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pMainUI)
        {
            PlayerScene::getSingleton().m_pGameWorld->m_pMainUI->hideAdsLoadingUI();
        }
        [[NativeXSDK sharedInstance] showReadyAdWithCustomPlacement:placement];
    }
}

//Called if there was no ad to load.
- (void)nativeXAdViewNoAdContent:(NativeXAdView *)adView
{
    NSLog(@"[%@] nativeXAdViewNoAdContent", adView.placement);
    LogicInfo::getSingleton().onNotify(NOTIFY_ADS_LOADINF_FAILD);
}

//Called if the enhanced ad view failed to load
- (void)nativeXAdView:(NativeXAdView *)adView didFailWithError:(NSError *)error
{
    LogicInfo::getSingleton().onNotify(NOTIFY_ADS_LOADINF_FAILD);
    NSLog(@"[%@] nativeXAdView:didFailWithError: %@", adView.placement, error);
}

- (void)nativeXAdViewDidExpire:(NativeXAdView *)adView
{
    LogicInfo::getSingleton().onNotify(NOTIFY_ADS_LOADINF_FAILD);
    NSLog(@"[%@] nativeXAdViewDidExpire", adView.placement);
}

//Called right before ad is about to display
- (void)nativeXAdViewWillDisplay:(NativeXAdView *)adView
{
    NSLog(@"Test App: [%@] nativeXAdViewWillDisplay", adView.placement);
}

//called after ad is fully displayed
- (void)nativeXAdViewDidDisplay:(NativeXAdView *)adView
{
    NSLog(@"[%@] nativeXAdViewDidDisplay", adView.placement);
}

//Called right before the view will dismiss
- (void)nativeXAdViewWillDismiss:(NativeXAdView *)adView
{
    NSLog(@"[%@] nativeXAdViewWillDismiss", adView.placement);
}

//Called after ad is fully dismissed
- (void)nativeXAdViewDidDismiss:(NativeXAdView *)adView
{
    NSLog(@"[%@] nativeXAdViewDidDismiss", adView.placement);
    if([adView.placement isEqualToString:[NSString stringWithUTF8String:NATIVE_X_GAMELAUNCH]] == YES)
        [[NativeXSDK sharedInstance] fetchAdWithCustomPlacement:adView.placement delegate:self];
    CAppPlatform::sharedAppPlatform()->getNativeXPoint();
}


/** Called when the Offer Wall is successfully initialized. */
- (void)nativeXSDKDidCreateSession
{
    NSLog(@"nativeXSDKDidCreateSession");
    if(isNativeCreated == NO)
    {
        [[NativeXSDK sharedInstance] fetchAdWithCustomPlacement:[NSString stringWithUTF8String:NATIVE_X_GAMELAUNCH] delegate:self];
        isNativeCreated = YES;
    }
}

/** Called when there is an error trying to initialize the Offer Wall.
 *
 * @param   error
 *          reason why create session call failed
 */
- (void)nativeXSDKDidFailToCreateSession: (NSError *) error
{
    NSLog(@"nativeXSDKDidFailToCreateSession:%@",error.description);
}

/** Called when the currency redemption is successfull.
 *
 * @param   redeemedCurrencyInfo
 *          an object containing messages, balances and recieptIDs
 */
- (void)nativeXSDKDidRedeemWithCurrencyInfo:(NativeXRedeemedCurrencyInfo *)redeemedCurrencyInfo
{
    
    if(redeemedCurrencyInfo)
    {
        // Called after redeemCurrency
        NSLog(@"Messages: %@", redeemedCurrencyInfo.messages);
        NSLog(@"Redeemed: %@", redeemedCurrencyInfo.balances);
        NSLog(@"Redeem Receipts: %@", redeemedCurrencyInfo.receipts);
        int count = 0;
        for(int i = 0; i < redeemedCurrencyInfo.balances.count; i++)
        {
            NativeXCurrency *currency = redeemedCurrencyInfo.balances[i];
            count += [currency.amount intValue];
        }
        
        if(count >0)
        {
            LogicInfo::getSingleton().m_pendingNativeXGem += count;
            if(PlayerScene::getSingleton().m_pGameWorld && PlayerScene::getSingleton().m_pGameWorld->m_pMainUI)
            {
                LogicInfo::getSingleton().m_addGemType = ADD_GEM_NATIVE_X;
                LogicInfo::getSingleton().requestAdsGem(count);
            }
        }
        //Reward the user the currency
        
        //Show successful redemption alert
        if(APP_DEBUG)
            [redeemedCurrencyInfo showRedeemAlert];
    }
}

/** Called when the currency redemption is unsuccessfull.
 *
 * @param   error
 *          reason why redeem currency call failed
 */
- (void)nativeXSDKDidRedeemWithError:(NSError *)error
{
    NSLog(@"nativeXSDKDidRedeemWithError:%@",error.description);
}

- (void)nativeXAdViewWillExpand:(NativeXAdView *)adView
{
    NSLog(@"[%@] nativeXAdViewWillDismiss", adView.placement);
}
- (void)nativeXAdViewDidExpand:(NativeXAdView *)adView
{
    NSLog(@"[%@] nativeXAdViewWillDismiss", adView.placement);
}

@end
