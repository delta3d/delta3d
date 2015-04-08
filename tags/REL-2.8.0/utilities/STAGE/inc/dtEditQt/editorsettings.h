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
#ifndef DELTA_EDITOR_SETTINGS
#define DELTA_EDITOR_SETTINGS

#include <QtCore/QSettings>

namespace dtEditQt
{

   /**
    * This class is a simple class inheriting most of its behavior from the QSettings
    * class.  It has static string identifiers for each setting that it works with.
    * If a new setting is required, first add it here as a static string and then use
    * it in the application.
    */
   class EditorSettings : public QSettings
   {
   public:
      ///Set to "delta3d.org"
      static const QString ORGANIZATION;

      ///Set to "leveleditor"
      static const QString APPLICATION;

      ///Group to store the main window settings in.
      static const QString MAINWIN_GROUP;

      ///Position of the main window.
      static const QString MAINWIN_POSITION;

      ///Size of the main window.
      static const QString MAINWIN_SIZE;

      static const QString MAINWIN_GEOMETRY;

      static const QString SPLITTER_GROUP;
      static const QString SPLITTER_SIZE;

      static const QString RECENT_PROJECTS;
      static const QString RECENT_MAPS;
      static const QString LIBRARY_PATHS;

      static const QString LIBRARY_PATH_0;
      static const QString LIBRARY_PATH_N;

      static const QString RECENT_PROJECT0;
      static const QString RECENT_PROJECT1;
      static const QString RECENT_PROJECT2;
      static const QString RECENT_PROJECT3;
      static const QString RECENT_MAP0;
      static const QString RECENT_MAP1;
      static const QString RECENT_MAP2;
      static const QString RECENT_MAP3;

      static const QString PREFERENCES_GROUP;

      static const QString SAVE_MILLISECONDS;
      static const QString LOAD_RECENT_PROJECTS;
      static const QString LOAD_RECENT_MAPS;
      static const QString SELECTION_COLOR;
      static const QString RIGID_CAMERA;
      static const QString ACTOR_CREATION_OFFSET;
      static const QString NUM_RECENT_PROJECTS;

      static const QString CAMERA_SPEED_GROUP;
      static const QString CLIP_DISTANCE_GROUP;

      static const QString VOLUME_EDIT_VISIBLE;
      static const QString VOLUME_EDIT_SHAPE;

      // Snap settings
      static const QString SNAP_GROUP;

      static const QString SNAP_TRANSLATION_ENABLED;
      static const QString SNAP_ROTATION_ENABLED;
      static const QString SNAP_SCALE_ENABLED;

      static const QString SNAP_TRANSLATION_VALUE;
      static const QString SNAP_ROTATION_VALUE;
      static const QString SNAP_SCALE_VALUE;

      ///Version identifier for the main window docking state.
      static const int MAINWIN_DOCK_STATE_ID;

      ///Current state of the main windows dockables and toolbars.
      static const QString MAINWIN_DOCK_STATE;

      ///External tool labels
      static const QString EXTERNAL_TOOLS;
      static const QString EXTERNAL_TOOL_TITLE;
      static const QString EXTERNAL_TOOL_COMMAND;
      static const QString EXTERNAL_TOOL_ARGS;
      static const QString EXTERNAL_TOOL_WORKING_DIR;
      static const QString EXTERNAL_TOOL_ICON;

      ///Plugins
      static const QString ACTIVE_PLUGINS;

      /**
       * Constructs the settings object.  This reads the settings from disk if they
       * exist.
       * @note
       *  The organization and application are specified by the corresponding static
       *  strings.  The settings class also forces the use of the INI file format
       *  for its settings.  Qt provides a native format instead of the cross-platform
       *  INI file support, however, this means that on Windows platforms, the settings
       *  are stored in the system registry.  We did not want to pollute the registry
       *  with the editor settings so we choose to put all the settings in a cross
       *  platform INI file.
       * @return
       */
      EditorSettings();

      /**
       * Destroys the settings object.  The destructor also causes a flush of the
       * settings to disk.
       * @return
       */
      virtual ~EditorSettings();

   private:
      EditorSettings& operator=(const EditorSettings&) { return *this; }
      EditorSettings(const EditorSettings&) {}
   };

} // namespace dtEditQt

#endif // DELTA_EDITOR_SETTINGS
