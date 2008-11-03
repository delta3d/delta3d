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
        UIResources::GPL_LICENSE_FILE = ":gpl.txt";
        
        ICON_FILE_NEW_MAP = ":/icons/newmap_small.png";
        ICON_FILE_OPEN_MAP = ":/icons/openmap_small.png";
        ICON_FILE_SAVE = ":/icons/savemap_small.png";

        ICON_EDIT_DUPLICATE = ":/icons/duplicate_small.png";
        ICON_EDIT_DELETE = ":/icons/trash_small.png";
        ICON_GROUND_CLAMP = ":/icons/groundclamp_small.png";
        ICON_EDIT_UNDO = ":/icons/undo.png";
        ICON_EDIT_REDO = ":/icons/redo.png";

        ICON_TOOLMODE_CAMERA = ":/icons/cameramode_small.png";
        ICON_TOOLMODE_SELECT = ":/icons/selectmode_small.png";
        ICON_TOOLMODE_TRANSLATE = ":/icons/translatemode_small.png";
        ICON_TOOLMODE_ROTATE = ":/icons/rotatemode_small.png";

        ICON_VIEWMODE_TEXTURES = ":/icons/texturemode_tiny.png";
        ICON_VIEWMODE_LIGHTING = ":/icons/lightingmode_tiny.png";
        ICON_VIEWMODE_WIREFRAME = ":/icons/wireframemode_tiny.png";
        ICON_VIEWMODE_TEXTURES_AND_LIGHTING = ":/icons/texturelightingmode_tiny.png";

        ICON_TINY_FOLDER = ":/icons/folder_tiny.png";
        ICON_TINY_FOLDER_OPEN = ":/icons/folderopen_tiny.png";

        LARGE_ICON_EDIT_DUPLICATE = ":/icons/duplicate_large.png";
        LARGE_ICON_EDIT_DELETE = ":/icons/trash_large.png";
        LARGE_ICON_EDIT_GOTO = ":/icons/goto_large.png";
        LARGE_ICON_EDIT_IMPORT = ":/icons/import_large.png";
        LARGE_ICON_EDIT_REFRESH = ":/icons/refresh_large.png";
        LARGE_ICON_EDIT_CATEGORY = ":/icons/category_large.png";       

        ICON_SOUND_STOP = ":/icons/stop_sound.png";
        ICON_SOUND_PLAY = ":/icons/play_sound.png";

        ICON_EDITOR_SKELETAL_MESH   = ":/icons/subeditor_skeletal.png";
        ICON_EDITOR_PARTICLE_SYSTEM = ":/icons/particle.ico";
        ICON_EDITOR_VIEWER          = ":/icons/viewer.ico";       

        ICON_SOUND_RESOURCE = ":/icons/sound_resource.png";
        ICON_STATICMESH_RESOURCE = ":/icons/staticmesh_resource.png";
        ICON_CHARACTER_RESOURCE = ":/icons/character_resource.png";
        ICON_TEXTURE_RESOURCE = ":/icons/texture_resource.png";
        ICON_PARTICLE_RESOURCE = ":/icons/particle_resource.png";
        ICON_TERRAIN_RESOURCE = ":/icons/terrain_resource.png";

        ICON_SOUND_TAB = ":/icons/sound_tab.png";
        ICON_STATICMESH_TAB = ":/icons/staticmesh_tab.png";
        ICON_CHARACTER_TAB = ":/icons/character_tab.png";
        ICON_TEXTURE_TAB = ":/icons/texture_tab.png";
        ICON_PARTICLE_TAB = ":/icons/particle_tab.png";
        ICON_TERRAIN_TAB = ":/icons/terrain_tab.png";

        ICON_ACTOR = ":/icons/actor_icon.png";
        SPLASH_SCREEN = ":/icons/splash.png";
        ICON_APPLICATION = ":/icons/application_icon.png";
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

