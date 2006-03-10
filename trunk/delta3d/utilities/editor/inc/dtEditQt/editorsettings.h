/*
 * Delta3D Open Source Game and Simulation Engine 
 * Simulation, Training, and Game Editor (STAGE)
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
#ifndef DELTA_EDITOR_SETTINGS
#define DELTA_EDITOR_SETTINGS

#include <QtCore/QSettings>
#include <osg/Referenced>
#include <osg/ref_ptr>

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

        static const QString RECENT_PROJECTS;
        static const QString RECENT_MAPS;

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

        ///Version identifier for the main window docking state.
        static const int MAINWIN_DOCK_STATE_ID;

        ///Current state of the main windows dockables and toolbars.
        static const QString MAINWIN_DOCK_STATE;

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
        EditorSettings &operator=(const EditorSettings &rhs) { return *this; }
        EditorSettings(const EditorSettings &rhs) { }
    };
}

#endif
