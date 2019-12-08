package android.hardware.display.outputstate;
import android.hardware.display.DisplayManagerPolicy2;
import android.hardware.display.outputstate.DispOutputState;

public class MainDispToDev1PlugOut implements DispOutputState {
    DisplayManagerPolicy2 mDMP;

    public MainDispToDev1PlugOut(DisplayManagerPolicy2 displayManagerPolicy) {
        this.mDMP = displayManagerPolicy;
    }

    public void devicePlugChanged(int format,int priority,boolean curPlugState) {
        if(true == curPlugState) {
            if(0 == priority) {
                mDMP.setDisplayOutput(DISPLAY_PRIMARY, format);
                mDMP.setOutputState(mDMP.getMainDispToDev0PlugIn());
            } else if(1 == priority) {
                mDMP.setOutputState(mDMP.getMainDispToDev1PlugIn());
            }
        }
    }
}
