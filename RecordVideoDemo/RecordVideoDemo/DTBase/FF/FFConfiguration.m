//
//  FFConfiguration.m
//  FFStory
//
//  Created by PageZhang on 16/3/3.
//  Copyright © 2016年 Chelun. All rights reserved.
//

@import CoreLocation;
@import AVFoundation;
@import AudioToolbox;
#import "FFConfiguration.h"
#import "FFAlertView.h"
#import <libkern/OSAtomic.h>


@interface FFPreference : NSObject {
    OSSpinLock lock;
    NSMutableDictionary *_storage;
}
@end

@implementation FFPreference
+ (instancetype)sharedInstance {
    static __strong FFPreference *_instance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        _instance = [self.class new];
    });
    return _instance;
}
- (instancetype)init {
    if (self = [super init]) {
        lock = OS_SPINLOCK_INIT;
        _storage = [[[NSUserDefaults standardUserDefaults] objectForKey:@"APP_Preference"] mutableCopy];
        if (_storage == nil) _storage = [NSMutableDictionary dictionary];
    }
    return self;
}
- (id)valueForConfig:(NSString *)key {
    OSSpinLockLock(&lock);
    id value = _storage[key];
    OSSpinLockUnlock(&lock);
    return value;
}
- (void)setValue:(id)value forConfig:(NSString *)key {
    OSSpinLockLock(&lock);
    _storage[key] = value;
    OSSpinLockUnlock(&lock);
    // 更新配置
    [[NSUserDefaults standardUserDefaults] setObject:_storage forKey:@"APP_Preference"];
    [[NSUserDefaults standardUserDefaults] synchronize];
}
@end


@implementation FFConfiguration

#pragma mark - Launch
+ (void)launchConfig:(void (^)(FFLaunchType, NSUInteger))completion {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSMutableArray *versions = [[defaults objectForKey:APP_BUNDLE_ID] mutableCopy];
    if (versions == nil) {
        completion(FFLaunchType_INSTALL, 0);
        // 更新记录
        [defaults setObject:@[APP_VERSION_SHORT] forKey:APP_BUNDLE_ID];
        [defaults synchronize];
    } else if (![versions containsObject:APP_VERSION_SHORT]) {
        NSArray *objs = [versions.lastObject componentsSeparatedByString:@"."];
        NSUInteger version = [objs[0] integerValue] *100;
        if (objs.count > 1) version += [objs[1] integerValue] *10;
        if (objs.count > 2) version += [objs[2] integerValue] *1;
        completion(FFLaunchType_UPGRADE, version);
        // 更新记录
        [versions addObject:APP_VERSION_SHORT];
        [defaults setObject:versions forKey:APP_BUNDLE_ID];
        [defaults synchronize];
    } else {
        completion(FFLaunchType_NORMAL, 0);
    }
}

#pragma mark - Notif
+ (BOOL)localNotif:(NSString *)body {
    return [self localNotif:nil body:body sound:nil userInfo:nil];
}
+ (BOOL)localNotif:(NSString *)title body:(NSString *)body sound:(NSString *)sound userInfo:(NSDictionary *)userInfo {
    if ([UIApplication sharedApplication].applicationState != UIApplicationStateActive) {
        UILocalNotification *localNotfi = [UILocalNotification new];
        localNotfi.timeZone = [NSTimeZone localTimeZone];
        localNotfi.alertBody = body;
        localNotfi.soundName = sound;
        localNotfi.userInfo = userInfo;
        if ([localNotfi respondsToSelector:@selector(setAlertTitle:)]) {
            localNotfi.alertTitle = title;
        }
        [[UIApplication sharedApplication] presentLocalNotificationNow:localNotfi];
        return YES;
    }
    return NO;
}

#pragma mark - Config
+ (id)valueForConfig:(NSString *)key {
    return [[FFPreference sharedInstance] valueForConfig:key];
}
+ (BOOL)boolForConfig:(NSString *)key {
    return [[self valueForConfig:key] boolValue];
}
+ (void)updateConfig:(NSString *)key value:(id)value {
    [[FFPreference sharedInstance] setValue:value forConfig:key];
}

#pragma mark - Play
+ (void)playSystemVibrate {
    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
}
+ (void)playSoundWithName:(NSString *)soundName {
    [self playSoundWithName:soundName vibrate:NO];
}
+ (void)playSoundAndVibrateWithName:(NSString *)soundName {
    [self playSoundWithName:soundName vibrate:YES];
}
+ (void)playSoundWithName:(NSString *)soundName vibrate:(BOOL)vibrate {
    SystemSoundID soundID;
    if (soundName.length) {
        static NSMutableDictionary *soundIDs = nil;
        if (soundIDs == nil) {
            soundIDs = [NSMutableDictionary dictionary];
        }
        if ([soundIDs objectForKey:soundName]) {
            soundID = [[soundIDs objectForKey:soundName] unsignedIntValue];
        } else {
            // 创建音效
            NSURL *filePath = [[NSBundle mainBundle] URLForResource:soundName.stringByDeletingPathExtension withExtension:soundName.pathExtension];
            AudioServicesCreateSystemSoundID((__bridge CFURLRef)filePath, &soundID);
            [soundIDs setObject:@(soundID) forKey:soundName];
        }
    } else {
        // 调用系统音效 http://iphonedevwiki.net/index.php/AudioServices
        soundID = 1007;
    }
    if (vibrate) {
        AudioServicesPlayAlertSound(soundID);
    } else {
        AudioServicesPlaySystemSound(soundID);
    }
    // 释放音效资源
//        AudioServicesDisposeSystemSoundID(soundID);
}

#pragma mark - Access
+ (BOOL)accessEnable:(FFAccessType)type {
    switch (type) {
        case FFAccessType_PHOTOS: {
            return [UIImagePickerController isSourceTypeAvailable:UIImagePickerControllerSourceTypePhotoLibrary];
        }
        case FFAccessType_CAMERA: {
            return [AVCaptureDevice authorizationStatusForMediaType:AVMediaTypeVideo] != AVAuthorizationStatusDenied;
        }
        case FFAccessType_MICROPHONE: {
            if (FFVersionGreaterThanIOS7()) {
                __block BOOL _granted = NO;
                dispatch_semaphore_t sema = dispatch_semaphore_create(0);
                AVAudioSession *audioSession = [AVAudioSession sharedInstance];
                [audioSession requestRecordPermission:^(BOOL granted) {
                    _granted = granted;
                    dispatch_semaphore_signal(sema);
                }];
                dispatch_semaphore_wait(sema, DISPATCH_TIME_FOREVER);
                return _granted;
            }
            return YES;
        }
        case FFAccessType_LOCATION: {
            return [CLLocationManager locationServicesEnabled] && [CLLocationManager authorizationStatus] != kCLAuthorizationStatusDenied;
        }
        case FFAccessType_NOTIFICATION: {
            if (FFVersionGreaterThanIOS8()) {
                UIUserNotificationType types = [[UIApplication sharedApplication] currentUserNotificationSettings].types;
                if (types == UIUserNotificationTypeNone) {
                    return NO;
                }
            } else {
                UIRemoteNotificationType type = [[UIApplication sharedApplication] enabledRemoteNotificationTypes];
                if (type == UIRemoteNotificationTypeNone) {
                    return NO;
                }
            }
            return YES;
        }
        default:break;
    }
}

+ (void)accessPrompt:(FFAccessType)type {
    return [self accessPrompt:type content:nil];
}
+ (void)accessPrompt:(FFAccessType)type content:(NSString *)content {
    NSString *message = nil;
    switch (type) {
        case FFAccessType_PHOTOS: {
            message = [NSString stringWithFormat:@"请前往\"设置\"-\"隐私\"-\"照片\"-\"%@\"中开启", APP_DISPLAY_NAME];
            if (!content) content = @"没有权限访问你的相册";
        } break;
        case FFAccessType_CAMERA: {
            message = [NSString stringWithFormat:@"请前往\"设置\"-\"隐私\"-\"相机\"-\"%@\"中开启", APP_DISPLAY_NAME];
            if (!content) content = @"没有权限访问你的相机";
        } break;
        case FFAccessType_MICROPHONE: {
            message = [NSString stringWithFormat:@"请前往\"设置\"-\"隐私\"-\"麦克风\"-\"%@\"中开启", APP_DISPLAY_NAME];
            if (!content) content = @"没有权限访问你的麦克风";
        } break;
        case FFAccessType_LOCATION: {
            message = [NSString stringWithFormat:@"请前往\"设置\"-\"隐私\"-\"定位服务\"-\"%@\"中开启", APP_DISPLAY_NAME];
            if (!content) content = @"没有权限访问你的位置信息";
        } break;
        case FFAccessType_NOTIFICATION: {
            message = [NSString stringWithFormat:@"请前往\"设置\"-\"通知\"-\"%@\"中开启", APP_DISPLAY_NAME];
        } break;
        default:break;
    }
    if (content.length) {
        message = [NSString stringWithFormat:@"%@，%@", content, message];
    }
    // 开启提示
    FFAlertView *alertView = [[FFAlertView alloc] initWithTitle:@"温馨提示" message:message];
    [alertView addActionWithTitle:@"设置" handler:^{
         [[UIApplication sharedApplication] openURL:[NSURL URLWithString:UIApplicationOpenSettingsURLString]];
    }];
    [alertView setCancelButtonWithTitle:nil handler:NULL];
    [alertView showInViewController:nil];
}

@end

