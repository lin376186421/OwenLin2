//
//  ViewController2.m
//  TestProgress
//
//  Created by 林 on 16/6/21.
//  Copyright © 2016年 林. All rights reserved.
//

#import "ViewController2.h"
#import "JTNumberScrollAnimatedView.h"

@interface ViewController2 ()
{
    IBOutlet JTNumberScrollAnimatedView *_aniView;
    
}

@end

@implementation ViewController2

- (void)viewDidLoad {
    [super viewDidLoad];
    [self.view setBackgroundColor:[UIColor colorWithWhite:0 alpha:0.7]];
    _aniView.textColor = [UIColor blackColor];
    _aniView.font = [UIFont systemFontOfSize:30];
    
    _aniView.minLength = 3;
    // Do any additional setup after loading the view.
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    [self testMask1];
    [self testMask2];
}

//遮罩
- (void)testMask1
{
    if ([self.view viewWithTag:11]) {
        [[self.view viewWithTag:11] removeFromSuperview];
    }
    
    UIView *v1 = [[UIView alloc] initWithFrame:CGRectZero];
    v1.size = CGSizeMake(100, 100);
    v1.center = CGPointMake(self.view.width/2-55, self.view.height/2);
    v1.backgroundColor = [UIColor whiteColor];
    v1.tag = 11;
    [self.view addSubview:v1];
    
    UIView *v2 = [[UIView alloc]initWithFrame:CGRectOffset(v1.bounds, -v1.width, 0)];
    v2.backgroundColor = [UIColor blackColor];
    v1.layer.mask = v2.layer;
    
    [UIView animateWithDuration:0.5 animations:^{
        v2.left = 0;
    } completion:^(BOOL finished) {
        [v2 removeFromSuperview];
//        [v1 removeFromSuperview];

    }];
}

- (IBAction)startAni:(id)sender {
    [_aniView setValue:@(7000)];
    [_aniView startAnimation];
}

- (void)testMask2
{
    if ([self.view viewWithTag:22]) {
        [[self.view viewWithTag:22] removeFromSuperview];
    }
    
    UIView *v1 = [[UIView alloc] initWithFrame:CGRectZero];
    v1.size = CGSizeMake(100, 100);
    v1.center = CGPointMake(self.view.width/2 + 55, self.view.height/2);
    v1.backgroundColor = [UIColor whiteColor];
    v1.tag = 22;
    [self.view addSubview:v1];
    
    UIView *v2 = [[UIView alloc]initWithFrame:CGRectZero];
    v2.size = CGSizeMake(v1.width, 1);
    v2.center = CGPointMake(v1.width/2, v1.height/2);
    v2.backgroundColor = [UIColor blackColor];
    v1.layer.mask = v2.layer;
    
    [UIView animateWithDuration:0.5 animations:^{
        v2.height = v1.height;
        v2.center = CGPointMake(v1.width/2, v1.height/2);
    } completion:^(BOOL finished) {
        [v2 removeFromSuperview];
//        [v1 removeFromSuperview];
    }];
    

}

@end
