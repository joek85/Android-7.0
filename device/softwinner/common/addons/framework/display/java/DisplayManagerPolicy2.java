
package android.hardware.display;

import java.util.ArrayList;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.media.AudioManager;
import android.os.Handler;
import android.os.Looper;
import android.os.SystemProperties;
import android.os.DisplayOutputManager;
import android.provider.Settings;
import android.util.Log;
import android.view.Display;
import android.widget.Toast;
import android.content.ContentResolver;
import android.database.ContentObserver;
import android.provider.Settings.SettingNotFoundException;

import java.io.File;
import java.io.FileWriter;
import java.io.FileReader;
import java.io.BufferedReader;
import java.io.IOException;
import android.hardware.display.outputstate.*;

/** @hide */
public class DisplayManagerPolicy2 {

    private static final String TAG = "DisplayManagerPolicy2";
    private Context mContext;
    private DisplayOutputManager mDm;
    private Handler mH = new Handler(Looper.getMainLooper());
    private boolean mBootCompleted = false;

    private static File mDisp2EnhanceIdFile;
    private static File mDisp2EnhanceModeFile;
    private static boolean canSetDisp2Enhance;

    private DispOutputState mainDispToDev0PlugIn;
    private DispOutputState mainDispToDev0PlugInExt;
    private DispOutputState mainDispToDev0PlugOut;
    private DispOutputState mainDispToDev1PlugIn;
    private DispOutputState mainDispToDev1PlugOut;
    private DispOutputState dualDisplayOutput;
    private DispOutputState mDispOutputState;

    class DataBaseObserver extends ContentObserver {
        DataBaseObserver(Handler handler) {
        super(handler);
        }

        void observe() {
            ContentResolver resolver = mContext.getContentResolver();
           /*
            resolver.registerContentObserver(Settings.System.getUriFor(
                Settings.System.DISPLAY_AREA_RATIO), false, this);
            update();

            int hpercent = Settings.System.getInt(resolver,
                Settings.System.DISPLAY_AREA_H_PERCENT, 100);
            int vpercent = Settings.System.getInt(resolver,
                Settings.System.DISPLAY_AREA_V_PERCENT, 100);
            mDm.setDisplayMargin(Display.TYPE_BUILT_IN, hpercent, vpercent);
            */
            //int brightness = Settings.System.getInt(resolver,
            //    Settings.System.COLOR_BRIGHTNESS, 100);
            //mDm.setDisplayBright(Display.TYPE_BUILT_IN, brightness);
            //int contrast = Settings.System.getInt(resolver,
            //    Settings.System.COLOR_CONTRAST, 100);
            //mDm.setDisplayContrast(Display.TYPE_BUILT_IN, contrast);
            //int saturation = Settings.System.getInt(resolver,
            //    Settings.System.COLOR_SATURATION, 100);
            //mDm.setDisplaySaturation(Display.TYPE_BUILT_IN, saturation);
        }

        @Override public void onChange(boolean selfChange) {
            update();
        }

        public void update() {
        /*
            ContentResolver resolver = mContext.getContentResolver();
            try{
            int dispAreaRatio = Settings.System.getInt(resolver,
                Settings.System.DISPLAY_AREA_RATIO);
            Settings.System.putInt(resolver,
                Settings.System.DISPLAY_AREA_H_PERCENT, dispAreaRatio);
            Settings.System.putInt(resolver,
                Settings.System.DISPLAY_AREA_V_PERCENT, dispAreaRatio);
            }catch(SettingNotFoundException e){
                Log.e(TAG, Settings.System.DISPLAY_AREA_RATIO +" not found");
            }
         */
        }
    }

    private BroadcastReceiver mBootCompletedReceiver = new BroadcastReceiver() {
        @Override
        public void onReceive(Context context, Intent intent) {
            mBootCompleted = true;
        }
    };

    private class DispDevice {
        int type;
        int revertPlugStateType;
        boolean hotplugSupport;
    };
    private ArrayList<DispDevice> mDispDevices =  new ArrayList<DispDevice>();

    public DisplayManagerPolicy2(Context context) {
        mContext = context;
        mDm = (DisplayOutputManager)context.getSystemService(Context.DISPLAYOUTPUT_SERVICE);

        DataBaseObserver observer = new DataBaseObserver(new Handler());
        observer.observe();

        IntentFilter filter = new IntentFilter(Intent.ACTION_BOOT_COMPLETED);
        mContext.registerReceiver(mBootCompletedReceiver, filter);

        mDisp2EnhanceIdFile = new File("/sys/class/disp/disp/attr/disp");
        mDisp2EnhanceModeFile = new File("/sys/class/disp/disp/attr/enhance_mode");
        if(mDisp2EnhanceIdFile.exists() && mDisp2EnhanceModeFile.exists()) {
            canSetDisp2Enhance = true;
            int enhanceMode = SystemProperties.getInt(
                "persist.sys.disp_enhance_mode", 0);
            setDisplay2EnhanceMode(0, enhanceMode);
        } else {
            canSetDisp2Enhance = false;
        }

        initOutputState();
    }

    private void initOutputState() {
        int dispFormat;
        DispDevice dispDevice;
        mainDispToDev0PlugIn = new MainDispToDev0PlugIn(this);
        mainDispToDev0PlugInExt = new MainDispToDev0PlugInExt(this);
        mainDispToDev0PlugOut = new MainDispToDev0PlugOut(this);
        mainDispToDev1PlugIn = new MainDispToDev1PlugIn(this);
        mainDispToDev1PlugOut= new MainDispToDev1PlugOut(this);
        dualDisplayOutput = new DualDisplayOutput(this);

        // 1: init the mDispDevices
        String[] defFormats = getDevicesDefFormat();
        for (int i = 0; i < defFormats.length; i++) {
            dispDevice = new DispDevice();
            dispFormat = Integer.valueOf(defFormats[i], 16);
            dispDevice.type = mDm.getDisplayTypeFromFormat(dispFormat);
            dispDevice.hotplugSupport = getHotplugSupport(dispDevice.type);
            dispDevice.revertPlugStateType = getRevertHotplugType(dispDevice.type);
            Log.d(TAG, "device[" + i + "]: type=" + dispDevice.type
                + ", hotPlugSupport=" + dispDevice.hotplugSupport
                + ", RPST=" + dispDevice.revertPlugStateType);
            mDispDevices.add(i, dispDevice);
        }

        // 2: init the mDispOutputState --> the current output state
        int dispType;
        int currentDispNum = 0;
        int mainPriority = 0;

        dispType = mDm.getDisplayOutputType(Display.TYPE_BUILT_IN);
        dispDevice = getDispDeviceByType(dispType);
        if(mDispDevices.contains(dispDevice)) {
            currentDispNum++; // always has maindisplay
            mainPriority = mDispDevices.indexOf(dispDevice);
            Log.d(TAG, "mainDispDevice[" + dispDevice.type + ","
                + mainPriority + "]");
        } else {
            Log.e(TAG, "main: initOutputState maybe failed, fixme!!!");
        }

        dispType = mDm.getDisplayOutputType(Display.TYPE_HDMI);
        dispDevice = getDispDeviceByType(dispType);
        if(mDispDevices.contains(dispDevice)) {
            currentDispNum++;
            Log.d(TAG, "extDispDevice[" + dispDevice.type + ","
                + mDispDevices.indexOf(dispDevice) + "]");
        } else {
            Log.d(TAG, "no external display for the type[" + dispType + "]");
        }

		Log.d(TAG, "currentDispNum=" + currentDispNum + ", mainPriority=" + mainPriority);
        if(1 == currentDispNum) {
            if(1 == mainPriority) {
                mDispOutputState = mainDispToDev1PlugIn; //always plugin
            } else {
                mDispOutputState = mainDispToDev0PlugIn; //always plugin
            }
        } else if(2 == currentDispNum) {
            mDispOutputState = dualDisplayOutput; //always pulgin
        } else {
            Log.d(TAG, "currentDispNum is " + String.valueOf(currentDispNum));
            mDispOutputState = null;
        }
    }

    public DispOutputState getMainDispToDev0PlugIn() {
        return mainDispToDev0PlugIn;
    }
    public DispOutputState getMainDispToDev0PlugInExt() {
        return mainDispToDev0PlugInExt;
    }
    public DispOutputState getMainDispToDev0PlugOut() {
        return mainDispToDev0PlugOut;
    }
    public DispOutputState getMainDispToDev1PlugIn() {
        return mainDispToDev1PlugIn;
    }
    public DispOutputState getMainDispToDev1PlugOut() {
        return mainDispToDev1PlugOut;
    }
    public DispOutputState getDualDisplayOutput() {
        return dualDisplayOutput;
    }
    public void setOutputState(DispOutputState state) {
        Log.d(TAG, "setOutputState: " + state);
        this.mDispOutputState = state;
    }
    public int setDisplayOutput(int disp, int dispFormat) {
        Log.d(TAG, "setDispOutput: disp=" + disp + ", dispFormat=" + dispFormat);
        return mDm.setDisplayOutput(disp, dispFormat);
    }

    private DispDevice getDispDeviceByType(int dispType) {
        int priority;
        for(priority = 0; priority < mDispDevices.size(); priority++) {
            DispDevice dispDevice = mDispDevices.get(priority);
            if(dispType == dispDevice.type) {
                return dispDevice;
            }
        }
        return null;
    }
    private int getDispTypeByPriority(int priority) {
        if(mDispDevices.size() <= priority)
            return 0;
        return mDispDevices.get(priority).type;
    }
    private synchronized void dispDevicePlugChanged(DispDevice dispDevice, boolean plugState) {
        int dispFormat = 0;
        int dispType = dispDevice.type;
        int priority = mDispDevices.indexOf(dispDevice);
        if(false == plugState) {
            if(0 == priority) {
                dispType = getDispTypeByPriority(1);
            }
        }
        dispFormat = mDm.makeDisplayFormat(dispType, 0xFF);// 0xFF means adaptive mode
        Log.d(TAG,"plug=" + plugState + ", device[" + priority+ ","
            + dispDevice.type + ", " + dispFormat + "]");
        if(null != mDispOutputState)
            mDispOutputState.devicePlugChanged(dispFormat, priority, plugState);
    }
    public synchronized void notifyDisplayDevicePlugedChanged(int displayType, boolean pluggedIn) {
        DispDevice dispDevice = getDispDeviceByType(displayType);
        DispDevice dispDevice1;
        hotplugTips(displayType, pluggedIn);
        if(null == dispDevice) {
            return;
        }
        if(false == dispDevice.hotplugSupport) {
            return;
        }
        if(0 != dispDevice.revertPlugStateType && true == pluggedIn) {
            dispDevice1 = getDispDeviceByType(dispDevice.revertPlugStateType);
            dispDevicePlugChanged(dispDevice1, false);
        }
        dispDevicePlugChanged(dispDevice, pluggedIn);
        if(0 != dispDevice.revertPlugStateType && false == pluggedIn) {
            dispDevice1 = getDispDeviceByType(dispDevice.revertPlugStateType);
            dispDevicePlugChanged(dispDevice1, true);
        }
    }

    private int getRevertHotplugType(int displayType) {
        int propertyValue;
        switch(displayType) {
        case DisplayOutputManager.DISPLAY_OUTPUT_TYPE_HDMI:
            propertyValue = SystemProperties.getInt("persist.sys.hdmi_rvthpd", 0);
            break;
        case DisplayOutputManager.DISPLAY_OUTPUT_TYPE_TV:
            propertyValue = SystemProperties.getInt("persist.sys.cvbs_rvthpd", 0);
            break;
        case DisplayOutputManager.DISPLAY_OUTPUT_TYPE_VGA:
            propertyValue = SystemProperties.getInt("persist.sys.vga_rvthpd", 0);
            break;
        case DisplayOutputManager.DISPLAY_OUTPUT_TYPE_LCD:
        default:
            return 0;
        }
        return propertyValue;
    }

    private boolean getHotplugSupport(int displayType) {
        int hotplugSupport = 0;
        switch(displayType) {
        case DisplayOutputManager.DISPLAY_OUTPUT_TYPE_HDMI:
            hotplugSupport = SystemProperties.getInt("persist.sys.hdmi_hpd", 0);
            break;
        case DisplayOutputManager.DISPLAY_OUTPUT_TYPE_TV:
            hotplugSupport = SystemProperties.getInt("persist.sys.cvbs_hpd", 0);
            break;
        case DisplayOutputManager.DISPLAY_OUTPUT_TYPE_VGA:
            hotplugSupport = SystemProperties.getInt("persist.sys.vga_hpd", 0);
            break;
        case DisplayOutputManager.DISPLAY_OUTPUT_TYPE_LCD:
        default:
            return false;
        }
        return (1 == hotplugSupport);
    }

    private String[] getDevicesDefFormat() {
        ArrayList<String> list = new ArrayList<String>();
        int i = 0;
        String prop_name = "persist.sys.disp_dev" + 0;
        String format = SystemProperties.get(prop_name, "0");
        Log.d(TAG, "prop_name[" + prop_name + "]: " + format);
        while("0" != format) {
            list.add(format);
            i++;
            prop_name = "persist.sys.disp_dev" + i;
            format = SystemProperties.get(prop_name, "0");
            Log.d(TAG, "prop_name[" + prop_name + "]: " + format);
        }
        return (String[])list.toArray(new String[list.size()]);
    }

    private void hotplugTips(int displayType, boolean pluggedIn) {
        if (mBootCompleted) {
            final String msg;
            String plugMsg = pluggedIn ? " has been plugged in" : " has been plugged out";
            switch(displayType) {
            case DisplayOutputManager.DISPLAY_OUTPUT_TYPE_HDMI:
                msg = "HDMI" + plugMsg;
                break;
            case DisplayOutputManager.DISPLAY_OUTPUT_TYPE_TV:
                msg = "CVBS" + plugMsg;
                break;
            case DisplayOutputManager.DISPLAY_OUTPUT_TYPE_VGA:
                msg = "VGA" + plugMsg;
                break;
            case DisplayOutputManager.DISPLAY_OUTPUT_TYPE_LCD:
                msg = "LCD" + plugMsg;
                break;
            default:
                msg = "unknow" + plugMsg;
            }
            mH.post(new Runnable() {
                @Override
                public void run() {
                    Toast.makeText(mContext, msg, Toast.LENGTH_LONG).show();
                }
            });
        }
    }

    public int getDisplay2EnhanceMode(int disp) {
        if(!canSetDisp2Enhance) {
            return -1;
        }
        FileWriter fw;
        BufferedReader reader;
        int mode = -1;
        try{
            fw = new FileWriter(mDisp2EnhanceIdFile);
            fw.write(String.valueOf(disp));
            fw.close();
            reader = new BufferedReader(new FileReader(mDisp2EnhanceModeFile));
            mode = Integer.parseInt(reader.readLine());
            reader.close();
        } catch(IOException e) {
            e.printStackTrace();
        }
        return mode;
    }
    public int setDisplay2EnhanceMode(int disp, int mode) {
        if(!canSetDisp2Enhance) {
            return -1;
        }
        FileWriter fw;
        try{
            fw = new FileWriter(mDisp2EnhanceIdFile);
            fw.write(String.valueOf(disp));
            fw.close();
            fw = new FileWriter(mDisp2EnhanceModeFile);
            fw.write(String.valueOf(mode));
            fw.close();
            SystemProperties.set("persist.sys.disp_enhance_mode",
                String.valueOf(mode));
        } catch(IOException e) {
            e.printStackTrace();
        }
        return 0;
    }
}

