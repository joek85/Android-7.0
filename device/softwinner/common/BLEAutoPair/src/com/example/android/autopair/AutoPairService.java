package com.example.android.autopair;

import android.app.Service;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothManager;
import android.bluetooth.BluetoothGattServer;
import android.bluetooth.BluetoothGattServerCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothProfile;
import android.bluetooth.BluetoothInputDevice;
import android.bluetooth.le.BluetoothLeScanner;
import android.bluetooth.le.ScanCallback;
import android.bluetooth.le.ScanFilter;
import android.bluetooth.le.ScanResult;
import android.bluetooth.le.ScanSettings;
import android.bluetooth.le.ScanRecord;

import android.content.Context;
import android.content.ComponentName;
import android.content.ServiceConnection;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.BroadcastReceiver;
import android.os.Handler;
import android.os.IBinder;
import android.os.ParcelUuid;
import android.util.Log;
import android.util.SparseArray;
import android.widget.Toast;


import java.util.ArrayList;
import java.util.List;
import java.util.Set;


public class AutoPairService extends Service {
    private static final String TAG = AutoPairService.class.getSimpleName();


    private Handler mHandler = new Handler();

    private BluetoothAdapter mBluetoothAdapter;
    private BluetoothLeScanner mBluetoothLeScanner;

    private ScanCallback mScanCallback;

    /**
     * Stops scanning after 60 seconds.
     */
    private static final long SCAN_PERIOD = 60000;

    private static final long CHECK_SCAN_PERIOD = 5000;

    private boolean scanStarted = false;
    private boolean connected = false;

    private static final ParcelUuid HID_Service_UUID = ParcelUuid
            .fromString("00001812-0000-1000-8000-00805f9b34fb");

    private static final String TARGET_DEVICE_NAME = "BLE RMC";
    private static final String TARGET_DEVICE_ADDRESS = "AC:83:F3:56:39:60";
    private static final byte[] TARGET_DEVICE_MD = {(byte)0x5d, (byte)0x00, (byte)0x4, (byte)0x0};

    private BluetoothInputDevice mService;
    private BluetoothDevice mTargetDevice;


    private final class InputDeviceServiceListener implements BluetoothProfile.ServiceListener {

        public void onServiceConnected(int profile, BluetoothProfile proxy) {
            Log.d(TAG, "Bluetooth service connected");
            mService = (BluetoothInputDevice) proxy;
            
        }

        public void onServiceDisconnected(int profile) {
            Log.d(TAG, "Bluetooth service disconnected");
            mService = null;
        }
    }

    private BroadcastReceiver mReceiver = new BroadcastReceiver() {

        @Override
        public void onReceive(Context context, Intent intent) {

            String action = intent.getAction();
            Log.v(TAG, "got action: " + action);

            if (action.equals(BluetoothAdapter.ACTION_STATE_CHANGED)) {
                int state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE, BluetoothAdapter.ERROR);
                if (state == BluetoothAdapter.STATE_OFF) {
                    //if (scanStarted)
                    //    stopScanning();
                } else if (state == BluetoothAdapter.STATE_ON) {
                    if (!isConnected())
                        startScanning();
                }
            } else if (action.equals(BluetoothDevice.ACTION_BOND_STATE_CHANGED)) {
                int state = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE, BluetoothDevice.BOND_NONE);
                int preState = intent.getIntExtra(BluetoothDevice.EXTRA_PREVIOUS_BOND_STATE, BluetoothDevice.BOND_NONE);
                BluetoothDevice dev = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

                String addr = dev.getAddress();
                if ((addr != null) && (addr.equals(TARGET_DEVICE_ADDRESS))) {
                    if ((preState == BluetoothDevice.BOND_NONE) && (state == BluetoothDevice.BOND_BONDING)) {
                        Toast.makeText(AutoPairService.this, getString(R.string.pairing), Toast.LENGTH_SHORT).show();
                    }

                    if ((state == BluetoothDevice.BOND_BONDED) && (preState == BluetoothDevice.BOND_BONDING)) {
                        Toast.makeText(AutoPairService.this, getString(R.string.connecting), Toast.LENGTH_SHORT).show();
                        connect(dev);
                    }
                }
            } else if (action.equals(BluetoothInputDevice.ACTION_CONNECTION_STATE_CHANGED)) {
                int state = intent.getIntExtra(BluetoothProfile.EXTRA_STATE, BluetoothProfile.STATE_DISCONNECTED);
                int preState = intent.getIntExtra(BluetoothProfile.EXTRA_PREVIOUS_STATE, BluetoothProfile.STATE_DISCONNECTED);
                BluetoothDevice dev = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);

                String addr = dev.getAddress();
                if ((addr != null) && (addr.equals(TARGET_DEVICE_ADDRESS))) {
                    if ((state == BluetoothProfile.STATE_CONNECTED) && (preState == BluetoothProfile.STATE_CONNECTING)) {
                        Toast.makeText(AutoPairService.this, getString(R.string.connected), Toast.LENGTH_SHORT).show();
                        connected = true;
                        stopScanning();
                    } else if (state == BluetoothProfile.STATE_CONNECTED) {
                        connected = true;
                        stopScanning();
                        //dev.fetchUuidsWithSdp();
                    }

                    if (state == BluetoothProfile.STATE_DISCONNECTED) {
                        connected = false;
                        startScanning();
                    }
                }
            }
        }
    };

    @Override
    public int onStartCommand(Intent intent, int flags, int startId) {
        return START_STICKY;
    }

    @Override
    public void onCreate() {
        Log.v(TAG, "onCreate()");

        mBluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        mBluetoothLeScanner = mBluetoothAdapter.getBluetoothLeScanner();

        mScanCallback = new BLEScanCallback();

        IntentFilter mFilter = new IntentFilter();
        mFilter.addAction(BluetoothAdapter.ACTION_STATE_CHANGED);
        mFilter.addAction(BluetoothDevice.ACTION_BOND_STATE_CHANGED);
        mFilter.addAction(BluetoothInputDevice.ACTION_CONNECTION_STATE_CHANGED);
        registerReceiver(mReceiver, mFilter);

        mBluetoothAdapter.getProfileProxy(AutoPairService.this, new InputDeviceServiceListener(),
                BluetoothProfile.INPUT_DEVICE);

        if ((mBluetoothAdapter.isEnabled()) && (!isConnected()))
            startScanning();

        //mHandler.postDelayed(mRunnable, 1000);

        super.onCreate();
    }

    @Override
    public void onDestroy() {
        Log.v(TAG, "onDestroy()");

        unregisterReceiver(mReceiver);

        if (scanStarted)
            stopScanning();

        mBluetoothAdapter.closeProfileProxy(BluetoothProfile.INPUT_DEVICE, mService);

        super.onDestroy();
    }

    @Override
    public IBinder onBind(Intent intent) {
        return null;
    }

    private void connect(BluetoothDevice device) {
        if (mService != null) {
            Log.v(TAG, "connecting to: " + device.getAddress());
            mService.connect(device);
        }
    }

    private void pair(final BluetoothDevice device) {
        boolean removed = false;

        Set<BluetoothDevice> devs = mBluetoothAdapter.getBondedDevices();
        if ((devs != null) && (devs.size() > 0)) {
            for (BluetoothDevice dev : devs) {
                String addr = dev.getAddress();
                if (addr != null) {
                    if (addr.equals(TARGET_DEVICE_ADDRESS)) {
                        Log.v(TAG, "removing old bonded info");
                        dev.removeBond();
                        removed = true;
                    }
                }
            }
        }

        if (removed) {
            Log.v(TAG, "pairing after unbond old device");
            mHandler.postDelayed(new Runnable() {
                @Override
                public void run() {
                    Log.v(TAG, "pairing to: " + device.getAddress());
                    device.createBond(BluetoothDevice.TRANSPORT_LE);
                }
            }, 2000);
        } else {
            Log.v(TAG, "pairing to: " + device.getAddress());
            device.createBond(BluetoothDevice.TRANSPORT_LE);
        }
    }

    private boolean isConnected() {
        if (connected)
            return true;

        if (mService != null) {
            List<BluetoothDevice> devs = mService.getConnectedDevices();
            if ((devs != null) && (devs.size() > 0)) {
                for (BluetoothDevice dev : devs) {
                    if (dev.getAddress().equals(TARGET_DEVICE_ADDRESS))
                        return true;
                }
            }
        }
        return false;
    }

    private void startScanning() {
        mBluetoothLeScanner = mBluetoothAdapter.getBluetoothLeScanner();
        if (mBluetoothLeScanner == null)
            return;

        Log.d(TAG, "Starting Scanning");

        mHandler.removeCallbacks(mStopScanningRunnable);
        mHandler.postDelayed(mStopScanningRunnable, SCAN_PERIOD);

        scanStarted = true;
        mBluetoothLeScanner.startScan(buildScanFilters(), buildScanSettings(), mScanCallback);
        //mBluetoothLeScanner.startScan(null, buildScanSettings(), mScanCallback);
    }

    private void stopScanning() {
        mBluetoothLeScanner = mBluetoothAdapter.getBluetoothLeScanner();
        if (mBluetoothLeScanner == null)
            return;

        Log.d(TAG, "Stopping Scanning");

        scanStarted = false;
        // Stop the scan, wipe the callback.
        mBluetoothLeScanner.stopScan(mScanCallback);
    }

    private List<ScanFilter> buildScanFilters() {
        List<ScanFilter> scanFilters = new ArrayList<ScanFilter>();

        ScanFilter.Builder builder = new ScanFilter.Builder();
        // Comment out the below line to see all BLE devices around you
        builder.setServiceUuid(HID_Service_UUID);
        scanFilters.add(builder.build());

        return scanFilters;
    }

    private ScanSettings buildScanSettings() {
        ScanSettings.Builder builder = new ScanSettings.Builder();
        builder.setScanMode(ScanSettings.SCAN_MODE_BALANCED);
        return builder.build();
    }

    private class BLEScanCallback extends ScanCallback {

        @Override
        public void onBatchScanResults(List<ScanResult> results) {
            super.onBatchScanResults(results);
            Log.v(TAG, "onBatchScanResults().");

            for (ScanResult rs : results) {
                BluetoothDevice dev = rs.getDevice();
                String name = dev.getName();
                if (name != null)
                    Log.v(TAG, "onBatchScanResults, device name: " + name);
            }
        }

        @Override
        public void onScanResult(int callbackType, ScanResult result) {
            super.onScanResult(callbackType, result);

            /*
            ScanRecord sr = result.getScanRecord();
            byte[] md = sr.getManufacturerSpecificData(0x5d);
            if (md != null) {
                Log.v(TAG, "onScanResult, manufacture data is not null");
                Log.v(TAG, "onScanResult, length: " + md.length);
                Log.v(TAG, "onScanResult, value: " + md[0] + ", " + md[1]);
            }
            */

            //Log.v(TAG, "onScanResult().");
            BluetoothDevice dev = result.getDevice();
            ScanRecord sr = result.getScanRecord();
            String name = sr.getDeviceName();
            String addr = dev.getAddress();
            if (name != null) {
                Log.v(TAG, "onScanResult, device name: " + name);

                if (name.equals(TARGET_DEVICE_NAME) && !isConnected() && scanStarted) {
                    stopScanning();
                    pair(dev);
                    return;
                }
            }

            if (addr != null) {
                Log.v(TAG, "onScanResult, device addr: " + addr);

                if (addr.equals(TARGET_DEVICE_ADDRESS) && !isConnected() && scanStarted) {
                    stopScanning();
                    dev.setAlias(TARGET_DEVICE_NAME);
                    pair(dev);
                    return;
                }
            }
        }

        @Override
        public void onScanFailed(int errorCode) {
            //Log.e(TAG, "onScanFailed: " + errorCode);

            if (errorCode != ScanCallback.SCAN_FAILED_ALREADY_STARTED)
                scanStarted = false;
        }
    }

    private Runnable mStopScanningRunnable = new Runnable() {
        @Override
        public void run() {
            stopScanning();
        }
    };

/*
    private Runnable mRunnable = new Runnable() {
        @Override
        public void run() {
            if (!isConnected()) {
                if (!mBluetoothAdapter.isDiscovering())
                    startScanning();
            } else {
                if (scanStarted)
                    stopScanning();
            }

            mHandler.postDelayed(this, CHECK_SCAN_PERIOD);
        }
    };
*/

}