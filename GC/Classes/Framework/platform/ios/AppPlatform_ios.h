//
//  AppPlatform_ios.h
//  GameClient
//
//  Created by qsun on 13-7-31.
//
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>  
#import <GameKit/GameKit.h>
#import <Tapjoy/Tapjoy.h>
#import "Aarki.h"
@interface AppPlatform_ios : UIViewController<UIAlertViewDelegate,TJCVideoAdDelegate,AarkiDelegate>
{
    UIAlertView* _appNewVersionAlert;
    NSString* _appNewVersionURL;
}

@property(nonatomic, assign) UIAlertView* appNewVersionAlert;
@property(nonatomic, retain) NSString* appNewVersionURL;

+(AppPlatform_ios*) sharedAppPlatform;

@end
