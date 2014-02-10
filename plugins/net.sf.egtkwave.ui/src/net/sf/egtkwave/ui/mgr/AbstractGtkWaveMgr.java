package net.sf.egtkwave.ui.mgr;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;

public abstract class AbstractGtkWaveMgr implements IGtkWaveMgr {
	protected Socket				fSocket;
	
	protected AbstractGtkWaveMgr(Socket sock) {
		fSocket = sock;
	}
	
	public synchronized String command(String cmd) throws IOException {
		String ret = null;
		int cmd_len = cmd.length()+1;
		byte data[] = new byte[cmd.length()+1+8];
		byte resp_data[] = new byte[0];
		byte tmp[] = new byte[1024];
		int sz;
		
		OutputStream out = fSocket.getOutputStream();
		InputStream in = fSocket.getInputStream();
		
		data[4] = (byte)(cmd_len >> 0);
		data[5] = (byte)(cmd_len >> 8);
		data[6] = (byte)(cmd_len >> 16);
		data[7] = (byte)(cmd_len >> 24);
		
		byte cmd_bytes[] = cmd.getBytes();
	
		for (int i=0; i<cmd_bytes.length; i++) {
			data[8+i] = cmd_bytes[i];
		}
		data[8+cmd_bytes.length] = 0;

		System.out.println("--> write");
		out.write(data);
		System.out.println("<-- write");
		out.flush();
	
		do {
			sz = in.read(tmp, 0, tmp.length);
		
			byte r_tmp[] = resp_data;
			
			resp_data = new byte[r_tmp.length + sz];
			
			for (int i=0; i<r_tmp.length; i++) {
				resp_data[i] = r_tmp[i];
			}
			for (int i=0; i<sz; i++) {
				resp_data[i+r_tmp.length] = tmp[i];
			}
			
			if (resp_data.length >= 9) {
				int len, status;
				
				len = ((int)resp_data[4] << 0) |
					  ((int)resp_data[5] << 8) |
					  ((int)resp_data[6] << 16) |
					  ((int)resp_data[7] << 24);
				
				status = resp_data[8];
				
				System.out.println("len=" + len + " resp_data.length=" + resp_data.length);
				if (resp_data.length >= (4+len)) {
					System.out.println("Complete");
					break;
				}
			}
			
		} while (true);
		
		ret = new String(resp_data, 5, resp_data.length-5);

		System.out.println("Read back sz=" + sz + " ret=" + ret);
	
		return ret;
	}

}
