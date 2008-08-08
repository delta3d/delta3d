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
#include <prefix/dtstageprefix-src.h>
#include <dtEditQt/uiresources.h>
#include <cstdlib>
#include <dtCore/globals.h>

namespace dtEditQt
{
    void UIResources::staticInitialize()
    {        
        UIResources::GPL_LICENSE_FILE = "gpl.txt";
        
        ICON_FILE_NEW_MAP = dtCore::FindFileInPathList("icons/newmap_small.png");
        ICON_FILE_OPEN_MAP = dtCore::FindFileInPathList("icons/openmap_small.png");
        ICON_FILE_SAVE = dtCore::FindFileInPathList("icons/savemap_small.png");

        ICON_EDIT_DUPLICATE = dtCore::FindFileInPathList("icons/duplicate_small.png");
        ICON_EDIT_DELETE = dtCore::FindFileInPathList("icons/trash_small.png");
        ICON_GROUND_CLAMP = dtCore::FindFileInPathList("icons/groundclamp_small.png");
        ICON_EDIT_UNDO = dtCore::FindFileInPathList("icons/undo.png");
        ICON_EDIT_REDO = dtCore::FindFileInPathList("icons/redo.png");

        ICON_TOOLMODE_CAMERA = dtCore::FindFileInPathList("icons/cameramode_small.png");
        ICON_TOOLMODE_SELECT = dtCore::FindFileInPathList("icons/selectmode_small.png");
        ICON_TOOLMODE_TRANSLATE = dtCore::FindFileInPathList("icons/translatemode_small.png");
        ICON_TOOLMODE_ROTATE = dtCore::FindFileInPathList("icons/rotatemode_small.png");

        ICON_VIEWMODE_TEXTURES = dtCore::FindFileInPathList("icons/texturemode_tiny.png");
        ICON_VIEWMODE_LIGHTING = dtCore::FindFileInPathList("icons/lightingmode_tiny.png");
        ICON_VIEWMODE_WIREFRAME = dtCore::FindFileInPathList("icons/wireframemode_tiny.png");
        ICON_VIEWMODE_TEXTURES_AND_LIGHTING = dtCore::FindFileInPathList("icons/texturelightingmode_tiny.png");

        ICON_TINY_FOLDER = dtCore::FindFileInPathList("icons/folder_tiny.png");
        ICON_TINY_FOLDER_OPEN = dtCore::FindFileInPathList("icons/folderopen_tiny.png");

        LARGE_ICON_EDIT_DUPLICATE = dtCore::FindFileInPathList("icons/duplicate_large.png");
        LARGE_ICON_EDIT_DELETE = dtCore::FindFileInPathList("icons/trash_large.png");
        LARGE_ICON_EDIT_GOTO = dtCore::FindFileInPathList("icons/goto_large.png");
        LARGE_ICON_EDIT_IMPORT = dtCore::FindFileInPathList("icons/import_large.png");
        LARGE_ICON_EDIT_REFRESH = dtCore::FindFileInPathList("icons/refresh_large.png");
        LARGE_ICON_EDIT_CATEGORY = dtCore::FindFileInPathList("icons/category_large.png");       

        ICON_SOUND_STOP = dtCore::FindFileInPathList("icons/stop_sound.png");
        ICON_SOUND_PLAY = dtCore::FindFileInPathList("icons/play_sound.png");

        ICON_EDITOR_SKELETAL_MESH   = dtCore::FindFileInPathList("icons/subeditor_skeletal.png");
        ICON_EDITOR_PARTICLE_SYSTEM = dtCore::FindFileInPathList("icons/particle_tab.png");
        ICON_EDITOR_VIEWER          = dtCore::FindFileInPathList("icons/viewer.png");       

        ICON_SOUND_RESOURCE = dtCore::FindFileInPathList("icons/sound_resource.png");
        ICON_STATICMESH_RESOURCE = dtCore::FindFileInPathList("icons/staticmesh_resource.png");
        ICON_CHARACTER_RESOURCE = dtCore::FindFileInPathList("icons/character_resource.png");
        ICON_TEXTURE_RESOURCE = dtCore::FindFileInPathList("icons/texture_resource.png");
        ICON_PARTICLE_RESOURCE = dtCore::FindFileInPathList("icons/particle_resource.png");
        ICON_TERRAIN_RESOURCE = dtCore::FindFileInPathList("icons/terrain_resource.png");

        ICON_SOUND_TAB =dtCore::FindFileInPathList ("icons/sound_tab.png");
        ICON_STATICMESH_TAB = dtCore::FindFileInPathList("icons/staticmesh_tab.png");
        ICON_CHARACTER_TAB = dtCore::FindFileInPathList("icons/character_tab.png");
        ICON_TEXTURE_TAB = dtCore::FindFileInPathList("icons/texture_tab.png");
        ICON_PARTICLE_TAB = dtCore::FindFileInPathList("icons/particle_tab.png");
        ICON_TERRAIN_TAB = dtCore::FindFileInPathList("icons/terrain_tab.png");

        ICON_ACTOR = dtCore::FindFileInPathList("icons/actor_icon.png");
        SPLASH_SCREEN = dtCore::FindFileInPathList("icons/splash.png");
        ICON_APPLICATION = dtCore::FindFileInPathList("icons/application_icon.png");
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

    std::string UIResources::ICON_EDITOR_SKELETAL_MESH("");
    std::string UIResources::ICON_EDITOR_PARTICLE_SYSTEM("");
    std::string UIResources::ICON_EDITOR_VIEWER("");

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

