//
//  InPurchase.h
//  ttgame-ipad
//
//  Created by  on 12-3-19.
//  Copyright (c) 2012年 Maxx. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>
#import <UIKit/UIKit.h>
#import "ECPurchase.h"

@interface InPurchaseIOS : NSObject<ECPurchaseTransactionDelegate,ECPurchaseProductDelegate> {
    int             _requestTime;
    NSString*       _productId;
    
    NSTimer *scheduleTimer;
    UIAlertView*    paymentView;
}

@property (nonatomic,retain) NSArray         *productList;
@property (nonatomic,retain) NSString        *productId;

+(InPurchaseIOS*) getInstance;

-(void)makePayment:(NSString*) productId;
-(void)resumePurchase;

-(void)productInfo:(NSArray*)products;
-(void)commitPurchase:(bool) result purchaseInfo:(NSDictionary*) purchaseInfo;
+(void)purgeInPurchase;

@end
