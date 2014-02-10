package net.sf.egtkwave.ui;

import net.sf.egtkwave.ui.mgr.IGtkWaveMgrFactory;

import org.eclipse.core.runtime.IConfigurationElement;
import org.eclipse.core.runtime.IExtension;
import org.eclipse.core.runtime.IExtensionPoint;
import org.eclipse.core.runtime.IExtensionRegistry;
import org.eclipse.core.runtime.Platform;
import org.eclipse.ui.plugin.AbstractUIPlugin;
import org.osgi.framework.BundleContext;

/**
 * The activator class controls the plug-in life cycle
 */
public class EGtkWavePlugin extends AbstractUIPlugin {

	// The plug-in ID
	public static final String PLUGIN_ID = "net.sf.egtkwave.ui"; //$NON-NLS-1$

	// The shared instance
	private static EGtkWavePlugin plugin;
	
	/**
	 * The constructor
	 */
	public EGtkWavePlugin() {
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#start(org.osgi.framework.BundleContext)
	 */
	public void start(BundleContext context) throws Exception {
		super.start(context);
		plugin = this;
	}

	/*
	 * (non-Javadoc)
	 * @see org.eclipse.ui.plugin.AbstractUIPlugin#stop(org.osgi.framework.BundleContext)
	 */
	public void stop(BundleContext context) throws Exception {
		plugin = null;
		super.stop(context);
	}
	
	public IGtkWaveMgrFactory getGtkWaveMgrFactory() {
		IExtensionRegistry rgy = Platform.getExtensionRegistry();
		IExtensionPoint ep = rgy.getExtensionPoint(PLUGIN_ID, "gtkWaveMgrFactory");
		IExtension e_l[] = ep.getExtensions();
		
		IGtkWaveMgrFactory ret = null;
		
		for (IConfigurationElement ce : e_l[0].getConfigurationElements()) {
			try {
				ret = (IGtkWaveMgrFactory)ce.createExecutableExtension("class");
			} catch (Exception e) {
				e.printStackTrace();
			}
		}
		
		return ret;
	}

	/**
	 * Returns the shared instance
	 *
	 * @return the shared instance
	 */
	public static EGtkWavePlugin getDefault() {
		return plugin;
	}

}
