//
//  GeXinDelegate.h
//  GameClient
//
//  Created by qsun on 14-10-15.
//
//

#ifndef __GameClient__GeXinDelegate__
#define __GameClient__GeXinDelegate__

#import "GexinSdk.h"

typedef enum {
    SdkStatusStoped,
    SdkStatusStarting,
    SdkStatusStarted
} SdkStatus;

@interface GeXinDelegate : UIResponder <UIApplicationDelegate, GexinSdkDelegate> {
    NSString *_deviceToken;
}

@property (strong, nonatomic) GexinSdk *gexinPusher;

@property (retain, nonatomic) NSString *appKey;
@property (retain, nonatomic) NSString *appSecret;
@property (retain, nonatomic) NSString *appID;
@property (retain, nonatomic) NSString *clientId;
@property (assign, nonatomic) SdkStatus sdkStatus;

@property (assign, nonatomic) int lastPayloadIndex;
@property (retain, nonatomic) NSString *payloadId;

- (id)init;

- (void) didRegisterForRemoteNotificationsWithDeviceToken:(NSString*) deviceToken;
- (void) didFailToRegisterForRemoteNotificationsWithError;

- (void)startSdkWith:(NSString *)appID appKey:(NSString *)appKey appSecret:(NSString *)appSecret;

- (void)startSdk;
- (void)stopSdk;

- (void)setDeviceToken:(NSString *)aToken;
- (BOOL)setTags:(NSArray *)aTag error:(NSError **)error;
- (NSString *)sendMessage:(NSData *)body error:(NSError **)error;

- (void)testSdkFunction;
- (void)testSendMessage;

@end

#endif /* defined(__GameClient__GeXinDelegate__) */
