package com.mgame.common;

import android.util.Log;

import com.igexin.sdk.PushManager;

public class GeXinDelegate {
	public void onStart()
	{		
		// SDK初始化，第三方程序启动时，都要进行SDK初始化工作
		Log.d("GetuiSdkDemo", "initializing sdk...");
		PushManager.getInstance().initialize(AppActivity.getContext());
	}
	
	public void onStop()
	{
		
	}
}
