//
//  UIView+Layer.h
//  FFStory
//
//  Created by PageZhang on 16/5/9.
//  Copyright © 2016年 eclicks. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface UIView (Layer)
@property (nonatomic, assign) IBInspectable CGFloat cornerRadius;
@property (nonatomic, assign) IBInspectable CGFloat borderWidth;
@property (nonatomic, strong) IBInspectable UIColor *borderColor;
@end
