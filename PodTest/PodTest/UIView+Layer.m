//
//  UIView+Layer.m
//  FFStory
//
//  Created by PageZhang on 16/5/9.
//  Copyright © 2016年 eclicks. All rights reserved.
//

#import "UIView+Layer.h"
#import "NSObject+BKAssociatedObjects.h"

@implementation UIView (Layer)

- (void)awakeFromNib {
    [super awakeFromNib];
    if (self.cornerRadius > 0) {
        self.layer.cornerRadius = self.cornerRadius;
        self.layer.masksToBounds = YES;
    }
    if (self.borderWidth > 0) {
        self.layer.borderWidth = self.borderWidth;
    }
    if (self.borderColor) {
        self.layer.borderColor = self.borderColor.CGColor;
    }
}

- (CGFloat)cornerRadius {
    return [[self bk_associatedValueForKey:@"layer.cornerRadius"] floatValue];
}
- (void)setCornerRadius:(CGFloat)cornerRadius {
    [self bk_associateValue:@(cornerRadius) withKey:@"layer.cornerRadius"];
}
- (CGFloat)borderWidth {
    return [[self bk_associatedValueForKey:@"layer.borderWidth"] floatValue];
}
- (void)setBorderWidth:(CGFloat)borderWidth {
    [self bk_associateValue:@(borderWidth) withKey:@"layer.borderWidth"];
}
- (UIColor *)borderColor {
    return [self bk_associatedValueForKey:@"layer.borderColor"];
}
- (void)setBorderColor:(UIColor *)borderColor {
    [self bk_associateValue:borderColor withKey:@"layer.borderColor"];
}

@end
