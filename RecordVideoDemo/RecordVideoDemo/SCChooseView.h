//
//  SCChooseView.h
//  RecordVideoDemo
//
//  Created by 林 on 2019/1/22.
//  Copyright © 2019 林. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SCChooseView : UIView

@property (nonatomic, copy) SCCustomCallBack selectBlock;

+ (instancetype)showInView:(UIView *)superView;

@end
