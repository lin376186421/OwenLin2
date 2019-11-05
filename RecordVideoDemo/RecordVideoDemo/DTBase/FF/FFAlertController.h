//
//  FFAlertController.h
//  FFStory
//
//  Created by PageZhang on 16/4/6.
//  Copyright © 2016年 Chelun. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface FFAlertController : NSObject

- (instancetype)initWithTitle:(NSString *)title;
- (instancetype)initWithTitle:(NSString *)title message:(NSString *)message;
- (void)addActionWithTitle:(NSString *)title handler:(void (^)(void))block;
- (void)setDestructiveButtonWithTitle:(NSString *)title handler:(void (^)(void))block;
- (void)setCancelButtonWithTitle:(NSString *)title handler:(void (^)(void))block;
- (BOOL)showInViewController:(UIViewController *)vc;

// 更新状态
+ (void)alertWillAppear;
+ (void)alertWillDisAppear;

+ (BOOL)isDismissed;
+ (void)dismiss;

@end
