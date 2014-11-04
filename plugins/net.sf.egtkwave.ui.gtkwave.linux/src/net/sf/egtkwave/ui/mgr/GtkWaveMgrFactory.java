package net.sf.egtkwave.ui.mgr;

import java.io.File;
import java.io.IOException;
import java.net.InetAddress;
import java.net.InetSocketAddress;
import java.net.ServerSocket;
import java.net.Socket;

import net.sf.egtkwave.ui.EGtkWavePlugin;

import org.eclipse.swt.widgets.Composite;
import org.osgi.framework.Bundle;

public class GtkWaveMgrFactory implements IGtkWaveMgrFactory {

	@Override
	public IGtkWaveMgr create(Composite parent) {
		Bundle bundle = EGtkWavePlugin.getDefault().getBundle();
		
		Bundle f_bundle = null;
		Bundle gtkw_bundle = null;
		for (Bundle b : bundle.getBundleContext().getBundles()) {
			if (b.getSymbolicName().equals("net.sf.egtkwave.ui.gtkwave.linux.x86_64")) {
				f_bundle = b;
			} else if (b.getSymbolicName().equals("net.sf.egtkwave.ui")) {
				gtkw_bundle = b;
			}
		}
	
		String f_bundle_loc = f_bundle.getLocation();
		System.out.println("f_bundle_loc=" + f_bundle_loc);
		if (f_bundle_loc.startsWith("reference:")) {
			f_bundle_loc = f_bundle_loc.substring("reference:".length());
		}
		if (f_bundle_loc.startsWith("file:")) {
			f_bundle_loc = f_bundle_loc.substring("file:".length());
		}
		System.out.println("f_bundle_loc=" + f_bundle_loc);
		
		File bundle_loc = new File(f_bundle_loc);
		File gtkwave_exe = new File(bundle_loc, "gtkwave/bin/gtkwave");
//		File gtkwave_exe = new File("c:/usr1/fun/egtkwave/gtkwave_mballance/gtkwave/gtkwave_win32/bin/gtkwave.exe");
		
		System.out.println("gtkwave_exe=" + gtkwave_exe.getAbsolutePath());

		String gtkw_bundle_loc = gtkw_bundle.getLocation();
		if (gtkw_bundle_loc.startsWith("reference:")) {
			gtkw_bundle_loc = gtkw_bundle_loc.substring("reference:".length());
		}
		if (gtkw_bundle_loc.startsWith("file:")) {
			gtkw_bundle_loc = gtkw_bundle_loc.substring("file:/".length());
		}
	
		int port = 0;
		try {
			InetSocketAddress addr = new InetSocketAddress(InetAddress.getLocalHost(), 0);
			ServerSocket sock = new ServerSocket();
			sock.bind(addr);
			port = sock.getLocalPort();
			sock.close();
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		System.out.println("create() " + port);
		System.out.println("embeddedHandle=" + Long.toHexString(parent.embeddedHandle));
		Process p = null;
		try {
			p = Runtime.getRuntime().exec(new String[] {
				gtkwave_exe.getAbsolutePath(),
				"--hidesst",
				"--nowm",
				"--nomenus",
				"--cmdserver",
				"" + port,
				"--xid=0x" + Long.toHexString(parent.embeddedHandle),
				"/scratch1/fun/egtkwave/gtkwave_mballance/gtkwave/tmp.vcd"
		});
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		// Now, try to connect to the server
		Socket sock = null;
		for (int i=0; i<100; i++) {
			try {
				sock = new Socket(InetAddress.getLoopbackAddress(), port);
				break;
			} catch (IOException e) {
				System.out.println("Exception: " + e.getMessage());
				sock = null;
				try {
					Thread.sleep(100);
				} catch (InterruptedException ex) {
					
				}
//				e.printStackTrace();
			}
		}
		
		System.out.println("Connected: " + sock);
		
		/*
		try {
		Process p = Runtime.getRuntime().exec(new String[] {
			gtkwave_exe.getAbsolutePath(),
			"-M",
			"-N"
			});
		RDR in_rdr = new RDR(p.getInputStream());
		RDR err_rdr = new RDR(p.getErrorStream());
		in_rdr.start();
		err_rdr.start();

		p.waitFor();
		
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		*/
		// TODO Auto-generated method stub
		return new GtkWaveMgr(p, sock);		
	}

}
