//
//  FlurryAdsDelegate.h
//  RobinHood
//
//  Created by qsun on 13-2-1.
//
//

#import <Foundation/Foundation.h>
#import "NativeXSDK.h"
@interface CNativeXAdDelegate : NSObject<NativeXAdViewDelegate,NativeXSDKDelegate>
{
    BOOL isNativeCreated;
}

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
@end
