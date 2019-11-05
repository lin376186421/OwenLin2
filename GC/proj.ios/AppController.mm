//
//  GameClientAppController.mm
//  GameClient
//
//  Created by zhs007 on 13-4-20.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//
#import <UIKit/UIKit.h>
#import "AppController.h"
#import "cocos2d.h"
#import "EAGLView.h"
#import "AppDelegate.h"

#import "RootViewController.h"
//#import "Flurry.h"
#include "HelloWorldScene.h"
#include "AppConfig.h"
#include "CAppPlatform.h"
#import <FacebookSDK/FacebookSDK.h>
#import <MobileAppTracker/MobileAppTracker.h>
#import "Aarki.h"
#import "AarkiContact.h"
#import "AarkiUserBalance.h"

@implementation AppController

@synthesize window;
@synthesize viewController;

#pragma mark -
#pragma mark Application lifecycle

// cocos2d application instance
static AppDelegate s_sharedApplication;

static AppController* s_pSharedAppController = NULL;
+(AppController*) sharedAppController
{
    if (s_pSharedAppController == NULL) {
        s_pSharedAppController = [[AppController alloc] init];
    }
    return s_pSharedAppController;
}

- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions {
    
    // Override point for customization after application launch.
    if (s_pSharedAppController == NULL) {
        s_pSharedAppController = self;
    }
        
    // Override point for customization after application launch.

    // Add the view controller's view to the window and display.
    window = [[UIWindow alloc] initWithFrame: [[UIScreen mainScreen] bounds]];
    EAGLView *__glView = [EAGLView viewWithFrame: [window bounds]
                                     pixelFormat: kEAGLColorFormatRGBA8
                                     depthFormat: GL_DEPTH_COMPONENT16
                              preserveBackbuffer: NO
                                      sharegroup: nil
                                   multiSampling: NO
                                 numberOfSamples:0 ];

    // Use RootViewController manage EAGLView
    viewController = [[RootViewController alloc] initWithNibName:nil bundle:nil];
    viewController.wantsFullScreenLayout = YES;
    viewController.view = __glView;
    
    [__glView setMultipleTouchEnabled:true];

    // Set RootViewController to window
    if ( [[UIDevice currentDevice].systemVersion floatValue] < 6.0)
    {
        // warning: addSubView doesn't work on iOS6
        [window addSubview: viewController.view];
    }
    else
    {
        // use this method on ios6
        [window setRootViewController:viewController];
    }
    
    [window makeKeyAndVisible];

    [[UIApplication sharedApplication] setStatusBarHidden: YES];
    
//    [UIApplication sharedApplication].idleTimerDisabled = YES;

    gexinDelegate = [[GeXinDelegate alloc] init];
    [gexinDelegate startSdk];
    
    [self registerRemoteNotification];
    
    CCApplication::sharedApplication()->run();
    
    return YES;
}

- (void)registerRemoteNotification
{
#ifdef __IPHONE_8_0 
    if ([[UIApplication sharedApplication] respondsToSelector:@selector(registerUserNotificationSettings:)]) {
        UIUserNotificationSettings *settings = [UIUserNotificationSettings settingsForTypes:UIUserNotificationTypeBadge|UIUserNotificationTypeSound|UIUserNotificationTypeAlert categories:nil];
        [[UIApplication sharedApplication] registerUserNotificationSettings:settings];
    }
    else {
        UIRemoteNotificationType myTypes = UIRemoteNotificationTypeBadge | UIRemoteNotificationTypeAlert | UIRemoteNotificationTypeSound;
        [[UIApplication sharedApplication] registerForRemoteNotificationTypes:myTypes];
    }
#else
    UIRemoteNotificationType myTypes = (UIRemoteNotificationType)(UIRemoteNotificationTypeAlert|UIRemoteNotificationTypeSound|UIRemoteNotificationTypeBadge);
    [[UIApplication sharedApplication] registerForRemoteNotificationTypes:myTypes];
#endif
}

- (void)application:(UIApplication *)app didRegisterForRemoteNotificationsWithDeviceToken:(NSData *)devToken {
    NSString* token = [NSString stringWithFormat:@"%@", devToken];
    CAppPlatform::sharedAppPlatform()->registerServerPush([token UTF8String]);
    
    [gexinDelegate didRegisterForRemoteNotificationsWithDeviceToken:token];
}

- (void)application:(UIApplication *)app didFailToRegisterForRemoteNotificationsWithError:(NSError *)err {
    NSLog(@"Error in registration. Error: %@", err);
    
    [gexinDelegate didFailToRegisterForRemoteNotificationsWithError];
}

- (void)application:(UIApplication *)application didReceiveRemoteNotification:(NSDictionary *)userInfo {

}

- (BOOL)application:(UIApplication *)application openURL:(NSURL *)url
  sourceApplication:(NSString *)sourceApplication annotation:(id)annotation {
    
    [FBAppCall handleOpenURL:url sourceApplication:sourceApplication fallbackHandler:^(FBAppCall *call) {
        
        if (call.appLinkData && call.appLinkData.targetURL) {
            [[NSNotificationCenter defaultCenter] postNotificationName:[NSString stringWithUTF8String:APP_STORE_URL] object:call.appLinkData.targetURL];
        }
        
    }];
    
    return YES;
}

- (void)applicationWillResignActive:(UIApplication *)application {
    /*
     Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
     Use this method to pause ongoing tasks, disable timers, and throttle down OpenGL ES frame rates. Games should use this method to pause the game.
     */
    cocos2d::CCDirector::sharedDirector()->pause();
    
//    HelloWorld::pause();
}

- (void)applicationDidBecomeActive:(UIApplication *)application {
    /*
     Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
     */
//    [FBAppCall handleDidBecomeActive];
//    NSDictionary *info = [[NSBundle mainBundle] infoDictionary];    //获取info－plist
//    NSString *facebookId = [info objectForKey:@"FacebookAppID"];
//    [FBSettings setDefaultAppID:facebookId];
//    [FBAppEvents activateApp];
    [MobileAppTracker measureSession];
    [gexinDelegate startSdk];
    
//    [AarkiContact registerApp:[NSString stringWithUTF8String:AARKI_SECRET_KEY]];
    
    [AarkiContact registerApp:[NSString stringWithUTF8String:AARKI_SECRET_KEY]];
    
    CCDirector::sharedDirector()->resume();
}

- (void)applicationDidEnterBackground:(UIApplication *)application {
    /*
     Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
     If your application supports background execution, called instead of applicationWillTerminate: when the user quits.
     */
    
    [self scheduleBgTask];
    [gexinDelegate stopSdk];
    
    CCApplication::sharedApplication()->applicationDidEnterBackground();
}

- (void)applicationWillEnterForeground:(UIApplication *)application {
    /*
     Called as part of  transition from the background to the inactive state: here you can undo many of the changes made on entering the background.
     */
    CCApplication::sharedApplication()->applicationWillEnterForeground();
}

// application will be killed
- (void)applicationWillTerminate:(UIApplication *)application
{
    /*
     Called when the application is about to terminate.
     See also applicationDidEnterBackground:.
     */
    [[FBSession activeSession] close];
    
    ((AppDelegate*)(cocos2d::CCApplication::sharedApplication()))->applicationWillTerminate();
}

UIBackgroundTaskIdentifier _bgTask;
-(void) scheduleBgTask
{
    UIDevice* device = [UIDevice currentDevice];
    
    BOOL backgroundSupported = NO;
    
    if ([device respondsToSelector:@selector(isMultitaskingSupported)])
    {
        backgroundSupported = device.multitaskingSupported;
    }
    if (backgroundSupported && _bgTask == UIBackgroundTaskInvalid)
    {
        UIApplication* app = [UIApplication sharedApplication];
        
        _bgTask = [app beginBackgroundTaskWithExpirationHandler:^{
            NSLog(@"background task %d ExpirationHandler fired remaining time %d.",_bgTask, (int)app.backgroundTimeRemaining);
        }];
        // Start the long-running task and return immediately.
        dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
            // Do the work associated with the task.
            while (app.applicationState == UIApplicationStateBackground && _bgTask != UIBackgroundTaskInvalid)
            {
                [NSThread sleepForTimeInterval:1.0];
                
                //do
            }
            
            [app endBackgroundTask:_bgTask];
            _bgTask = UIBackgroundTaskInvalid;
        });
    }
}

#pragma mark -
#pragma mark Memory management

- (void)applicationDidReceiveMemoryWarning:(UIApplication *)application {
    /*
     Free up as much memory as possible by purging cached data objects that can be recreated (or reloaded from disk) later.
     */

//    CCDirector::sharedDirector()->purgeCachedData();
    
    CCLog("Did Receive Memory Warning!");
}


- (void)dealloc {
    
    [gexinDelegate release];
    [super dealloc];
}


@end

