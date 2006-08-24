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
#ifndef __UIResources__h
#define __UIResources__h

#include <string>

namespace dtEditQt
{

    /**
     * This class contains static strings that define locations to resources such as
     * icons.
     */
    class UIResources
    {
        public:

            static void staticInitialize();

            static std::string GPL_LICENSE_FILE;

            static std::string ICON_FILE_NEW_MAP;
            static std::string ICON_FILE_OPEN_MAP;
            static std::string ICON_FILE_SAVE;

            static std::string ICON_EDIT_DUPLICATE;
            static std::string ICON_EDIT_DELETE;
            static std::string ICON_GROUND_CLAMP;
            static std::string ICON_EDIT_UNDO;
            static std::string ICON_EDIT_REDO;

            static std::string ICON_TOOLMODE_CAMERA;
            static std::string ICON_TOOLMODE_SELECT;
            static std::string ICON_TOOLMODE_TRANSLATE;
            static std::string ICON_TOOLMODE_ROTATE;

            static std::string ICON_VIEWMODE_TEXTURES;
            static std::string ICON_VIEWMODE_LIGHTING;
            static std::string ICON_VIEWMODE_WIREFRAME;
            static std::string ICON_VIEWMODE_TEXTURES_AND_LIGHTING;

            static std::string ICON_TINY_FOLDER;
            static std::string ICON_TINY_FOLDER_OPEN;

            static std::string LARGE_ICON_EDIT_DUPLICATE;
            static std::string LARGE_ICON_EDIT_DELETE;
            static std::string LARGE_ICON_EDIT_GOTO;
            static std::string LARGE_ICON_EDIT_IMPORT;
            static std::string LARGE_ICON_EDIT_REFRESH;
            static std::string LARGE_ICON_EDIT_CATEGORY;

            static std::string ICON_SOUND_STOP;
            static std::string ICON_SOUND_PLAY;

            static std::string ICON_SOUND_RESOURCE;
            static std::string ICON_STATICMESH_RESOURCE;
            static std::string ICON_TEXTURE_RESOURCE;
            static std::string ICON_CHARACTER_RESOURCE;
            static std::string ICON_PARTICLE_RESOURCE;
            static std::string ICON_TERRAIN_RESOURCE;

            static std::string ICON_SOUND_TAB;
            static std::string ICON_STATICMESH_TAB;
            static std::string ICON_TEXTURE_TAB;
            static std::string ICON_CHARACTER_TAB;
            static std::string ICON_PARTICLE_TAB;
            static std::string ICON_TERRAIN_TAB;

            static std::string ICON_ACTOR;
            static std::string SPLASH_SCREEN;
            static std::string ICON_APPLICATION;
    };

}

#endif
