package net.sf.egtkwave.ui.mgr;

public class TreeNode {
	private TreeNode			fParent;
	private String				fId;
	private String				fName;
	private String				fKind;
	private boolean				fHasChildren;
	
	public TreeNode(
			TreeNode		parent,
			String			id,
			String			kind,
			String			name,
			boolean			has_children) {
		fParent = parent;
		fId = id;
		fName = name;
		fKind = kind;
		fHasChildren = has_children;
		
		System.out.println("hasChildren: " + has_children);
	}

	public void setParent(TreeNode n) {
		
	}
	public TreeNode getParent() {
		return fParent;
	}
	
	public String getId() {
		return fId;
	}
	
	public String getName() {
		return fName;
	}
	
	public boolean hasChildren() {
		return fHasChildren;
	}

}
