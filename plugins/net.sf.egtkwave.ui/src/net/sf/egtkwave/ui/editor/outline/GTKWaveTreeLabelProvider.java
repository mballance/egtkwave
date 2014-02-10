package net.sf.egtkwave.ui.editor.outline;

import java.io.IOException;

import net.sf.egtkwave.ui.editor.GTKWaveEditor;
import net.sf.egtkwave.ui.mgr.IGtkWaveMgr;

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
		IGtkWaveMgr mgr = fEditor.getWaveMgr();
	
		if (element instanceof String) {
			String ret = "";
			try {
				ret = mgr.command("gtkwave::getTreeNodeName " + element.toString());
				ret = ret.trim();
			} catch (IOException e) {
				
			}
			return ret;
//			return element.toString();
		} else {
			return super.getText(element);
		}
	}
	

}
