/*
 * Delta3D Open Source Game and Simulation Engine Level Editor
 * Copyright (C) 2005, BMH Associates, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 *
 * @author Matthew W. Campbell
*/
#include "dtEditQt/editorsettings.h"

namespace dtEditQt
{

    ///////////////////////////////////////////////////////////////////////////////
    const QString EditorSettings::ORGANIZATION("delta3d.org");
    const QString EditorSettings::APPLICATION("leveleditor");

    const QString EditorSettings::MAINWIN_GROUP("MainWindow");
    const QString EditorSettings::MAINWIN_POSITION("MainWindow_Position");
    const QString EditorSettings::MAINWIN_SIZE("MainWindow_Size");
    const int EditorSettings::MAINWIN_DOCK_STATE_ID = 0;
    const QString EditorSettings::MAINWIN_DOCK_STATE("MainWindow_DockState");

    const QString EditorSettings::RECENT_PROJECTS("Recently_Loaded_Projects");
    const QString EditorSettings::RECENT_MAPS("Recently_Loaded_Maps");

    const QString EditorSettings::RECENT_PROJECT0("Recent_Project_0");
    const QString EditorSettings::RECENT_PROJECT1("Recent_Project_1");
    const QString EditorSettings::RECENT_PROJECT2("Recent_Project_2");
    const QString EditorSettings::RECENT_PROJECT3("Recent_Project_3");

    const QString EditorSettings::RECENT_MAP0("Recent_Map_0");
    const QString EditorSettings::RECENT_MAP1("Recent_Map_1");
    const QString EditorSettings::RECENT_MAP2("Recent_Map_2");
    const QString EditorSettings::RECENT_MAP3("Recent_Map_3");

    const QString EditorSettings::PREFERENCES_GROUP("User_Preferences");

    const QString EditorSettings::LOAD_RECENT_PROJECTS("Load_Recent_Projects");
    const QString EditorSettings::LOAD_RECENT_MAPS("Load_Recent_Maps");
    const QString EditorSettings::SAVE_MILLISECONDS("Autosave_delay");
    const QString EditorSettings::SELECTION_COLOR("Selection_Color");
    const QString EditorSettings::RIGID_CAMERA("Rigid_Camera_Movement");

    ///////////////////////////////////////////////////////////////////////////////
    EditorSettings::EditorSettings() : QSettings(QSettings::IniFormat,
                                                 QSettings::UserScope,
                                                 EditorSettings::ORGANIZATION,
                                                 EditorSettings::APPLICATION)
    {

    }

    ///////////////////////////////////////////////////////////////////////////////
    EditorSettings::~EditorSettings()
    {

    }

}
