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

import java.awt.*;
import javax.swing.*;

@SuppressWarnings("serial")
public class JarExtractorProgressBar extends JFrame
{
    private JProgressBar _progress;
    private JLabel _label;
    private JPanel _panel;
    
    public JarExtractorProgressBar(int maxValue)
    {
        setTitle("Data Extraction");
        setSize(310, 130);
        setLocationRelativeTo(null); //center of screen
        setBackground(Color.gray);
        
        _panel = new JPanel();
        _panel.setPreferredSize(new Dimension(310, 130));
        getContentPane().add(_panel);
        
        _label = new JLabel("Starting data extraction process");
        _label.setPreferredSize(new Dimension(280, 24));
        _panel.add(_label);
        
        _progress = new JProgressBar();
        _progress.setPreferredSize(new Dimension(300, 20));
        _progress.setMinimum(0);
        _progress.setMaximum(maxValue);
        _progress.setValue(0);
        _progress.setBounds(20, 35, 260, 20);
        _panel.add(_progress);
        
    }
    
    public void SetValue(int value, String label)
    {
        //update label
        _label.setText(label);
        
        //update progress bar
        _progress.setValue(value);
     }
}