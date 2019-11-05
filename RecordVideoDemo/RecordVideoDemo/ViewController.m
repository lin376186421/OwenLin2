//
//  ViewController.m
//  RecordVideoDemo
//
//  Created by 林 on 2019/1/21.
//  Copyright © 2019 林. All rights reserved.
//

#import "ViewController.h"
#import "SCGPUCreatVideoController.h"
@interface ViewController ()

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (IBAction)click:(id)sender {
    
    WEAK_SELF
     UIAlertController *vc = [UIAlertController alertControllerWithTitle:@"选择主题" message:nil preferredStyle:UIAlertControllerStyleActionSheet];
    [vc addAction:[UIAlertAction actionWithTitle:@"1" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        [weakSelf chooseId:0];
    }]];
    [vc addAction:[UIAlertAction actionWithTitle:@"2" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        [weakSelf chooseId:1];
    }]];
    [vc addAction:[UIAlertAction actionWithTitle:@"3" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        [weakSelf chooseId:2];
    }]];
    [vc addAction:[UIAlertAction actionWithTitle:@"4" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        [weakSelf chooseId:3];
    }]];
    [vc addAction:[UIAlertAction actionWithTitle:@"5" style:UIAlertActionStyleDefault handler:^(UIAlertAction *action) {
        [weakSelf chooseId:4];
    }]];
    [vc addAction:[UIAlertAction actionWithTitle:@"取消" style:UIAlertActionStyleCancel handler:^(UIAlertAction *action) {
        [vc dismissViewControllerAnimated:YES completion:nil];
    }]];
    [self presentViewController:vc animated:YES completion:NULL];
    
}

- (void)chooseId:(NSUInteger)gId
{
    //://create/video?id=2
    SCGPUCreatVideoController *vc = [[SCGPUCreatVideoController alloc] init];
    vc.sourceId = [NSString stringWithFormat:@"local%zd",gId];
    UINavigationController *nav = [[UINavigationController alloc] initWithRootViewController:vc];
    [self presentViewController:nav animated:YES completion:nil];
}

@end
