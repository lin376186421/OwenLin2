//
//  SearchUAViewController.m
//  PodTest
//
//  Created by 林 on 16/8/12.
//  Copyright © 2016年 林. All rights reserved.
//

#import "SearchUAViewController.h"
#import "SearchDetailViewController.h"

@implementation DataObj

static NSString *name[] = {@"lrp",@"wzc",@"ll",@"lwd"};
static NSString *num[] = {@"18011111111",@"18022222222",@"18033333333",@"18044444444"};
static NSString *address[] = {@"江西广丰见底",@"江西婺源乡村",@"江西广丰少阳",@"江西广丰永丰"};
static NSInteger age[] = {26,26,25,25};

+(instancetype)creatSelfWithIndex:(NSInteger)index
{
    DataObj *obj = [DataObj new];
    obj.name = name[index];
    obj.num = num[index];
    obj.address = address[index];
    obj.age = age[index];
    return obj;
}

-(NSMutableDictionary *)selfToDic
{
    NSMutableDictionary*dic = [NSMutableDictionary dictionaryWithDictionary:[self yy_modelToJSONObject]];
    return dic;
}

@end

@interface SearchUAViewController ()<UITableViewDelegate,UITableViewDataSource>
{
    
    IBOutlet UITableView *tableview;
    NSMutableArray *data;
}

@end

@implementation SearchUAViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    data = [NSMutableArray array];
    for (int i = 0; i < 4; i ++) {
        [data addObject:[DataObj creatSelfWithIndex:i]];
    }
    
    [tableview reloadData];
    
    // Do any additional setup after loading the view.
}


-(CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath
{
    return 60;
}

-(NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section
{
    return data.count;
}

-(UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath
{
    UITableViewCell *cell = [tableview dequeueReusableCellWithIdentifier:@"CELL"];
    UILabel *nameLb = [cell.contentView viewWithTag:111];
    UILabel *desLb = [cell.contentView viewWithTag:222];
    DataObj *obj = [data objectAtIndex:indexPath.row];
    nameLb.text = obj.name;
    desLb.text = obj.address;
    return cell;
}

-(void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath
{
    DataObj *obj = [data objectAtIndex:indexPath.row];
    UIStoryboard *story = [UIStoryboard storyboardWithName:@"Main" bundle:nil];
    SearchDetailViewController *vc = [story instantiateViewControllerWithIdentifier:@"SearchDetailViewController"];
    vc.obj = obj;
    [self presentOtherViewController:vc animate:YES completion:^{
        
    }];
    
}

@end
