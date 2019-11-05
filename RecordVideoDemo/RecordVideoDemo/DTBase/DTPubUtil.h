//
//  DTPubUtil.h
//  DrivingTest
//
//  Created by hunter on 9/23/12.
//  Copyright (c) 2012 Bo Pai. All rights reserved.
//

#import <Foundation/Foundation.h>



@interface DTPubUtil : NSObject

+ (void)showToastWithText:(NSString *)text view:(UIView*)view;
+ (void)showToastWithText:(NSString *)text;
+ (void)showToastWithText:(NSString *)text delay:(NSTimeInterval)delay;
+ (void)showLoadingWithText:(NSString *)text;
+ (void)stopLoading;
+ (void)stopLoadingDelay:(NSTimeInterval)delay;

// MARK: CLLoading

+ (void)showAlertWithMessage:(NSString *)messsage;

+ (void)showHUDNoNetWorkHintInWindow;
+ (void)showHUDErrorHintInWindow:(NSString *)msg;
+ (void)showHUDSuccessHintInWindow:(NSString *)msg;
+ (void)showHUDMessageInWindow:(NSString *)msg;
+ (void)showHUDMessageInWindow:(NSString *)msg textOffset:(CGFloat)offset;
+ (void)showLoadingHUDMessageInWindow:(NSString *)msg;
+ (void)stopLoadingHUD;
+ (void)stopLoadingHUDDelay:(CGFloat)delay;

+ (void)addBlockOnMainThread:(void (^)(void))block;
+ (void)addBlockOnGlobalThread:(void (^)(void))block;
+ (void)addBlockOnBackgroundThread:(void (^)(void))block;
+ (void)addBlock:(void (^)(void))block withDelay:(CGFloat)delay;

@end

@interface UIView (CoreUtil)

/**
 *  调试用的方法
 */
- (void)showSubViewLayerborder;
- (void)showSubView;
- (void)showLayerborder;

UIColor * randomColor();

@end
