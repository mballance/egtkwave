package net.sf.egtkwave.ui.editor.outline;

import java.util.List;

import net.sf.egtkwave.ui.mgr.IGtkWaveMgr;
import net.sf.egtkwave.ui.mgr.TreeNode;

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
		if (fInput != null) {
			List<TreeNode> roots = fInput.getTreeNodeRoots();
			return roots.toArray();
		} else {
			return new Object[0];
		}
	}

	@Override
	public Object[] getChildren(Object parent) {
		if (parent instanceof TreeNode) {
			List<TreeNode> children = fInput.getTreeNodeChildren((TreeNode)parent);
			return children.toArray();
		} else {
			return new Object[0];
		}
	}

	@Override
	public Object getParent(Object element) {
		if (element instanceof TreeNode) {
			return ((TreeNode)element).getParent();
		}

		return null;
	}

	@Override
	public boolean hasChildren(Object element) {
		if (element instanceof TreeNode) {
			return ((TreeNode)element).hasChildren();
		} 
		return false;
	}

}
