package android.hardware.display.outputstate;
import android.hardware.display.DisplayManagerPolicy2;
import android.hardware.display.outputstate.DispOutputState;

public class MainDispToDev0PlugInExt implements DispOutputState {
    DisplayManagerPolicy2 mDMP;

    public MainDispToDev0PlugInExt(DisplayManagerPolicy2 displayManagerPolicy) {
        this.mDMP = displayManagerPolicy;
    }

    public void devicePlugChanged(int d1Format, int priority, boolean curPlugState) {
        if(false == curPlugState) {
            if(0 == priority) {
                if(0 == mDMP.setDisplayOutput(DISPLAY_PRIMARY, d1Format)) {
                    mDMP.setOutputState(mDMP.getMainDispToDev1PlugIn());
                } else {
                    // the device with d1Format will not to display on mainDisp since it display on other way.
                    mDMP.setOutputState(mDMP.getMainDispToDev0PlugOut());
                }
            } else if(1 == priority) {
                mDMP.setDisplayOutput(DISPLAY_EXTERNAL, 0);
                mDMP.setOutputState(mDMP.getMainDispToDev0PlugIn());
            }
        }
    }
}
