//
//  FFAlertView.m
//  FFStory
//
//  Created by PageZhang on 16/3/17.
//  Copyright © 2016年 Chelun. All rights reserved.
//

#import "FFAlertView.h"
#import "FFActionSheet.h"
#import "UIAlertView+BlocksKit.h"
#import "NSObject+BKAssociatedObjects.h"

@interface FFAlertView () {
    int _itemsCount;
    id associatedObject;
}
@end

@implementation FFAlertView

+ (void)callTel:(NSString *)tel
          title:(NSString *)title
           from:(UIViewController *)from {
    FFAlertView *alertView = [[FFAlertView alloc] initWithTitle:title message:tel];
    [alertView setCancelButtonWithTitle:@"取消" handler:NULL];
    [alertView addActionWithTitle:@"确定" handler:^{
        [DTPubUtil callPhoneNumber:tel];
    }];
    [alertView showInViewController:from];
}

- (instancetype)initWithTitle:(NSString *)title {
    return [self initWithTitle:title message:nil];
}
- (instancetype)initWithTitle:(NSString *)title message:(NSString *)message {
    if (self = [super init]) {
        if (FFVersionGreaterThanIOS8()) {
            associatedObject = [UIAlertController alertControllerWithTitle:title message:message preferredStyle:UIAlertControllerStyleAlert];
        } else {
            associatedObject = [UIAlertView bk_alertViewWithTitle:title message:message];
        }
        [FFAlertController bk_associateValue:associatedObject withKey:@"FFAlertController"];
    }
    return self;
}
- (void)addActionWithTitle:(NSString *)title handler:(void (^)(void))block {
    _itemsCount++;
    if (FFVersionGreaterThanIOS8()) {
        [associatedObject addAction:[UIAlertAction actionWithTitle:title style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
            [self.class alertWillDisAppear];
            if (block) block();
        }]];
    } else {
        [associatedObject bk_addButtonWithTitle:title handler:^{
            [self.class alertWillDisAppear];
            if (block) block();
        }];
    }
}
- (void)setDestructiveButtonWithTitle:(NSString *)title handler:(void (^)(void))block {
    _itemsCount++;
    if (FFVersionGreaterThanIOS8()) {
        [associatedObject addAction:[UIAlertAction actionWithTitle:title style:UIAlertActionStyleDestructive handler:^(UIAlertAction *action) {
            [self.class alertWillDisAppear];
            if (block) block();
        }]];
    } else {
        [associatedObject bk_addButtonWithTitle:title handler:^{
            [self.class alertWillDisAppear];
            if (block) block();
        }];
    }
}
- (void)setCancelButtonWithTitle:(NSString *)title handler:(void (^)(void))block {
    _itemsCount++;
    if (title == nil) title = @"确定";
    if (FFVersionGreaterThanIOS8()) {
        [associatedObject addAction:[UIAlertAction actionWithTitle:title style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
            [self.class alertWillDisAppear];
            if (block) block();
        }]];
    } else {
        [associatedObject bk_setCancelButtonWithTitle:title handler:^{
            [self.class alertWillDisAppear];
            if (block) block();
        }];
    }
}
- (BOOL)showInViewController:(UIViewController *)vc {
    if (![self.class isDismissed]) return NO;
    [self.class alertWillAppear];
    
    if (_itemsCount == 0) {
        [self setCancelButtonWithTitle:nil handler:NULL];
    }
    if (FFVersionGreaterThanIOS8()) {
        if (vc == nil) vc = FFWindowTopViewController();
        [vc presentViewController:associatedObject animated:YES completion:NULL];
    } else {
        [associatedObject show];
    }
    return YES;
}

@end
