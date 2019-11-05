//
//  TableViewController.m
//  PodTest
//
//  Created by 林 on 16/6/22.
//  Copyright © 2016年 林. All rights reserved.
//

#import "TableViewController.h"

@interface TableViewController ()
{
    
}
@end

@implementation TableViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    self.tableView.tableFooterView = [[UIView alloc]initWithFrame:CGRectMake(0, 0, self.view.bounds.size.width, 50)];
    self.tableView.tableFooterView.backgroundColor = [UIColor redColor];
    
    self.tableView.tableHeaderView = [[UIView alloc]initWithFrame:CGRectMake(0, 0, self.view.bounds.size.width, 50)];
    self.tableView.tableHeaderView.backgroundColor = [UIColor redColor];
    
    
    UIEdgeInsets inset = self.tableView.contentInset;
    self.tableView.contentInset = UIEdgeInsetsMake(inset.top-self.tableView.tableHeaderView.height, inset.left, inset.bottom - self.tableView.tableFooterView.height , inset.right);
    
    
    // Do any additional setup after loading the view.
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

/*
#pragma mark - Navigation

// In a storyboard-based application, you will often want to do a little preparation before navigation
- (void)prepareForSegue:(UIStoryboardSegue *)segue sender:(id)sender {
    // Get the new view controller using [segue destinationViewController].
    // Pass the selected object to the new view controller.
}
*/
-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 90;
}


- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return 10;
}


- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    NSInteger tag = indexPath.row % 3 + 1;
    NSString *idStr = [NSString stringWithFormat:@"Cell%zd",tag];
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:idStr];
    return cell;
}

@end
