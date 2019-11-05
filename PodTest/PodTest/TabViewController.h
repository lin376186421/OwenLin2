//
//  TabViewController.h
//  TestProgress
//
//  Created by 林 on 16/6/21.
//  Copyright © 2016年 林. All rights reserved.
//

#import <UIKit/UIKit.h>
#import "DTHorizontalScrollView.h"

@interface TabViewController : UIViewController<DTHorizontalScrollViewDataSource,DTHorizontalScrollViewDelegate>

@property (nonatomic , strong) NSArray *controllerList;
@property (weak, nonatomic) IBOutlet DTHorizontalScrollView *scrollView;

@end
