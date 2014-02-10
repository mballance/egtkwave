package net.sf.egtkwave.ui.jni;

public class GtkWaveJni {
	static {
		System.loadLibrary("egtkwave");
	}
	
	public static native long launch(
			long			parent_hwnd,
			String			gtkwave_path);
	
	public static native long terminate(
			long			pid);

}
