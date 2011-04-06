package org.delta3d.lms.MessageApplet;

import java.io.*;
import java.util.*;

//This class can be used as a java application (instead of an applet) so that you
//can more easily test and debug the code. It's main purpose is to mimick the
//functionality of the MessageSocketServer class, however in the form of an application.

public class TestMessageSocket implements IGetRunning
{
	private int _port = 4444;
	
	public static void main(String[] args)
	{
		TestMessageSocket test = new TestMessageSocket();
		test.RunThreaded();
		
		
	}
	
	public void RunThreaded()
	{
        _running = true;
        
        LmsMessageAppletThread messageThread = new LmsMessageAppletThread(_port, this, null);
        messageThread.start();
        
        //code to allow manual exit from console (replaces stop() method in applet)
        ManualExit(messageThread);
	}
	
	private void ManualExit(LmsMessageAppletThread messageThread)
	{
	    //code to allow manual exit from console
        Scanner in = new Scanner(System.in);
        System.out.println("Type 'exit' to quit:");
        String exit = in.next();
        
        if (exit.equalsIgnoreCase("exit"))
        {
        	_running = false;
        	
        	LmsServerSocket lmsServerSocket = messageThread.getLmsServerSocket();
        	
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
	}
	
    //implementation of IGetRunning
    private Boolean _running = false;
    public Boolean getRunning()
    {
        return _running;
    }
}
