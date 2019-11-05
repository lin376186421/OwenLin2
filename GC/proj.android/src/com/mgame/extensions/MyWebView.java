package com.mgame.extensions;

import java.io.UnsupportedEncodingException;
import java.util.HashMap;

import org.cocos2dx.lib.Cocos2dxActivity;
import org.cocos2dx.lib.Cocos2dxEditBoxDialog;
import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.cocos2dx.lib.Cocos2dxHelper;

import android.R.string;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.ActionBar.LayoutParams;
import android.content.Context;
import android.content.Intent;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.Typeface;
import android.net.Uri;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.text.InputFilter;
import android.text.InputType;
import android.view.Gravity;
import android.view.View;
import android.view.View.OnFocusChangeListener;
import android.view.KeyEvent;
import android.view.View.OnLayoutChangeListener;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputMethodManager;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;
import android.widget.TextView.OnEditorActionListener;

import com.mgame.common.AppActivity;
import com.mgame.common.AppConfig;
import com.mgame.common.AppPlatform;


public class MyWebView {

	private final int mPosX;
	private final int mPosY;
	private final int mWidth;
	private final int mHeight;
	private final int mDirWidth;
	private final int mDirHeight;
	private final String mWebURL;
	final static int MESSAGE_REMOVE_WEBVIEW = 10009;
	public MyWebView(String webURL,int xPos,int yPos,int width, int height,int dirWidth,int dirHeight)
	{

		this.mPosX = xPos;
		this.mPosY = yPos;
		this.mWidth = width;
		this.mHeight = height;
		this.mDirWidth = dirWidth;
		this.mDirHeight = dirHeight;
		this.mWebURL = webURL;
	}

	private class MyWebViewClient extends WebViewClient {        
        //重写父类方法，让新打开的网页在当前的WebView中显示
	        public boolean shouldOverrideUrlLoading(WebView view, String url) {
	        	if(url != mWebURL)//网页里面的链接 用自带浏览器打开
	        	{
	        		Uri uri = Uri.parse(url);  
					Intent intent=new Intent(Intent.ACTION_VIEW, uri); 
					AppActivity.getAppContext().startActivity(intent);
	        	}
	        	else
	        	{
	               view.loadUrl(url);
	               System.out.println("___shouldOverrideUrlLoading"+url);
	        	}
	        	return true;
	          }
	        public void onPageStarted(WebView view, String url, Bitmap favicon) {
	        	System.out.println("___onPageStarted"+url);
	        }
	        public void onPageFinished(WebView view, String url) {
	        	AppPlatform.nativeWebViewDidFinishLoad(1);
	        	System.out.println("onPageFinished"+url);
	        }
	        public void onReceivedError(WebView view, int errorCode,
	                String description, String failingUrl) {
	        	AppPlatform.nativeWebViewDidFinishLoad(0);
	        	System.out.println("___onReceivedError"+description+failingUrl);
	        }
	      }
	private LinearLayout mLayout = null;
	private WebView mWebview = null;
	public void showMyWebView()
	{
		if(mLayout == null)
		{
			View rootView = Cocos2dxGLSurfaceView.getInstance().getRootView();
			final LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.FILL_PARENT, LinearLayout.LayoutParams.FILL_PARENT);
					
			mLayout = new LinearLayout(AppActivity.getAppContext());
			mLayout.setLayoutParams(layoutParams);
			mLayout.setOrientation(LinearLayout.VERTICAL);
			mLayout.setBackgroundColor(0);
			
			mWebview = new WebView(AppActivity.getAppContext());
			mWebview.getSettings().setJavaScriptEnabled(true);
			mWebview.loadUrl(mWebURL);
			mWebview.setBackgroundColor(0);
			mWebview.getSettings().setCacheMode(WebSettings.LOAD_NO_CACHE);//不使用缓存数据  实时更新
			
			WindowManager wm = AppActivity.getAppContext().getWindowManager();
			 
		    int winWidth = wm.getDefaultDisplay().getWidth();
		    int winHeight = wm.getDefaultDisplay().getHeight();
		    float scale = Math.max((float)winWidth/(float)mDirWidth,(float)winHeight/(float)mDirHeight);
//		    scale = 0.75f;
		    int left = (int)(mPosX*scale);
		    int top = (int)(mPosY*scale);
		    int right = (int)(winWidth-mPosX*scale-mWidth*scale);
			int bottom = (int)(winHeight-mPosY*scale-mHeight*scale);
			mLayout.setPadding(left, top, right, bottom);
		
			
			LayoutParams lp =  new LayoutParams((int)(mWidth*scale), (int)(mHeight*scale));
			mWebview.setLayoutParams(lp);
			
			MyWebViewClient webviewclient = new MyWebViewClient();
			mWebview.setWebViewClient(webviewclient);
			
			mLayout.addView(mWebview);
			
			((ViewGroup)rootView).addView(mLayout);
		}
	}
	public Handler mHandler = new Handler(Looper.getMainLooper()) {
		public void handleMessage(Message msg) {
			if (msg.what == MESSAGE_REMOVE_WEBVIEW) {
				if(mLayout != null)
				{
					View rootView = Cocos2dxGLSurfaceView.getInstance().getRootView();
					mWebview.setWebViewClient(null);
					((ViewGroup)rootView).removeView(mLayout);
					mLayout = null;
				}
			}
		}
	};
	public void hideMyWebView()
	{
		mHandler.sendEmptyMessage(MESSAGE_REMOVE_WEBVIEW);
	}
	
	public static class MyWebViewMessage {
		public int xPos;
		public int yPos;
		public int width;
		public int height;
		public int dirWidth;
		public int dirHeight;
		public String webURL;


		public MyWebViewMessage(String webURL, int xPos,int yPos,int width, int height,int dirWidth,int dirHeight) {

			this.xPos = xPos;
			this.yPos = yPos;
			this.width = width;
			this.height = height;
			this.dirWidth = dirWidth;
			this.dirHeight = dirHeight;
			this.webURL = webURL;
			

		}

		public void showWebView() {
			
			MyWebView webView = new MyWebView(webURL,xPos, yPos, width, height,dirWidth,dirHeight);
			webView.showMyWebView();
			AppActivity.s_myWebView = webView;
//			AppActivity.s_textInputField = textInputField;
		}
	}
}

