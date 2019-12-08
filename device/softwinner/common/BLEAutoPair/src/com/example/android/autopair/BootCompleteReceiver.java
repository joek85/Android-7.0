package com.example.android.autopair;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;


public class BootCompleteReceiver extends BroadcastReceiver {
    private static final String ACTION = "android.intent.action.BOOT_COMPLETED";

    @Override 
    public void onReceive(Context context, Intent intent) {

        String action = intent.getAction();
        Log.v("BLEAutoPair", "got action: " + action);

        if (action.equals(ACTION)) {
            Log.d("BLEAutoPair", "Starting autopair service...");

            Intent i = new Intent(context, AutoPairService.class);
            context.startService(i);
        }
    }
}
