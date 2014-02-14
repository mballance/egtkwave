package net.sf.egtkwave.ui.mgr;

import java.util.ArrayList;
import java.util.List;

public class TclStringUtils {
	

	public static List<Object> splitList(String list) {
		return splitList(list, 0, list.length());
	}
	
	public static List<Object> splitList(String list, int offset, int length) {
		int idx = offset;
		StringBuilder sb = new StringBuilder();
		List<Object> ret = new ArrayList<Object>();
		
		System.out.println("splitList: " + list.subSequence(offset, offset+length));

		while (idx < (offset+length)) {
			while (idx < (offset+length) && 
					Character.isWhitespace(list.charAt(idx))) {
				idx++;
			}
			
			if (idx >= (offset+length)) {
				break;
			}
			
			if (list.charAt(idx) == '{') {
				int level = 1;
				int end = idx+1;
				// List element
				
				// scan forward to the closing brace
				while (end < (offset+length) && level != 0) {
					if (list.charAt(end) == '{') {
						level++;
					} else if (list.charAt(end) == '}') {
						level--;
					}
					end++;
				}
				List<Object> spl = splitList(list, idx+1, (end-idx-2));
				
				if (spl.size() > 1) {
					ret.add(spl);
				} else {
					if (spl.get(0) instanceof String) {
						String str = (String)spl.get(0);
						
						if (str.indexOf('[') != -1) {
							ret.add(str);
						} else {
							ret.add(spl);
						}
					} else {
						ret.add(spl);
					}
				}
				
				idx = end+1;
			} else if (list.charAt(idx) == '"') {
				// String element
				int last_ch = -1, ch;
				sb.setLength(0);
				
				while (idx < (offset+length)) {
					sb.append((ch = list.charAt(idx)));
					idx++;
					if (ch == '"' && last_ch != '\\') {
						break;
					}
					last_ch = ch;
				}
			} else {
				// Whitespace-delimited token
				sb.setLength(0);
				
				while (idx < (offset+length) && 
						!Character.isWhitespace(list.charAt(idx))) {
					sb.append(list.charAt(idx));
					idx++;
				}
				
				ret.add(sb.toString());
			}
		}
	
		return ret;
	}

}
