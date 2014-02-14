package net.sf.egtkwave.ui.mgr;

import java.io.IOException;
import java.util.List;

import org.eclipse.core.runtime.IProgressMonitor;

public interface IGtkWaveMgr {
	
	void dispose();
	
	
	List<TreeNode> getTreeNodeRoots();
	
	List<TreeNode> getTreeNodeChildren(TreeNode node);
	
	void addSignals(List<TreeNode> nodes);
	
	void loadFile(IProgressMonitor monitor, String path);

	String command(String cmd) throws IOException;

}
