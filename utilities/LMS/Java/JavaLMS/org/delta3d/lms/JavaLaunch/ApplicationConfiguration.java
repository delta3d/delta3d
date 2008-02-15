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

import java.net.*;
import java.io.*;
import java.util.logging.*;
import java.util.ArrayList;

import javax.xml.parsers.*;
import org.xml.sax.*;
import org.w3c.dom.*;

/*
 * This class is designed to read in and manage the application settings required
 * to launch a Delta3D game library. The constructor is passed in the command line arguments
 * from the launching java application and looks for a parameter specifying the url of
 * the JavaLaunch configuration file. This configuration file is then read-in and parsed
 * so that the contained settings can be readily available to the users of this class.
 */
public class ApplicationConfiguration
{
	private String _configFileUrl = "";
	private OperatingSystem _operatingSystem = OperatingSystem.WINDOWS;
	private String _applicationName = "";
	private String _programDirectory = "";
	private String _launcherLibrary = "";
	private String _gameLibrary = "";
	private String _dataSubdirectory = "";
	private ArrayList<String> _dataExtractFiles = null;
	private DataOverwrite _dataOverwrite = DataOverwrite.NEWER;
	private Level _loggingLevel = Level.WARNING;
	private String _librarySubdirectory = "";
	private ArrayList<String> _libraryList = null;
	
	/*
	 * An accessor method that returns the url of the JavaLaunch configuration file.
	 * 
	 * @return A string containing the url of the JavaLaunch configuration file.
	 */
	public String GetConfigFileUrl() {return _configFileUrl;}
	/*
	 * An accessor method that sets the url of the JavaLaunch configuration file. In general,
	 * the config file url should be set via the command line arguments passed in to the
	 * constructor. However this method allows the config file to be set manually (at which
	 * point ParseConfigFile should also be called).
	 * 
	 * @param configFileUrl The url of the JavaLaunch configuration file.
	 */
	public void SetConfigFileUrl(String configFileUrl) {_configFileUrl = configFileUrl;}
	
	/*
	 * A utility method that returns an enumerated OperatingSystem value based on the os
	 * that was used to run the application.
	 * 
	 * @return an enumerated OperatingSystem value (ex. WINDOWS, LINUX)
	 */
	public OperatingSystem GetOperatingSystem() {return _operatingSystem;}
	
	public String GetUserDirectory() {return DetermineUserDirectory();}
	
	/*
	 * An accessor method that returns the name of the application as retrieved from the
	 * JavaLaunch configuration file.
	 * 
	 * @return A string containing the name of the application.
	 */
	public String GetApplicationName() {return _applicationName;}
	
	/*
	 * An accessor method that returns the name of the program directory as retrieved from
	 * the persistent backing store (i.e. registry) or the user (via a dialog prompt).
	 * 
	 * @return A string containing the location of the program directory.
	 */
	public String GetProgramDirectory() {return _programDirectory;}
	
	/*
	 * An accessor method that returns the name of the launcher library as retrieved from the
	 * JavaLaunch configuration file.
	 * 
	 * @return A string containing the name of the launcher library.
	 */
	public String GetLauncherLibrary() {return _launcherLibrary;}
	
	/*
	 * An accessor method that returns the name of the game library as retrieved from the
	 * JavaLaunch configuration file.
	 * 
	 * @return A string containing the name of the game library.
	 */
	public String GetGameLibrary() {return _gameLibrary;}
	
	/*
	 * An accessor method that returns the name of the data subdirectory as retrieved from the
	 * JavaLaunch configuration file.
	 * 
	 * @return A string containing the name of the data subdirectory.
	 */
	public String GetDataSubdirectory() {return _dataSubdirectory;}
	
	/*
	 * An accessor method that returns the name of the data directory, created by appending
	 * the data subdirectory on to the program directory.
	 * 
	 * @return A string containing the name of the data directory.
	 */
	public String GetDataDirectory() {return _programDirectory + "/" + _dataSubdirectory;}
	
	/*
	 * An accessor method that returns an array list of java .jar file that need to be extracted
	 * into a local data subdirectory. These files will contain the data that the simulation needs
	 * to run (i.e. characters, models, terrains, etc.) This list is retrieved from the JavaLaunch
	 * configuration file.
	 * 
	 * @return An array list of strings containing the name of the .jar files to be extracted.
	 */
	public ArrayList<String> GetDataExtractFiles() {return _dataExtractFiles;}
	
	/*
	 * An accessor method that returns an enumerated DataOverwrite value as retrieved from the JavaLaunch
	 * configuration file. The DataOverwrite value determines whether or not extracted data files should
	 * overwrite any existing data files in the data subdirectory.
	 * 
	 * @return An enumerated DataOverwrite value (ALL, NONE, NEWER) default: NEWER.
	 */
	public DataOverwrite GetDataOverwrite() {return _dataOverwrite;}
	
	/*
	 * An accessor method that returns an enumerated java.util.logging.Level value as retrieved from the JavaLaunch
	 * configuration file. The Level value determines the level of detail the logger displays for debugging purposes.
	 * 
	 * @return An enumerated java.util.logging.Level value (ALL, CONFIG, FINE, FINER, FINEST, INFO, OFF, SEVERE, WARNING) default: WARNING.
	 */
	public Level GetLoggingLevel() {return _loggingLevel;}
	
	/*
	 * An accessor method that returns the name of the code library subdirectory as retrieved from the
	 * JavaLaunch configuration file.
	 * 
	 * @return A string containing the name of the code library subdirectory.
	 */
	public String GetLibrarySubdirectory() {return _librarySubdirectory;}
	
	/*
	 * An accessor method that returns the name of the code library directory, created by appending
	 * the library subdirectory on to the program directory.
	 * 
	 * @return A string containing the name of the code library directory.
	 */
	public String GetLibraryDirectory() {return _programDirectory + "/" + _librarySubdirectory;}
	
	/*
	 * An accessor method that returns an array list of code libraries that need to be pre-loaded
	 * into memory before launching the game library. These files will be stored in the Java Web Start
	 * cache in compressed .jar files, and Java Web Start will take care of finding and loading
	 * them via the System.load() method.
	 * 
	 * @return An array list of strings containing the name of the code libraries to pre-load into memory.
	 */
	public ArrayList<String> GetLibraryList() {return _libraryList;}
	
	/*
	 * Constructs the ApplicationConfiguration object, and extracts the url of the JavaLaunch
	 * configuration file from the command line arguments passed in as a parameter. Also, it
	 * determines the operating system that was used to launch the application.
	 * 
	 * @param cmdLineArgs The command line arguments passed into the java application; should
	 * 		contain the url of the JavaLaunch configuration file.
	 */
	ApplicationConfiguration(String[] cmdLineArgs)
	{
		//locate
		FindConfigFileInCommandLineArgs(cmdLineArgs);
		
		_operatingSystem = DetermineOperatingSystem();
	}
	
	/*
	 * This utility method uses the ParmeterParse class to find the url of the JavaLaunch config file
	 * as specified by the '-c' token.
	 * 
	 * @param args The command line arguments passed into the java application.
	 * 
	 * @return A boolean value set to TRUE if the config file url was found in the command line arguments.
	 */
	private Boolean FindConfigFileInCommandLineArgs(String[] args)
	{
		ParameterParser parser = new ParameterParser(args);
		_configFileUrl = parser.GetConfigFile();

		if (_configFileUrl.equals(""))
		{
			return false;
		}
		else
		{
			return true;
		}
	}
	
	/*
	 * A utility method to determine the operating system used to launch the application.
	 * 
	 * @return An enumerated OperatingSystem value (WINDOWS, LINUX).
	 */
	private OperatingSystem DetermineOperatingSystem()
	{
		String os = System.getProperty("os.name").toUpperCase();
		
		if (os.indexOf("WINDOWS") != -1)
		{
			return OperatingSystem.WINDOWS;
		}
		else
		{
			return OperatingSystem.LINUX;
		}
	}
	
	private String DetermineUserDirectory()
	{
		return System.getProperty("user.home");
	}
	
	/*
	 * This method reads and parses the JavaLauch config file in order expose the settings to
	 * users of this class. This code provides the primary functionality of the class.
	 * 
	 * @return A boolean value, TRUE if the config file was successfully parsed.
	 */
	public Boolean ParseConfigFile()
	{
		//get input stream for config file
		URL url = null;
		URLConnection urlConn = null;

		try
		{
			url = new URL(_configFileUrl);
			urlConn = url.openConnection();
		}
		catch (MalformedURLException e)
		{
			NativeLibraryLaunch.logger.warning("Invalid URL for application config xml file");
			return false;
		}
		catch (IOException e)
		{
			NativeLibraryLaunch.logger.warning("Could not access config xml file");
			return false;
		}
		
		//create xml dom document from config file
		DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
		Document document = null;
		
		try
		{
			DocumentBuilder builder = factory.newDocumentBuilder();
			document = builder.parse(urlConn.getInputStream());
		}
		catch (SAXException e)
		{
			NativeLibraryLaunch.logger.warning("JavaLaunch SAXException: " + e.getMessage());
			return false;
		}
		catch (ParserConfigurationException e)
		{
			NativeLibraryLaunch.logger.warning("JavaLaunch ParserConfigurationException: " + e.getMessage());
			return false;
		}
		catch (IOException e)
		{
			NativeLibraryLaunch.logger.warning("JavaLaunch IOException: " + e.getMessage());
			return false;
		}
		
		//read in application name
		NodeList appNameNodes = document.getElementsByTagName("applicationName");
		if (appNameNodes.getLength() > 0)
		{
			_applicationName = appNameNodes.item(0).getFirstChild().getNodeValue();
		}
		
		//read in name of launcher library
		NodeList launchLibNodes = document.getElementsByTagName("launcherLibrary");
		for (int i=0; i < launchLibNodes.getLength(); i++)
		{
			Node node = launchLibNodes.item(i);
			if (node.getAttributes().getNamedItem("platform").getNodeValue().equalsIgnoreCase(_operatingSystem.name()))
			{
				_launcherLibrary = node.getFirstChild().getNodeValue();
			}
		}
		
		//read in name of game library
		NodeList gameLibNodes = document.getElementsByTagName("gameLibrary");
		for (int i=0; i < gameLibNodes.getLength(); i++)
		{
			Node node = gameLibNodes.item(i);
			if (node.getAttributes().getNamedItem("platform").getNodeValue().equalsIgnoreCase(_operatingSystem.name()))
			{
				_gameLibrary = node.getFirstChild().getNodeValue();
			}
		}
		
		//read in data subdirectory (where data extract files get extracted to)
		NodeList dataSubDirNodes = document.getElementsByTagName("dataDirectory");
		if (dataSubDirNodes.getLength() > 0)
		{
			_dataSubdirectory = dataSubDirNodes.item(0).getFirstChild().getNodeValue();
		}
		
		//read in data extract files
		_dataExtractFiles = new ArrayList<String>();
		NodeList dataExtractNodes = document.getElementsByTagName("dataExtractFile");
		for (int i=0; i < dataExtractNodes.getLength(); i++)
		{
			Node node = dataExtractNodes.item(i);
			_dataExtractFiles.add(node.getFirstChild().getNodeValue());
		}
		
		//read in data overwrite value
		NodeList dataOverwriteNodes = document.getElementsByTagName("dataOverwrite");
		if (dataOverwriteNodes.getLength() > 0)
		{
			String strDataOverwrite = dataOverwriteNodes.item(0).getFirstChild().getNodeValue();
			
			if (strDataOverwrite.equalsIgnoreCase("ALL"))
			{
				_dataOverwrite = DataOverwrite.ALL;
			}
			else if (strDataOverwrite.equalsIgnoreCase("NONE"))
			{
				_dataOverwrite = DataOverwrite.NONE;
			}
			else if (strDataOverwrite.equalsIgnoreCase("NEWER"))
			{
				_dataOverwrite = DataOverwrite.NEWER;
			}
			else
			{
				_dataOverwrite = DataOverwrite.NEWER; //default
			}
		}
		
		//read in logging level
		NodeList loggingLevelNodes = document.getElementsByTagName("loggingLevel");
		if (loggingLevelNodes.getLength() > 0)
		{
			String strLoggingLevel = loggingLevelNodes.item(0).getFirstChild().getNodeValue();
			
			_loggingLevel = Utility.GetLoggingLevelFromString(strLoggingLevel);
		}
		
		//read in library subdirectory (where code library files get extracted to)
		NodeList librarySubDirNodes = document.getElementsByTagName("libraryDirectory");
		if (librarySubDirNodes.getLength() > 0)
		{
			_librarySubdirectory = librarySubDirNodes.item(0).getFirstChild().getNodeValue();
		}
		
		//read in library list
		_libraryList = new ArrayList<String>();
		NodeList libraryNodes = document.getElementsByTagName("library");
		for (int i=0; i < libraryNodes.getLength(); i++)
		{
			Node node = libraryNodes.item(i);
			if (node.getAttributes().getNamedItem("platform").getNodeValue().equalsIgnoreCase(_operatingSystem.name()))
			{
				_libraryList.add(node.getFirstChild().getNodeValue());
			}
		}
		
		return true;
	}

	/*
	 * This method uses the Utility.GetProgramDirectory() method to set the location of the
	 * program directory. The program directory is the root directory of the application, and on
	 * initial installation of the application the user is prompted with a dialog to provide this
	 * directory. This directory is then remembered in the java backing store (i.e. registry) from
	 * which it is retrieved on subsequent application runs.
	 */
	public void SetProgramDirectory()
	{
		if (_applicationName.equals("")) return;
		
		_programDirectory = Utility.GetProgramDirectory(this);
	}
}
