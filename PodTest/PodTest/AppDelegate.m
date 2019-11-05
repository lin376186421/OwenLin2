//
//  AppDelegate.m
//  PodTest
//
//  Created by 林 on 16/6/20.
//  Copyright © 2016年 林. All rights reserved.
//

#import "AppDelegate.h"
#import <MobileCoreServices/MobileCoreServices.h>
//#import "DKDemoBaseManager.h"
@interface AppDelegate ()

@end

@implementation AppDelegate

+ (void)update
{
    if ([CSSearchableIndex isIndexingAvailable]) {
        // 生成全局唤起应用对象
        NSArray *keywords = @[@"车轮考驾照", @"驾考宝典", @"驾校一点通", @"元贝驾考", @"学车宝典", @"驾考无忧", @"金手指", @"58学车", @"驾考团", @"易驾考", @"驾考大师", @"驾考助手", @"驾照考试通"];
        NSMutableArray *arr = [NSMutableArray array];
        [keywords enumerateObjectsUsingBlock:^(id  _Nonnull obj, NSUInteger idx, BOOL * _Nonnull stop) {
            CSSearchableItemAttributeSet *attr = [[CSSearchableItemAttributeSet alloc] initWithItemContentType:(__bridge NSString *)kUTTypeItem];
            attr.title = obj;
            attr.contentDescription = @"车轮考驾照，交管局2016最新题库";
            [arr addObject:[[CSSearchableItem alloc] initWithUniqueIdentifier:[NSString stringWithFormat:@"spotlight://wakeup?keyword=%@", obj] domainIdentifier:@"search.pottest" attributeSet:attr]];
        }];
        
        [[CSSearchableIndex defaultSearchableIndex] deleteSearchableItemsWithDomainIdentifiers:@[@"search.pottest"] completionHandler:^(NSError * _Nullable error) {
            [[CSSearchableIndex defaultSearchableIndex] indexSearchableItems:arr completionHandler:^(NSError * _Nullable error) {
                
            }];
        }];
        
    }
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    // Override point for customization after application launch.
    
    NSString *activityType = launchOptions[UIApplicationLaunchOptionsUserActivityTypeKey];
    if ([activityType isEqualToString:CSSearchableItemActionType]) {
        NSDictionary *activityDictionary = launchOptions[UIApplicationLaunchOptionsUserActivityDictionaryKey];
        NSLog(@"%@",activityDictionary);
        return NO;
    }
    
//    [DKDemoBaseManager setup];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
    // Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
    // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    // Called as part of the transition from the background to the inactive state; here you can undo many of the changes made on entering the background.
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
}

- (void)applicationWillTerminate:(UIApplication *)application {
    // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
}


-(BOOL)application:(UIApplication *)application continueUserActivity:(NSUserActivity *)userActivity restorationHandler:(void (^)(NSArray * _Nullable))restorationHandler{
    return YES;
}

-(BOOL)application:(UIApplication *)application willContinueUserActivityWithType:(NSString *)userActivityType{
    return YES;
}

@end
