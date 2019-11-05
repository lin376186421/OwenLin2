//
//  GameClientAppController.h
//  GameClient
//
//  Created by zhs007 on 13-4-20.
//  Copyright __MyCompanyName__ 2013年. All rights reserved.
//

#import "GeXinDelegate.h"

@class RootViewController;

@interface AppController : NSObject <UIAccelerometerDelegate, UIAlertViewDelegate, UITextFieldDelegate,UIApplicationDelegate> {
    UIWindow *window;
    RootViewController    *viewController;
    
    GeXinDelegate* gexinDelegate;
}

@property (nonatomic, retain) UIWindow *window;
@property (nonatomic, retain) RootViewController *viewController;

+(AppController*) sharedAppController;

- (void)registerRemoteNotification;

@end

