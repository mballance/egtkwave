package net.sf.egtkwave.ui.mgr;

import java.net.Socket;

import net.sf.egtkwave.ui.jni.GtkWaveJni;

public class GtkWaveMgr extends AbstractGtkWaveMgr implements IGtkWaveMgr {
	private long			fPid;
	
	public GtkWaveMgr(long pid, Socket sock) {
		super(sock);
		fPid = pid;
	}

	@Override
	public void dispose() {
		// TODO: fix once we have a command link with GTKWave
		GtkWaveJni.terminate(fPid);
	}
	
}
