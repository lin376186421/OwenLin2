//
//  FFConfiguration.h
//  FFStory
//
//  Created by PageZhang on 16/3/3.
//  Copyright © 2016年 Chelun. All rights reserved.
//

#import <Foundation/Foundation.h>

typedef NS_ENUM(NSInteger, FFLaunchType) {
    FFLaunchType_NORMAL,
    FFLaunchType_INSTALL,
    FFLaunchType_UPGRADE
};
typedef NS_ENUM(NSInteger, FFAccessType) {
    FFAccessType_PHOTOS,
    FFAccessType_CAMERA,
    FFAccessType_MICROPHONE,
    FFAccessType_LOCATION,
    FFAccessType_NOTIFICATION
};

@interface FFConfiguration : NSObject

/* 启动应用
 版本号最多2个小数点，次版本号最大允许到9
 update情况下，根据version确定是从哪个版本升级过来
 example：
 1.3 -> 130
 1.3.1 -> 131
 12.9.9 -> 1299
 */
+ (void)launchConfig:(void (^)(FFLaunchType type, NSUInteger version))completion;

// 系统通知
+ (BOOL)localNotif:(NSString *)body;
+ (BOOL)localNotif:(NSString *)title body:(NSString *)body sound:(NSString *)sound userInfo:(NSDictionary *)userInfo;

// 参数值
+ (id)valueForConfig:(NSString *)key;
+ (BOOL)boolForConfig:(NSString *)key;
+ (void)updateConfig:(NSString *)key value:(id)value;

// 声音和振动
+ (void)playSystemVibrate;
+ (void)playSoundWithName:(NSString *)soundName;
+ (void)playSoundAndVibrateWithName:(NSString *)soundName;

// 系统权限
+ (BOOL)accessEnable:(FFAccessType)type;
+ (void)accessPrompt:(FFAccessType)type;
+ (void)accessPrompt:(FFAccessType)type content:(NSString *)content;

@end
