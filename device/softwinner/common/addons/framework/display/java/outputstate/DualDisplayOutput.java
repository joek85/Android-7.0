package android.hardware.display.outputstate;
import android.hardware.display.DisplayManagerPolicy2;
import android.hardware.display.outputstate.DispOutputState;

public class DualDisplayOutput implements DispOutputState {
    DisplayManagerPolicy2 mDMP;

    public DualDisplayOutput(DisplayManagerPolicy2 displayManagerPolicy) {
    this.mDMP = displayManagerPolicy;
    }

    public void devicePlugChanged(int d1Format,int priority,boolean curPlugState) {
        if(false == curPlugState) {
            if(0 == priority) {
                mDMP.setDisplayOutput(DISPLAY_PRIMARY, d1Format);
                mDMP.setDisplayOutput(DISPLAY_EXTERNAL, 0);
                mDMP.setOutputState(mDMP.getMainDispToDev1PlugIn());
            } else if(1 == priority) {
                mDMP.setDisplayOutput(DISPLAY_EXTERNAL, 0);
                mDMP.setOutputState(mDMP.getMainDispToDev0PlugIn());
            }
        }
    }
}
