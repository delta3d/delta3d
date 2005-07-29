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
#include "dtEditQt/uiresources.h"
#include <stdlib.h>


namespace dtEditQt
{
    void UIResources::staticInitialize()
    {        
        static char* ptr = getenv("DELTA_ROOT");
        static std::string prefix;

        prefix = std::string(ptr);                
        if (prefix[prefix.length()-1] == '/' || prefix[prefix.length()-1] == '\\') {
            prefix = prefix.substr(0,prefix.length()-1);
        }
        
        prefix += "/utilities/editor/";
        UIResources::GPL_LICENSE_FILE = prefix + "gpl.txt";
        
        ICON_FILE_NEW_MAP = (prefix+"icons/newmap_small.png");
        ICON_FILE_OPEN_MAP = (prefix+"icons/openmap_small.png");
        ICON_FILE_SAVE = (prefix+"icons/savemap_small.png");

        ICON_EDIT_DUPLICATE = (prefix+"icons/duplicate_small.png");
        ICON_EDIT_DELETE = (prefix+"icons/trash_small.png");
        ICON_GROUND_CLAMP = (prefix+"icons/groundclamp_small.png");
        ICON_EDIT_UNDO = (prefix+"icons/undo.png");
        ICON_EDIT_REDO = (prefix+"icons/redo.png");

        ICON_TOOLMODE_CAMERA = (prefix+"icons/cameramode_small.png");
        ICON_TOOLMODE_SELECT = (prefix+"icons/selectmode_small.png");
        ICON_TOOLMODE_TRANSLATE = (prefix+"icons/translatemode_small.png");
        ICON_TOOLMODE_ROTATE = (prefix+"icons/rotatemode_small.png");

        ICON_VIEWMODE_TEXTURES = (prefix+"icons/texturemode_tiny.png");
        ICON_VIEWMODE_LIGHTING = (prefix+"icons/lightingmode_tiny.png");
        ICON_VIEWMODE_WIREFRAME = (prefix+"icons/wireframemode_tiny.png");
        ICON_VIEWMODE_TEXTURES_AND_LIGHTING = (prefix+"icons/texturelightingmode_tiny.png");

        ICON_TINY_FOLDER = (prefix+"icons/folder_tiny.png");
        ICON_TINY_FOLDER_OPEN = (prefix+"icons/folderopen_tiny.png");

        LARGE_ICON_EDIT_DUPLICATE = (prefix+"icons/duplicate_large.png");
        LARGE_ICON_EDIT_DELETE = (prefix+"icons/trash_large.png");
        LARGE_ICON_EDIT_GOTO = (prefix+"icons/goto_large.png");
        LARGE_ICON_EDIT_IMPORT = (prefix+"icons/import_large.png");
        LARGE_ICON_EDIT_REFRESH = (prefix+"icons/refresh_large.png");
        LARGE_ICON_EDIT_CATEGORY = (prefix+"icons/category_large.png");

        ICON_SOUND_STOP = (prefix+"icons/stop_sound.png");
        ICON_SOUND_PLAY = (prefix+"icons/play_sound.png");

        ICON_SOUND_RESOURCE = (prefix+"icons/sound_resource.png");
        ICON_STATICMESH_RESOURCE = (prefix+"icons/staticmesh_resource.png");
        ICON_CHARACTER_RESOURCE = (prefix+"icons/character_resource.png");
        ICON_TEXTURE_RESOURCE = (prefix+"icons/texture_resource.png");
        ICON_PARTICLE_RESOURCE = (prefix+"icons/particle_resource.png");
        ICON_TERRAIN_RESOURCE = (prefix+"icons/terrain_resource.png");

        ICON_SOUND_TAB = (prefix+"icons/sound_tab.png");
        ICON_STATICMESH_TAB = (prefix+"icons/staticmesh_tab.png");
        ICON_CHARACTER_TAB = (prefix+"icons/character_tab.png");
        ICON_TEXTURE_TAB = (prefix+"icons/texture_tab.png");
        ICON_PARTICLE_TAB = (prefix+"icons/particle_tab.png");
        ICON_TERRAIN_TAB = (prefix+"icons/terrain_tab.png");

        ICON_ACTOR = (prefix+"icons/actor_icon.png");
        SPLASH_SCREEN = (prefix+"icons/splash.png");
        ICON_APPLICATION = (prefix+"icons/application_icon.png");
    }

    //STATIC VARS...
    std::string UIResources::GPL_LICENSE_FILE("");

    std::string UIResources::ICON_FILE_NEW_MAP("");
    std::string UIResources::ICON_FILE_OPEN_MAP("");
    std::string UIResources::ICON_FILE_SAVE("");

    std::string UIResources::ICON_EDIT_DUPLICATE("");
    std::string UIResources::ICON_EDIT_DELETE("");
    std::string UIResources::ICON_GROUND_CLAMP("");
    std::string UIResources::ICON_EDIT_UNDO("");
    std::string UIResources::ICON_EDIT_REDO("");

    std::string UIResources::ICON_TOOLMODE_CAMERA("");
    std::string UIResources::ICON_TOOLMODE_SELECT("");
    std::string UIResources::ICON_TOOLMODE_TRANSLATE("");
    std::string UIResources::ICON_TOOLMODE_ROTATE("");

    std::string UIResources::ICON_VIEWMODE_TEXTURES("");
    std::string UIResources::ICON_VIEWMODE_LIGHTING("");
    std::string UIResources::ICON_VIEWMODE_WIREFRAME("");
    std::string UIResources::ICON_VIEWMODE_TEXTURES_AND_LIGHTING("");

    std::string UIResources::ICON_TINY_FOLDER("");
    std::string UIResources::ICON_TINY_FOLDER_OPEN("");

    std::string UIResources::LARGE_ICON_EDIT_DUPLICATE("");
    std::string UIResources::LARGE_ICON_EDIT_DELETE("");
    std::string UIResources::LARGE_ICON_EDIT_GOTO("");
    std::string UIResources::LARGE_ICON_EDIT_IMPORT("");
    std::string UIResources::LARGE_ICON_EDIT_REFRESH("");
    std::string UIResources::LARGE_ICON_EDIT_CATEGORY("");

    std::string UIResources::ICON_SOUND_STOP("");
    std::string UIResources::ICON_SOUND_PLAY("");

    std::string UIResources::ICON_SOUND_RESOURCE("");
    std::string UIResources::ICON_STATICMESH_RESOURCE("");
    std::string UIResources::ICON_CHARACTER_RESOURCE("");
    std::string UIResources::ICON_TEXTURE_RESOURCE("");
    std::string UIResources::ICON_PARTICLE_RESOURCE("");
    std::string UIResources::ICON_TERRAIN_RESOURCE("");

    std::string UIResources::ICON_SOUND_TAB("");
    std::string UIResources::ICON_STATICMESH_TAB("");
    std::string UIResources::ICON_CHARACTER_TAB("");
    std::string UIResources::ICON_TEXTURE_TAB("");
    std::string UIResources::ICON_PARTICLE_TAB("");
    std::string UIResources::ICON_TERRAIN_TAB("");

    std::string UIResources::ICON_ACTOR("");
    std::string UIResources::SPLASH_SCREEN("");
    std::string UIResources::ICON_APPLICATION("");  

}

