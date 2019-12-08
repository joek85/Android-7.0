package android.hardware.display.outputstate;
import android.hardware.display.DisplayManagerPolicy2;
import android.hardware.display.outputstate.DispOutputState;
import android.os.SystemProperties;

public class MainDispToDev1PlugIn implements DispOutputState {
    DisplayManagerPolicy2 mDMP;
	int mDispPolicy;

    public MainDispToDev1PlugIn(DisplayManagerPolicy2 displayManagerPolicy) {
        this.mDMP = displayManagerPolicy;
        mDispPolicy = Integer.valueOf(SystemProperties.get("persist.sys.disp_policy", "0"), 16);
    }

    public void devicePlugChanged(int d0Format,int priority,boolean curPlugState) {
        if(false == curPlugState && 1 == priority) {
            mDMP.setOutputState(mDMP.getMainDispToDev1PlugOut());
        } else if(true == curPlugState && 0 == priority) {
            // hwc will exchange on calling mDMP.setDisplayOutput(DISPLAY_EXTERNAL, d0Format),
            // than the device with this format will be on mainDisp.
            // if it return success(0), mean DualDisplayOutput,
            // otherwise only mainDisp exist, the externalDisp was be closed or displays on other way.
            if(1 == mDispPolicy) {
               mDMP.setDisplayOutput(DISPLAY_PRIMARY, d0Format);
               mDMP.setOutputState(mDMP.getMainDispToDev0PlugInExt());
            } else {
                if(0 == mDMP.setDisplayOutput(DISPLAY_EXTERNAL, d0Format)) {
                    mDMP.setOutputState(mDMP.getDualDisplayOutput());
                } else {
                    mDMP.setOutputState(mDMP.getMainDispToDev0PlugInExt());
                }
            }
        }
    }
}

