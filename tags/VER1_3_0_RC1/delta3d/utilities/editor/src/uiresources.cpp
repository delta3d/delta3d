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
#include <dtEditQt/uiresources.h>
#include <stdlib.h>
#include <osgDB/FileUtils>

namespace dtEditQt
{
    void UIResources::staticInitialize()
    {        
        UIResources::GPL_LICENSE_FILE = "gpl.txt";
        
        ICON_FILE_NEW_MAP = osgDB::findDataFile("icons/newmap_small.png");
        ICON_FILE_OPEN_MAP = osgDB::findDataFile("icons/openmap_small.png");
        ICON_FILE_SAVE = osgDB::findDataFile("icons/savemap_small.png");

        ICON_EDIT_DUPLICATE = osgDB::findDataFile("icons/duplicate_small.png");
        ICON_EDIT_DELETE = osgDB::findDataFile("icons/trash_small.png");
        ICON_GROUND_CLAMP = osgDB::findDataFile("icons/groundclamp_small.png");
        ICON_EDIT_UNDO = osgDB::findDataFile("icons/undo.png");
        ICON_EDIT_REDO = osgDB::findDataFile("icons/redo.png");

        ICON_TOOLMODE_CAMERA = osgDB::findDataFile("icons/cameramode_small.png");
        ICON_TOOLMODE_SELECT = osgDB::findDataFile("icons/selectmode_small.png");
        ICON_TOOLMODE_TRANSLATE = osgDB::findDataFile("icons/translatemode_small.png");
        ICON_TOOLMODE_ROTATE = osgDB::findDataFile("icons/rotatemode_small.png");

        ICON_VIEWMODE_TEXTURES = osgDB::findDataFile("icons/texturemode_tiny.png");
        ICON_VIEWMODE_LIGHTING = osgDB::findDataFile("icons/lightingmode_tiny.png");
        ICON_VIEWMODE_WIREFRAME = osgDB::findDataFile("icons/wireframemode_tiny.png");
        ICON_VIEWMODE_TEXTURES_AND_LIGHTING = osgDB::findDataFile("icons/texturelightingmode_tiny.png");

        ICON_TINY_FOLDER = osgDB::findDataFile("icons/folder_tiny.png");
        ICON_TINY_FOLDER_OPEN = osgDB::findDataFile("icons/folderopen_tiny.png");

        LARGE_ICON_EDIT_DUPLICATE = osgDB::findDataFile("icons/duplicate_large.png");
        LARGE_ICON_EDIT_DELETE = osgDB::findDataFile("icons/trash_large.png");
        LARGE_ICON_EDIT_GOTO = osgDB::findDataFile("icons/goto_large.png");
        LARGE_ICON_EDIT_IMPORT = osgDB::findDataFile("icons/import_large.png");
        LARGE_ICON_EDIT_REFRESH = osgDB::findDataFile("icons/refresh_large.png");
        LARGE_ICON_EDIT_CATEGORY = osgDB::findDataFile("icons/category_large.png");

        ICON_SOUND_STOP = osgDB::findDataFile("icons/stop_sound.png");
        ICON_SOUND_PLAY = osgDB::findDataFile("icons/play_sound.png");

        ICON_SOUND_RESOURCE = osgDB::findDataFile("icons/sound_resource.png");
        ICON_STATICMESH_RESOURCE = osgDB::findDataFile("icons/staticmesh_resource.png");
        ICON_CHARACTER_RESOURCE = osgDB::findDataFile("icons/character_resource.png");
        ICON_TEXTURE_RESOURCE = osgDB::findDataFile("icons/texture_resource.png");
        ICON_PARTICLE_RESOURCE = osgDB::findDataFile("icons/particle_resource.png");
        ICON_TERRAIN_RESOURCE = osgDB::findDataFile("icons/terrain_resource.png");

        ICON_SOUND_TAB =osgDB::findDataFile ("icons/sound_tab.png");
        ICON_STATICMESH_TAB = osgDB::findDataFile("icons/staticmesh_tab.png");
        ICON_CHARACTER_TAB = osgDB::findDataFile("icons/character_tab.png");
        ICON_TEXTURE_TAB = osgDB::findDataFile("icons/texture_tab.png");
        ICON_PARTICLE_TAB = osgDB::findDataFile("icons/particle_tab.png");
        ICON_TERRAIN_TAB = osgDB::findDataFile("icons/terrain_tab.png");

        ICON_ACTOR = osgDB::findDataFile("icons/actor_icon.png");
        SPLASH_SCREEN = osgDB::findDataFile("icons/splash.png");
        ICON_APPLICATION = osgDB::findDataFile("icons/application_icon.png");
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

