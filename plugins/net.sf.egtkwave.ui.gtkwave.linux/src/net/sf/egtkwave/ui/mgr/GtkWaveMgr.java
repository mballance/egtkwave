package net.sf.egtkwave.ui.mgr;

import java.io.IOException;
import java.net.Socket;
import java.util.List;

import org.eclipse.core.runtime.IProgressMonitor;

public class GtkWaveMgr extends AbstractGtkWaveMgr {
	private long				fPid;

	public GtkWaveMgr(Process pid, Socket sock) {
		super(sock);
//		fPid = pid;
	}
	
	public void dispose() {
		System.out.println("TODO: kill ");
	}

}
