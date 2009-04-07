var javawsInstalled = 0;
var javaws142Installed = 0;
var javaws150Installed = 0;
isIE = "false";
JREVersion = "Not Installed";

if (navigator.mimeTypes && navigator.mimeTypes.length)
{
	jnlpMimeType = navigator.mimeTypes['application/x-java-jnlp-file'];
	if (jnlpMimeType)
	{
		javawsInstalled = 1;
		JREVersion = "Pre 1.4.2";
		
		java142Mime = navigator.mimeTypes['application/x-java-applet;version=1.4.2'];
		if (java142Mime)
		{
			javaws142Installed = 1;
			JREVersion = "1.4.2";
		}
		
		java150Mime = navigator.mimeTypes['application/x-java-applet;version=1.5'];
		if (java150Mime)
		{
			javaws150Installed = 1;
			JREVersion = "1.5";
		}
		
		if (navigator.plugins)
		{
			navigator.plugins.refresh(false);
		}
	}
}
else
{
	isIE = "true";
}