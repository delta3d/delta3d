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
import java.util.logging.*;

public class JavaLaunchLogger extends Logger
{
    private String _logFileName = "";
    private Boolean _isInitialized = false;
    
    public JavaLaunchLogger(String loggerName, String logFileName)
    {
        super(loggerName, null);

        _logFileName = logFileName;
        
        LogManager.getLogManager().addLogger(this);
        
        try
        {
            Initialize();
        }
        catch (IOException e)
        {
            System.out.println("Error initializing application logging...continuing without logging");
        }
    }

    private Boolean Initialize() throws IOException
    {
        _isInitialized = false;
        
        FileHandler fileHandler = new FileHandler(_logFileName); //log file
        fileHandler.setFormatter(new SimpleFormatter()); //set to simple text output
        this.addHandler(fileHandler);
        
        _isInitialized = true;
        return _isInitialized;
    }
     
    public void severe(String arg0)
    {
        if (_isInitialized = true)
        {
            super.severe(arg0);
        }
    }
    
    public void warning(String arg0)
    {
        if (_isInitialized = true)
        {
            super.warning(arg0);
        }
    }
    
    public void info(String arg0)
    {
        if (_isInitialized = true)
        {
            super.info(arg0);
        }
    }
    
    public void fine(String arg0)
    {
        if (_isInitialized = true)
        {
            super.fine(arg0);
        }
    }
    
    public void finer(String arg0)
    {
        if (_isInitialized = true)
        {
            super.finer(arg0);
        }
    }
    
    public void finest(String arg0)
    {
        if (_isInitialized = true)
        {
            super.finest(arg0);
        }
    }
    
    public void config(String arg0)
    {
        if (_isInitialized = true)
        {
            super.config(arg0);
        }
    }
}