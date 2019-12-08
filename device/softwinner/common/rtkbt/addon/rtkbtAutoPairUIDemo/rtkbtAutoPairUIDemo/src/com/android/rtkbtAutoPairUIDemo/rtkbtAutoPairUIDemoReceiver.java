/*
 * Copyright (C) 2015 The Realtek Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

package com.realtek.rtkbt.AutoPairUIDemo;

import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothRtkbt;
import android.bluetooth.BluetoothDevice;
import android.util.Log;
import android.widget.Toast;
import android.util.Log;
import java.util.HashMap;


/*
 *BluetoothRTKAutoPairBroadcastReceiver sync state with bluetooth,
 *STATE_ON start BluetoothRTKAutoPairService
 *STATE_TURNING_OFF stop BluetoothRTKAutoPairService
 */


public class rtkbtAutoPairUIDemoReceiver extends BroadcastReceiver {
    private static final String TAG = "<===AutoPairUIDemoReceiver===>";
    private static final boolean RTK_DBG = true;
    private static final String RTKBT_AUTOPAIR_MESSAGE = "com.realtek.autopair.message";
    private static final String RTKBT_AUTOPAIR_MESSAGE_FIELD_RCUADDR = "com.realtek.autopair.message.rcuaddr";
    private static final String RTKBT_AUTOPAIR_MESSAGE_FIELD_MESSAGE_ID = "com.realtek.autopair.message.id";
    private static final String RTKBT_AUTOPAIR_MESSAGE_FIELD_DATA = "com.realtek.autopair.message.data";

    private static final int RTKBT_AUTOPAIR_State_Idle                     = 0;
    private static final int RTKBT_AUTOPAIR_State_Found                    = 1;
    private static final int RTKBT_AUTOPAIR_State_RemoveBond               = 2;
    private static final int RTKBT_AUTOPAIR_State_BLEBond                  = 3;
    private static final int RTKBT_AUTOPAIR_State_ClassicBond              = 4;
    private static final int RTKBT_AUTOPAIR_State_SaveNewRCU               = 5;
    private static final int RTKBT_AUTOPAIR_State_PairSuccess_to_connect   = 6;
    private static final int RTKBT_AUTOPAIR_State_ConnectSuccess           = 7;

    private static final int RTKBT_AUTOPAIR_State_PairFail                 = 0x101;
    private static final int RTKBT_AUTOPAIR_State_PairTimeout              = 0x102;
    private static final int RTKBT_AUTOPAIR_State_ConnectFail              = 0x103;

    @Override
    public void onReceive(Context context, Intent intent) {
        Log.i(TAG, "receive intent:" + intent.getAction());

        if(intent.getAction().equals(RTKBT_AUTOPAIR_MESSAGE)) {
            Log.d(TAG, "RTKBT_AUTOPAIR_MESSAGE: ID:"+intent.getIntExtra(RTKBT_AUTOPAIR_MESSAGE_FIELD_MESSAGE_ID, 0)+" ADDR:"+intent.getStringExtra(RTKBT_AUTOPAIR_MESSAGE_FIELD_RCUADDR)+" DATA:"+intent.getByteArrayExtra(RTKBT_AUTOPAIR_MESSAGE_FIELD_DATA));
            ShowMessage(context, intent);
        }
    }
    private void ShowMessage(Context context, Intent intent)
    {
        int message_id = intent.getIntExtra(RTKBT_AUTOPAIR_MESSAGE_FIELD_MESSAGE_ID, 0);
        String rcuaddr = intent.getStringExtra(RTKBT_AUTOPAIR_MESSAGE_FIELD_RCUADDR);
        byte[] data =intent.getByteArrayExtra(RTKBT_AUTOPAIR_MESSAGE_FIELD_DATA);
        switch(message_id)
        {
            case RTKBT_AUTOPAIR_State_Found:
                Toast.makeText(context,"Found RCU("+rcuaddr+")",Toast.LENGTH_LONG).show();
                break;
            case RTKBT_AUTOPAIR_State_RemoveBond:
                Toast.makeText(context,"RCU("+rcuaddr+") Remove Old RCU Bonding Info",Toast.LENGTH_LONG).show();
                break;
            case RTKBT_AUTOPAIR_State_BLEBond:
                Toast.makeText(context,"RCU("+rcuaddr+") Start BLE Bonding",Toast.LENGTH_LONG).show();
                break;
            case RTKBT_AUTOPAIR_State_ClassicBond:
                Toast.makeText(context,"RCU("+rcuaddr+") Start BREDR Bonding",Toast.LENGTH_LONG).show();
                break;
            case RTKBT_AUTOPAIR_State_SaveNewRCU:
                Toast.makeText(context,"RCU("+rcuaddr+") Save New RCU Bonding Info",Toast.LENGTH_LONG).show();
                break;
            case RTKBT_AUTOPAIR_State_PairSuccess_to_connect:
                Toast.makeText(context,"RCU("+rcuaddr+") Connecting to RCU",Toast.LENGTH_LONG).show();
                break;
            case RTKBT_AUTOPAIR_State_ConnectSuccess:
                Toast.makeText(context,"RCU("+rcuaddr+") Connected to RCU, Pair success",Toast.LENGTH_LONG).show();
                break;
            case RTKBT_AUTOPAIR_State_PairFail:
                Toast.makeText(context,"RCU("+rcuaddr+") Pair Failed",Toast.LENGTH_LONG).show();
                break;
            case RTKBT_AUTOPAIR_State_PairTimeout:
                Toast.makeText(context,"RCU("+rcuaddr+") Pair Timeout",Toast.LENGTH_LONG).show();
                break;
            case RTKBT_AUTOPAIR_State_ConnectFail:
                Toast.makeText(context,"RCU("+rcuaddr+") Connect Fail",Toast.LENGTH_LONG).show();
                break;
        }
    }
}
