//
//  FFActionSheet.m
//  FFStory
//
//  Created by PageZhang on 16/3/17.
//  Copyright © 2016年 Chelun. All rights reserved.
//

#import "FFActionSheet.h"
#import "FFAlertView.h"
#import "UIActionSheet+BlocksKit.h"
#import "NSObject+BKAssociatedObjects.h"

@interface FFActionSheet () {
    BOOL _cancelConfig;
    id associatedObject;
}
@end

@implementation FFActionSheet

+ (void)callTel:(NSString *)tel
          title:(NSString *)title
           from:(UIViewController *)from
         sender:(UIView *)sender {
    FFActionSheet *actionSheet = [[FFActionSheet alloc] initWithTitle:title];
    [actionSheet addActionWithTitle:[NSString stringWithFormat:@"拨打 %@", tel] handler:^{
        [DTPubUtil callPhoneNumber:tel];
    }];
    [actionSheet showInViewController:from sender:sender];
}

- (instancetype)initWithTitle:(NSString *)title {
    return [self initWithTitle:title message:nil];
}
- (instancetype)initWithTitle:(NSString *)title message:(NSString *)message {
    if (self = [super init]) {
        if (FFVersionGreaterThanIOS8()) {
            associatedObject = [UIAlertController alertControllerWithTitle:title message:message preferredStyle:UIAlertControllerStyleActionSheet];
        } else {
            if (title.length==0) {
                title = message;
            } else if (message.length) {
                title = [NSString stringWithFormat:@"%@\n%@", title, message];
            }
            associatedObject = [UIActionSheet bk_actionSheetWithTitle:title];
        }
        [FFAlertController bk_associateValue:associatedObject withKey:@"FFAlertController"];
    }
    return self;
}
- (void)addActionWithTitle:(NSString *)title handler:(void (^)(void))block {
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
    if (FFVersionGreaterThanIOS8()) {
        [associatedObject addAction:[UIAlertAction actionWithTitle:title style:UIAlertActionStyleDestructive handler:^(UIAlertAction *action) {
            [self.class alertWillDisAppear];
            if (block) block();
        }]];
    } else {
        [associatedObject bk_setDestructiveButtonWithTitle:title handler:^{
            [self.class alertWillDisAppear];
            if (block) block();
        }];
    }
}
- (void)setCancelButtonWithTitle:(NSString *)title handler:(void (^)(void))block {
    if (title == nil) title = @"取消";
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
    _cancelConfig = YES;
}
- (BOOL)showInViewController:(UIViewController *)vc {
    return [self showInViewController:vc sender:nil];
}
- (BOOL)showInViewController:(UIViewController *)vc sender:(UIView *)sender {
    if (![self.class isDismissed]) return NO;
    [self.class alertWillAppear];
    
    if (!_cancelConfig) {
        [self setCancelButtonWithTitle:nil handler:NULL];
    }
    if (vc == nil) vc = FFWindowTopViewController();
    if (FFVersionGreaterThanIOS8()) {
        if (FF_iPad()) {
            [associatedObject setModalPresentationStyle:UIModalPresentationPopover];
            UIPopoverPresentationController *popPresenter = [associatedObject popoverPresentationController];
            if (sender) {
                popPresenter.sourceView = sender;
                popPresenter.sourceRect = sender.bounds;
            } else {
                popPresenter.sourceView = vc.view;
                popPresenter.sourceRect = CGRectMake(0, vc.view.bounds.size.height-1, vc.view.bounds.size.width, 1);
            }
            popPresenter.permittedArrowDirections = UIPopoverArrowDirectionUp|UIPopoverArrowDirectionDown;
        }
        [vc presentViewController:associatedObject animated:YES completion:NULL];
    } else {
        [associatedObject showInView:vc.view];
    }
    return YES;
}

@end
