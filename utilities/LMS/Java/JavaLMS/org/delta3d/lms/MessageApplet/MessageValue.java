package org.delta3d.lms.MessageApplet;

public class MessageValue
{
	public enum Simulation
	{
		RUNNING,
		STOPPED_NORMAL,
		STOPPED_ABNORMAL
	}
	
	public enum LaunchPage
	{
		LISTENING,
		STOPPED
	}
	
	public enum ObjectiveCompletion
	{
		COMPLETE,
		INCOMPLETE
	}
}
