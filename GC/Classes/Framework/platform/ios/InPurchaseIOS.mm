//
//  InPurchase.m
//  ttgame-ipad
//
//  Created by  on 12-3-19.
//  Copyright (c) 2012年 Maxx. All rights reserved.
//

#import "InPurchaseIOS.h"
#import "NGNKit.h"
#import "InPurchase.h"
#import "SBJSON.h"
#import "GTMBase64.h"
//#include "CUINodeCache.h"
#import <UIKit/UIKit.h>

@implementation InPurchaseIOS
@synthesize productId = _productId;

static InPurchaseIOS* _inPurchase;
+(InPurchaseIOS*) getInstance
{
    if (_inPurchase == nil) {
        _inPurchase = [[InPurchaseIOS alloc] init];
    }
    
    return _inPurchase;
}

#pragma mark  - * Init
-(id)init{
    if (self = [super init]) {
        
        [[ECPurchase shared] addTransactionObserver];
        
        [[ECPurchase shared] setProductDelegate:self];
        [[ECPurchase shared] setTransactionDelegate:self];
        [[ECPurchase shared] setVerifyRecepitMode:ECVerifyRecepitModeServer];
        
        scheduleTimer = nil;
        paymentView = nil;
        
        //        NSMutableArray *products = [NSMutableArray array];
        //        //add products
        //        [[ECPurchase shared] requestProductData:products];
    }
    
    return self;
}

int preTransactionState = -1;
-(void) paymentRequest
{
    SKPaymentTransaction *transaction = nil;
    int count = [SKPaymentQueue defaultQueue].transactions.count;
    if (count > 0) {
        transaction = [[SKPaymentQueue defaultQueue].transactions objectAtIndex:count - 1];
        if (transaction.transactionState != preTransactionState) {
            _requestTime = 0;
            preTransactionState = transaction.transactionState;
        }
    }

    _requestTime ++;
    if (_requestTime >= REQUEST_TIMEOUT) {
        
        if (transaction && transaction.transactionState != SKPaymentTransactionStatePurchasing) {
            [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
        }
        if ([ECPurchase shared].networkQueue){
            [[ECPurchase shared].networkQueue cancelAllOperations];
        }
        
#if PURCHASE_VIEW
//        CUINodeCache::sharedUINodeCache()->addAlertViewFromFile(ALERT_TYPE_MESSAGE1, NULL, [NSLocalizedString(@"APPSTORE_CONNECT_FAILED",nil) UTF8String], InPurchase::sharedInPurchase()->pDelegate, NULL, ALERT_MESSAGE_NAME);
#else
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Payment Error" message:@"Cannot connect to iTunes Store." delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alertView show];
        [alertView release];
#endif
        [self finishedPayment];
    }
}

-(void) finishedPayment
{
    _requestTime = 0;
    preTransactionState = -1;
    if (scheduleTimer) {
        [scheduleTimer invalidate];
        scheduleTimer = nil;
    }

    if (paymentView) {
        [paymentView dismissWithClickedButtonIndex:0 animated:false];
        [paymentView release];
        paymentView = nil;
    }

}

#pragma mark - *functions
-(void)productInfo:(NSArray*)products{
    [[ECPurchase shared] requestProductData:products];
}

-(void)resumePurchase{
    
//    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
    [self commitPurchase:false purchaseInfo:NULL];
}

-(void)startPayment{
    if (![SKPaymentQueue canMakePayments]){
        

#if PURCHASE_VIEW
//        CUINodeCache::sharedUINodeCache()->addAlertViewFromFile(ALERT_TYPE_MESSAGE1,NULL, [NSLocalizedString(@"NETWORK_CONNECT_FAILED",nil) UTF8String], InPurchase::sharedInPurchase()->pDelegate, NULL, ALERT_MESSAGE_NAME);

#else
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Payment Error" message:@"Cannot Make Payment!" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alertView show];
        [alertView release];
#endif
    }else {
#if PURCHASE_VIEW
    //        CUINodeCache::sharedUINodeCache()->addAlertViewFromFile(ALERT_TYPE_CONNECT_SERVER, NULL, [NSLocalizedString(@"PURCHASE_VERIFYING_PRODUCTS",nil) UTF8String], InPurchase::sharedInPurchase()->pDelegate, NULL, ALERT_CONNECT_NAME);
#else  
//        paymentView = [[UIAlertView alloc] initWithTitle:@"" message:@"Verifying Products..." delegate:nil cancelButtonTitle:nil otherButtonTitles:nil];
//        [paymentView show];
#endif
        [[ECPurchase shared] addPayment:self.productId];
        
        _requestTime = 0;
        
        scheduleTimer = [NSTimer scheduledTimerWithTimeInterval:1.0f target:self selector:@selector(paymentRequest) userInfo:nil repeats:YES];
    }
}

-(void) makePayment:(NSString *)productId
{
    self.productId = productId;
    
//    CUINodeCache::sharedUINodeCache()->addAlertViewFromFile(ALERT_TYPE_CONNECT_SERVER, NULL, [NSLocalizedString(@"NETWORK_CONNECT_WAITING",nil) UTF8String], InPurchase::sharedInPurchase()->pDelegate, NULL, ALERT_CONNECT_NAME);
    
    if (!NGNKit::sharedKit()->connectedToNetwork()) {
        NSLog(@"Cannot connect to App Store,Please check your network and try again!");
        //        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Network_Error_Title", nil)
        //                                                            message:NSLocalizedString(@"Network_Error_Message", nil)
        //                                                           delegate:nil cancelButtonTitle:NSLocalizedString(@"Button_OK", nil) otherButtonTitles:nil];
        //        [alertView show];
        //        [alertView release];
        
//        CUINodeCache::sharedUINodeCache()->addAlertViewFromFile(ALERT_TYPE_MESSAGE1, NULL, [NSLocalizedString(@"NETWORK_CONNECT_FAILED",nil) UTF8String], InPurchase::sharedInPurchase()->pDelegate, NULL, ALERT_MESSAGE_NAME);
        
    }
    else{
        [self startPayment];
    }
}

-(void) commitPurchase:(bool)result purchaseInfo:(NSDictionary *)purchaseInfo
{
    string info = "";
    if (purchaseInfo) {
        SBJsonWriter *writer = [[SBJsonWriter alloc] autorelease];
        NSString* infoData = [writer stringWithObject:purchaseInfo];
        info = [infoData UTF8String];
    }
    
    InPurchase::sharedInPurchase()->completedPurchase(result, info);
}

#pragma ECPurchase
-(void) didRestoreTransaction:(SKPaymentTransaction *)transaction
{
    NSLog(@"transaction.payment.transactionIdentifier=%@", transaction.payment.productIdentifier);
    
    self.productId = transaction.payment.productIdentifier;;
    
    NSString *recepit = [GTMBase64 stringByEncodingData:transaction.transactionReceipt];
	NSDictionary* purchaseInfo = [NSDictionary dictionaryWithObjectsAndKeys:recepit, @"receipt", nil];
    
    [self commitPurchase:true purchaseInfo:purchaseInfo];

//    [HttpService logSale:self.productId];
}

-(void) didCompleteTransaction:(SKPaymentTransaction *)transaction
{
    NSString *recepit = [GTMBase64 stringByEncodingData:transaction.transactionReceipt];
	NSDictionary* purchaseInfo = [NSDictionary dictionaryWithObjectsAndKeys:recepit, @"receipt", nil];
    
    [self commitPurchase:true purchaseInfo:purchaseInfo];
    
//    CUINodeCache::sharedUINodeCache()->addAlertViewFromFile(ALERT_TYPE_MESSAGE1, NULL, [NSLocalizedString(@"PURCHASE_SUCCESSED",nil) UTF8String], InPurchase::sharedInPurchase()->pDelegate, NULL, ALERT_MESSAGE_NAME);
    
//    [HttpService logSale:self.productId];
    
//    [[NGNKit sharedKit] playEffect:SOUND_PAYMENT_SUCCEED];
    
    [self finishedPayment];
}

-(void) didFailedTransaction:(SKPaymentTransaction *)transaction
{
    NSLog(@"transaction.transactionIdentifier=%@", transaction.transactionIdentifier);
    NSLog(@"transaction.payment.productIdentifier=%@", transaction.payment.productIdentifier);
    
    [self commitPurchase:false purchaseInfo:NULL];
    
    if (transaction.error.code == SKErrorPaymentCancelled) {
#if PURCHASE_VIEW
//        CUINodeCache::sharedUINodeCache()->addAlertViewFromFile(ALERT_TYPE_MESSAGE1, NULL, [NSLocalizedString(@"PURCHASE_CANCELED",nil) UTF8String], InPurchase::sharedInPurchase()->pDelegate, NULL, ALERT_MESSAGE_NAME);
#else
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Purchase Cancelled" message:@"" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alertView show];
    [alertView release];
#endif
    }else{
#if PURCHASE_VIEW
//        CUINodeCache::sharedUINodeCache()->addAlertViewFromFile(ALERT_TYPE_MESSAGE1, NULL, [transaction.error.localizedDescription UTF8String], InPurchase::sharedInPurchase()->pDelegate, NULL, ALERT_MESSAGE_NAME);
#else
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:transaction.error.localizedDescription message:@"" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alertView show];
    [alertView release];
#endif
    }
    
    [self finishedPayment];
}

-(void) didCompleteTransactionAndVerifySucceed:(NSString *)productIdentifier purchaseInfo:(NSDictionary *)purchaseInfo
{        
    [self commitPurchase:true purchaseInfo:purchaseInfo];
#if PURCHASE_VIEW    
//    CUINodeCache::sharedUINodeCache()->addAlertViewFromFile(ALERT_TYPE_MESSAGE1, NULL, [NSLocalizedString(@"PURCHASE_SUCCESSED",nil) UTF8String],InPurchase::sharedInPurchase()->pDelegate, NULL,ALERT_MESSAGE_NAME);
#else
//    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Purchase Succeeded!" message:@"" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
//    [alertView show];
//    [alertView release];
#endif
//    [HttpService logSale:self.productId];
    
//    [[NGNKit sharedKit] playEffect:SOUND_PAYMENT_SUCCEED];
    
    [self finishedPayment];
    
}

-(void) didCompleteTransactionAndVerifyFailed:(NSString *)productIdentifier withError:(NSString *)error
{
//    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Payment Error"
//                                                        message:error
//                                                       delegate:nil cancelButtonTitle:NSLocalizedString(@"Button_OK", nil) otherButtonTitles:nil];
//    [alertView show];
//    [alertView release];
//    [[CUINodeCache sharedUINodeCache] removeAlertNode:ALERT_CONNECT_NAME];

    if (error) {
#if PURCHASE_VIEW          
//        CUINodeCache::sharedUINodeCache()->addAlertViewFromFile(ALERT_TYPE_MESSAGE1, NULL, [error UTF8String],InPurchase::sharedInPurchase()->pDelegate, NULL, ALERT_MESSAGE_NAME);
#else
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Payment Error" message:error delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    [alertView show];
    [alertView release];
#endif
        
    }else{
 #if PURCHASE_VIEW  
    //        CUINodeCache::sharedUINodeCache()->addAlertViewFromFile(ALERT_TYPE_MESSAGE1, NULL, [NSLocalizedString(@"PURCHASE_FAILED",nil) UTF8String],InPurchase::sharedInPurchase()->pDelegate, NULL, ALERT_MESSAGE_NAME);
#else
        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Payment Error" message:@"Payment Failed" delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
        [alertView show];
        [alertView release];
#endif
    
    }
    
    [self commitPurchase:false purchaseInfo:NULL];
    
    [self finishedPayment];
}

-(void) didReceivedProducts:(NSArray *)products
{
    NSLog(@"%@", @"Received Products.");
}

+(void) purgeInPurchase
{
    [_inPurchase release];
    _inPurchase = nil;
}

-(void)dealloc{
    self.productId = nil;
    [[ECPurchase shared] removeTransactionObserver];
    
    [super dealloc];
}

@end
