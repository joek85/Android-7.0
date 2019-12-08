package android.hardware.display.outputstate;

public interface DispOutputState {
    public final int DISPLAY_PRIMARY = 0x1;
    public final int DISPLAY_EXTERNAL = 0x2;
    public void devicePlugChanged(int format, int priority, boolean curPlugState);
}
