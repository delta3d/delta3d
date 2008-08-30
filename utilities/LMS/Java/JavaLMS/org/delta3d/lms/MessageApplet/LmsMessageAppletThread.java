package org.delta3d.lms.MessageApplet;

import java.net.*;
import netscape.javascript.*;

public class LmsMessageAppletThread extends Thread
{
	private IGetRunning _server;
	private JSObject _win = null;
	private int _port = 4444;
	
	private LmsServerSocket _lmsServer = null;
	public LmsServerSocket getLmsServerSocket() {return _lmsServer;}

	LmsMessageAppletThread(int port, IGetRunning server, JSObject win)
	{
		_port = port;
		_server = server;
        _win = win;
	}
	
	public void run()
	{
		Boolean serverExitNormally = true;
		
		//loop while server applet is still running (browser page still open)
		while (_server.getRunning() == true)
		{
			//initialize server socket
			try
			{
				if (_lmsServer == null)
				{
					_lmsServer = new LmsServerSocket(_port);
				}
			}
			catch (LmsConnectionException e)
			{
				System.out.println("Server failed to initialize on port: " + _port);
				serverExitNormally = false;
				break;
			}
			
			//wait for incoming connection
			try
			{
				LmsMessage handshakeMessage = _lmsServer.Connect();
				SendMessageToWebPageScript(handshakeMessage);
				
			}
			catch (SocketException e)
			{
				//launch page applet closed the socket before a client connected
				serverExitNormally = true;
				break;
			}
			catch (LmsConnectionException e)
			{
				System.out.println(e.getMessage());
				serverExitNormally = false;
				break;
			}
			catch (InvalidLmsMessageException e)
			{
				System.out.println(e.getMessage());
				serverExitNormally = false;
				break;
			}
			
			//MAIN LOOP: receive lms messages while connected to client
			Boolean clientExitNormally = ExecuteMessageLoop();
			
			//notify LmsServerSocket that client has disconnected
			_lmsServer.Disconnect(clientExitNormally);

			//close the socket once disconnected
			try
			{
				_lmsServer.CloseSocket();
			}
			catch (LmsConnectionException e)
			{
				System.out.println(e.getMessage());
				serverExitNormally = false;
				break;
			}
		} //END WHILE
		
		String serverNormal = "";
		if (serverExitNormally)
		{
			serverNormal = "normally";
		}
		else
		{
			serverNormal = "abnormally";
		}
		
		System.out.println("Server thread exited " + serverNormal);
	}
	
	private Boolean ExecuteMessageLoop()
	{
		Boolean exitNormally = true;
		
		//receive lms messages while connected to client (launch page may have stopped while waiting for client connection)
		while (_lmsServer.GetServerState() == LmsServerSocket.ServerState.CONNECTED)
		{
			//wait for next message to be sent
			LmsMessage lmsMessage;
			try
			{
				lmsMessage = _lmsServer.ReceiveLmsMessage();
			}
			catch (SocketException e)
			{
				//launch page applet closed the socket
				exitNormally = true;
				break;
			}
			catch (LmsConnectionException e)
			{
				System.out.println(e.getMessage());
				SendMessageToWebPageScript("ERROR:LmsMessageAppletThread.ExecuteMessageLoop:" + e.getMessage());
				exitNormally = false;
				break;
			}
			
			//if the message is null, then exit (shouldn't ever happen, but just in case)
			if (lmsMessage == null)
			{
				SendMessageToWebPageScript("ERROR:LmsMessageAppletThread.ExecuteMessageLoop:lmsMessage is null");
				exitNormally = false;
				break;
			}
			
			//did the simulation tell us that is stopping?
			if (lmsMessage.GetType() == MessageType.SIMULATION &&
				lmsMessage.GetValue() != MessageValue.Simulation.RUNNING.toString())
			{
				//simulation stopped
				SendMessageToWebPageScript(lmsMessage);
				exitNormally = true;
				break;
			}
			
			//this message should be an objective message
			if (lmsMessage.GetType() == MessageType.OBJECTIVE_COMPLETION ||
				lmsMessage.GetType() == MessageType.OBJECTIVE_SCORE)
			{
			    //call javascript function on html page to pass it the message
				SendMessageToWebPageScript(lmsMessage);
			}
			else
			{
				//if this is not a proper objective message, then we will just ignore it
				System.out.println("LmsServerSocket expected an Objective-related message: " + lmsMessage.toString());
				
				//we can optionally do this:
				//throw new InvalidLmsMessageException("LmsServerSocket expected an Objective-related message: " + lmsMessage.toString());
			}		
		}
		
		return exitNormally;
	}
	
	private void SendMessageToWebPageScript(LmsMessage lmsMessage)
	{
		SendMessageToWebPageScript(lmsMessage.toString());
	}
	
	private void SendMessageToWebPageScript(String message)
	{
        if (_win != null)
        {
            Object[] obj = {message};
            _win.call("DisplayMessage", obj);
        }
	}
}