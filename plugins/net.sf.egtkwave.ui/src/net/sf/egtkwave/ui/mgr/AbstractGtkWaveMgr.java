package net.sf.egtkwave.ui.mgr;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.Socket;
import java.util.ArrayList;
import java.util.List;

import org.eclipse.core.runtime.IProgressMonitor;

public abstract class AbstractGtkWaveMgr implements IGtkWaveMgr {
	protected Socket				fSocket;
	
	protected AbstractGtkWaveMgr(Socket sock) {
		fSocket = sock;
	}

	@SuppressWarnings({"rawtypes","unchecked"})
	protected List<TreeNode> str2TreeNodeList(TreeNode parent, String node_list) {
		List<TreeNode> ret = new ArrayList<TreeNode>();
		List<Object> obj_l = TclStringUtils.splitList(node_list);
		System.out.println("node_list=" + node_list + " " + obj_l.size());
		
		for (Object node_o : obj_l) {
			if (node_o instanceof List) {
				List<String> node_info = (List)node_o;
				System.out.println("node_info[3]=\"" + 
						node_info.get(3) + "\"");
				TreeNode node = new TreeNode(
						parent,
						node_info.get(0), // id
						node_info.get(1), // name
						node_info.get(2), // kind
						node_info.get(3).equals("1")  // has_children
						);
				ret.add(node);
			} else {
				// Error:
			}
		}
		
		return ret;
	}
	
	public List<TreeNode> getTreeNodeRoots() {
		String roots_s = null;
		
		try {
			roots_s = command("gtkwave::getTreeNodeRoots");
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		System.out.println("Roots: " + roots_s);
	
		return str2TreeNodeList(null, roots_s);
	}
	
	public List<TreeNode> getTreeNodeChildren(TreeNode node) {
		String children_s = null;
		
		try {
			children_s = command("gtkwave::getTreeNodeChildren " + node.getId());
		} catch (IOException e) {
			e.printStackTrace();
		}
	
		return str2TreeNodeList(node, children_s);
	}
	
	public void addSignals(List<TreeNode> nodes) {
		StringBuilder add_list = new StringBuilder();
		add_list.append("gtkwave::addSignalsFromList {");
		for (TreeNode n : nodes) {
			String fullname = null;
			try {
				fullname = command("gtkwave::getTreeNodeFullName " + n.getId());
			} catch (IOException e) {}
			
			fullname = fullname.replace("[", "\\[");
			fullname = fullname.replace("]", "\\]");
			
			System.out.println("fullname=\"" + fullname + "\" " +
					(int)fullname.charAt(fullname.length()-1));
			
			add_list.append(fullname);
			add_list.append(" ");
		}
		add_list.append("}");
		
		System.out.println("add_list=" + add_list.toString());
		
		if (add_list.length() > "gtkwave::addSignalsFromList {}".length()) {
			try {
				command(add_list.toString());
			} catch (IOException e) {}
		}
	}
	
	public void loadFile(IProgressMonitor monitor, String path) {
		String result = null;
		try {
			result = command("gtkwave::loadFile " + path);
		} catch (IOException e) {
			e.printStackTrace();
		}
		System.out.println("Result: " + result);
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
				
				len = (((int)resp_data[4] & 0xFF) << 0) |
					  (((int)resp_data[5] & 0xFF) << 8) |
					  (((int)resp_data[6] & 0xFF) << 16) |
					  (((int)resp_data[7] & 0xFF) << 24);
				
				status = resp_data[8];
				
				System.out.println("GtkWaveMgr: len=" + len + " resp_data.length=" + resp_data.length);
				if (resp_data.length >= (4+len)) {
					System.out.println("Complete");
					break;
				}
			}
			
		} while (true);
	
		// 
		ret = new String(resp_data, 9, resp_data.length-10);

		System.out.println("Read back sz=" + sz + " ret=" + ret);
	
		return ret;
	}

}
