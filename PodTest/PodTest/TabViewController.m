//
//  TabViewController.m
//  TestProgress
//
//  Created by 林 on 16/6/21.
//  Copyright © 2016年 林. All rights reserved.
//

#import "TabViewController.h"

@interface TabViewController ()

@end

@implementation TabViewController



- (void)viewDidLoad {
    [super viewDidLoad];
    self.scrollView.horizontalDelegate = self;
    self.scrollView.horizontalDataSource = self;
    [self.scrollView setCurrentIndex:0];
    // Do any additional setup after loading the view.
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (NSInteger)numberOfItems
{
    return _controllerList.count;
}


- (DTHorizontalScrollItemView *)horizontalScrollView:(DTHorizontalScrollView *)scroller itemViewForIndex:(NSInteger)index
{
    DTHorizontalScrollItemView *view = [scroller dequeueReusableItemView];
    if (view == nil) {
        view = [[DTHorizontalScrollItemView alloc] initWithFrame:scroller.bounds];
        view.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
        UIView *targetView = ((UIViewController *)_controllerList[index]).view;
        targetView.autoresizingMask = UIViewAutoresizingFlexibleHeight | UIViewAutoresizingFlexibleWidth;
        [view addSubview:targetView];
    }
    return view;
}

- (void)horizontalScrollView:(DTHorizontalScrollView *)scroller didSelectIndex:(NSInteger)index
{
    NSLog(@"1");
    UIViewController *vc = [self.controllerList objectAtIndex:index];
    [vc viewWillAppear:NO];
}

- (void)horizontalScrollViewDidFinishedAnimation:(DTHorizontalScrollView *)scroller
{
    NSLog(@"2");
}

- (void)horizontalScrollViewDidScrollToTheLastItem:(DTHorizontalScrollView *)scroller
{
    NSLog(@"3");
}

- (void)horizontalScrollViewDidScroll:(DTHorizontalScrollView *)scroller
{
    NSLog(@"4");
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
