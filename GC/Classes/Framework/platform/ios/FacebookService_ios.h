//
//  FacebookService.h
//  ttgame-ipad
//
//  Created by owenlin on 13-10-23.
//  Copyright (c) 2013年 Maxx. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <FacebookSDK/FacebookSDK.h>

@interface FacebookService : NSObject<CLLocationManagerDelegate,FBFriendPickerDelegate>
{
    FBSession            *_fbsession;
    
    NSMutableDictionary *_userPermissions;
    int                 _currentAPICall;
    CLLocationManager   *_locationManager;
    CLLocation          *_mostRecentLocation;
    NSMutableArray      *_resultData;
    
    id                      _delegate;
    SEL                     _didLoginSucceedSelector;
    SEL                     _didLoginFailSelector;
    
    SEL                     _didRequestSucceedSelector;
    SEL                     _didRequestFailSelector;
    
    NSString                *_todoString;
    Class                   _originalClass;
    
    int                      _m_faildCount;
}
@property (nonatomic, strong) FBSession              *fbsession;
@property (nonatomic, retain) NSMutableDictionary   *userPermissions;
@property (nonatomic,assign)  int                   currentAPICall;
@property (nonatomic,assign)  int                   m_faildCount;
@property (nonatomic, retain) CLLocationManager     *locationManager;
@property (nonatomic, retain) CLLocation            *mostRecentLocation;

@property (nonatomic,assign) id                      delegate;
@property (nonatomic,assign) SEL                     didLoginSucceedSelector;
@property (nonatomic,assign) SEL                     didLoginFailSelector;

@property (nonatomic,assign) SEL                     didRequestSucceedSelector;
@property (nonatomic,assign) SEL                     didRequestFailSelector;

@property (nonatomic,retain) FBFrictionlessRecipientCache *m_friendCache;

@property (nonatomic,retain) NSString                *todoString;
@property (retain, nonatomic) FBFriendPickerViewController *friendPickerController;

-(BOOL)isLogin;
-(void)login:(BOOL)allowLoginUI;
-(void)logOut;
-(void)apiGetInvitabelFriends;
-(void)apiGraphMe;
-(void)apiGraphFriends;
-(void)getPictureWithID:(NSString *)facebookId;
-(void)getPictureWithIDNow:(NSString *)facebookId;
-(void)inviteFriendsPlayGame;
-(void)startInvite:(NSArray *)suggestedFriends;
-(void)postMessage:(NSString *)message;
-(void)postMessageWithPhoto:(NSString *)message;
-(void)close;
-(void)apiClear;
-(BOOL)apiNotClear;
@end
