//
//  FFAlertController.m
//  FFStory
//
//  Created by PageZhang on 16/4/6.
//  Copyright © 2016年 Chelun. All rights reserved.
//

#import "FFAlertController.h"
#import "FFAlertView.h"
#import "FFActionSheet.h"
#import "NSObject+BKAssociatedObjects.h"

@implementation FFAlertController
- (instancetype)initWithTitle:(NSString *)title {return nil;}
- (instancetype)initWithTitle:(NSString *)title message:(NSString *)message {return nil;}
- (void)addActionWithTitle:(NSString *)title handler:(void (^)(void))block {}
- (void)setDestructiveButtonWithTitle:(NSString *)title handler:(void (^)(void))block {}
- (void)setCancelButtonWithTitle:(NSString *)title handler:(void (^)(void))block {}
- (BOOL)showInViewController:(UIViewController *)vc {return NO;}

+ (void)alertWillAppear {
    [FFAlertController bk_associateValue:@(YES) withKey:@"FFAlertController.Appear"];
}
+ (void)alertWillDisAppear {
    [FFAlertController bk_removeAllAssociatedObjects];
}

+ (BOOL)isDismissed {
    return ![[FFAlertController bk_associatedValueForKey:@"FFAlertController.Appear"] boolValue];
}
+ (void)dismiss {
    if (![self isDismissed]) {
        [self alertWillDisAppear];
        // 注销视图
        id associatedObject = [FFAlertController bk_associatedValueForKey:@"FFAlertController"];
        if (FFVersionGreaterThanIOS8()) {
            [associatedObject dismissViewControllerAnimated:NO completion:NULL];
        } else {
            [associatedObject dismissWithClickedButtonIndex:[associatedObject cancelButtonIndex] animated:NO];
        }
    }
}

@end
