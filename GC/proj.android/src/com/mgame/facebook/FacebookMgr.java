
package com.mgame.facebook;


import java.io.ByteArrayOutputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Map.Entry;
import java.util.Set;

import org.cocos2dx.lib.Cocos2dxGLSurfaceView;
import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.json.simple.parser.JSONParser;

import com.facebook.FacebookAuthorizationException;
import com.facebook.FacebookException;
import com.facebook.FacebookOperationCanceledException;
import com.facebook.FacebookRequestError;
import com.facebook.RequestBatch;
import com.facebook.Session;
import com.facebook.SessionDefaultAudience;
import com.facebook.SessionState;
import com.facebook.UiLifecycleHelper;
//import com.facebook.FacebookGraphObjectException;;
import com.facebook.Request;
import com.facebook.Response;
import com.facebook.android.FacebookError;

import com.facebook.model.GraphMultiResult;
import com.facebook.model.GraphObject;
import com.facebook.model.GraphObjectList;
import com.facebook.model.GraphUser;
import com.facebook.widget.FacebookDialog;
import com.facebook.widget.WebDialog;
import com.flurry.sdk.fa;
import com.flurry.sdk.nw;
import com.mgame.common.AppActivity;
import com.mgame.common.AppConfig;
import com.mgame.common.AppPlatform;

import android.R.bool;
import android.R.string;
import android.annotation.SuppressLint;
import android.app.Activity;
import android.app.AlertDialog;
import android.app.Application;
import android.app.Fragment;
import android.app.ActionBar.LayoutParams;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Bitmap.CompressFormat;
import android.graphics.Bitmap.Config;
import android.graphics.BitmapFactory;
import android.graphics.Matrix;
import android.graphics.Rect;
import android.os.Bundle;
import android.provider.ContactsContract.Contacts.Data;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.SeekBar;



@SuppressLint("SdCardPath")
public class FacebookMgr
{
	private enum apiCall {
		kAPIClear,
	    kAPIGraphMe,
	    kAPIInviteFriends,
	    kAPIGraphFriends,
	    kAPIPostMessage,
	    kAPIPostMessageWithPhoto,
	    kAPIOpenSession,
	    kAPILogin,
    }
	
	private static final String TAG = "FacebookMgr";
	private static final String FBPostLinkCaption = "I am playing this great game Battle Glory and I thought that you might like it. Check it out.";

	private static final List<String> PERMISSIONS = new ArrayList<String>() {
	        {
	            add("user_friends");
	            add("email");
	            add("publish_actions");
	            add("public_profile");
//	            add("publish_checkins");
//	            add("photo_upload");
//	            add("video_upload");
//	            add("share_item");
//	            add("basic_info");
	        }
	    };
	 
	 
	private apiCall m_apiCall = apiCall.kAPIClear;
	
	private int m_faildCount = 0;//锟斤拷锟斤拷失效
	private int MAX_FAILD_COUNT = 0;
	private GraphUser m_user;
	private List<GraphUser> m_friends;
	private String m_pendingMessage;
	public UiLifecycleHelper m_uiHelper;
	
	public FacebookMgr()
	{
		
	}
	
	private static FacebookMgr s_appFacebookPlatform = null;
	public static FacebookMgr getFacebookPlatform(){
		if (s_appFacebookPlatform == null) {
			s_appFacebookPlatform = new FacebookMgr();
		}
		return s_appFacebookPlatform;
	}
	
	public void setUser(GraphUser user)
	{
		m_user = user;
		FacebookMgr.nativeFBSetUser(m_user.getId(), m_user.getName());
	}
	
	public GraphUser getUser()
	{
		return m_user;
	}
	public void setFriends(List<GraphUser> friends)
	{

	}
	
	public List<GraphUser> getFriends()
	{
		return m_friends;
	}
	
	private FacebookDialog.Callback dialogCallback = new FacebookDialog.Callback() {
        @Override
        public void onError(FacebookDialog.PendingCall pendingCall, Exception error, Bundle data) {
            Log.d("HelloFacebook", String.format("Error: %s", error.toString()));
            requestResult(false, null,getCurCallType());
        }

        @Override
        public void onComplete(FacebookDialog.PendingCall pendingCall, Bundle data) {
            Log.d("HelloFacebook", "Success!");
            requestResult(true, null,getCurCallType());
        }
    };
    public FacebookDialog.Callback getDialogCallback()
    {
    	return dialogCallback;
    }
	private Session.StatusCallback reOpencallback = new Session.StatusCallback() {
		@Override
		public void call(Session session, SessionState state,
				Exception exception) {

			System.out.println("SessionState" + state);
			if (session.isOpened()) {
				if(m_apiCall == apiCall.kAPILogin)
				{
					apiClear();
					startApiGraphMe();
				}
				else if(m_apiCall == apiCall.kAPIGraphFriends)
				{
					apiClear();
					startApiGraphFriends();
				}
				else if(m_apiCall == apiCall.kAPIPostMessage)
				{
//					apiClear();
//					postMessage(m_pendingMessage);
				}
				else if(m_apiCall == apiCall.kAPIPostMessageWithPhoto)
				{
//					apiClear();
//					postMessageWithPhoto(m_pendingMessage);
				}
			}
			
		}
	};

	public void reOpenSession()
	{
		Session session = getActiveSession();
		session.openForPublish(new Session.OpenRequest(AppActivity.getAppContext()).setCallback(reOpencallback).setPermissions(PERMISSIONS));
	}
	public Session getActiveSession()
	{
		Session session = Session.getActiveSession();
		if(session == null)
		{
			session = new Session(AppActivity.getAppContext());
			Session.setActiveSession(session);
		}
		return session;
	}
	public void init()
	{
		Session session = getActiveSession();
		if(session.getState() == SessionState.CREATED_TOKEN_LOADED)
		{
			FacebookMgr.nativeFBBindActivity(true);
			login(false);
		}
	}
	public void login(Boolean allowLoginUI)
	{
		if(apiNotClear())
			return;
		
		m_apiCall = apiCall.kAPILogin;
		Session session = getActiveSession();
		Activity activity = AppActivity.getAppContext();
		if(session.getState() == SessionState.CREATED)
		{
			reOpenSession();
			
			return;
		}
		
		Session.openActiveSession(activity,allowLoginUI, new Session.StatusCallback() {
			@Override
			public void call(Session session, SessionState state, Exception exception) {
				apiClear();
				if(state == SessionState.CLOSED_LOGIN_FAILED || state == SessionState.OPENING || state == SessionState.CLOSED)
				{
					Log.i(TAG, "session open faild:");
				}
				else
				{
					Log.i(TAG, "session open succeed:");
					startApiGraphMe();
				}
			}
		});		
	}

	public void startApiGraphMe()
	{
		if(apiNotClear())
			return;
		m_apiCall = apiCall.kAPIGraphMe;
		Session session = getActiveSession();
		Request request = Request.newMeRequest(session, new Request.GraphUserCallback() {
            @Override
            public void onCompleted(GraphUser me, Response response) {
            	if(m_apiCall == apiCall.kAPIGraphMe)
            	{
            		FacebookRequestError error = response.getError();
                    if (error == null && me != null) {
                        requestResult(true, null,apiCall.kAPIGraphMe);
                        Log.i(TAG, "newMeRequest:"+me.getName()+me.getId());
                        setUser(me);
                        FacebookMgr.nativeFBBindActivity(true);
                    }
                    else
                    {
                    	FacebookMgr.nativeFBBindActivity(false);
                    	requestResult(false, null,apiCall.kAPIGraphMe);
                    	Log.i(TAG, "newMeRequest:"+error.getErrorMessage());
                    }
            	}
            }
        });
        request.executeAsync();
	}
	public void apiGraphFriends()
	{
		
		
		if(apiNotClear())
			return;
		
		m_apiCall = apiCall.kAPIGraphFriends;

		Session session = getActiveSession();

		Activity activity = AppActivity.getAppContext();
		Session.openActiveSession(activity,true, new Session.StatusCallback() {
			@Override
			public void call(Session session, SessionState state, Exception exception) {
				
				if(state == SessionState.CLOSED_LOGIN_FAILED || state == SessionState.OPENING || state == SessionState.CLOSED)
				{
					apiClear();
					Log.i(TAG, "session open faild:");
				}
				else
				{
					Log.i(TAG, "session open succeed:");
					startApiGraphFriends();
				}
			}
		});	
	}
	public void startApiGraphFriends()
	{
		Session session = getActiveSession();
		m_apiCall = apiCall.kAPIGraphFriends;
		
		if(!session.getPermissions().contains("user_friends"))
		{
			session.requestNewPublishPermissions(new Session.NewPermissionsRequest(AppActivity.getAppContext(), "user_friends"));
			return;
		}
		Request request = Request.newGraphPathRequest(session, "/me/invitable_friends", new Request.Callback() {
            @Override
            public void onCompleted(Response response) {
           	 if(m_apiCall == apiCall.kAPIGraphFriends)
           	 {
           		 FacebookRequestError error = response.getError();
               	 if(error == null)
               	 {
               		try {
               			GraphObject graphObject = response.getGraphObject();
						JSONArray dataArray = (JSONArray)graphObject.getProperty("data");
               			ArrayList<String> friendIds = new ArrayList<String>();
               			ArrayList<String> friendNames = new ArrayList<String>();
               			ArrayList<String> friendUrls = new ArrayList<String>();

               			int count = dataArray.length()/100;
               			for(int i = 0; i <= count; i ++)
               			{
               				int num = 100;
               				if(i == count)
               					num = dataArray.length()-100*i;
               				friendIds.clear();
               				friendNames.clear();
               				friendUrls.clear();
               				for(int j = 0; j < num; j++)
               				{
               					JSONObject obj = dataArray.getJSONObject(j+i*100);
                   				friendIds.add(obj.getString("id"));
                   				friendNames.add(obj.getString("name"));
                   				JSONObject picture = obj.getJSONObject("picture");
                   				JSONObject pdata = picture.getJSONObject("data");
                   				friendUrls.add(pdata.getString("url"));
               				}
               		
               				FacebookMgr.nativeFBSetFriends(friendIds,friendNames,friendUrls);
               				
               			}
               			FacebookMgr.nativeFBGetFriendFinish();
               			requestResult(true, null,m_apiCall);
                  		 Log.i(TAG, "invitable_friends:succeed");
               			
               		  } catch (Exception e) {
               		   e.printStackTrace();
               		  }
               	    
               	 }
               	 else
               	 {
               		 requestResult(false, null,m_apiCall);
               		 Log.i(TAG, "invitable_friends:"+error.getErrorMessage());
               	 }
           	 }
            }
        });
		Bundle invitableParams = new Bundle();
		invitableParams.putString("fields", "id,name,picture");
		request.setParameters(invitableParams);
        request.executeAsync();
	}
	
	 private void onSessionStateChange(Session session, SessionState state, Exception exception) {
	        if ( (exception instanceof FacebookOperationCanceledException || exception instanceof FacebookAuthorizationException)) {
	          
	        } else if (state == SessionState.OPENED_TOKEN_UPDATED) {
	            
	        }
	    }
	public void postMessageWithPhoto(String message)
	{
		
		
		if(apiNotClear())
			return;
		
		
		m_apiCall = apiCall.kAPIPostMessageWithPhoto;
		m_pendingMessage = message;
		Session session = getActiveSession();

		
		
		Activity activity = AppActivity.getAppContext();
		Session.openActiveSession(activity,true, new Session.StatusCallback() {
		@Override
		public void call(Session session, SessionState state, Exception exception) {
			
			if(state == SessionState.CLOSED_LOGIN_FAILED || state == SessionState.OPENING || state == SessionState.CLOSED)
			{
				apiClear();
				Log.i(TAG, "session open faild:");
			}
			else
			{
				Log.i(TAG, "session open succeed:");

				startPostMessageWithPhoto(m_pendingMessage);
			}
		}
	});	
	}
	public void startPostMessageWithPhoto(String message)
	{
		m_apiCall = apiCall.kAPIPostMessageWithPhoto;		
		Session session = getActiveSession();
		
		
		String imagePath = AppActivity.getAppContext().getFilesDir().getAbsolutePath()+"/my_screen.png";
		Bitmap image = BitmapFactory.decodeFile(imagePath);
		
		if(image == null)
		{
			requestResult(false, null,apiCall.kAPIPostMessageWithPhoto);
			return;
		}

		 {
			 if(!session.getPermissions().contains("publish_actions"))
				{
					session.requestNewPublishPermissions(new Session.NewPermissionsRequest(AppActivity.getAppContext(), "publish_actions"));
					return;
				}
			 Request request = Request
		             .newUploadPhotoRequest(session,image, new Request.Callback() {
		                 @Override
		                 public void onCompleted(Response response) {
		                	 if(m_apiCall == apiCall.kAPIPostMessageWithPhoto)
		                	 {
		                		 FacebookRequestError error = response.getError();
			                	 if(error == null)
			                	 {
			                		 requestResult(true, null,m_apiCall);
			                		 Log.i(TAG, "newStatusUpdateRequest:succeed");
			                	 }
			                	 else
			                	 {
			                		 requestResult(false, null,m_apiCall);
			                		 Log.i(TAG, "newStatusUpdateRequest:"+error.getErrorMessage());
			                	 }
		                	 }
		                 }
		             });
			 Bundle params = new Bundle();
			 params.putParcelable("source", image);
			 if(m_pendingMessage.length() > 0)
			 params.putString("message", m_pendingMessage);
			 request.setParameters(params);
			 request.executeAsync();
		 }
	}
	
	public void postMessage(String message)
	{
		
	}

	public void startPostMessage(String message)
	{
		
	}
	
	public String m_pendingFidsString = null;
	public void startInvite(String fidsString)
	{
		if(apiNotClear())
			return;
		m_pendingFidsString = fidsString;
		m_apiCall = apiCall.kAPIInviteFriends;
		Session session = getActiveSession();
		Activity activity = AppActivity.getAppContext();
		
		
		Session.openActiveSession(activity,true, new Session.StatusCallback() {
		@Override
		public void call(Session session, SessionState state, Exception exception) {
			
			if(state == SessionState.CLOSED_LOGIN_FAILED || state == SessionState.OPENING || state == SessionState.CLOSED)
			{
				if(m_apiCall == apiCall.kAPIInviteFriends)
				{
				}
				apiClear();
				Log.i(TAG, "session open faild:");
			}
			else
			{
				Log.i(TAG, "session open succeed:");
				Bundle params = new Bundle();
		        params.putString("message", FBPostLinkCaption);      
		        params.putString("to", m_pendingFidsString);
		        Log.i(TAG, "startInvite---"+m_pendingFidsString);
		        showDialogWithoutNotificationBar("apprequests", params);
			}
		}
	});	
	}
	private void showDialogWithoutNotificationBar(String action, Bundle params) {
		// Create the dialog
		m_apiCall = apiCall.kAPIInviteFriends;
		Activity activity = AppActivity.getAppContext();
		WebDialog dialog = new WebDialog.Builder(activity, Session.getActiveSession(), action, params).setOnCompleteListener(
				new WebDialog.OnCompleteListener() {
			@Override
			public void onComplete(Bundle values, FacebookException error) {
				if(m_apiCall == apiCall.kAPIInviteFriends)
				{
					if (error != null) {
						 requestResult(false, null,m_apiCall);
						 Log.i(TAG, "invite error:"+error.getMessage());
					}
					else
					{
						 requestResult(true, null,m_apiCall);
//						 Bundle val = (Bundle)values.get("to");
                		 Log.i(TAG, "invite succeed");
					}
				}
			}
		}).build();
		
		// Hide the notification bar and resize to full screen
		Window dialog_window = dialog.getWindow();
    	dialog_window.setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
    	
    	// Store the dialog information in attributes
    	// Show the dialog
    	dialog.show();
	}
	
		
	
	public Boolean apiNotClear()
	{
		if(m_apiCall != apiCall.kAPIClear)
		{
			m_faildCount += 1;
			if(m_faildCount >= MAX_FAILD_COUNT)
			{
				m_faildCount = 0;
				m_apiCall = apiCall.kAPIClear;
				return false;
			}
				
			return true;
		}
		else
		{
			return false;
		}
	}
	public apiCall getCurCallType()
	{
		return m_apiCall;
	}
	public void apiClear()
	{
		m_apiCall = apiCall.kAPIClear;
	}
	public void loginOut()
	{
		apiClear();
		Session session = getActiveSession();
		if (!session.isClosed()) {
	           session.closeAndClearTokenInformation();
	           Session.setActiveSession(null);
	           FacebookMgr.nativeFBLoginOutSucceed();
	       }   
	}
	
	public void requestResult(Boolean result,Object obj, apiCall callType)
	{
		apiClear();
		int type = callType.ordinal();
		FacebookMgr.nativeFBRequestResult(result,type);
		
		if(result)
		{
			m_faildCount = 0;
		}
		
	}
	
	
	public void fbInit()
	{
		init();
	}
	
	public void fbLogin()
	{
		login(true);
	}
	
	public void fbLoginOut()
	{
		loginOut();
	}
	
	public void fbApiGraphFriends()
	{
		apiGraphFriends();
	}
	
	public void fbPostMessage(String message)
	{
		postMessage(message);
	}
	
	public void fbPostMessageWithPhoto(String message)
	{
		postMessageWithPhoto(message);
	}
	
	public void fbStartInvite(String fidsString)
	{
		startInvite(fidsString);
	}
	
	
	public static native void nativeFBBindActivity(boolean activity);
	public static native void nativeFBLoginOutSucceed();
	public static native void nativeFBSetUser(String fbId,String fbName);
	public static native void nativeFBSetFriends(ArrayList<String> friendIds , ArrayList<String> friendNames, ArrayList<String> friendUrls);
	public static native void nativeFBGetFriendFinish();
	public static native void nativeFBRequestResult(boolean result, int callType);

}
