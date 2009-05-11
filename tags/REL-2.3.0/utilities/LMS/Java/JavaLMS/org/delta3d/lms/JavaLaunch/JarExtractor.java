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
import java.util.*;
import java.util.jar.*;

public class JarExtractor
{
    private String _jarFile = "";
    public String GetJarFile()
    {
        return _jarFile;
    }
    public void SetJarFile(String jarFile)
    {
        _jarFile = jarFile;
    }

    private String _extractDirectory = "";
    public String GetExtractDirectory()
    {
        return _extractDirectory;
    }
    public void SetExtractDirectory(String extractDirectory)
    {
        _extractDirectory = extractDirectory;
    }
    
    private DataOverwrite _dataOverwrite = DataOverwrite.NONE;
    public DataOverwrite GetDataOverwrite()
    {
        return _dataOverwrite;
    }
    public void SetOverwrite(DataOverwrite dataOverwrite)
    {
        _dataOverwrite = dataOverwrite;
    }
    
    //default constructor
    public JarExtractor(){}
    
    //constructor
    public JarExtractor(String jarFile, String extractDirectory, DataOverwrite dataOverwrite)
    {
        _jarFile = jarFile;
        _extractDirectory = extractDirectory;
        _dataOverwrite = dataOverwrite;
    }
    
    public void Extract()
    {
        try
        {
        	NativeLibraryLaunch.logger.info("In Extract with " + _jarFile);
        	
            JarFile jar = new JarFile(_jarFile);
            
            //ensure directory name ends with '/'
            if (!(_extractDirectory.endsWith("/") || _extractDirectory.endsWith("\\")))
            {
                _extractDirectory += "/";
            }
            
            //initialize progress bar
            int numJarEntries = GetJarEntryCount(jar);
            
            JarExtractorProgressBar progressBar = new JarExtractorProgressBar(numJarEntries);
            //note: progressBar will only be made visible if at least one file needs extraction
            
            int count = 0; //counter for progress bar
            
            Enumeration<JarEntry> entries = jar.entries();
            for (Enumeration e = entries; e.hasMoreElements();)
            {
                JarEntry jen = (JarEntry) e.nextElement();
                
                //update progress bar
                count++;
                progressBar.SetValue(count, "Extracting file " + count + " of " + numJarEntries);
                
                //do not extract manifest files
                if (jen.toString().startsWith("META-INF") || jen.toString().endsWith("/"))
                {
                    continue; //skip to next entry
                }
                
                //build output file object
                File outputFile = new File(_extractDirectory + jen.toString());
                
                //if file already exists, check if we should overwrite it based on _dataOverwrite value
                if (outputFile.exists())
                {
                    if (AllowOverwrite(jen, outputFile) == false)
                    {
                        continue; //skip extracting this file
                    }
                }
                
                //only make progress bar visible if at least one file needs extraction
                if (!progressBar.isVisible())
                {
                    progressBar.setVisible(true);
                }

                //create file's directory structure, if needed
                File parentDirectory = new File(outputFile.getParent());
                parentDirectory.mkdirs();
                
                //create input and output streams to read in jar entry and write out file
                InputStream in = new BufferedInputStream(jar.getInputStream(jen));
                OutputStream out = new BufferedOutputStream(new FileOutputStream(outputFile));
                
                //define read/write buffer
                byte[] buffer = new byte[2048];
                
                //write out buffer and loop until done
                while (true)
                {
                    int nBytes = in.read(buffer);
                    if (nBytes <= 0)
                    {
                        break;
                    }
                    out.write(buffer, 0, nBytes);
                }
                out.flush();
                out.close();
                in.close();
                
                //set timestamp to match that of jar entry's
                outputFile.setLastModified(jen.getTime());
                
                NativeLibraryLaunch.logger.info("Extracted file: " + outputFile);
            }
            
            //close progress bar
            progressBar.setVisible(false);
            progressBar.dispose();
        }
        catch (IOException ioe)
        {
            NativeLibraryLaunch.logger.severe(ioe.getMessage());
        }
    }
    
    private int GetJarEntryCount(JarFile jarFile)
    {
        int count = 0;
        
        Enumeration<JarEntry> entries = jarFile.entries();
        for (Enumeration e = entries; e.hasMoreElements();)
        {
            e.nextElement();
            count++;
        }
        
        return count;
    }
    
    private Boolean AllowOverwrite(JarEntry jen, File outputFile)
    {
        Boolean allowOverwrite = false;
        
        if (_dataOverwrite == DataOverwrite.ALL)
        {
            allowOverwrite = true;
        }
        else if (_dataOverwrite == DataOverwrite.NONE)
        {
            allowOverwrite = false;
        }
        else if (_dataOverwrite == DataOverwrite.NEWER)
        {
            if (jen.getTime() > outputFile.lastModified())
            {
                allowOverwrite = true;
            }
            else
            {
                allowOverwrite = false;
            }
        }
        
        return allowOverwrite;
    }
}