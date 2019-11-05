//
//  DTPubUtil.m
//  DrivingTest
//
//  Created by hunter on 9/23/12.
//  Copyright (c) 2012 Bo Pai. All rights reserved.
//

#import "DTPubUtil.h"

#import "JGProgressHUD.h"

@implementation DTPubUtil

#pragma mark - Private Methods

+ (void)showToastWithText:(NSString *)text view:(UIView*)view
{
    if (text.length <= 0) {
        return;
    }
    
    [self stopLoading];
    
    JGProgressHUD *toast = [[JGProgressHUD alloc] initWithStyle:JGProgressHUDStyleDark];
    toast.indicatorView = nil;
    toast.userInteractionEnabled = NO;
    
    toast.textLabel.text = text;
    toast.position = JGProgressHUDPositionBottomCenter;
    toast.marginInsets = UIEdgeInsetsMake(0, 0, SCREEN_HEIGHT * 0.1, 0);
    
    [toast showInView:view];
    [toast dismissAfterDelay:2.5];
}

+ (void)showToastWithText:(NSString *)text
{
    [self showToastWithText:text delay:2.5];
}

+ (void)showToastWithText:(NSString *)text delay:(NSTimeInterval)delay
{
    if (text.length <= 0) {
        return;
    }
    
    [self stopLoading];
    if (delay<=0) {
        delay = 2.5f;
    }
    
    JGProgressHUD *toast = [[JGProgressHUD alloc] initWithStyle:JGProgressHUDStyleDark];
    toast.indicatorView = nil;
    toast.userInteractionEnabled = NO;
    
    toast.textLabel.text = text;
    toast.position = JGProgressHUDPositionBottomCenter;
    toast.marginInsets = UIEdgeInsetsMake(0, 0, SCREEN_HEIGHT * 0.1, 0);
    
    [toast showInView:[UIApplication sharedApplication].keyWindow];
    [toast dismissAfterDelay:delay];
}

+ (void)showLoadingWithText:(NSString *)text
{
    [self stopLoading];
    
    JGProgressHUD *loadingView = [[JGProgressHUD alloc] initWithStyle:JGProgressHUDStyleDark];
//    loadingView.indicatorView = nil;
    loadingView.userInteractionEnabled = YES;
    
    loadingView.textLabel.text = text;
    loadingView.position = JGProgressHUDPositionCenter;
//    loadingView.marginInsets = UIEdgeInsetsMake(0, 0, SCR_HEIGHT * 0.1, 0);
    
    [loadingView showInView:[UIApplication sharedApplication].keyWindow];
}

+ (void)stopLoading
{
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    for (UIView *view in window.subviews) {
        if ([view isKindOfClass:[JGProgressHUD class]]) {
            JGProgressHUD *loading = (JGProgressHUD *)view;
            loading.hidden = YES;
            [loading dismissAfterDelay:0.01];
        }
    }
}

+ (void)stopLoadingDelay:(NSTimeInterval)delay
{
    UIWindow *window = [UIApplication sharedApplication].keyWindow;
    for (UIView *view in window.subviews) {
        if ([view isKindOfClass:[JGProgressHUD class]]) {
            JGProgressHUD *loading = (JGProgressHUD *)view;
            loading.hidden = YES;
            [loading dismissAfterDelay:delay];
        }
    }
}

#pragma mark - CLProgressHUD


+ (void)showHUDNoNetWorkHintInWindow
{
    [self showToastWithText:@"网络连接异常"];
//    CLProgressHUD *hud = [self getCurrentHUD];
//    if (hud==nil) {
//        hud = [[CLProgressHUD alloc] init];
//        [hud showInView:[[UIApplication sharedApplication].delegate window] animated:YES];
//    } else {
//        [hud stopHide];
//    }
//    hud.type = CLProgressHUDTypeTextAndAnimationWarning;
//    hud.message = @"网络不给力";
//    [hud hide:2.5f];
}

+ (void)showHUDErrorHintInWindow:(NSString *)msg
{
    [self showToastWithText:msg];
//    CLProgressHUD *hud = [self getCurrentHUD];
//    if (hud==nil) {
//        hud = [[CLProgressHUD alloc] init];
//        [hud showInView:[[UIApplication sharedApplication].delegate window] animated:YES];
//    } else {
//        [hud stopHide];
//    }
//
//    if ([msg isEqualToString:@"网络不给力"] || [msg isEqualToString:@"网络异常"]) {
//        hud.type = CLProgressHUDTypeTextAndAnimationWarning;
//    } else {
//        hud.type = CLProgressHUDTypeTextAndAnimationError;
//    }
//    hud.message = msg;
//    [hud hide:2.5f];
}

+ (void)showHUDSuccessHintInWindow:(NSString *)msg
{
    [self showToastWithText:msg];
//    CLProgressHUD *hud = [self getCurrentHUD];
//    if (hud==nil) {
//        hud = [[CLProgressHUD alloc] init];
//        [hud showInView:[[UIApplication sharedApplication].delegate window] animated:YES];
//    } else {
//        [hud stopHide];
//    }
//    hud.type = CLProgressHUDTypeTextAndAnimationSuccess;
//    hud.message = msg;
//    [hud hide:2.5f];
}

+ (void)showHUDMessageInWindow:(NSString *)msg
{
    [self showToastWithText:msg];
//    if (!([DTPubUtil deviceType]==DTDeviceTypeIPhone320x568)&&[[[UIApplication sharedApplication].delegate window] firstTextResponder]) {
//        [self showHUDMessageInWindow:msg textOffset:-64];
//    } else {
//        [self showHUDMessageInWindow:msg textOffset:0.0f];
//    }
}

+ (void)showHUDMessageInWindow:(NSString *)msg textOffset:(CGFloat)offset
{
    [self showToastWithText:msg];
//    CLProgressHUD *hud = [self getCurrentHUD];
//    if (hud==nil) {
//        hud = [[CLProgressHUD alloc] init];
//        [hud showInView:[[UIApplication sharedApplication].delegate window] animated:YES];
//    } else {
//        [hud stopHide];
//    }
//    hud.type = CLProgressHUDTypeText;
//    hud.message = msg;
//    hud.offset = offset;
//    if (hud.userInteractionEnabled) {
//        hud.type = CLProgressHUDTypeLoading;
//    } else {
//        [hud hide:2.5f];
//    }
}

+ (void)showLoadingHUDMessageInWindow:(NSString *)msg
{
    [self showLoadingWithText:msg];
//    CLProgressHUD *hud = [self getCurrentHUD];
//    if (hud==nil) {
//        hud = [[CLProgressHUD alloc] init];
//        [hud showInView:[[UIApplication sharedApplication].delegate window] animated:YES];
//    } else {
//        [hud stopHide];
//    }
//    hud.type = CLProgressHUDTypeRunAnimationLoading;
//    hud.message = msg;
}

+ (void)stopLoadingHUD
{
    [self stopLoading];
//    CLProgressHUD *hud = [self getCurrentHUD];
//    [hud hide:0.01f];
}

+ (void)stopLoadingHUDDelay:(CGFloat)delay
{
    [self stopLoadingDelay:delay];
//    CLProgressHUD *hud = [self getCurrentHUD];
//    [hud hide:delay];
}


+ (void)addBlockOnMainThread:(void (^)(void))block
{
    dispatch_async(dispatch_get_main_queue(), ^{
        block();
    });
}

+ (void)addBlockOnGlobalThread:(void (^)(void))block
{
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), ^{
        block();
    });
}

+ (void)addBlockOnBackgroundThread:(void (^)(void))block
{
    [self performSelectorInBackground:@selector(runBlockInBackground:) withObject:block];
}

+ (void)addBlock:(void (^)(void))block withDelay:(CGFloat)delay
{
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delay * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
        block();
    });
}

@end


@implementation UIView (CoreUtil)

UIColor * randomColor() {
    return [UIColor colorWithRed:arc4random()%256/255.f green:arc4random()%256/255.f blue:arc4random()%256/255.f alpha:1.0];
}

- (void)showSubViewLayerborder
{
#ifdef DEBUG
    [self showLayerborder];
    for (UIView *view in self.subviews) {
        [view showSubViewLayerborder];
    }
#endif
}

- (void)showSubView
{
#ifdef DEBUG
    for (UIView *view in self.subviews) {
        view.backgroundColor = randomColor();
        [view showSubView];
    }
#endif
}

- (void)showLayerborder
{
#ifdef DEBUG
    self.layer.borderWidth = 1.0f;
    self.layer.borderColor = randomColor().CGColor;
#endif
}

@end
