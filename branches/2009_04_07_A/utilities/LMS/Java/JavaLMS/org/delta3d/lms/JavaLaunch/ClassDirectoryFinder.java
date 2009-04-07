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

import java.io.*;

enum LaunchType {CLASS, JAR, JWS}

public class ClassDirectoryFinder
{
    private Object _object = null;
    
    private LaunchType _launchType = LaunchType.CLASS;
    public LaunchType GetLaunchType()
    {
        return _launchType;
    }
    
    //constructor
    public ClassDirectoryFinder(Object object)
    {
        _object = object;
    }
    
    public String GetDirectory()
    {
        String classDirectory = "";
        
        //get the url location of the given object's bytecode file
        String classUrl = _object.getClass().getProtectionDomain().getCodeSource().getLocation().toString();
        
        NativeLibraryLaunch.logger.info("classUrl: " + classUrl);
        
        if (new File(classUrl).getName().startsWith("RM"))
        {
            //class loaded from Java Web Start .jar file
            //NOTE: JWS prepends "RM" to beginning of jar files when they are installed on client
            _launchType = LaunchType.JWS;
            classDirectory = GetDirectoryJavaWebStart(classUrl);
        }
        else if (classUrl.endsWith(".jar"))
        {
            //class loaded from a java .jar file
            _launchType = LaunchType.JAR;
            classDirectory = GetDirectoryJar(classUrl);
        }
        else
        {
            //class loaded from a java .class file
            _launchType = LaunchType.CLASS;
            classDirectory = GetDirectoryClass(classUrl);
        }
        
        NativeLibraryLaunch.logger.info("classDirectory: " + classDirectory);
        
        return classDirectory;
    }
    
    private String GetDirectoryJavaWebStart(String classUrl)
    {
        String classDirectory = "";
        
        //example classUrl: file:C:/Documents%20and%20Settings/chrisd/Application%20Data/Sun/Java/Deployment/
        //                         cache/javaws/http/D65.15.129.58/P80/DM~chrisd/DMCapitolPoliceLaunch/RMJavaLaunch.jar
        
        //get parent directory of .jar file
        classDirectory = new File(classUrl).getParent() + "\\";
        
        //replace '%20' escaped spaces with real spaces
        classDirectory = ReplaceEscapedSpaces(classDirectory);
        
        //remove leading 'file:' characters
        classDirectory = classDirectory.substring(5);
        
        //replace '\' characters with '/' for compatibility (Windows accepts either in a path!)
        classDirectory = classDirectory.replace("\\", "/");
        
        return classDirectory;
    }
    
    private String GetDirectoryJar(String classUrl)
    {
        String classDirectory = "";
        
        //example classUrl: file:/C:/Program%20Files/Java/JavaLaunch/JavaLaunch/JavaLaunch.jar
        
        //get parent directory of .jar file
        classDirectory = new File(classUrl).getParent() + "\\";
        
        //remove leading 'file:\' characters
        classDirectory = classDirectory.substring(6);
        
        //replace '\' characters with '/' for compatibility (Windows accepts either in a path!)
        classDirectory = classDirectory.replace("\\", "/");
        
        //replace '%20' escaped spaces with real spaces
        classDirectory = ReplaceEscapedSpaces(classDirectory);
        
        return classDirectory;
    }
    
    private String GetDirectoryClass(String classUrl)
    {
        String classDirectory = "";
        
        //example classUrl: file:/C:/Program%20Files/Java/JavaLaunch/JavaLaunch/
        
        //remove leading 'file:/' characters
        classDirectory = classUrl.substring(6);
        
        //replace '%20' escaped spaces with real spaces
        classDirectory = ReplaceEscapedSpaces(classDirectory);
        
        return classDirectory;
    }
    
    private String ReplaceEscapedSpaces(String value)
    {
        return value.replace("%20", " ");
    }
}