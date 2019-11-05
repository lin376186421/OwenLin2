//
//  ViewController1.m
//  TestFrameWork
//
//  Created by 林 on 17/2/8.
//  Copyright © 2017年 林. All rights reserved.
//

#import "ViewController1.h"
#import "DrawView.h"

@interface ViewController1 ()

@end

@implementation ViewController1

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
    DrawView *view = [[DrawView alloc] initWithFrame:self.view.bounds];
    view.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
    [self.view addSubview:view];
}

@end
