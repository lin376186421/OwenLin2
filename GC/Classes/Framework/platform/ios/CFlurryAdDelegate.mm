////
////  FlurryAdsDelegate.m
////  RobinHood
////
////  Created by qsun on 13-2-1.
////
////
//
//#import "CFlurryAdDelegate.h"
//#import "Flurry.h"
//#import "FlurryAds.h"
//#import "NGNKit.h"
//#include "CAppPlatform.h"
//@implementation CFlurryAdDelegate
//
//- (id) init
//{
//    if(self = [super init]){
//        
//    }
//    
//    return self;
//}
//// Other code
//// Set the delegate
//- (void) spaceDidReceiveAd:(NSString*)adSpace
//{
//    // Show the ad if desired
////    [FlurryAds displayAdForSpace:[self myAdSpace] onView:[self view]];
//}
//
//- (void) spaceDidFailToReceiveAd:(NSString*)adSpace error:(NSError *)error
//{
//    // Handle failure to receive ad    
//    
//    CCLOG([[error localizedDescription] UTF8String],"");
//}
//- (BOOL) spaceShouldDisplay:(NSString*)adSpace interstitial:(BOOL)interstitial
//{
//    // Decide if the Ad should be displayed
//    return true;
//}
//- (void) spaceDidFailToRender:(NSString *)space error:(NSError *)error
//{
//    // Handle a failure to render the ad
//}
//- (void)spaceWillDismiss:(NSString *)adSpace
//{
//    // Handle the user dismissing the ad
//}
//- (void)spaceDidDismiss:(NSString *)adSpace
//{
//    // Handle the closing of the ad
//}
//- (void)spaceWillLeaveApplication:(NSString *)adSpace
//{
//    // Handle the user leaving the application
//}
//
//- (void) spaceDidReceiveClick:(NSString*)adSpace
//{
//    
//}
//- (void)videoDidFinish:(NSString *)adSpace
//{
//    LogicInfo::getSingleton().m_addGemType = ADD_GEM_FLURRLY;
//    LogicInfo::getSingleton().requestAdsGem(1);
//    NSLog(@"videoDidFinish %@",adSpace);
//}
//
//@end
