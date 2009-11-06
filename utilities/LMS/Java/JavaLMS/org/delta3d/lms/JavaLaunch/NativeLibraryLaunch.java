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
import javax.swing.*;
import java.awt.*;
import java.io.*;

//for debugging message box
//Example Usage: JOptionPane.showMessageDialog(new JFrame(), "Exiting...");
//import javax.swing.*;

/*
 * The purpose of this class is to provide a wrapper application which can prepare and launch
 * a native Delta3D game application compiled into a library (.dll/.so). The game library will
 * be launched via an intermediate "launcher" library made specifically for this purpose. The
 * Delta3D game library should inherit from dtGame::GameEntryPoint so that it can be launched
 * via Delta3D's dtGame::GameApplication class.
 */
public class NativeLibraryLaunch
{
	/*
	 * A java.util.logging.logger component used to debug application
	 */
    public static Logger logger = null;
    
    /*
     * A utility object used to read in and manage all the java launch application settings
     */
    private ApplicationConfiguration _appConfig = null;
    
    /*
     * A member variable used to store and create the command line arguments passed on to the
     * native game library
     */
    private String[] _cmdLineArgs;
    
    /*
     * The definition of the native method to be called within the game launcher library
     * 
     * @param cmdLineArgs The command line arguments to be passed on the native game library
     */
    public native void JavaLaunch(String[] cmdLineArgs);
    
    /*
     * The entry point for the java application
     * 
     * @param args The java command line arguments. Should contain only the url of the JavaLaunch
     * configuration file (prefaced by a '-c' token).
     */
	public static void main(String[] args)
    {
		new NativeLibraryLaunch(args).Start();
	}
    
    /*
     * Constructs this NativeLibraryLaunch class. Initializes the application configuration object
     * based on the JavaLaunch.xml config file url found in the command line arguments.
     * 
     * @param args The java command line arguments. Should contain only the url of the JavaLaunch
     * configuration file (prefaced by a '-c' token).
     */
    public NativeLibraryLaunch(String[] args)
    {
    	//_parser = new ParameterParser(args);
    	
        _cmdLineArgs = args;
        
        _appConfig = new ApplicationConfiguration(args);
        _appConfig.ParseConfigFile();
        _appConfig.SetProgramDirectory(); //reads from system store (i.e. registry for windows) or prompts if not found
        
        //must have valid program directory
        if (_appConfig.GetProgramDirectory().equals(""))
        {
        	JOptionPane.showMessageDialog(new Frame(), "Program Directory not found...exiting", "Error", JOptionPane.ERROR_MESSAGE);
        	System.exit(1);
        }
        
        //must supply library name in launch parameters
        if (_appConfig.GetGameLibrary().equals(""))
        {
            JOptionPane.showMessageDialog(new Frame(), "Error: No application library identified in launch parameters...exiting", "Error", JOptionPane.ERROR_MESSAGE);
            System.exit(1);
        }
    }

    /*
     * This method contains the primary code that launches the game application library. It extracts any
     * data files to a local directory, loads all required dependency libraries, loads the game launcher
     * library, and configures the command line arguments that are passed on to the game code.
     */
    private void Start()
    {        
        //set up application logging
        NativeLibraryLaunch.logger = new JavaLaunchLogger("org.delta3d.lms.JavaLaunch", _appConfig.GetProgramDirectory() + "/JavaLaunchLog.txt");
        NativeLibraryLaunch.logger.setLevel(_appConfig.GetLoggingLevel());
        
        //extract data jar files to data directory
        ExtractDataFiles();
        
        //load dependences
        for (String library : _appConfig.GetLibraryList())
        {
        	try
        	{
        		//this method looks for the library first within .jar files stored in the Java Web Start cache, then
        		//within the system path. Note that this method appends the appropriate library extension (.dll or .so)
        		//to the file name, so we need to strip the extension if it exists.
        		//JOptionPane.showMessageDialog(new JFrame(), "Trying to load from JWS cache: " + Utility.StripExtension(library));
        		String libLoadName = Utility.StripExtension(library);
        		
        		if (_appConfig.GetOperatingSystem() == OperatingSystem.LINUX)
        		{
        			if (libLoadName.startsWith("lib"))
        			{
        				libLoadName = libLoadName.replaceFirst("lib", "");
        			}
        		}

        		System.loadLibrary(libLoadName);
        		
        		NativeLibraryLaunch.logger.info("Library loaded from JWS cache: " + library);
        	}
        	catch (UnsatisfiedLinkError e)
        	{
        		//if we can't find the library via the above method, then see if the library is located in the
        		//library directory using an absolute path (including the library extension). See the Java docs
        		//on the differences between loadLibrary() and load() if needed.

        		try
        		{
        			NativeLibraryLaunch.logger.info("Library not in JWS cache; attempting to load from program directory...");
        			System.load(_appConfig.GetLibraryDirectory() + "/" + library);
        			NativeLibraryLaunch.logger.info("Library loaded from program directory: " + _appConfig.GetLibraryDirectory() + "/" + library);
        		}
        		catch (UnsatisfiedLinkError e2)
        		{
        			NativeLibraryLaunch.logger.severe("Could not find library '" + library + "'in JWS cache or program directory; exiting...");
        			JOptionPane.showMessageDialog(new JFrame(), "Could not find library '" + library + "' in JWS cache or program directory; exiting...");
        			System.exit(1);
        		}
        	}
        }
        
        //load Game launch library
        String gameLaunch = Utility.StripExtension(_appConfig.GetLauncherLibrary());
		
		if (_appConfig.GetOperatingSystem() == OperatingSystem.LINUX)
		{
			if (gameLaunch.startsWith("lib"))
			{
				gameLaunch = gameLaunch.replaceFirst("lib", "");
			}
		}
		
		System.loadLibrary(gameLaunch);
        
        NativeLibraryLaunch.logger.info("Game library loaded from JWS cache: " + gameLaunch);
        
        //add data and working directories to command line args that we pass on to the native code
    	AddCommandLineArgs();
    	
        //call native library's entry point method, passing in the command line arguments
        try
        {
        	JavaLaunch(_cmdLineArgs);
        }
        catch (UnsatisfiedLinkError e)
        {
            NativeLibraryLaunch.logger.severe("Error linking to native JavaLaunch method; exiting...");
            JOptionPane.showMessageDialog(new JFrame(), "Error linking to native JavaLaunch method; exiting...");
            System.exit(1);
        }

        System.exit(0);
    }

    /*
     * This method adds directory and game library parameters (retrieved from the config file) to
     * the command line arguments that are passed on to the native simulation code.
     */
    private void AddCommandLineArgs()
    {
    	int cmdLineExpansion = 6;
    	
    	//make array bigger to hold new arguments
        _cmdLineArgs = (String[])Utility.ArrayExpand(_cmdLineArgs, cmdLineExpansion);
        
        //get position of first new array location (for read-ability reasons)
        int startPosition =_cmdLineArgs.length - cmdLineExpansion;
        
        //add data directory to list of command arguments
        if (!_appConfig.GetDataDirectory().equals(""))
        {
            _cmdLineArgs[startPosition] = "-d";
            _cmdLineArgs[startPosition + 1] = _appConfig.GetDataDirectory();
        }

        //add working directory to list of command arguments
        if (!_appConfig.GetLibraryDirectory().equals(""))
        {
        	File dir = new File(_appConfig.GetLibraryDirectory());
        	if (dir.exists() && dir.isDirectory())
        	{
                _cmdLineArgs[startPosition + 2] = "-w";
                _cmdLineArgs[startPosition + 3] = _appConfig.GetLibraryDirectory();
        	}
        	else
        	{
        		//if the library directory does not exist, then set the working directory to the program directory
                _cmdLineArgs[startPosition + 2] = "-w";
                _cmdLineArgs[startPosition + 3] = _appConfig.GetProgramDirectory();
        	}
        }
        else
        {
        	//if no library directory was set in the app config file, then set the working directory to the program directory
        	_cmdLineArgs[startPosition + 2] = "-w";
            _cmdLineArgs[startPosition + 3] = _appConfig.GetProgramDirectory();
        }
        
        //add game library to list of command arguments
        if (!_appConfig.GetGameLibrary().equals(""))
        {
        	_cmdLineArgs[startPosition + 4] = "-a";
        	
        	String appLibrary = Utility.StripExtension(_appConfig.GetGameLibrary());
    		
    		if (_appConfig.GetOperatingSystem() == OperatingSystem.LINUX)
    		{
    			if (appLibrary.startsWith("lib"))
    			{
    				appLibrary = appLibrary.replaceFirst("lib", "");
    			}
    		}
        	
        	_cmdLineArgs[startPosition + 5] = Utility.StripExtension(appLibrary);
        }
    }

    /*
     * This method will locate the data jar files (either in the launch directory or the
     * Java Web Start cache) and extract them into the data directory
     */
    private void ExtractDataFiles()
    {
        //find directory for JavaLaunch executable (i.e. the .class or .jar file)
        ClassDirectoryFinder dirFinder = new ClassDirectoryFinder(this);
        String classDirectory = dirFinder.GetDirectory();
        
        //append the data jar file name onto the class directory; if running from Java Web Start
        //then we also need to prepend "RM" to the name of the data jar file. (the data jar file
        //needs to be in the same directory as the main code files)
        String prepend = "";
        if (dirFinder.GetLaunchType() == LaunchType.JWS)
        {
            prepend = "RM";
        }
        
        for (String dataJar : _appConfig.GetDataExtractFiles())
        {
            String extractJar = classDirectory + prepend + dataJar;
            
            NativeLibraryLaunch.logger.info("extractJar: " + extractJar);
            
            //extract data jar into data directory
            JarExtractor jarExtractor = new JarExtractor(extractJar, _appConfig.GetProgramDirectory(), _appConfig.GetDataOverwrite());
            jarExtractor.Extract();
        }
    }    
}
