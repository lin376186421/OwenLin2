//
//  GeXinDelegate.cpp
//  GameClient
//
//  Created by qsun on 14-10-15.
//
//

#import <Foundation/Foundation.h>
#include "GeXinDelegate.h"
#include "AppConfig.h"

@implementation GeXinDelegate

@synthesize gexinPusher = _gexinPusher;
@synthesize appKey = _appKey;
@synthesize appSecret = _appSecret;
@synthesize appID = _appID;
@synthesize clientId = _clientId;
@synthesize sdkStatus = _sdkStatus;
@synthesize lastPayloadIndex = _lastPaylodIndex;
@synthesize payloadId = _payloadId;

- (void)dealloc
{
    [_deviceToken release];
    
    [_gexinPusher release];
    [_appKey release];
    [_appSecret release];
    [_appID release];
    [_clientId release];
    [_payloadId release];
    
    [super dealloc];
}

- (NSString *)currentLogFilePath
{
    NSMutableArray * listing = [NSMutableArray array];
    NSString *docsDirectory = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES) objectAtIndex:0];
    NSArray * fileNames = [[NSFileManager defaultManager] contentsOfDirectoryAtPath:docsDirectory error:nil];
    if (!fileNames) {
        return nil;
    }
    
    for (NSString * file in fileNames) {
        if (![file hasPrefix:@"_log_"]) {
            continue;
        }
        
        NSString * absPath = [docsDirectory stringByAppendingPathComponent:file];
        BOOL isDir = NO;
        if ([[NSFileManager defaultManager] fileExistsAtPath:absPath isDirectory:&isDir]) {
            if (isDir) {
                [listing addObject:absPath];
            } else {
                [listing addObject:absPath];
            }
        }
    }
    
    [listing sortUsingComparator:^(NSString *l, NSString *r) {
     return [l compare:r];
     }];
    
    if (listing.count) {
        return [listing objectAtIndex:listing.count - 1];
    }
    
    return nil;
}

- (id) init
{
    _appID = [NSString stringWithUTF8String:GETUI_APP_ID];
    _appKey = [NSString stringWithUTF8String:GETUI_APP_KEY];
    _appSecret = [NSString stringWithUTF8String:GETUI_APP_SECRET];
            
    return self;
}

- (void) didRegisterForRemoteNotificationsWithDeviceToken:(NSString*) deviceToken
{
    NSString *token = [[deviceToken description] stringByTrimmingCharactersInSet:[NSCharacterSet characterSetWithCharactersInString:@"<>"]];
    [_deviceToken release];
	_deviceToken = [[token stringByReplacingOccurrencesOfString:@" " withString:@""] retain];
    NSLog(@"deviceToken:%@", _deviceToken);
    
    // [3]:向个推服务器注册deviceToken
    if (_gexinPusher) {
        [_gexinPusher registerDeviceToken:_deviceToken];
    }
}

- (void) didFailToRegisterForRemoteNotificationsWithError
{
    // [3-EXT]:如果APNS注册失败，通知个推服务器
    if (_gexinPusher) {
        [_gexinPusher registerDeviceToken:@""];
    }
}

- (void)startSdkWith:(NSString *)appID appKey:(NSString *)appKey appSecret:(NSString *)appSecret
{
    if (!_gexinPusher) {
        _sdkStatus = SdkStatusStoped;
        
        self.appID = appID;
        self.appKey = appKey;
        self.appSecret = appSecret;
        
        [_clientId release];
        _clientId = nil;
        
        NSError *err = nil;
        _gexinPusher = [GexinSdk createSdkWithAppId:_appID
                                             appKey:_appKey
                                          appSecret:_appSecret
                                         appVersion:@"0.0.0"
                                           delegate:self
                                              error:&err];
        if (_gexinPusher) {
            _sdkStatus = SdkStatusStarting;
        }
    }
}

- (void)startSdk
{
    _appID = [NSString stringWithUTF8String:GETUI_APP_ID];
    _appKey = [NSString stringWithUTF8String:GETUI_APP_KEY];
    _appSecret = [NSString stringWithUTF8String:GETUI_APP_SECRET];
    
    [self startSdkWith:_appID appKey:_appKey appSecret:_appSecret];
}

- (void)stopSdk
{
    if (_gexinPusher) {
        [_gexinPusher destroy];
        [_gexinPusher release];
        _gexinPusher = nil;
        
        _sdkStatus = SdkStatusStoped;
        
        [_clientId release];
        _clientId = nil;
    }
}

- (BOOL)checkSdkInstance
{
    if (!_gexinPusher) {
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"错误" message:@"SDK未启动" delegate:nil cancelButtonTitle:nil otherButtonTitles:@"确定", nil];
        [alertView show];
        [alertView release];
        return NO;
    }
    return YES;
}

- (void)setDeviceToken:(NSString *)aToken
{
    if (![self checkSdkInstance]) {
        return;
    }
    
    [_gexinPusher registerDeviceToken:aToken];
}

- (BOOL)setTags:(NSArray *)aTags error:(NSError **)error
{
    if (![self checkSdkInstance]) {
        return NO;
    }
    
    return [_gexinPusher setTags:aTags];
}

- (NSString *)sendMessage:(NSData *)body error:(NSError **)error {
    if (![self checkSdkInstance]) {
        return nil;
    }
    
    return [_gexinPusher sendMessage:body error:error];
}

- (void)testSdkFunction
{
    
}

- (void)testSendMessage
{
    
}

#pragma mark - GexinSdkDelegate
- (void)GexinSdkDidRegisterClient:(NSString *)clientId
{
    // [4-EXT-1]: 个推SDK已注册
    _sdkStatus = SdkStatusStarted;
    [_clientId release];
    _clientId = [clientId retain];
    
    //    [self stopSdk];
}

- (void)GexinSdkDidReceivePayload:(NSString *)payloadId fromApplication:(NSString *)appId
{
    // [4]: 收到个推消息
    [_payloadId release];
    _payloadId = [payloadId retain];
}

- (void)GexinSdkDidSendMessage:(NSString *)messageId result:(int)result {
    // [4-EXT]:发送上行消息结果反馈
//    NSString *record = [NSString stringWithFormat:@"Received sendmessage:%@ result:%d", messageId, result];
}

- (void)GexinSdkDidOccurError:(NSError *)error
{
    // [EXT]:个推错误报告，集成步骤发生的任何错误都在这里通知，如果集成后，无法正常收到消息，查看这里的通知。
}

@end