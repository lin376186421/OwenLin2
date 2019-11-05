//
//  TableViewController.h
//  PodTest
//
//  Created by 林 on 16/6/22.
//  Copyright © 2016年 林. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface TableViewController : UIViewController<UITableViewDelegate,UITableViewDataSource>
@property (weak, nonatomic) IBOutlet UITableView *tableView;

@end
