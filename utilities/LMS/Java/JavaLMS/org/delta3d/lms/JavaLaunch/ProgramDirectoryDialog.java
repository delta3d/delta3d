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
import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

@SuppressWarnings("serial")
public class ProgramDirectoryDialog extends JDialog
{
    private Boolean _dlgOK = false;
    public Boolean getDlgOK()
    {
        return _dlgOK;
    }
    
    private JTextField _txtDirectory = null;
    public String getDirectory()
    {
        return _txtDirectory.getText();
    }
    
    //constructor
    //public ProgramDirectoryDialog(String applicationName, JFrame owner)
    public ProgramDirectoryDialog(ApplicationConfiguration appConfig, JFrame owner)
    {
        super(owner, appConfig.GetApplicationName() + " Program Directory", true);
        
        //create main panel
        JPanel pnlMain = new JPanel();
        pnlMain.setLayout(new FlowLayout(FlowLayout.LEFT));
        
        //create description label
        JLabel lbl = new JLabel("Please select a local directory to store application data");
        pnlMain.add(lbl);
        
        //create text box
        _txtDirectory = new JTextField("", 40);
        
        //set default directory based on platform
        if (appConfig.GetOperatingSystem() == OperatingSystem.WINDOWS)
        {
        	_txtDirectory.setText("C:/Program Files/Delta3dWebApplications/" + appConfig.GetApplicationName().replace(" ", ""));
        }
        else
        {
        	_txtDirectory.setText(appConfig.GetUserDirectory() + "/Delta3dWebApplications/" + appConfig.GetApplicationName().replace(" ", ""));
        }

        pnlMain.add(_txtDirectory);
        
        //create browse button
        JButton btnBrowse = new JButton("...");
        btnBrowse.addActionListener(new ActionListener()
                                                {
                                                    public void actionPerformed(ActionEvent Event)
                                                    {
                                                        //prompt user with directory chooser dialog
                                                        ChooseDirectory();
                                                    }
                                                }
                                            );
        btnBrowse.setPreferredSize(new Dimension(20, (int)_txtDirectory.getPreferredSize().getHeight()));
        pnlMain.add(btnBrowse);
        
        //add main panel
        add(pnlMain);
        
        //create Ok button
        JButton btnOk = new JButton("Ok");
        btnOk.addActionListener(new ActionListener()
                                        {
                                            public void actionPerformed(ActionEvent event)
                                            {
                                                if (MakeDirectory())
                                                {
                                                    _dlgOK = true;
                                                    setVisible(false);  
                                                }
                                            }
                                        }
                                    );
        
        //create Cancel button
        JButton btnCancel = new JButton("Cancel");
        btnCancel.addActionListener(new ActionListener()
                                            {
                                                public void actionPerformed(ActionEvent event)
                                                {
                                                    setVisible(false);
                                                }
                                            }
                                        );

        //create bottom panel
        JPanel pnlBottom = new JPanel();
        pnlBottom.setLayout(new FlowLayout(FlowLayout.RIGHT));
        pnlBottom.add(btnOk);
        pnlBottom.add(btnCancel);
        
        //make Ok and Cancel button the same size
        btnOk.setPreferredSize(new Dimension(75, (int)btnOk.getPreferredSize().getHeight()));
        btnCancel.setPreferredSize(new Dimension(75, (int)btnCancel.getPreferredSize().getHeight()));
        
        //add bottom panel
        add(pnlBottom, BorderLayout.SOUTH);
        
        //set size of dialog
        setSize(500, 150);
    }
    
    private void ChooseDirectory()
    {
        JFileChooser chooser = new JFileChooser();
        
        File dir = new File(_txtDirectory.getText());
        
        if (!dir.isDirectory())
        {
            dir = new File("C:\\");
        }
        chooser.setCurrentDirectory(dir);
        chooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
        
        int result = chooser.showOpenDialog(null);
        if (result == JFileChooser.APPROVE_OPTION)
        {
            if (_txtDirectory != null)
            {
                _txtDirectory.setText(chooser.getSelectedFile().getPath());
            }
        }
    }
    
    private Boolean MakeDirectory()
    {
        Boolean retVal = false;
        File dir = new File(_txtDirectory.getText());
        
        if (!dir.isDirectory())
        {
            //prompt user to create new directory
            String message = "Directory '" + dir.getPath() + "' does not exist. Do you wish to create it?";
            int result = JOptionPane.showConfirmDialog(this,
                                                                     message,
                                                                     "Create Directory",
                                                                     JOptionPane.OK_CANCEL_OPTION,
                                                                     JOptionPane.QUESTION_MESSAGE);
            if (result == JOptionPane.OK_OPTION)
            {
                retVal = dir.mkdirs();
                if (retVal == false)
                {
        			JOptionPane.showMessageDialog(new JFrame(), "Could not create directory " + dir);
                }
            }
            else
            {
                retVal = false;
            }
        }
        else
        {
            retVal = true;
        }
        return retVal;
    }
}