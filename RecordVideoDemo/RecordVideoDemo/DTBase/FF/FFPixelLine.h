//
//  FFPixelLine.h
//  FFStory
//
//  Created by PageZhang on 15/6/17.
//  Copyright (c) 2015年 PageZhang. All rights reserved.
//

#import <UIKit/UIKit.h>

typedef NS_ENUM(NSInteger, FFLinePosition) {
    FFLinePositionTop    = 1,
    FFLinePositionLeft   = 2,
    FFLinePositionBottom = 3,
    FFLinePositionRight  = 4,
    FFLinePositionTLBR   = 5,
    FFLinePositionBLTR   = 6
};

@interface FFPixelLine : UIView
@property (nonatomic) IBInspectable NSInteger position;
@property (nonatomic, strong) IBInspectable UIColor *stokeColor;
@end
