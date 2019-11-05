package com.mgame.common;

import android.content.res.Resources;

public class AppRes {
	public static String getRString(int id){
		
		try {
			String str = AppActivity.getAppContext().getString(id);
			return str;
		} catch (Exception e) {
			e.printStackTrace();
		}
	
		return "";
	}
	
	public static int getRInt(String name, String defType){
		Resources res = AppActivity.getAppContext().getResources();
		int val = res.getIdentifier(name, defType, AppActivity.getAppContext().getPackageName());
		return val;
	}
}
