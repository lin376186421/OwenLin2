//
//  TTViewController3.m
//  TestFrameWork
//
//  Created by 林 on 17/2/8.
//  Copyright © 2017年 林. All rights reserved.
//

#import "TTViewController3.h"
#import "TTViewController.h"
#import "ViewController1.h"

@interface TTViewController3 ()

@end

@implementation TTViewController3

- (id)init
{
    NSBundle *myBundle = [TTViewController getBundle];
    
    //self = [super initWithNibName: @"ViewController1" bundle: nil];
    //从bundle中获取界面文件
    self = [self initWithNibName: [NSString stringWithUTF8String: object_getClassName(self)] bundle: myBundle];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        
    }
    return self;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    UIImage *image = [UIImage imageWithContentsOfFile:[TTViewController getBundlePath:@"pop.png"]];
    UIImageView *iv = [[UIImageView alloc] initWithImage:image];
    
    [self.view addSubview:iv];
    // Do any additional setup after loading the view from its nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)action:(id)sender {
    
        ViewController1 *vc = [[ViewController1 alloc] init];
    if (self.navigationController) {
        [self.navigationController pushViewController:vc animated:YES];
    } else {
        [self presentViewController:vc animated:YES completion:nil];
    }
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/

@end
