//
//  SCCreateVideoProgressView.h
//  SuperCoach
//
//  Created by 林 on 2018/12/20.
//  Copyright © 2018 Lin Feihong. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface SCCreateVideoProgressView : UIView
- (instancetype)initWithFrame:(CGRect)frame;
-(void)updateProgressWithValue:(CGFloat)progress;
-(void)resetProgress;
@end


@interface SCCreateVideoRecordView : UIView

- (void)startAni;
- (void)stopAni;
-(void)resetProgress;

-(void)updateProgressWithValue:(CGFloat)progress;

@end
