//
//  CollectViewController.h
//  PodTest
//
//  Created by 林 on 16/6/22.
//  Copyright © 2016年 林. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface CollectViewController : UIViewController<UICollectionViewDelegate, UICollectionViewDataSource,UICollectionViewDelegateFlowLayout>
@property (weak, nonatomic) IBOutlet UICollectionView *collectionView;

@end
