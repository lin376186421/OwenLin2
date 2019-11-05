//
//  DTHorizontalScrollView.h
//  YHHB
//
//  Created by Hunter Huang on 11/30/11.
//  Copyright (c) 2011 vge design. All rights reserved.
//

#import <UIKit/UIKit.h>

@protocol DTHorizontalScrollViewDataSource;
@protocol DTHorizontalScrollViewDelegate;
@class DTHorizontalScrollItemView;

@interface DTHorizontalScrollView : UIScrollView 

@property (nonatomic, weak) id<DTHorizontalScrollViewDataSource> horizontalDataSource;
@property (nonatomic, weak) id<DTHorizontalScrollViewDelegate> horizontalDelegate;
@property (nonatomic, assign) NSInteger currentIndex;
@property (nonatomic, assign) CGFloat viewPadding;
@property (nonatomic) BOOL respondent;

- (DTHorizontalScrollItemView *)dequeueReusableItemView;

- (void)clearMemory;
- (void)moveToNext;
- (void)reloadData;

- (void)resizeToFrame:(CGRect)rect animated:(BOOL)animated;

- (void)setCurrentIndex:(NSInteger)index animated:(BOOL)animated;
- (void)setCurrentIndex:(NSInteger)index animated:(BOOL)animated event:(BOOL)event;

- (DTHorizontalScrollItemView *)itemViewAtIndex:(NSUInteger)index;

@end

@protocol DTHorizontalScrollViewDataSource <NSObject>

- (NSInteger)numberOfItems;
- (DTHorizontalScrollItemView *)horizontalScrollView:(DTHorizontalScrollView *)scroller itemViewForIndex:(NSInteger)index;

@end

@protocol DTHorizontalScrollViewDelegate <NSObject>

@optional
- (void)horizontalScrollView:(DTHorizontalScrollView *)scroller didSelectIndex:(NSInteger)index;
- (void)horizontalScrollViewDidFinishedAnimation:(DTHorizontalScrollView *)scroller;
- (void)horizontalScrollViewDidScrollToTheLastItem:(DTHorizontalScrollView *)scroller;
- (void)horizontalScrollViewDidScroll:(DTHorizontalScrollView *)scroller;

@end

@interface DTHorizontalScrollItemView : UIView

@property (nonatomic) NSInteger index;
@property (nonatomic) BOOL showing;

- (void)viewDidShow;
- (void)viewDidHidden;

- (void)prepareForReuse;

@end
