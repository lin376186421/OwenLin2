//
//  SearchDetailViewController.m
//  PodTest
//
//  Created by 林 on 16/8/12.
//  Copyright © 2016年 林. All rights reserved.
//

#import "SearchDetailViewController.h"
#import <YYModel.h>
#import "AppDelegate.h"

@interface SearchDetailViewController ()
{
    IBOutlet UILabel *nameLb;
    
    IBOutlet UILabel *addressLb;
    IBOutlet UILabel *numLb;
    IBOutlet UILabel *ageLb;
}

@end

@implementation SearchDetailViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view.
}

-(void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    [self setUp];
}

-(void)setObj:(DataObj *)obj
{
    _obj = obj;
}

-(void)setUp
{
    nameLb.text = _obj.name;
    addressLb.text = _obj.address;
    numLb.text = _obj.num;
    ageLb.text = [@(_obj.age) stringValue];
    
    NSUserActivity *userActivity = [[NSUserActivity alloc]initWithActivityType:@"com.owenlinttttt.iOS-9-Search.displayShow"];
    NSMutableDictionary*dic = [_obj selfToDic];
    userActivity.userInfo = dic;
    userActivity.title = _obj.name;
    userActivity.keywords = [NSSet setWithObjects:_obj.num,_obj.address,[@(_obj.age) stringValue], nil];
    userActivity.eligibleForHandoff = NO;//不用于设备间的传输
    userActivity.eligibleForSearch = YES;//用于Spotlight搜索
    [userActivity becomeCurrent];
    
    [AppDelegate update];
}

@end
