package org.delta3d.lms.MessageApplet;

import java.io.*;
import java.net.*;
import java.util.Random;

public class LmsServerSocket
{
	public enum ServerState
	{
		DISCONNECTED,
		HANDSHAKING,
		CONNECTED,
		ERROR
	}
	
	static int BUFFER_SIZE = 128000;
	
	private int _port;
	private ServerSocket _server = null;
	public ServerSocket getServerSocket() {return _server;}
	
	private Socket _socket = null;
	public Socket getSocket() {return _socket;}
	
	private String _serverID = "";
	public String getServerID() {return _serverID;}
	
	@SuppressWarnings("unused")
	private String _clientID = "";
	
	private char _endOfMessageChar = '\n';
	public char GetEndOfMessageChar() {return _endOfMessageChar;}
	public void SetEndOfMessageChar(char endOfMessageChar) {_endOfMessageChar = endOfMessageChar;}

	private ServerState _serverState = ServerState.DISCONNECTED;
	public ServerState GetServerState() {return _serverState;}
	public void SetServerState(ServerState serverState) {_serverState = serverState;}
	
	//for future compatibility with multi-byte values
	@SuppressWarnings("unused")
	private Boolean _reverseBytes = false;
	
	private BufferedInputStream _input;
	private BufferedOutputStream _output;
	   
	public LmsServerSocket(int port) throws LmsConnectionException
	{
		_port = port;
		
		_serverID = CreateRandomID();
		
		try
		{
			_server = new ServerSocket(_port);
		}
		catch (IOException e)
		{
			throw new LmsConnectionException("LmsServerSocket failed to connect on port:" + _port, e);
		}
	}

	private String CreateRandomID()
	{
		Random generator = new Random();
		Integer r = new Integer(generator.nextInt(10000000)); //random number between 0 and 10 million
		return r.toString();
	}
	
	public LmsMessage Connect() throws SocketException, LmsConnectionException, InvalidLmsMessageException
	{
		try
		{
			_socket = _server.accept();
			_serverState = ServerState.HANDSHAKING;
		}
		catch (SocketException e)
		{
			//launch page applet closed the socket
			throw e;
		}
		catch (IOException e)
		{
			_serverState = ServerState.ERROR;
			throw new LmsConnectionException("LmsServerSocket failed to accept connection", e);
		}

		try
		{
			_input = new BufferedInputStream(_socket.getInputStream(), BUFFER_SIZE);
			_output = new BufferedOutputStream(_socket.getOutputStream(), BUFFER_SIZE);
		}
		catch (IOException e)
		{
			_serverState = ServerState.ERROR;
			throw new LmsConnectionException("LmsServerSocket failed to initialize socket IO streams", e);
		}
		
		return ProcessHandshake();
	}

	public void Disconnect()
	{
		Disconnect(true);
	}
	
	public void Disconnect(Boolean normal)
	{
		_clientID = "";
		
		String normalString = "";
		if (normal)
		{
			_serverState = ServerState.DISCONNECTED;
			normalString = "normally";
		}
		else
		{
			_serverState = ServerState.ERROR;
			normalString = "abnormally";
		}

		System.out.println("Client disconnected " + normalString);
	}
	
	private LmsMessage ProcessHandshake() throws SocketException, LmsConnectionException, InvalidLmsMessageException
	{
		//PROTOCOL:
		//first byte sent by client should be value specifying whether the client uses a
		//reversed byte order or not (i.e. big/little endian issue)
		_reverseBytes = ProcessReverseBytesRequest();
			
		//PROTOCOL:
		//wait for first message from client (should be of form (id):SIMULATION:RUNNING)
		LmsMessage handshakeMessage = null;
		try
		{
			handshakeMessage = ReceiveLmsMessage();
		}
		catch (LmsConnectionException e)
		{
			_serverState = ServerState.ERROR;
			throw e;
		}
			
		if (handshakeMessage.GetType() == MessageType.SIMULATION &&
			handshakeMessage.GetValue().equals(MessageValue.Simulation.RUNNING.toString()))
		{
			//retrieve the clientID
			_clientID = handshakeMessage.GetSenderID();
			
			//send confirmation "handshake" message back to let client know we are listening
			try
			{
				SendLmsMessage(new LmsMessage(_serverID, MessageType.LAUNCH_PAGE, MessageValue.LaunchPage.LISTENING.toString()));
				_serverState = ServerState.CONNECTED;
			}
			catch (LmsConnectionException e)
			{
				_serverState = ServerState.ERROR;
				throw e;
			}
		}
		else
		{
			throw new InvalidLmsMessageException("LmsServerSocket was expecting an (id):SIMULATION:RUNNING message, but received: " +
												handshakeMessage.toString());
		}
		
		//return the handshake message that the server received so that it can be sent to the web page
		return handshakeMessage;
	}
	
	private Boolean ProcessReverseBytesRequest() throws LmsConnectionException
	{
		Boolean returnValue = false;
		
		//does client use a reversed byte order (i.e. big/little endian issue)?
		//note: this is not required for the transfer of strings, but is
		//included for future compatibility with multi-byte values.
		byte requestedValue[] = new byte[1];
		requestedValue[0] = 1;
		
		//read one byte to determine if the client is requesting reverse bytes or not
		try
		{
			_input.read(requestedValue);
		}
		catch (IOException e)
		{
			_serverState = ServerState.ERROR;
			throw new LmsConnectionException("LmsServerSocket failed to read ReverseBytes request", e);
		}
		
		if (requestedValue[0] == 1) 
		{
			//server requested reverse bytes
			returnValue = true;
		}
		else
		{
			//server requested normal byte order
			returnValue = false;
		}
		
		return returnValue;
	}
	
	public void SendLmsMessage(LmsMessage lmsMessage) throws LmsConnectionException
	{
		SendString(lmsMessage.toString());
	}
	
	private void SendString(String str) throws LmsConnectionException
	{
		//add terminating character
		str += '\n';
			
		//convert string into an array of bytes
		ByteArrayOutputStream byteStream;
		byteStream = new ByteArrayOutputStream(str.length());
			
		DataOutputStream out;
		out = new DataOutputStream(byteStream);

		try
		{
			for (int i=0; i<str.length(); i++)
				out.write((byte) str.charAt(i));
			
			_output.write(byteStream.toByteArray(), 0, byteStream.size());
			_output.flush();
			
			System.out.println("Server: Sending " + str.replace("\n", ""));
		}
		catch (IOException e)
		{
			_serverState = ServerState.ERROR;
			throw new LmsConnectionException("LmsServerSocket failed to send string: " + str, e);
		}
	}

	public LmsMessage ReceiveLmsMessage() throws SocketException, LmsConnectionException
	{
		return new LmsMessage(ReceiveString());
	}
	
	private String ReceiveString() throws SocketException, LmsConnectionException
	{
		try
		{
			char c;
			String out;

			out = new String("");

			while ((c = (char)_input.read()) != _endOfMessageChar)
				out = out + String.valueOf(c);
	
		 	System.out.println("Server: Received " + out);

			return out;
		}
		catch (SocketException e)
		{
			//socket closed by launch page
			throw e;
		}
		catch (IOException e)
		{
			_serverState = ServerState.ERROR;
			throw new LmsConnectionException("LmsServerSocket failed to receive expected string", e);
		}
	}
	
	public void CloseSocket() throws LmsConnectionException
	{
		try
		{
			if (_socket != null)
			{
				_socket.close();
			}
		 	System.out.println("Server: Closing socket.");
		}
		catch (IOException e)
		{
			throw new LmsConnectionException("LmsServerSocket failed attempting to close socket", e);
		}

	}
}