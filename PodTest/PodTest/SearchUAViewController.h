//
//  SearchUAViewController.h
//  PodTest
//
//  Created by 林 on 16/8/12.
//  Copyright © 2016年 林. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <YYModel.h>
@interface DataObj : NSObject<YYModel>

@property (strong,nonatomic) NSString *name;
@property (strong,nonatomic) NSString *num;
@property (strong,nonatomic) NSString *address;
@property (assign,nonatomic) NSInteger age;

+(instancetype)creatSelfWithIndex:(NSInteger)index;
-(NSMutableDictionary *)selfToDic;
@end

@interface SearchUAViewController : UIViewController

@end
