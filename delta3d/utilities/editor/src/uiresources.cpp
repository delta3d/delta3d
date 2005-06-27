/*
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
 * @author Matthew W. Campbell
*/
#include "dtEditQt/uiresources.h"


namespace dtEditQt 
{
   //absolute paths
   //const std::string deltaRoot = "DELTA_ROOT";
   //char* ptr = getenv( deltaRoot.c_str() );
   //const std::string UIResources::mPrefix(std::string(ptr) + "/utilities/editor/icons/");

   //relative path from delta3d/bin
   const std::string UIResources::mPrefix("../utilities/editor/icons/");

   const std::string UIResources::ICON_FILE_NEW_MAP(mPrefix+"newmap_small.png");
   const std::string UIResources::ICON_FILE_OPEN_MAP(mPrefix+"openmap_small.png");
   const std::string UIResources::ICON_FILE_SAVE(mPrefix+"savemap_small.png");

   const std::string UIResources::ICON_EDIT_DUPLICATE(mPrefix+"duplicate_small.png");
   const std::string UIResources::ICON_EDIT_DELETE(mPrefix+"trash_small.png");
   const std::string UIResources::ICON_GROUND_CLAMP(mPrefix+"groundclamp_small.png");
   const std::string UIResources::ICON_EDIT_UNDO(mPrefix+"undo.png");
   const std::string UIResources::ICON_EDIT_REDO(mPrefix+"redo.png");
   
   const std::string UIResources::ICON_TOOLMODE_CAMERA(mPrefix+"cameramode_small.png");
   const std::string UIResources::ICON_TOOLMODE_SELECT(mPrefix+"selectmode_small.png");
   const std::string UIResources::ICON_TOOLMODE_TRANSLATE(mPrefix+"translatemode_small.png");
   const std::string UIResources::ICON_TOOLMODE_ROTATE(mPrefix+"rotatemode_small.png");
   
   const std::string UIResources::ICON_VIEWMODE_TEXTURES(mPrefix+"texturemode_tiny.png");
   const std::string UIResources::ICON_VIEWMODE_LIGHTING(mPrefix+"lightingmode_tiny.png");
   const std::string UIResources::ICON_VIEWMODE_WIREFRAME(mPrefix+"wireframemode_tiny.png");
   const std::string UIResources::ICON_VIEWMODE_TEXTURES_AND_LIGHTING(mPrefix+"texturelightingmode_tiny.png");
   
   const std::string UIResources::ICON_TINY_FOLDER(mPrefix+"folder_tiny.png");
   const std::string UIResources::ICON_TINY_FOLDER_OPEN(mPrefix+"folderopen_tiny.png");
   
   const std::string UIResources::LARGE_ICON_EDIT_DUPLICATE(mPrefix+"duplicate_large.png");
   const std::string UIResources::LARGE_ICON_EDIT_DELETE(mPrefix+"trash_large.png");
   const std::string UIResources::LARGE_ICON_EDIT_GOTO(mPrefix+"goto_large.png");
   const std::string UIResources::LARGE_ICON_EDIT_IMPORT(mPrefix+"import_large.png");
   const std::string UIResources::LARGE_ICON_EDIT_REFRESH(mPrefix+"refresh_large.png");
   const std::string UIResources::LARGE_ICON_EDIT_CATEGORY(mPrefix+"category_large.png");

   const std::string UIResources::ICON_SOUND_STOP(mPrefix+"stop_sound.png");
   const std::string UIResources::ICON_SOUND_PLAY(mPrefix+"play_sound.png");

   const std::string UIResources::SPLASH_SCREEN(mPrefix+"splash.png");
   
}
