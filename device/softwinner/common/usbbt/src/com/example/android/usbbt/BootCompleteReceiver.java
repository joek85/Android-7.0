package com.example.android.usbbt;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.util.Log;


public class BootCompleteReceiver extends BroadcastReceiver {
    private static final String ACTION = "android.intent.action.BOOT_COMPLETED";

    @Override 
    public void onReceive(Context context, Intent intent) {

        if (intent.getAction().equals(ACTION))
        {
            Log.d("USBBT", "start service...");

            Intent i = new Intent(context, USBBTService.class);
            i.setAction(USBBTService.ACTION);
            context.startService(i);
        }
    }
}
