//
//  TTViewController.m
//  TestFrameWork
//
//  Created by 林 on 17/2/8.
//  Copyright © 2017年 林. All rights reserved.
//

#import "TTViewController.h"
#import "ViewController1.h"
#import "TTViewController3.h"

@interface TTViewController ()

@end

@implementation TTViewController

+ (TTViewController *)createSelf
{
    TTViewController *vc = [[TTViewController alloc] init];
    NSLog(@"ssssss");
    return vc;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    self.view.backgroundColor = [UIColor whiteColor];
    
    UIButton *button = [UIButton buttonWithType:UIButtonTypeCustom];
    button.frame = self.view.bounds;
    [button setTitle:@"测试" forState:UIControlStateNormal];
    [button setTitleColor:[UIColor blackColor] forState:UIControlStateNormal];
    button.titleLabel.font = [UIFont systemFontOfSize:20];
    [self.view addSubview:button];
    [button addTarget:self action:@selector(testAction:) forControlEvents:UIControlEventTouchUpInside];
    // Do any additional setup after loading the view from its nib.
}

- (IBAction)testAction:(id)sender {
    
//    [[NSBundle mainBundle] loadNibNamed:@"TestFrameWork.framework/yourXibName" owner:nil options:nil];

    
    TTViewController3 * vc = [[TTViewController3 alloc] init];
//    ViewController1 *vc = [[ViewController1 alloc] init];
    if (self.navigationController) {
        [self.navigationController pushViewController:vc animated:YES];
    } else {
        [self presentViewController:vc animated:YES completion:nil];
    }
}


+ (NSBundle *)getBundle
{
    return [NSBundle bundleWithPath:[[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent: @"Test.bundle"]];
//    return [NSBundle bundleWithPath: [[NSBundle mainBundle] pathForResource: @"Test" ofType: @"bundle"]];
}

+ (NSString *)getBundlePath: (NSString *) assetName{
    NSBundle *myBundle = [self getBundle];
    if (myBundle && assetName) {
        return [[myBundle resourcePath] stringByAppendingPathComponent: assetName];
    }
    
    return nil;
}

@end
