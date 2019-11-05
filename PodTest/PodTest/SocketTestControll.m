//
//  SocketTestControll.m
//  PodTest
//
//  Created by 林 on 2017/6/22.
//  Copyright © 2017年 林. All rights reserved.
//

#import <SocketRocket/SocketRocket.h>

#import "SocketTestControll.h"
#import "DESUtil_DT.h"

@interface SocketTestControll ()<SRWebSocketDelegate>
{
    SRWebSocket *_webSocket;
    IBOutlet UILabel *_resultLabel;
    NSMutableString *_log;
    NSUInteger _webSocketStaus;
    IBOutlet UIButton *_socketBtn;
    IBOutlet UITextView *_textField0;
    IBOutlet UITextView *_testView;
    IBOutlet UITextView *_textView2;
}
@end

@implementation SocketTestControll


+ (NSString *)doDESCipher:(NSString *)sTextIn type:(NSUInteger)type
{
    NSString *ret = [DESUtil_DT doCipher:sTextIn key:@"J1K2T3K4" context:(type == 1 ? kCCEncrypt : kCCDecrypt)];
    return ret;
}

- (void)viewDidLoad {
    [super viewDidLoad];
    _textField0.text = @"http://172.16.0.34:12345?token=111";
    _textView2.text = @"{      }";
    self.title = @"SocketTestControll";
    self.webStatus = DT_WEB_STATUS_CLOSE;
    // Do any additional setup after loading the view.
}

- (void)viewWillAppear:(BOOL)animated
{
    [super viewWillAppear:animated];
    
//    [self connetAction:nil];
}

- (void)viewDidAppear:(BOOL)animated
{
    [super viewDidAppear:animated];
    
}

- (void)viewDidDisappear:(BOOL)animated
{
    [super viewDidDisappear:animated];
    
    [self deallocWebSocket];
}

- (void)deallocWebSocket
{
//    if (_webSocket.readyState == SR_OPEN || _webSocket.readyState == SR_CONNECTING) {
//        [_webSocket close];
//    }//close 里面有判断 可忽略
    [_webSocket close];
    _webSocket.delegate = nil;
    _webSocket = nil;
}

- (void)setWebStatus:(DT_WEB_STATUS)webStatus
{
    _webStatus = webStatus;
    
    [_socketBtn setTitle:@"连接" forState:UIControlStateNormal];
    switch (_webStatus) {
        case DT_WEB_STATUS_CONNETED:
            [_socketBtn setTitle:@"断开" forState:UIControlStateNormal];
            [self addLog:@"连接成功"];
            break;
        case DT_WEB_STATUS_CONNETING:
            [_socketBtn setTitle:@"取消" forState:UIControlStateNormal];
            [self addLog:@"正在连接"];
            break;
        case DT_WEB_STATUS_FAILD:
            [self addLog:@"连接失败"];
            break;
        case DT_WEB_STATUS_CLOSE:
            [self addLog:@"连接关闭"];
            break;
        default:
            break;
    }
}

- (BOOL)isConnet
{
    return _webStatus == DT_WEB_STATUS_CONNETED;
}

- (void)addLog:(NSString *)log
{
    if (_log == nil) {
        _log = [NSMutableString string];
    }
    [_log appendString:@"\n"];
    [_log appendString:log];
    _resultLabel.text = _log;
}

- (IBAction)connetAction:(id)sender {
    
    switch (_webStatus) {
        case DT_WEB_STATUS_CONNETED:
            [_webSocket closeWithCode:SRStatusCodeNormal reason:@"主动断开连接"];
            break;
        case DT_WEB_STATUS_CONNETING:
            [_webSocket closeWithCode:SRStatusCodeNormal reason:@"主动取消"];
            break;
        case DT_WEB_STATUS_FAILD:
        case DT_WEB_STATUS_CLOSE:
        {
            [self deallocWebSocket];
            
            _webSocket = [[SRWebSocket alloc] initWithURL:[NSURL URLWithString:_textField0.text]];//wss://echo.websocket.org
            _webSocket.delegate = self;
            self.webStatus = DT_WEB_STATUS_CONNETING;
            [_webSocket open];
        }
            break;
        default:
            break;
    }
    
}

- (IBAction)sendAciton:(id)sender {
    
    [_testView resignFirstResponder];
    [_textView2 resignFirstResponder];
    [_textField0 resignFirstResponder];
    
    if (!self.isConnet) {
        [self addLog:@"WebSocket未连接成功"];
        return;
    }
    
    NSMutableDictionary *dic = [NSMutableDictionary dictionary];
    [dic setValue:(_testView.text.length ? _testView.text : @"") forKey:@"cmd"];
    if (_textView2.text.length) {
        NSData *jsonData = [_textView2.text dataUsingEncoding:NSUTF8StringEncoding];
        id obj = [NSJSONSerialization JSONObjectWithData:jsonData options:NSJSONReadingAllowFragments error:nil];
        if (obj && [obj isKindOfClass:[NSDictionary class]]) {
//            [dic setValue:obj forKey:@"data"];
            [dic addEntriesFromDictionary:obj];
        } else {
            [dic setValue:_textView2.text forKey:@"data"];
        }
    }
    
    NSData *data = [NSJSONSerialization dataWithJSONObject:dic options:NSJSONWritingPrettyPrinted error:nil];
    NSString *jsonStr = [[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding];
    jsonStr = [[self class] doDESCipher:jsonStr type:1];
    [_webSocket send:jsonStr];
    
//    if (arc4random() % 100 > 50) {
//         [_webSocket sendPing:nil];
//    } else {
//        [_webSocket send:@"sss"];
//    }
}

- (void)receiveMessage:(NSString *)message
{
    if ([message isKindOfClass:[NSString class]]) {
        message = [[self class] doDESCipher:message type:2];
        [self addLog:[NSString stringWithFormat:@"Received \"%@\"", message]];
    } else {
        [self addLog:@"返回内容不是字符串"];
    }
}

///--------------------------------------
#pragma mark - SRWebSocketDelegate
///--------------------------------------

- (void)webSocketDidOpen:(SRWebSocket *)webSocket;
{
    NSLog(@"Websocket Connected");
    self.webStatus = DT_WEB_STATUS_CONNETED;
}

- (void)webSocket:(SRWebSocket *)webSocket didReceiveMessage:(id)message
{
    [self receiveMessage:message];
//    [self addLog:[NSString stringWithFormat:@"didReceiveMessage \"%@\"", message]];
}

- (void)webSocket:(SRWebSocket *)webSocket didFailWithError:(NSError *)error;
{
    self.webStatus = DT_WEB_STATUS_FAILD;
    [self addLog:[NSString stringWithFormat:@"Websocket Failed With Error %@", error]];
    [self deallocWebSocket];
}

- (void)webSocket:(SRWebSocket *)webSocket didReceiveMessageWithString:(nonnull NSString *)string
{
    NSLog(@"Received \"%@\"", string);
    [self receiveMessage:string];
//    [self addLog:[NSString stringWithFormat:@"Received \"%@\"", string]];
}

- (void)webSocket:(SRWebSocket *)webSocket didCloseWithCode:(NSInteger)code reason:(NSString *)reason wasClean:(BOOL)wasClean;
{
    NSLog(@"WebSocket closed");
    self.webStatus = DT_WEB_STATUS_CLOSE;
    [self addLog:[NSString stringWithFormat:@"Websocket close %@", reason]];
    [self deallocWebSocket];
}

- (void)webSocket:(SRWebSocket *)webSocket didReceivePong:(NSData *)pongPayload;
{
    NSLog(@"WebSocket received pong");
    [self addLog:@"WebSocket received pong"];
}


@end
