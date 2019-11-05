//
//  SCCreateVideoProgressView.m
//  SuperCoach
//
//  Created by 林 on 2018/12/20.
//  Copyright © 2018 Lin Feihong. All rights reserved.
//
#import "UIColor+Util.h"
#import "SCCreateVideoProgressView.h"


@interface SCCreateVideoProgressView ()

@property (nonatomic, assign) CGFloat progress;

@property (nonatomic,strong ) CAShapeLayer *backLayer;
@property (nonatomic, strong) CAShapeLayer *progressLayer;

@end

@implementation SCCreateVideoProgressView
- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        
    }
    return self;
}


-(void)updateProgressWithValue:(CGFloat)progress
{
    if (progress >= 1.f) {
        progress = 1.f;
    }
    _progress = progress;
    _progressLayer.opacity = 0;
    [self setNeedsDisplay];
}
- (void)drawRect:(CGRect)rect {
    
    [self drawCycleProgress];
    
}
- (void)drawCycleProgress
{
    CGPoint center = CGPointMake(self.frame.size.width/2, self.frame.size.height/2);
    CGFloat radius = self.frame.size.width/2;
    CGFloat startA = - M_PI_2;
    CGFloat endA = -M_PI_2 + M_PI * 2 * _progress;
    
    if (!_backLayer && self.frame.size.width > 0 && self.frame.size.height > 0) {
        _backLayer = [CAShapeLayer layer];
        _backLayer.frame = self.bounds;
        _backLayer.fillColor = [[UIColor clearColor] CGColor];
        _backLayer.strokeColor = [[UIColor whiteColor] CGColor];
        _backLayer.opacity = 1; //背景颜色的透明度
        _backLayer.lineCap = kCALineCapRound;
        _backLayer.lineWidth = 5;
        UIBezierPath *path0 = [UIBezierPath bezierPathWithArcCenter:center radius:radius startAngle:0 endAngle: M_PI * 2 clockwise:YES];
        _backLayer.path =[path0 CGPath];
        [self.layer addSublayer:_backLayer];
    }
    
    if (_progressLayer) {
        [_progressLayer removeFromSuperlayer];
        _progressLayer = nil;
    }
    
    _progressLayer = [CAShapeLayer layer];
    _progressLayer.frame = self.bounds;
    _progressLayer.fillColor = [[UIColor clearColor] CGColor];
    _progressLayer.strokeColor = [UIColor colorWithString:@"07B168"].CGColor;
    _progressLayer.opacity = 1; //背景颜色的透明度
    _progressLayer.lineCap = kCALineCapButt;
    _progressLayer.lineWidth = 5;
    UIBezierPath *path = [UIBezierPath bezierPathWithArcCenter:center radius:radius startAngle:startA endAngle:endA clockwise:YES];
    _progressLayer.path =[path CGPath];
    [self.layer addSublayer:_progressLayer];
    
}

-(void)resetProgress
{
    [self updateProgressWithValue:0];
}

@end

typedef enum : NSUInteger {
    SCCreateVideoRecordViewAniNormal,//正常状态
    SCCreateVideoRecordViewAniBiging,//放大中
    SCCreateVideoRecordViewAniScalingSmall,//来回缩放
    SCCreateVideoRecordViewAniScalingBig,//来回缩放
} SCCreateVideoRecordViewAniStatus;

@interface SCCreateVideoRecordView()
{
    UIView *_midView;
    CAShapeLayer *_progressLayer;
    
    UIView *_whiteView;
    UIView *_maskView;
    
    CADisplayLink *_link;
    
    CGFloat _whiteR;
    CGFloat _clearR;
    
}

@property (nonatomic, assign) SCCreateVideoRecordViewAniStatus aniStatus;

@end


@implementation SCCreateVideoRecordView

- (instancetype)initWithFrame:(CGRect)frame
{
    self = [super initWithFrame:frame];
    if (self) {
        
        self.backgroundColor = [UIColor clearColor];
        
        UIBezierPath *path = [UIBezierPath bezierPathWithArcCenter:CGPointMake(self.width/2, self.height/2) radius:51.f startAngle:-M_PI_2 endAngle:-M_PI_2+2*M_PI clockwise:YES];
        CAShapeLayer *shapeLayer = [CAShapeLayer layer];
        shapeLayer.frame = self.bounds;
        shapeLayer.path = path.CGPath;
        shapeLayer.strokeColor = SCGreenColor.CGColor;
        shapeLayer.fillColor = [UIColor clearColor].CGColor;
        shapeLayer.lineWidth = 2.f;
        shapeLayer.lineCap = kCALineCapRound;
        shapeLayer.lineJoin = kCALineJoinRound;
        shapeLayer.strokeStart = 0.f;
        shapeLayer.strokeEnd = 0.f;
        _progressLayer = shapeLayer;
        [self.layer addSublayer:_progressLayer];
        
        _midView = [[UIView alloc] initWithFrame:CGRectMake(self.width/2-60/2, self.height/2-60/2, 60, 60)];
        _midView.backgroundColor = SCGreenColor;
        _midView.layer.cornerRadius = _midView.height/2;
        _midView.layer.masksToBounds = YES;
        _midView.userInteractionEnabled = NO;
        _midView.layer.masksToBounds = YES;
        [self addSubview:_midView];

    }
    return self;
}

- (void)setAniStatus:(SCCreateVideoRecordViewAniStatus)aniStatus
{
    _aniStatus = aniStatus;
    if (_aniStatus == SCCreateVideoRecordViewAniNormal) {
        _progressLayer.strokeEnd = 0.f;
        _progressLayer.hidden = YES;
        [self removeLink];
        _whiteR = 80.f;
        _clearR = 70.f;
        [self setNeedsDisplay];
    } else if (_aniStatus == SCCreateVideoRecordViewAniBiging) {
        [self createLink];
        _progressLayer.hidden = YES;
    } else {
        _progressLayer.hidden = NO;
    }
}

- (void)createLink
{
    [self removeLink];
    _link = [CADisplayLink displayLinkWithTarget:self selector:@selector(tick:)];
    [_link addToRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
}

- (void)tick:(CADisplayLink *)link
{
    if (link == _link) {
        
        CGFloat whiteMaxR = 100.f;
        CGFloat clearMaxR = 90.f;
        CGFloat clearMinR = 65.f;
        CGFloat offR = 0.6;
        if (_aniStatus == SCCreateVideoRecordViewAniBiging) {
            offR = 0.7;
            if (_whiteR < whiteMaxR) {
                _whiteR += offR;
            }
            
            if (_clearR < clearMaxR) {
                _clearR += offR;
            }
            
            if (_clearR >= clearMaxR && _whiteR >= whiteMaxR) {
                _clearR = clearMaxR;
                _whiteR = whiteMaxR;
                self.aniStatus = SCCreateVideoRecordViewAniScalingSmall;
            }
        } else if (_aniStatus == SCCreateVideoRecordViewAniScalingSmall) {
            if (_clearR > clearMinR) {
                _clearR -= offR;
            }
            
            if (_clearR <= clearMinR) {
                _clearR = clearMinR;
                self.aniStatus = SCCreateVideoRecordViewAniScalingBig;
            }
        } else if (_aniStatus == SCCreateVideoRecordViewAniScalingBig) {
            if (_clearR < clearMaxR) {
                _clearR += offR;
            }
            
            if (_clearR >= clearMaxR) {
                _clearR = clearMaxR;
                self.aniStatus = SCCreateVideoRecordViewAniScalingSmall;
            }
        }
        [self setNeedsDisplay];
    }
}

- (void)drawRect:(CGRect)rect
{
    [super drawRect:rect];
    CGContextRef context = UIGraphicsGetCurrentContext();
    
    CGContextSaveGState(context);
    
    CGFloat r = _whiteR;
    CGContextSetFillColorWithColor(context, [[UIColor whiteColor] colorWithAlphaComponent:0.2].CGColor);
    CGRect whiteRect = CGRectMake(self.width/2-r/2, self.height/2-r/2, r, r);
    CGContextAddEllipseInRect(context, whiteRect);
    
    CGContextFillPath(context);
    
//    CGContextRestoreGState(context);
//    CGContextSaveGState(context);
    
    CGFloat clearR = _clearR;

    CGRect clearRect = CGRectMake(self.width/2-clearR/2, self.height/2-clearR/2 ,clearR , clearR);
    CGContextAddEllipseInRect(context, clearRect);

    CGContextClip(context);
    CGContextClearRect(context, clearRect);

}

- (void)removeLink
{
    if (_link) {
        [_link invalidate];
        _link = nil;
    }
}

- (void)startAni
{
    [UIView animateWithDuration:0.5 animations:^{
        CGPoint center = _midView.center;
        CGRect rect = _midView.frame;
        rect.size = CGSizeMake(28, 28);
        _midView.frame = rect;
        _midView.layer.cornerRadius = 4;
        _midView.center = center;
    }];
    self.aniStatus = SCCreateVideoRecordViewAniBiging;
}


- (void)stopAni
{
    [UIView animateWithDuration:0.5 animations:^{
        CGPoint center = _midView.center;
        CGRect rect = _midView.frame;
        rect.size = CGSizeMake(60, 60);
        _midView.frame = rect;
        _midView.layer.cornerRadius = 30;
        _midView.center = center;
    }];
    self.aniStatus = SCCreateVideoRecordViewAniNormal;
}

-(void)resetProgress
{
    self.aniStatus = SCCreateVideoRecordViewAniNormal;
    [self updateProgressWithValue:0];
}

-(void)updateProgressWithValue:(CGFloat)progress
{
    if (progress >= 1.f) {
        progress = 1.f;
    }
    _progressLayer.strokeEnd = progress;
}

@end
