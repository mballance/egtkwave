package net.sf.egtkwave.ui.editor.outline;

import java.io.IOException;

import net.sf.egtkwave.ui.editor.GTKWaveEditor;
import net.sf.egtkwave.ui.mgr.IGtkWaveMgr;
import net.sf.egtkwave.ui.mgr.TreeNode;

import org.eclipse.jface.viewers.LabelProvider;
import org.eclipse.swt.graphics.Image;

public class GTKWaveTreeLabelProvider extends LabelProvider {
	private GTKWaveEditor			fEditor;
	
	public GTKWaveTreeLabelProvider(GTKWaveEditor editor) {
		fEditor = editor;
	}
	
	@Override
	public Image getImage(Object element) {
		// TODO Auto-generated method stub
		return super.getImage(element);
	}

	@Override
	public String getText(Object element) {
		if (element instanceof TreeNode) {
			TreeNode n = (TreeNode)element;
			
			return n.getName();
		} else {
			return super.getText(element);
		}
	}
	

}
