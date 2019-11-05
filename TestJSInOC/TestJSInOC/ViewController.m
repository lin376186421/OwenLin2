//
//  ViewController.m
//  TestJSInOC
//
//  Created by 林 on 17/2/10.
//  Copyright © 2017年 林. All rights reserved.
//

#import "ViewController.h"
#import "JSUIWebViewViewController.h"
#import "UIWebViewJSViewController.h"
#import "JSWKWebViewViewController.h"
#import "WKWebViewJSViewController.h"

@interface ViewController ()<UIWebViewDelegate>
{
    IBOutlet UIWebView *_webView;
}

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    NSString* path = [[NSBundle mainBundle] pathForResource:@"index" ofType:@"html"];
    NSURL* url = [NSURL fileURLWithPath:path];
    NSURLRequest* request = [NSURLRequest requestWithURL:url];
    [_webView loadRequest:request];
//    [_webView loadHTMLString:<#(nonnull NSString *)#> baseURL:<#(nullable NSURL *)#>]
//    [_webView loadRequest:[NSURLRequest requestWithURL:[NSURL URLWithString:@"https://www.baidu.com"]]];
    // Do any additional setup after loading the view, typically from a nib.
}

- (IBAction)test1:(id)sender {
    NSString *insertString = [NSString stringWithFormat:
                              @"var script = document.createElement('script');"
                              "script.type = 'text/javascript';"
                              "script.text = \"function jsFunc() { "
                              "var a=document.getElementsByTagName('body')[0];"
                              "alert('%@');"
                              "}\";"
                              "document.getElementsByTagName('head')[0].appendChild(script);", @"test1Action"];
//    NSLog(@"insert string %@",insertString);
    [_webView stringByEvaluatingJavaScriptFromString:insertString];
    
    NSString *jsStr = @"var script = document.createElement('script');"
    "script.type = 'text/javascript';"
    "script.text = \"function myFunction() { "   //定义myFunction方法
    "var field = document.getElementsByName('word')[0];"
    "field.value='WWDC2014';"
    "document.forms[0].submit();"
    "}\";"
    "document.getElementsByTagName('head')[0].appendChild(script);";
    [_webView stringByEvaluatingJavaScriptFromString:jsStr];
    
}

- (IBAction)wkweb2js:(id)sender {
    WKWebViewJSViewController *vc = [[WKWebViewJSViewController alloc] initWithNibName:@"WKWebViewJSViewController" bundle:nil];
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)js2wkweb:(id)sender {
    JSWKWebViewViewController *vc = [[JSWKWebViewViewController alloc] initWithNibName:@"JSWKWebViewViewController" bundle:nil];
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)js2web:(id)sender {
    JSUIWebViewViewController *vc = [[JSUIWebViewViewController alloc] initWithNibName:@"JSUIWebViewViewController" bundle:nil];
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)web2js:(id)sender {
    UIWebViewJSViewController *vc = [[UIWebViewJSViewController alloc] initWithNibName:@"UIWebViewJSViewController" bundle:nil];
    [self.navigationController pushViewController:vc animated:YES];
}

- (IBAction)test2:(id)sender {
//    [_webView stringByEvaluatingJavaScriptFromString:@"jsFunc();"];
    [_webView stringByEvaluatingJavaScriptFromString:@"myFunction();"];

}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (BOOL)webView:(UIWebView *)webView shouldStartLoadWithRequest:(NSURLRequest *)request navigationType:(UIWebViewNavigationType)navigationType
{
    return YES;
}

- (void)webViewDidStartLoad:(UIWebView *)webView
{
    
}

- (void)webViewDidFinishLoad:(UIWebView *)webView
{
//    NSString *jsStr = @"var script = document.createElement('script');"
//    "script.type = 'text/javascript';"
//    "script.text = \"function myFunction() { "   //定义myFunction方法
//    "var field = document.getElementsByName('word')[0];"
//    "field.value='WWDC2014';"
//    "document.forms[0].submit();"
//    "}\";"
//    "document.getElementsByTagName('head')[0].appendChild(script);";
   
}

- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error
{
    
}


@end
