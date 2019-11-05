//
//  FacebookService.m
//  ttgame-ipad
//
//  Created by owenlin on 13-10-23.
//  Copyright (c) 2013年 Maxx. All rights reserved.
//

#import "FacebookService_ios.h"
#import "AppController.h"
#import "CAppPlatform.h"
#import "RootViewController.h"
#import "AppConfig.h"
#import<Social/Social.h>
#import "CFacebookService.h"
//#include "SBJson.h"
//#import "NGNKit.h"
//#import "CImageTransform.h"
//Class object_getClass(id object);

#define MAX_FAILD_COUNT 0

@implementation FacebookService
@synthesize fbsession            = _fbsession;
@synthesize userPermissions     = _userPermissions;
@synthesize currentAPICall      =_currentAPICall;
@synthesize locationManager     = _locationManager;
@synthesize mostRecentLocation  = _mostRecentLocation;

@synthesize delegate                    = _delegate;
@synthesize didLoginSucceedSelector     = _didLoginSucceedSelector;
@synthesize didLoginFailSelector        = _didLoginFailSelector;
@synthesize didRequestSucceedSelector   = _didRequestSucceedSelector;
@synthesize didRequestFailSelector      = _didRequestFailSelector;
@synthesize todoString                  = _todoString;

@synthesize m_faildCount                =_m_faildCount;


/////////////////////////////////////////////////////////////////////////////
#pragma mark - * 内部
-(void)setDelegate:(id)delegate{
//    _delegate = delegate;
//    _originalClass = object_getClass(delegate);
}

-(bool)isDelegate{
//    Class currentClass = object_getClass(self.delegate);
//    if (currentClass == _originalClass) {
//        return YES;
//    }
    return NO;
}
#pragma mark - *init
-(id)init
{
    if (self = [super init]) {
        
        _m_faildCount = 0;
        [self apiClear];
        self.m_friendCache = NULL;
        if(!_fbsession.isOpen)
        {
            _fbsession=[[FBSession alloc] init];
            [FBSession setActiveSession: _fbsession];
        }
        
        if(_fbsession.state==FBSessionStateCreatedTokenLoaded)
        {
            CFacebookService::sharedFaceBook()->setFBBindActivity(true);
            [self login:NO];
        }
    }
    return self;
}

-(BOOL)apiNotClear
{
    if(_currentAPICall != kAPIClear)
    {
        _m_faildCount += 1;
        if(_m_faildCount >= MAX_FAILD_COUNT)
        {
            _m_faildCount = 0;
            _currentAPICall = kAPIClear;
            return false;
        }
        
        return true;
    }
    else
    {
        return false;
    }
}

-(void)apiClear
{
    _currentAPICall = kAPIClear;
}

-(void)dealloc{
    
    [_todoString release];
    [_fbsession release];
    [_userPermissions release];
    
    [_locationManager stopUpdatingLocation];
    _locationManager.delegate = nil;
    [_locationManager release];
    
    [_mostRecentLocation release];
    
    [_friendPickerController release];
    
    [super dealloc];
}
#pragma mark - function

-(BOOL)isLogin
{
    return [[FBSession activeSession] isOpen];
}

-(void)login:(BOOL)allowLoginUI
{
    if([self apiNotClear])
        return;
    if (allowLoginUI) {
        NSArray *permissions = [[NSArray alloc] initWithObjects:
                                @"email",
                                @"publish_actions",
                                @"public_profile",
                                @"user_friends",
                                @"publish_stream",
                                nil];
        [FBSession openActiveSessionWithPublishPermissions:permissions defaultAudience:FBSessionDefaultAudienceEveryone allowLoginUI:allowLoginUI completionHandler:^(FBSession *session,
                                                                                                                                                                      FBSessionState status,
                                                                                                                                                                      NSError *error) {
            if (status == FBSessionStateClosedLoginFailed || status == FBSessionStateCreatedOpening)
            {
                CFacebookService::sharedFaceBook()->setFBBindActivity(false);
                [self fbDidLoginFaild:error];
            }
            else
            {
                [self fbLoginSucceed];
            }
        }];
    }
    else {
        [[FBSession activeSession] openWithCompletionHandler:^(FBSession *session,
                                                               FBSessionState status,
                                                               NSError *error) {
            // we recurse here, in order to update buttons and labels
            if (status == FBSessionStateClosedLoginFailed || status == FBSessionStateCreatedOpening)
            {
                CFacebookService::sharedFaceBook()->setFBBindActivity(false);
                [self fbDidLoginFaild:error];
            }
            else
            {
                [self fbLoginSucceed];
            }
        }];
    }
}

-(void)logOut
{
    if ([[FBSession activeSession] isOpen]) {
        FacebookMgr::getSingleton().fbLoginOut();
        [[FBSession activeSession] closeAndClearTokenInformation];
        [FBSession setActiveSession:nil];
    }
}

-(void)close
{
    [[FBSession activeSession] close];
}

//获取当前登录用户的信息
-(void)apiGraphMe
{
    if([self apiNotClear])
        return;
    _currentAPICall=kAPIGraphMe;
    [[FBRequest requestForMe]
     startWithCompletionHandler:
     ^(FBRequestConnection *connection, NSDictionary<FBGraphUser> *result, NSError *error)
     {
         // Did everything come back okay with no errors?
         if (!error && result)
         {
             NSLog(@"user id=%@",result.objectID);
             NSLog(@"user name=%@",result.name);
             NSLog(@"user name=%@",[result objectForKey:@"gender"]);
             NSMutableDictionary *data = [NSMutableDictionary dictionary];
             [data setObject:result.objectID forKey:@"id"];
             [data setObject:result.name forKey:@"name"];
             [data setObject:[result objectForKey:@"gender"] forKey:@"gender"];
             CFacebookService::sharedFaceBook()->fbSetUser([[data objectForKey:@"id"] UTF8String], [[data objectForKey:@"name"] UTF8String]);
             [self fbRequestSucceed:data];
         }
         else
         {
             [self fbRequestFaild:error];
             CFacebookService::sharedFaceBook()->setFBBindActivity(false);
         }
}];
}
-(void)apiGetInvitabelFriends
{
    if([self apiNotClear])
        return;
    _currentAPICall=kAPIGraphFriends;

    [FBRequestConnection startWithGraphPath:@"/me/invitable_friends" completionHandler:^(FBRequestConnection *connection, id result, NSError *error)
     {
         if (!error && result)
         {
             NSLog(@"result=%@",result);
             NSArray *dataArray = [result objectForKey:@"data"];
             vector<coc::FBFriendInfo*> fList;
             for(int i = 0; i < dataArray.count; i++)
             {
                 FBGraphObject *fbObject = (FBGraphObject *)dataArray[i];
                 string friendId = [(NSString *)[fbObject objectForKey:@"id"] UTF8String];
                 string friendName = [(NSString *)[fbObject objectForKey:@"name"] UTF8String];
                 coc::FBFriendInfo *finfo = new coc::FBFriendInfo();
                 finfo->fid = friendId;
                 finfo->fname = friendName;
                 finfo->fSpriteFrame = NULL;
                 finfo->isSelect = false;
                 fList.push_back(finfo);
             }
             CFacebookService::sharedFaceBook()->fbSetFriends(fList);
             [self fbRequestSucceed:result];
         }
         else{
             [self fbRequestFaild:error];
         }
     }];
}
// 获取好友信息
-(void)apiGraphFriends
{
    if([self apiNotClear])
        return;
    _currentAPICall=kAPIGraphFriends;
    if ([FBSession.activeSession.permissions indexOfObject:@"user_friends"] == NSNotFound) {
        // if we don't already have the permission, then we request it now
        [FBSession.activeSession requestNewPublishPermissions:@[@"user_friends"]
                                              defaultAudience:FBSessionDefaultAudienceEveryone
                                            completionHandler:^(FBSession *session, NSError *error) {
                                                if (!error) {
                                                    
                                                } else if (error.fberrorCategory != FBErrorCategoryUserCancelled) {
                                                    NSLog(@"%@",error.description);
                                                }
                                            }];
        return;
    }

    NSDictionary *parameter = [NSDictionary dictionaryWithObject:@"id,name,picture" forKey:@"fields"];
    [FBRequestConnection startWithGraphPath:@"/me/invitable_friends"
                                    parameters:parameter
                                    HTTPMethod:@"GET"
                          completionHandler:^(FBRequestConnection *connection, id result, NSError *error)
     {
         if (!error && result)
         {
             NSLog(@"result=%@",result);
             NSArray *dataArray = [result objectForKey:@"data"];
             vector<coc::FBFriendInfo*> fList;
             for(int i = 0; i < dataArray.count; i++)
             {
                 FBGraphObject *fbObject = (FBGraphObject *)dataArray[i];
                 string friendId = [(NSString *)[fbObject objectForKey:@"id"] UTF8String];
                 string friendName = [(NSString *)[fbObject objectForKey:@"name"] UTF8String];
                 NSString *photoStr = [[[fbObject objectForKey:@"picture"] objectForKey:@"data"] objectForKey:@"url"];
                 string fphoto = [photoStr UTF8String];
                 coc::FBFriendInfo *finfo = new coc::FBFriendInfo();
                 finfo->fid = friendId;
                 finfo->fname = friendName;
                 finfo->fphoto = fphoto;
                 finfo->fSpriteFrame = NULL;
                 finfo->isSelect = false;
                 fList.push_back(finfo);
             }
             CFacebookService::sharedFaceBook()->fbSetFriends(fList);
             [self fbRequestSucceed:result];
         }
         else{
             [self fbRequestFaild:error];
         }
     }];

}


//弹出好友界面 勾选好友
-(void)inviteFriendsPlayGame
{
    if([self apiNotClear])
        return;
     _currentAPICall=kAPIInviteFriends;
    if (self.friendPickerController == nil) {
        // Create friend picker, and get data loaded into it.
        self.friendPickerController = [[FBFriendPickerViewController alloc] init];
        self.friendPickerController.title = @"Pick Friends";
        self.friendPickerController.delegate = self;
    }
    [self.friendPickerController loadData];
    [self.friendPickerController clearSelection];
    [(UIViewController *)[AppController sharedAppController].viewController presentViewController:self.friendPickerController animated:YES completion:nil];
}

#pragma mark - FBFriendPickerDelegate

//弹出好友界面 可勾选
- (void)facebookViewControllerDoneWasPressed:(id)sender {
    
    NSMutableArray *idArray=[[NSMutableArray alloc] init];
    for (id<FBGraphUser> user in self.friendPickerController.selection) {
        [idArray addObject:user.objectID];
    }
    [(UIViewController *)[AppController sharedAppController].viewController dismissModalViewControllerAnimated:YES];
    [self startInvite:idArray];
    [idArray release];
}

- (void)facebookViewControllerCancelWasPressed:(id)sender {
    [(UIViewController *)[AppController sharedAppController].viewController dismissModalViewControllerAnimated:YES];
    [self apiClear];
    NSLog(@"user cancel");
}


//向好友群发消息 邀请好友玩游戏 idArray存放要邀请好友们的id
-(void)startInvite:(NSArray *)suggestedFriends
{
    if(suggestedFriends.count>0)
    {
        _currentAPICall = kAPIInviteFriends;
        NSLog(@"%@",suggestedFriends);
        NSMutableDictionary* params =   [NSMutableDictionary dictionaryWithObjectsAndKeys:
                                         [suggestedFriends componentsJoinedByString:@","], @"to", // Ali

                                         nil];
        
        if (self.m_friendCache == NULL) {
            self.m_friendCache = [[FBFrictionlessRecipientCache alloc] init];
        }
        
        [self.m_friendCache prefetchAndCacheForSession:nil];

        [FBWebDialogs presentRequestsDialogModallyWithSession:nil
                                                      message:[NSString stringWithUTF8String:FBPostLinkCaption]
                                                        title:nil
                                                   parameters:params
                                                      handler:^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
                                                          if (error) {
                                                              NSLog(@"Error sending request.");
                                                              [self fbRequestFaild:error];
                                                          } else {
                                                              if (result == FBWebDialogResultDialogNotCompleted) {
                                                                  [self fbRequestFaild:error];
                                                                  NSLog(@"User canceled request.");
                                                              } else {
                                                                  NSString *str = [resultURL absoluteString];
                                                                  NSRange range = [str rangeOfString:@"error_code"];
                                                                  if (range.location != NSNotFound) {
                                                                      [self fbRequestFaild:error];
                                                                      NSLog(@"User canceled request.");
                                                                  }else {
                                                                      NSLog(@"Request Sent.");
                                                                      [self fbRequestSucceed:(id)result];
                                                                  }
                                                              }
                                                          }}
                                                  friendCache:self.m_friendCache];
    }
}

//更新空间状态
-(void)postMessageWithPhoto:(NSString *)message
{
    if([self apiNotClear])
        return;
   
    _currentAPICall=kAPIPostMessageWithPhoto;
    
    BOOL canPostPhoto = [FBDialogs canPresentShareDialogWithPhotos];
    
    CAppPlatform::sharedAppPlatform()->getScreenShot();
    UIImage* image = [[[UIImage alloc]initWithContentsOfFile:[NSString stringWithFormat:@"%@/Documents/%@",NSHomeDirectory(),[NSString stringWithUTF8String:ScreenShotFile]]] autorelease];
    
    if(image == nil)
    {
        NSString *linkURL = [NSString stringWithUTF8String:APP_STORE_URL];
        FBLinkShareParams *shareParams = [[FBLinkShareParams alloc] init];
        shareParams.link = [NSURL URLWithString:linkURL];
        shareParams.name = [NSString stringWithUTF8String:CAppPlatform::sharedAppPlatform()->getAppName().c_str()];
        shareParams.caption= [NSString stringWithUTF8String:FBPostLinkCaption];
        shareParams.linkDescription = [NSString stringWithString:message];
        NSDictionary *params = @{
                                 @"name" : shareParams.name,
                                 @"caption" : shareParams.caption,
                                 @"description" : shareParams.linkDescription,
                                 @"link" : linkURL
                                 };
        
        [FBWebDialogs presentFeedDialogModallyWithSession:nil
                                               parameters:params
                                                  handler:
         ^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
             if (error) {
                 NSLog(@"Error publishing story.");
                 [self fbRequestFaild:error];
             } else {
                 if (result == FBWebDialogResultDialogNotCompleted) {
                     [self fbRequestFaild:error];
                     NSLog(@"User canceled story publishing.");
                 } else {
                     NSLog(@"Story published.");
                     [self fbRequestSucceed:(id)result];
                 }
             }}];

        return;
    }
    canPostPhoto = false;
    if (canPostPhoto){
        NSArray *array = [NSArray arrayWithObject:image];
        [FBDialogs presentShareDialogWithPhotos:array
                                    clientState:nil
                                        handler:^(FBAppCall *call, NSDictionary *results, NSError *error) {
                                            if(error) {
                                                NSLog(@"Error publishing story.");
                                                [self fbRequestFaild:error];
                                            } else if (results[@"completionGesture"] && [results[@"completionGesture"] isEqualToString:@"cancel"]) {
                                                [self fbRequestFaild:error];
                                                NSLog(@"User canceled story publishing.");
                                            } else {
                                                NSLog(@"Story published.");
                                                [self fbRequestSucceed:results];
                                            }
                                        }];
        
    }else {
        if ([FBSession.activeSession.permissions indexOfObject:@"publish_actions"] == NSNotFound) {
            // if we don't already have the permission, then we request it now
            [FBSession.activeSession requestNewPublishPermissions:@[@"publish_actions"]
                                                  defaultAudience:FBSessionDefaultAudienceEveryone
                                                completionHandler:^(FBSession *session, NSError *error) {
                                                    if (!error) {
                                                        if(!APP_DEBUG)
                                                            [self postMessageWithPhoto:message];
                                                    } else if (error.fberrorCategory != FBErrorCategoryUserCancelled) {
                                                        UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Permission denied"
                                                                                                            message:@"Unable to get permission to post"
                                                                                                           delegate:nil
                                                                                                  cancelButtonTitle:@"OK"
                                                                                                  otherButtonTitles:nil];
                                                        [alertView show];
                                                    }
                                                }];
            return;
        }
        NSMutableDictionary* photosParams = [NSMutableDictionary dictionaryWithObjectsAndKeys:
                                             image,@"source",
                                             message,@"message",
                                             nil];
        
        [FBRequestConnection startWithGraphPath:@"me/photos"
                                     parameters:photosParams
                                     HTTPMethod:@"POST"
                              completionHandler:^(FBRequestConnection *connection, id result, NSError *error)
         {
             if (!error && result)
             {
                 [self fbRequestSucceed:result];
             }
             else
             {
                 [self fbRequestFaild:error];
             }
         }];
        
    }
}

//更新空间状态
-(void)postMessage:(NSString *)message
{
    if([self apiNotClear])
        return;
    
    _currentAPICall=kAPIPostMessage;

    if ([FBSession.activeSession.permissions indexOfObject:@"publish_actions"] == NSNotFound) {
        // if we don't already have the permission, then we request it now
        [FBSession.activeSession requestNewPublishPermissions:@[@"publish_actions"]
                                              defaultAudience:FBSessionDefaultAudienceFriends
                                            completionHandler:^(FBSession *session, NSError *error) {
                                                if (!error) {
                                                    if(!APP_DEBUG)
                                                        [self postMessageWithPhoto:message];
                                                } else if (error.fberrorCategory != FBErrorCategoryUserCancelled) {
                                                    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Permission denied"
                                                                                                        message:@"Unable to get permission to post"
                                                                                                       delegate:nil
                                                                                              cancelButtonTitle:@"OK"
                                                                                              otherButtonTitles:nil];
                                                    [alertView show];
                                                }
                                            }];
        return;
    }
    
    NSString *linkURL = [NSString stringWithUTF8String:APP_STORE_URL];
    FBLinkShareParams *shareParams = [[FBLinkShareParams alloc] init];
    shareParams.link = [NSURL URLWithString:linkURL];
    shareParams.name = [NSString stringWithUTF8String:CAppPlatform::sharedAppPlatform()->getAppName().c_str()];
    shareParams.caption= [NSString stringWithUTF8String:FBPostLinkCaption];
    shareParams.linkDescription = [NSString stringWithFormat:@"%@",message];
    
    BOOL canPost = [FBDialogs canPresentShareDialogWithParams:shareParams];
    if (canPost)
    {
        [FBDialogs presentShareDialogWithParams:shareParams
                                    clientState:nil
                                        handler:^(FBAppCall *call, NSDictionary *results, NSError *error) {
                                            if(error) {
                                                NSLog(@"Error publishing story.");
                                                [self fbRequestFaild:error];
                                            } else if (results[@"completionGesture"] && [results[@"completionGesture"] isEqualToString:@"cancel"]) {
                                                [self fbRequestFaild:error];
                                                NSLog(@"User canceled story publishing.");
                                            } else {
                                                NSLog(@"Story published.");
                                                [self fbRequestSucceed:results];
                                            }
                                        }];
        
    }else {
        NSDictionary *params = @{
                                 @"name" : shareParams.name,
                                 @"caption" : shareParams.caption,
                                 @"description" : shareParams.linkDescription,
                                 @"link" : linkURL
                                 };
        
        // Invoke the dialog
        [FBWebDialogs presentFeedDialogModallyWithSession:nil
                                               parameters:params
                                                  handler:
         ^(FBWebDialogResult result, NSURL *resultURL, NSError *error) {
             if (error) {
                 [self fbRequestFaild:error];
                 NSLog(@"Error publishing story.");
             } else {
                 if (result == FBWebDialogResultDialogNotCompleted) {
                     [self fbRequestFaild:error];
                     NSLog(@"User canceled story publishing.");
                 } else {
                     [self fbRequestSucceed:(id)result];
                     NSLog(@"Story published.");
                 }
             }}];

    
    }
}

- (void) performPublishAction:(void (^)(void)) action {
    if ([FBSession.activeSession.permissions indexOfObject:@"publish_actions"] == NSNotFound) {
        [FBSession.activeSession requestNewPublishPermissions:@[@"publish_actions"]
                                              defaultAudience:FBSessionDefaultAudienceNone
                                            completionHandler:^(FBSession *session, NSError *error) {
                                                if (!error) {
                                                    action();
                                                } else if (error.fberrorCategory != FBErrorCategoryUserCancelled){
                                                    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"Permission denied"
                                                                                                        message:@"Unable to get permission to post"
                                                                                                       delegate:nil
                                                                                              cancelButtonTitle:@"OK"
                                                                                              otherButtonTitles:nil];
                                                    [alertView show];
                                                }
                                            }];
    } else {
        action();
    }
}

#pragma mark - *FBSessionDelegate

- (void)fbLoginSucceed {
    [self apiGraphMe];
}

- (void)fbDidLoginFaild:(NSError *)error {
    NSLog(@"fbDidLoginFaild%@",[error debugDescription]);
    [[FBSession activeSession] closeAndClearTokenInformation];
    [FBSession setActiveSession:nil];
    CFacebookService::sharedFaceBook()->fbRequestFinish(_currentAPICall, 0);
}

#pragma mark - *FBRequestDelegate

-(void)fbRequestSucceed:(id)result
{
    if(result)
        NSLog(@"fbRequestSucceed%@",result);
    CFacebookService::sharedFaceBook()->fbRequestFinish(_currentAPICall, 1);
    [self apiClear];
    
}
-(void)fbRequestFaild:(NSError *)error
{
    NSLog(@"fbRequestFaild%@",[error debugDescription]);
    CFacebookService::sharedFaceBook()->fbRequestFinish(_currentAPICall, 0);
    [self apiClear];
}

@end
