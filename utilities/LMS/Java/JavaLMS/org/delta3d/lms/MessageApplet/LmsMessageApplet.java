package org.delta3d.lms.MessageApplet;

import java.io.IOException;

import javax.swing.JApplet;
import netscape.javascript.JSObject;

@SuppressWarnings("serial")
public class LmsMessageApplet extends JApplet  implements IGetRunning
{
	private int _port;
	private LmsMessageAppletThread _messageThread = null;
	
    public void init()
    {
        String port = getParameter("port");
        if (port != null)
        {
        	_port = Integer.parseInt(port);
        }
        else
        {
        	_port = 4444; //default value
        }
    }
    
    public void start()
    {
    	_running = true;
    	
        _messageThread = new LmsMessageAppletThread(_port, this, JSObject.getWindow(this));
        _messageThread.start();
    }
    
    public void stop()
    {
    	_running = false;
    	
    	LmsServerSocket lmsServerSocket = _messageThread.getLmsServerSocket();
    	
    	if (lmsServerSocket != null)
    	{
    		try
    		{
        		if (lmsServerSocket.GetServerState() == LmsServerSocket.ServerState.CONNECTED)
        		{
        			lmsServerSocket.getSocket().close();
        		}

        		lmsServerSocket.getServerSocket().close();
    		}
    		catch (IOException e)
    		{
    			//ignore
    		}
    	}
    }

    //implementation of IGetRunning
    private Boolean _running = false;
    public Boolean getRunning()
    {
        return _running;
    }
}
