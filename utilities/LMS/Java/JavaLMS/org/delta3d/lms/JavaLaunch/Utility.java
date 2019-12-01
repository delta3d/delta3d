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

import java.util.prefs.*;
import java.util.logging.*;
import java.lang.reflect.Array;

public class Utility
{

    public static String GetProgramDirectory(ApplicationConfiguration appConfig)
    {
        String programDirectory = "";
        
        Preferences prefs = Preferences.userNodeForPackage(NativeLibraryLaunch.class);
        
        programDirectory = prefs.get(appConfig.GetApplicationName(), "");
        
        if (programDirectory.equals(""))
        {
            ProgramDirectoryDialog dlg = new ProgramDirectoryDialog(appConfig, null);
            dlg.setLocationRelativeTo(null);
            dlg.setVisible(true);
            
            if (dlg.getDlgOK())
            {
                programDirectory = dlg.getDirectory();
                prefs.put(appConfig.GetApplicationName(), programDirectory);
                
            }
            dlg.dispose();
        }
        
        return programDirectory;
    }
    
    public static Level GetLoggingLevelFromString(String value)
    {
        Level level = Level.WARNING; //default value
        
        if (value.equalsIgnoreCase("ALL"))
        {
            level = Level.ALL;
        }
        else if (value.equalsIgnoreCase("CONFIG"))
        {
            level = Level.CONFIG;
        }
        else if (value.equalsIgnoreCase("FINE"))
        {
            level = Level.FINE;
        }
        else if (value.equalsIgnoreCase("FINER"))
        {
            level = Level.FINER;
        }
        else if (value.equalsIgnoreCase("FINEST"))
        {
            level = Level.FINEST;
        }
        else if (value.equalsIgnoreCase("INFO"))
        {
            level = Level.INFO;
        }
        else if (value.equalsIgnoreCase("OFF"))
        {
            level = Level.OFF;
        }
        else if (value.equalsIgnoreCase("SEVERE"))
        {
            level = Level.SEVERE;
        }
        else if (value.equalsIgnoreCase("WARNING"))
        {
            level = Level.WARNING;
        }
        
        return level;
    }

    public static Object ArrayExpand(Object array, int expansion)
    {
    	Class arrayClass = array.getClass();
    	if (!arrayClass.isArray())
    	{
    		return null;
    	}
    	
    	//determine the old and new length for the array
    	int initialLength = Array.getLength(array);
    	int newLength = initialLength + expansion;
    	
    	//get the type of the array (i.e. String, int, etc.)
    	Class componentType = arrayClass.getComponentType();
    	
    	//create a new array of the given type with the new length
    	Object newArray = Array.newInstance(componentType, newLength);
    	
    	//copy the original values into the new array
    	System.arraycopy(array, 0, newArray, 0, initialLength);
    	
    	return newArray;
    }

    public static String StripExtension(String fileName)
    {
    	String returnString = fileName;
    	
    	if (returnString.toUpperCase().endsWith(".DLL"))
    	{
    		returnString = returnString.substring(0, returnString.length() - 4);
    	}
    	else if (returnString.toUpperCase().endsWith(".SO"))
    	{
    		returnString = returnString.substring(0, returnString.length() - 3);
    	}
    	
    	return returnString;
    }
}