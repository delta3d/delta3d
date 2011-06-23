package org.delta3d.lms.MessageApplet;

public class LmsMessage
{
	private String _senderID = null;
	public String GetSenderID()
	{
		return _senderID;
	}
	public void SetSenderID(String senderID)
	{
		_senderID = senderID;
	}
	
	private MessageType _type = null;
	public MessageType GetType()
	{
		return _type;
	}
	public void SetType(MessageType type)
	{
		_type = type;
	}
	
	//note: value must be stored as a string because it
	//might be either an enumerated value or an objective's
	//raw score
	private String _value = null;
	public String GetValue()
	{
		return _value;
	}
	public void SetValue(String value)
	{
		_value = value;
	}
	
	private String _objectiveID = null;
	public String GetObjectiveID()
	{
		return _objectiveID;
	}
	public void SetObjectiveID(String objectiveID)
	{
		_objectiveID = objectiveID;
	}
	
	private String _delimiter = ":";
	public String GetDelimiter()
	{
		return _delimiter;
	}
	public void SetDelimiter(String delimiter)
	{
		_delimiter = delimiter;
	}
	
	public LmsMessage()
	{
	}
	
	public LmsMessage(String senderID, MessageType type, String value)
	{
		_senderID = senderID;
		_type = type;
		_value = value;
	}
	
	public LmsMessage(String senderID, MessageType type, String value, String objectiveID)
	{
		this(senderID, type, value);
		
		_value = value;
		_objectiveID = objectiveID;
	}
	
	public LmsMessage(String messageString)
	{
		BuildFromString(messageString);
	}
	
	public void BuildFromString(String messageString)
	{
		String tempArray[] = null;
		tempArray = messageString.split(_delimiter);
		
		if (tempArray.length > 0)
		{
			_senderID = tempArray[0];
		}
		
		try
		{
			if (tempArray.length > 1)
			{
				_type = (MessageType) Enum.valueOf(MessageType.class, tempArray[1]);
			}
		}
		catch (IllegalArgumentException e)
		{
			_type = null;
		}

		
		if (tempArray.length > 2)
		{
			_value = tempArray[2];
		}
		
		if (tempArray.length > 3)
		{
			_objectiveID = tempArray[3];
		}
	}
	
	public String toString()
	{
		String tempString = "";
		
		if (_senderID != null)
			tempString += _senderID;
		
		if (_type != null)
			tempString += _delimiter + _type.toString();
		
		if (_value != null)
			tempString += _delimiter + _value;
		
		if (_objectiveID != null)
			tempString += _delimiter + _objectiveID;
		
		return tempString;
	}
}