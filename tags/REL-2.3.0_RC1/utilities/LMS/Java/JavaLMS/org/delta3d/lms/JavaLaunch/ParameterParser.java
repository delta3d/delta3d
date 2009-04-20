/* -*-java-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 2.1 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * @author Christopher DuBuc
 */
package org.delta3d.lms.JavaLaunch;

import java.util.logging.*;

public class ParameterParser
{
	//constants used to identify named parameters
	private final String APPLICATION_NAME_TOKEN = "-n";
	private final String APPLICATION_LIB_TOKEN = "-a";
	private final String WORKING_DIR_TOKEN = "-w";
	private final String CONFIG_FILE_TOKEN = "-c";
	private final String DATA_JAR_FILE_TOKEN = "-j";
	private final String LOGGING_LEVEL_TOKEN = "-l";
	private final String DATA_OVERWRITE_TOKEN = "-o";
	
	//member parameter variables w/ accessors
	private String _applicationName = "";
	public String GetApplicationName() {return GetDefaultApplicationName();}
	
	private String _applicationLibraryFile = "";
	public String GetApplicationLibraryFile() {return _applicationLibraryFile;}
	
	private String _workingDirectory = "";
	public String GetWorkingDirectory() {return _workingDirectory;}
	//working directory may need to be set manually
	public void SetWorkingDirectory(String workingDirectory) {_workingDirectory = workingDirectory;}
	
	private String _configFile = "";
	public String GetConfigFile() {return _configFile;}
	
	private String _dataJar = "";
	public String GetDataJar() {return _dataJar;}
	
	private String _loggingLevelString = "";
	public Level GetLoggingLevel() {return Utility.GetLoggingLevelFromString(_loggingLevelString);}
	
	private String _dataOverwriteString = "";
	public DataOverwrite GetDataOverwrite()
	{
		if (_dataOverwriteString.equalsIgnoreCase("ALL"))
		{
			return DataOverwrite.ALL;
		}
		else if (_dataOverwriteString.equalsIgnoreCase("NONE"))
		{
			return DataOverwrite.NONE;
		}
		else if (_dataOverwriteString.equalsIgnoreCase("NEWER"))
		{
			return DataOverwrite.NEWER;
		}
		else
		{
			return DataOverwrite.NEWER; //default
		}
	}
	
	//member vars
	private String[] _args;
	
	//constructor
	ParameterParser(String[] args)
	{
		_args = args;
		
		_applicationName = GetParameter(APPLICATION_NAME_TOKEN);
		_applicationLibraryFile = GetParameter(APPLICATION_LIB_TOKEN);
		_workingDirectory = GetParameter(WORKING_DIR_TOKEN);
		_configFile = GetParameter(CONFIG_FILE_TOKEN);
		_dataJar = GetParameter(DATA_JAR_FILE_TOKEN);
		_loggingLevelString = GetParameter(LOGGING_LEVEL_TOKEN);
		_dataOverwriteString = GetParameter(DATA_OVERWRITE_TOKEN);
	}
	
	private String GetParameter(String parameterToken)
	{
		String returnValue = "";
		
		for (int i=0; i<_args.length; i++)
		{
			if (_args[i].equals(parameterToken))
			{
				if ((i + 1) < _args.length)
				{
					returnValue = _args[i + 1];
					break;
				}
			}
		}
		
		return returnValue;
	}
	
	private String GetDefaultApplicationName()
	{
		//if application name was given as a parameter, then use it
		if (!_applicationName.equals(""))
		{
			return _applicationName;
		}
		
        //if application name is not explicitly given, then name it the same as the library name (minus the extension)
        if (_applicationLibraryFile.toLowerCase().endsWith(".dll"))
        {
            return _applicationLibraryFile.substring(0, _applicationLibraryFile.length() - 4);
        }
        else if (_applicationLibraryFile.toLowerCase().endsWith(".so"))
        {
            return _applicationLibraryFile.substring(0, _applicationLibraryFile.length() - 3);
        }
        else
        {
            return _applicationLibraryFile;
        }
	
	}
}
