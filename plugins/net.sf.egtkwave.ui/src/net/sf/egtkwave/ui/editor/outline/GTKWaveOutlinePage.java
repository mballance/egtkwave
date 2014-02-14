package net.sf.egtkwave.ui.editor.outline;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;

import net.sf.egtkwave.ui.editor.GTKWaveEditor;
import net.sf.egtkwave.ui.mgr.TreeNode;

import org.eclipse.jface.action.Action;
import org.eclipse.jface.action.IMenuListener;
import org.eclipse.jface.action.IMenuManager;
import org.eclipse.jface.action.MenuManager;
import org.eclipse.jface.viewers.ISelection;
import org.eclipse.jface.viewers.IStructuredSelection;
import org.eclipse.jface.viewers.TreeViewer;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Control;
import org.eclipse.swt.widgets.Menu;
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
		
		// Setup menus
		MenuManager menuMgr = new MenuManager();
	    menuMgr.setRemoveAllWhenShown(true);
	    menuMgr.addMenuListener(new IMenuListener() {
	        public void menuAboutToShow(IMenuManager manager) {
	            GTKWaveOutlinePage.this.fillContextMenu(manager);
	        }
	    });
	    Menu menu = menuMgr.createContextMenu(tree_viewer.getControl());
	    tree_viewer.getControl().setMenu(menu);
//	    getSite().registerContextMenu(menuMgr, tree_viewer);		
	}
	
	private void fillContextMenu(IMenuManager menu_mgr) {
		menu_mgr.add(new Action("Foo") {

			@Override
			public void run() {
				ISelection sel = getTreeViewer().getSelection();
				IStructuredSelection ssel = (IStructuredSelection)sel;
				Iterator it = ssel.iterator();
				List<TreeNode> add_l = new ArrayList<TreeNode>();
				
				while (it.hasNext()) {
					Object o = it.next();
					
					if (o instanceof TreeNode) {
						System.out.println("Add " + ((TreeNode)o).getName());
						add_l.add((TreeNode)o);
					}
				}
				
				fEditor.getWaveMgr().addSignals(add_l);
				
				// TODO Auto-generated method stub
				super.run();
			}
		});
		
	}

	@Override
	public Control getControl() {
		// TODO Auto-generated method stub
		return super.getControl();
	}

	
	
}
