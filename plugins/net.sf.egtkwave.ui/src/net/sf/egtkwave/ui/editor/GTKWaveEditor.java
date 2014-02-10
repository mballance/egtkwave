package net.sf.egtkwave.ui.editor;

import net.sf.egtkwave.ui.EGtkWavePlugin;
import net.sf.egtkwave.ui.editor.outline.GTKWaveOutlinePage;
import net.sf.egtkwave.ui.mgr.IGtkWaveMgr;
import net.sf.egtkwave.ui.mgr.IGtkWaveMgrFactory;

import org.eclipse.core.runtime.IProgressMonitor;
import org.eclipse.swt.SWT;
import org.eclipse.swt.layout.GridData;
import org.eclipse.swt.layout.GridLayout;
import org.eclipse.swt.widgets.Composite;
import org.eclipse.swt.widgets.Display;
import org.eclipse.ui.IEditorInput;
import org.eclipse.ui.IEditorSite;
import org.eclipse.ui.PartInitException;
import org.eclipse.ui.part.EditorPart;
import org.eclipse.ui.views.contentoutline.IContentOutlinePage;

public class GTKWaveEditor extends EditorPart {
	private IGtkWaveMgr				fWaveMgr;
	private GTKWaveOutlinePage		fOutline;

	public GTKWaveEditor() {
		// TODO Auto-generated constructor stub
	}

	public IGtkWaveMgr getWaveMgr() {
		return fWaveMgr;
	}
	
	@Override
	public void doSave(IProgressMonitor monitor) {
		// TODO Auto-generated method stub

	}

	@Override
	public void doSaveAs() {
		// TODO Auto-generated method stub

	}
	
	@Override
	public void dispose() {
		super.dispose();
		
		if (fWaveMgr != null) {
			fWaveMgr.dispose();
		}
	}

	@Override
	public void init(IEditorSite site, IEditorInput input)
			throws PartInitException {
		setSite(site);
		setInput(input);
	}

	@Override
	public boolean isDirty() {
		// TODO Auto-generated method stub
		return false;
	}

	@Override
	public boolean isSaveAsAllowed() {
		// TODO Auto-generated method stub
		return false;
	}
	
	private class Creator implements Runnable {
		private Composite		fParent;
		
		
		
		public Creator(Composite parent) {
			fParent = parent;
		}
		
		public void run() {
			IGtkWaveMgrFactory f = EGtkWavePlugin.getDefault().getGtkWaveMgrFactory();
		
			fWaveMgr = f.create(fParent);
			
			if (fOutline != null) {
				fOutline.refresh();
			}

			/*
			try {
			Thread.sleep(2000);
			} catch (Exception e) {}
			 */
			Display.getDefault().asyncExec(new Runnable() {
				
				@Override
				public void run() {
					Composite p = fParent.getParent();
					fParent.setSize(p.getSize());
					/*
					fParent.layout();
					fParent.redraw();
					fParent.getParent().layout();
					fParent.getParent().redraw();
					 */
				}
			});

			/*
			try {
				String roots = fWaveMgr.command("gtkwave::getTreeNodeRoots");
				String roots_l[] = roots.split(" ");
				
				for (String root : roots_l) {
					root = root.trim();
					
					System.out.println("root=" + root + ";");
					
					String children =  fWaveMgr.command("gtkwave::getTreeNodeChildren " + root);
					System.out.println("children=" + children);
				}
			} catch (IOException e) {
				e.printStackTrace();
			}
			 */
		}
	}

	@Override
	public void createPartControl(Composite parent) {
		Composite ef = new Composite(parent, SWT.EMBEDDED);
		ef.setLayout(new GridLayout());
		ef.setLayoutData(new GridData(SWT.FILL, SWT.FILL, true, true));
		
		Creator c = new Creator(ef);
//		Creator c = new Creator(parent);
		Display.getCurrent().asyncExec(c);
		
		
	
//		ef.setSize(100, 100);
	}

	@Override
	public void setFocus() {
		System.out.println("setFocus: " + Thread.currentThread());
		// TODO Auto-generated method stub

	}

	@Override
	@SuppressWarnings("rawtypes")
	public Object getAdapter(Class adapter) {
		System.out.println("getAdapter: " + adapter.getClass().getName());
		if (adapter.equals(IContentOutlinePage.class)) {
			if (fOutline == null) {
				fOutline = new GTKWaveOutlinePage(this);
			}
			return fOutline;
		}
		return super.getAdapter(adapter);
	}
}
