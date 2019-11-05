package com.mgame.common;

import static com.mgame.gcm.CommonUtilities.EXTRA_MESSAGE;

import org.cocos2dx.lib.Cocos2dxActivity;

import com.mgstudio.bg.R;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.res.Resources;
import android.os.AsyncTask;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;
import android.os.Message;
import android.util.Log;
import android.view.MotionEvent;
import android.view.View;
import android.view.inputmethod.InputMethodManager;
import android.widget.EditText;

import com.mgame.extensions.MyWebView;
import com.mgame.extensions.TextInputField;

public class AppActivity extends Cocos2dxActivity{
	public static final String TAG = AppActivity.class.getSimpleName();

	public static AppActivity s_appActivity = null;

	public ActivityDelegate m_activityDelegate = null;

	public static MyWebView s_myWebView = null;
	public static TextInputField s_textInputField = null;
	public static IAPBilling s_iapBilling = new IAPBilling();
	
	public static boolean isWaitPurchase = false;
	
	public AsyncTask<Void, Void, Void> mRegisterTask;
	
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		
		setActivityDelegate(new ActivityDelegate());
	}

	public static AppActivity getAppContext() {
		return s_appActivity;
	}

	public void setActivityDelegate(ActivityDelegate activityDelegate) {
		m_activityDelegate = activityDelegate;
	}

	public void sendEmptyMessage(int messageCode) {
		mHandler.sendEmptyMessage(messageCode);
	}

	public Handler mHandler = new Handler(Looper.myLooper()) {
		public void handleMessage(Message msg) {
			if (m_activityDelegate != null) {
				m_activityDelegate.handleMessage(msg);
			}
		}
	};
		
	@Override
	protected void onActivityResult(int requestCode, int resultCode, Intent data) {
		super.onActivityResult(requestCode, resultCode, data);
		
		if (s_iapBilling != null) {
			s_iapBilling.onActivityResult(requestCode, resultCode, data);	
		}
	}

	protected final BroadcastReceiver mHandleMessageReceiver = new BroadcastReceiver() {
		@Override
		public void onReceive(Context context, Intent intent) {
			String newMessage = intent.getExtras().getString(EXTRA_MESSAGE);
			// mDisplay.append(newMessage + "\n");
			Log.d("GCM", newMessage);
		}
	};

	public boolean dispatchTouchEvent(MotionEvent ev) {
		if (ev.getAction() == MotionEvent.ACTION_UP) {
			View v = getCurrentFocus();
			if (isShouldHideInput(v, ev)) {
				if (hideInputMethod(this, v)) {
					return true;
				}
			}
		}
		return super.dispatchTouchEvent(ev);
	}

	public static boolean isShouldHideInput(View v, MotionEvent event) {
		if (v != null && (v instanceof EditText)) {
			
			int leftTop[] = { (int)v.getLeft(), (int)v.getTop() };
			
			int left = leftTop[0], top = leftTop[1], bottom = top
					+ v.getHeight(), right = left + v.getWidth();
			if (event.getX() > left && event.getX() < right
					&& event.getY() > top && event.getY() < bottom) {
				return false;
			} else {
				return true;
			}
		}
		return false;
	}

	public static Boolean hideInputMethod(Context context, View v) {
		InputMethodManager imm = (InputMethodManager) context
				.getSystemService(Context.INPUT_METHOD_SERVICE);
		if (imm != null) {

			if (s_textInputField != null) {
				s_textInputField.onEndEditing();
			}		

			return imm.hideSoftInputFromWindow(v.getWindowToken(), 0);
		}
		return false;
	}
	
	@Override  
    public void onWindowFocusChanged(boolean hasFocus) {  
        super.onWindowFocusChanged(hasFocus);  
    }  
	
	@Override
	protected void onStart() {
		super.onStart();		
	}
	
	@Override
	protected void onStop() {
		super.onStop();
	}
	
	@Override
	protected void onPause() {
		
//		if (isWaitPurchase) {
//			return;
//		}
		
		super.onPause();
	}
	
	@Override
	protected void onResume() {
		super.onResume();
	}
	
	@Override
	protected void onDestroy() {
		
		nativeApplicationWillTerminate();
		
		Log.d("test", "on destory test !!!");
		
		super.onDestroy();
	}

	public static native void nativeApplicationWillTerminate();
}