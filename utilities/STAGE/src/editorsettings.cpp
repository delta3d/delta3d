/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2005-2008, Alion Science and Technology Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 *
 * Matthew W. Campbell
 */
#include <prefix/stageprefix.h>
#include "dtEditQt/editorsettings.h"

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   const QString EditorSettings::ORGANIZATION("delta3d.org");
   const QString EditorSettings::APPLICATION("leveleditor");

   const QString EditorSettings::MAINWIN_GROUP("MainWindow");
   const QString EditorSettings::MAINWIN_POSITION("MainWindow_Position");
   const QString EditorSettings::MAINWIN_SIZE("MainWindow_Size");
   const QString EditorSettings::MAINWIN_GEOMETRY("MainWindow_Geometry");
   const int EditorSettings::MAINWIN_DOCK_STATE_ID = 0;
   const QString EditorSettings::MAINWIN_DOCK_STATE("MainWindow_DockState");

   const QString EditorSettings::SPLITTER_GROUP("Splitters");
   const QString EditorSettings::SPLITTER_SIZE("Splitter");

   const QString EditorSettings::RECENT_PROJECTS("Recently_Loaded_Projects");
   const QString EditorSettings::RECENT_MAPS("Recently_Loaded_Maps");

   const QString EditorSettings::LIBRARY_PATHS("Custom_Library_Paths");
   const QString EditorSettings::LIBRARY_PATH_0("Library_Path_0");
   const QString EditorSettings::LIBRARY_PATH_N("Library_Path_");

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
   const QString EditorSettings::ACTOR_CREATION_OFFSET("Actor_Creation_Offset");
   const QString EditorSettings::NUM_RECENT_PROJECTS("Num_Recent_Projects");

   const QString EditorSettings::CAMERA_SPEED_GROUP("Camera_Speed");
   const QString EditorSettings::CLIP_DISTANCE_GROUP("Clip_Distance");

   const QString EditorSettings::VOLUME_EDIT_VISIBLE("Volume_Edit_Visible");
   const QString EditorSettings::VOLUME_EDIT_SHAPE("Volume_Edit_Shape");

   const QString EditorSettings::SNAP_GROUP("Snap_Group");

   const QString EditorSettings::SNAP_TRANSLATION_ENABLED("Snap_Translation_Enabled");
   const QString EditorSettings::SNAP_ROTATION_ENABLED("Snap_Rotation_Enabled");
   const QString EditorSettings::SNAP_SCALE_ENABLED("Snap_Scale_Enabled");

   const QString EditorSettings::SNAP_TRANSLATION_VALUE("Snap_Translation_Value");
   const QString EditorSettings::SNAP_ROTATION_VALUE("Snap_Rotation_Value");
   const QString EditorSettings::SNAP_SCALE_VALUE("Snap_Scale_Value");

   const QString EditorSettings::EXTERNAL_TOOLS("ExternalTools");
   const QString EditorSettings::EXTERNAL_TOOL_TITLE("Title");
   const QString EditorSettings::EXTERNAL_TOOL_COMMAND("Command");
   const QString EditorSettings::EXTERNAL_TOOL_ARGS("Arguments");
   const QString EditorSettings::EXTERNAL_TOOL_WORKING_DIR("WorkingDir");
   const QString EditorSettings::EXTERNAL_TOOL_ICON("Icon");

   const QString EditorSettings::ACTIVE_PLUGINS("Active_Plugins");

   ///////////////////////////////////////////////////////////////////////////////
   // note: this initialization means storing INI file on 
   //   "/Users/username/AppData/Roaming/delta3d.org/leveleditor.ini" for a win7 setup, 
   //   without any chance to customize the directory path...
   ///////////////////////////////////////////////////////////////////////////////
   EditorSettings::EditorSettings()
      : QSettings(QSettings::IniFormat
      , QSettings::UserScope
      , EditorSettings::ORGANIZATION
      , EditorSettings::APPLICATION)
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   EditorSettings::~EditorSettings()
   {
   }

} // namespace dtEditQt
