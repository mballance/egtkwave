package net.sf.egtkwave.ui.mgr;

import java.io.IOException;

public interface IGtkWaveMgr {
	
	void dispose();
	
	String command(String cmd) throws IOException;

}
