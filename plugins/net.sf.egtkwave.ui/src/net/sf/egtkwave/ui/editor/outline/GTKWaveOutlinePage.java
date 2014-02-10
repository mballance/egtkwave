package net.sf.egtkwave.ui.editor.outline;

import net.sf.egtkwave.ui.editor.GTKWaveEditor;

import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.ui.views.contentoutline.ContentOutlinePage;

public class GTKWaveOutlinePage extends ContentOutlinePage {
	private GTKWaveEditor			fEditor;
	
	public GTKWaveOutlinePage(GTKWaveEditor editor) {
		fEditor = editor;
	}
	
	public void refresh() {
		System.out.println("refresh");
		getTreeViewer().setInput(fEditor.getWaveMgr());
		getTreeViewer().refresh();
	}

	@Override
	public void createControl(Composite parent) {
		super.createControl(parent);
		
		TreeViewer tree_viewer = getTreeViewer();
		
		tree_viewer.setContentProvider(new GTKWaveTreeContentProvider());
		tree_viewer.setLabelProvider(new GTKWaveTreeLabelProvider(fEditor));

		System.out.println("setInput");
		tree_viewer.setInput(fEditor.getWaveMgr());
	}

	@Override
	public Control getControl() {
		// TODO Auto-generated method stub
		return super.getControl();
	}

	
	
}
