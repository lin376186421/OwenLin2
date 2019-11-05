package com.mgame.common;

import android.os.Message;

import com.mgame.extensions.TextInputField.TextInputFieldMessage;
import com.mgame.extensions.MyWebView.MyWebViewMessage;;

public class ActivityDelegate{
	
	public void onInit()
	{
		
	}
	
	public void handleMessage(Message msg)
	{
		switch (msg.what) {
		case AppPlatform.MESSAGE_SHOW_INPUT_FIELD_DIALOG:
			
			TextInputFieldMessage inputFieldMessage = (TextInputFieldMessage)msg.obj;
			inputFieldMessage.showTextField();
			break;
		case AppPlatform.MESSAGE_SHOW_WEBVIEW:
			MyWebViewMessage weviewMessage = (MyWebViewMessage)msg.obj;
			weviewMessage.showWebView();
			break;

		default:
			break;
		}
	}
	
	public void sendEmptyMessage(int messageCode)
	{
		AppActivity.getAppContext().sendEmptyMessage(messageCode);
	}
	
}
