//
//  NativeXSDK.h
//  NativeXSDK
//
//  This file is subject to the SDK Source Code License Agreement defined in file
//  "SDK_SourceCode_LicenseAgreement", which is part of this source code package.
//
//  Created by Patrick Brennan on 10/6/11.
//  Copyright 2013 NativeX. All rights reserved.

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import "NativeXAdView.h"
#import "NativeXCurrency.h"
#import "NativeXRedeemedCurrencyInfo.h"
#import "NativeXInAppPurchaseTrackRequest.h"

/** These are predefined placements that can be associated with Multi-Format Ads 
 *
 *  AD PLACEMENTS:
 *  - NativeX Ad Placement: a pre-defined list of game interaction points.
 *  - Custom Placement: a string that describes the interaction point of an ad within your game.
 *
 *  BENEFITS:
 *  - Higher eCPMs
 *  - Control over ad format
 *  - Enhanced Reporting
 *  - Turn off ads per placement
 **/

typedef enum {
    kAdPlacementGameLaunch,              //"Game Launch"
    kAdPlacementMainMenuScreen,          //"Main Menu Screen"
    kAdPlacementPauseMenuScreen,         //"Pause Menu Screen"
    kAdPlacementPlayerGeneratedEvent,    //"Player Generated Event"
    kAdPlacementLevelCompleted,          //"Level Completed"
    kAdPlacementLevelFailed,             //"Level Failed"
    kAdPlacementPlayerLevelsUp,          //"Player Levels Up"
    kAdPlacementP2P_CompetitionWon,      //"P2P competition won"
    kAdPlacementP2P_CompetitionLost,     //"P2P competition lost"
    kAdPlacementStoreOpen,               //"Store Open"
    kAdPlacementExitFromApp              //"Exit Ad from Application"
} NativeXAdPlacement;

//portrait: 320x50 or 768x66
//landscape: 480x32 or 1024x66
typedef enum {
    kBannerPositionTop,
    kBannerPositionBottom,
} NativeXBannerPosition;

@protocol NativeXSDKDelegate;

/** Main class for NativeXSDK */
@interface NativeXSDK : NSObject

@property (nonatomic, strong) id<NativeXSDKDelegate> delegate;
@property (nonatomic, strong) UIViewController *presenterViewController;
@property (nonatomic, assign) BOOL shouldOutputDebugLog;

#pragma mark - Monetization API

/** 
 * Provides access to the NativeXSDK shared object.
 *
 * @return a singleton NativeXSDK instance.
 */
+ (id)sharedInstance;

/** 
 * Provides access to the NativeXSDK version
 *
 * @return NativeXSDK Version
 */
- (NSString *)getSDKVersion;

/**
 * Use this method to get sessionId for current session
 *
 * @return the current sessionId
 */
- (NSString *)getSessionId;

/**
 * Create a session with NativeX offer network.
 * Call this in AppDidFinishLaunchingWithOptions{}
 *
 * @param   appId (required)
 *          the unique Identifier you receive from NativeX
 */
- (void)createSessionWithAppId:(NSString *)appId;

/**
 * Create a session with NativeX offer network.
 * Call this in AppDidFinishLaunchingWithOptions{}
 *
 * @param   appId (required) 
 *          the unique Identifier you receive from NativeX
 * @param   publisherUserId (optional) 
 *          an id that can be used for publisher currency postback
 */
- (void)createSessionWithAppId:(NSString *)appId
       andPublisherUserId:(NSString *)publisherUserId;

/** 
 * Call to fetch and return balances for user
 * Automatically called once on create session
 */
- (void)redeemCurrency;

#pragma mark - NativeX Ad Methods

/**
 * Checks to see if there is an ad of this placement ready 
 * to show.
 *
 * @param   placement (required)
 *          A pre-defined set of game interaction points
 */
- (BOOL)isAdReadyWithPlacement:(NativeXAdPlacement)placement;

/**
 * Checks to see if there is an ad of this placement ready
 * to show.
 *
 * @param   customPlacement (required)
 *          A string that describes the interaction point of an ad within your game
 */
- (BOOL)isAdReadyWithCustomPlacement:(NSString*)customPlacement;

/**
 * Show a multi-format ad with a NativeX placement from key window,
 * used for targeting certain ads for certain in app placements.
 *
 * @param   placement (required) 
 *          A pre-defined set of game interaction points
 */
- (void)showAdWithPlacement:(NativeXAdPlacement)placement;

/**
 * Show a multi-format ad  only if it is loaded and ready to instantly show with a custom
 * placement from key window, used for targeting certain ads for certain in app placements.
 *
 * @param   placement (required)
 *          A pre-defined set of game interaction points
 */
- (void)showReadyAdWithPlacement:(NativeXAdPlacement)placement;

/**
 * Show a multi-format ad  only if it is loaded and ready to instantly show with a custom
 * placement from key window, used for targeting certain ads for certain in app placements.
 *
 * @param   customPlacement (required)
 *          A string that describes the interaction point of an ad within your game
 */
- (void)showReadyAdWithCustomPlacement:(NSString *)customPlacement;

/**
 * Show a multi-format ad with a custom placement from key window,
 * used for targeting certain ads for certain in app placements.
 * 
 * @param   customPlacement (required) 
 *          A string that describes the interaction point of an ad within your game
 */
- (void)showAdWithCustomPlacement:(NSString *)customPlacement;

/**
 * Fetch/cache a multi-format ad that presents modally (fullscreen)
 *
 * @param   placement (required) 
 *          A pre-defined set of game interaction points
 * @param   aDelegate (optional) 
 *          used to set delegate
 */
- (void)fetchAdWithPlacement:(NativeXAdPlacement)placement delegate:(id<NativeXAdViewDelegate>)aDelegate;

/**
 * Fetch/cache a multi-format ad that presents modally (fullscreen)
 * 
 * @param   customPlacement (required) 
 *          A string that describes the interaction point of an ad within your game
 * @param   aDelegate (optional) 
 *          used to set delegate
 */
- (void)fetchAdWithCustomPlacement:(NSString *)customPlacement delegate:(id<NativeXAdViewDelegate>)aDelegate;

/**
 * Dismiss the adview (Insterstitial)
 *
 * @param   placement (required)
 *          A pre-defined game interaction point
 *
 */
- (void)dismissAdWithPlacement:(NativeXAdPlacement)placement;

/**
 * Dismiss the adview (Insterstitial)
 *
 * @param   placement (required)
 *          A custom game interaction point
 *
 */
- (void)dismissAdWithCustomPlacement:(NSString*)placement;

#pragma mark NativeX Banner Methods

/**
 *  Fetch/cache a multi-format banner that presents inline
 *
 *  @param  placement  (required)
 *          A pre-defined game interaction point
 *  @param  adPosition (required)
 *          A pre-defined ad position (top or bottom)
 *  @param  aDelegate  (optional)
 *          delegate object for protocol methods
 */
- (void)fetchBannerWithPlacement:(NativeXAdPlacement)placement
                        position:(NativeXBannerPosition)adPosition
                        delegate:(id<NativeXAdViewDelegate>)aDelegate;

/**
 *  Show a multi-format banner with a NativeX placement from keywindow
 *
 *  @param  placement (required)
 *          A pre-defined game interaction point
 *  @param  adPosition (required)
 *          A pre-defined NativeX ad position (top or botton)
 */
- (void)showBannerWithPlacement:(NativeXAdPlacement)placement position:(NativeXBannerPosition)adPosition;

/**
 *  Fetch/cache a multi-format banner that presents inline
 *
 *  @param  placement  (required)
 *          A string representation of game interaction point
 *  @param  adPosition (required)
 *          A pre-defined NativeX ad position (top or bottom)
 *  @param  aDelegate  (optional)
 *          delegate object for protocol methods
 */
- (void)fetchBannerWithCustomPlacement:(NSString *)placement
                              position:(NativeXBannerPosition)adPosition
                              delegate:(id<NativeXAdViewDelegate>)aDelegate;

/**
 *  Show a multi-format banner
 *
 *  @param  placement (required)
 *          A pre-defined game interaction point
 *  @param  adPosition (required)
 *          A pre-defined NativeX ad position (top or botton)
 */
- (void)showBannerWithCustomPlacement:(NSString *)placement position:(NativeXBannerPosition)adPosition;

/**
 * Advanced: use custom positioning by updating the frame for a fetched banner
 *
 * @param   newFrame
 *          the new position for the banner
 * @param   placement
 *          defines which banner frame you would like to update
 * @warning could result in weird layout behavior for html ad. May need to setup custom template
 */
- (void)setCustomFrame:(CGRect)newFrame forBannerWithPlacement:(NSString *)placement;

/**
 * Dismiss the adview (Banner)
 *
 * @param   placement (required)
 *          A pre-defined game interaction point
 *
 */
- (void)dismissBannerWithPlacement:(NativeXAdPlacement)placement;

/**
 * Dismiss the adview (Banner)
 *
 * @param   placement (required)
 *          A custom game interaction point
 *
 */
- (void)dismissBannerWithCustomPlacement:(NSString*)placement;

#pragma mark NativeX Ads Common Methods

/**
 *  Converts a NativeX pre-defined placement to a string
 *
 *  @param  placement (required)
 *          NativeX pre-defined placement using enum
 *
 *  @return NSString representation of NativeX placement
 */
- (NSString *)convertAdPlacementToString:(NativeXAdPlacement)placement;

#pragma mark - In App Purchase Tracking (IAPT) methods
/**
 * Used for In App Purchase Tracking
 * 
 * @param   trackRecord     
 *          data record of purchase
 * @param   delegate
 *          used to set delegate file
 * 
 * @return  NativeXInAppPurchaseRequest
 */
- (NativeXInAppPurchaseTrackRequest *)trackInAppPurchaseWithTrackRecord:(NativeXInAppPurchaseTrackRecord *)trackRecord
                                                               delegate:(id<NativeXInAppPurchaseTrackDelegate>)delegate; //if the delegate is about to be deallocated clear return value's delegate property

#pragma mark - NativeX Advertiser API (CPE Campaigns)

/**
 * call this to connect to NativeX and inform that the app "actionID" was performed
 * 
 * @param   actionID (required) 
 *          the unique Action Identifier for cost-per-event campaigns, you receive from NativeX
 */
- (void)actionTakenWithActionID:(NSString*)actionID;

@end

#pragma mark - Monetization Protocol Definition

@protocol NativeXSDKDelegate <NSObject>

#pragma mark Required Delegate Methods
@required

/** Called when the Offer Wall is successfully initialized. */
- (void)nativeXSDKDidCreateSession;

/** Called when there is an error trying to initialize the Offer Wall. 
 *
 * @param   error
 *          reason why create session call failed
 */
- (void)nativeXSDKDidFailToCreateSession: (NSError *) error;

/** Called when the currency redemption is successfull. 
 *
 * @param   redeemedCurrencyInfo
 *          an object containing messages, balances and recieptIDs
 */
- (void)nativeXSDKDidRedeemWithCurrencyInfo:(NativeXRedeemedCurrencyInfo *)redeemedCurrencyInfo;

/** Called when the currency redemption is unsuccessfull.
 *
 * @param   error
 *          reason why redeem currency call failed
 */
- (void)nativeXSDKDidRedeemWithError:(NSError *)error;

@optional

/** Called when redirecting user away from application (safari or app store) */
- (void)nativeXSDKWillRedirectUser;


@end
