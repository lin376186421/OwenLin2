package com.mgame.common;

import java.io.IOException;

import com.mgame.activity.GameClient;
import com.mgstudio.bg.R;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.media.AudioManager;
import android.media.MediaPlayer;
import android.media.MediaPlayer.OnCompletionListener;
import android.media.RingtoneManager;
import android.net.Uri;

public class NotificationReceiver extends BroadcastReceiver {

	public static int pid = 0;

	@Override
	public void onReceive(Context context, Intent intent) {

		String title = intent.getStringExtra("title");
		String message = intent.getStringExtra("message");

		NotificationManager notificationManager = (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);
		final Intent notificationIntent = new Intent(context, GameClient.class);
		notificationIntent.setAction(Intent.ACTION_MAIN);
		notificationIntent.addCategory(Intent.CATEGORY_LAUNCHER);
		notificationIntent.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);

		PendingIntent contentIntent = PendingIntent.getActivity(context, NotificationReceiver.pid, notificationIntent, 0);

		Notification notif = new Notification(R.drawable.icon, "", System.currentTimeMillis());
		notif.setLatestEventInfo(context, title, message, contentIntent);
		// Clear the notification when it is pressed
		notif.flags |= Notification.FLAG_AUTO_CANCEL;

		Uri defaultRingtoneUri = RingtoneManager.getDefaultUri(RingtoneManager.TYPE_NOTIFICATION);

		MediaPlayer mediaPlayer = new MediaPlayer();

		try {
			mediaPlayer.setDataSource(context, defaultRingtoneUri);
			mediaPlayer.setAudioStreamType(AudioManager.STREAM_NOTIFICATION);
			mediaPlayer.prepare();
			mediaPlayer.setOnCompletionListener(new OnCompletionListener() {

				@Override
				public void onCompletion(MediaPlayer mp) {
					mp.release();
				}
			});
			mediaPlayer.start();
		} catch (IllegalArgumentException e) {
			e.printStackTrace();
		} catch (SecurityException e) {
			e.printStackTrace();
		} catch (IllegalStateException e) {
			e.printStackTrace();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		notificationManager.notify(NotificationReceiver.pid, notif);
				
		pid++;
		if (pid > 10) {
			pid = 0;
		}
	}

	public static void clearMyNotif() {
		for (int i = 0; i < 10; i++) {
			NotificationManager nmanager = (NotificationManager) AppActivity
					.getAppContext().getApplicationContext()
					.getSystemService(Context.NOTIFICATION_SERVICE);
			nmanager.cancel(i);
		}

	}

}
