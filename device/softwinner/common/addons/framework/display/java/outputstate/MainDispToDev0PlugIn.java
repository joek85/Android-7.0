package android.hardware.display.outputstate;
import android.hardware.display.DisplayManagerPolicy2;
import android.hardware.display.outputstate.DispOutputState;
import android.os.SystemProperties;

public class MainDispToDev0PlugIn implements DispOutputState {
    DisplayManagerPolicy2 mDMP;
	int mDispPolicy;

    public MainDispToDev0PlugIn(DisplayManagerPolicy2 displayManagerPolicy) {
        this.mDMP = displayManagerPolicy;
        mDispPolicy = Integer.valueOf(SystemProperties.get("persist.sys.disp_policy", "0"), 16);
    }

    public void devicePlugChanged(int format, int priority, boolean curPlugState) {
        if(false == curPlugState && 0 == priority) {
            mDMP.setOutputState(mDMP.getMainDispToDev0PlugOut());
        } else if(1 == priority && true == curPlugState) {
            if(1 == mDispPolicy) {
                mDMP.setOutputState(mDMP.getMainDispToDev0PlugInExt());
            } else {
                if(0 == mDMP.setDisplayOutput(DISPLAY_EXTERNAL, format)) {
                    mDMP.setOutputState(mDMP.getDualDisplayOutput());
                } else {
                    mDMP.setOutputState(mDMP.getMainDispToDev0PlugInExt());
                }
            }
        }
    }
}
