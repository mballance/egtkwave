package net.sf.egtkwave.ui.editor.outline;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import net.sf.egtkwave.ui.mgr.IGtkWaveMgr;

import org.eclipse.jface.viewers.ITreeContentProvider;
import org.eclipse.jface.viewers.Viewer;

public class GTKWaveTreeContentProvider implements ITreeContentProvider {
	private IGtkWaveMgr				fInput; 
	
	@Override
	public void dispose() {
		// TODO Auto-generated method stub
	}
	
	@Override
	public void inputChanged(Viewer viewer, Object oldInput, Object newInput) {
		System.out.println("inputChanged: " + newInput);
		if (newInput != null) {
			fInput = (IGtkWaveMgr)newInput;
		}
	}

	@Override
	public Object[] getElements(Object inputElement) {
		String roots = "";
		try {
			roots = fInput.command("gtkwave::getTreeNodeRoots");
		} catch (IOException e) { }
		
		String roots_l[] = roots.split(" ");
		
		for (int i=0; i<roots_l.length; i++) {
			roots_l[i] = roots_l[i].trim();
		}
		
		System.out.println("roots_l.length=" + roots_l.length);
		
		return roots_l;
	}

	@Override
	public Object[] getChildren(Object parentElement) {
		if (parentElement instanceof String) {
			List<String> ret = new ArrayList<String>();
			String roots = "";
			try {
				roots = fInput.command("gtkwave::getTreeNodeChildren " + parentElement.toString());
			} catch (IOException e) { }
			
			String roots_l[] = roots.split(" ");
		
			for (int i=0; i<roots_l.length; i++) {
				String c = roots_l[i].trim();
				if (!c.equals("")) {
					ret.add(c);
				}
			}
			
			System.out.println("children roots_l.length=" + roots_l.length);
		
			return ret.toArray(new String[ret.size()]);
		} else {
			return new Object[0];
		}
	}

	@Override
	public Object getParent(Object element) {
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public boolean hasChildren(Object element) {
		return (getChildren(element).length > 0);
	}

}
