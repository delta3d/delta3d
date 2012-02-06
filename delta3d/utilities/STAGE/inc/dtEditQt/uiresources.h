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
#ifndef __UIResources__h
#define __UIResources__h

#include <string>

#include <dtEditQt/export.h>

namespace dtEditQt
{

   /**
    * This class contains static strings that define locations to resources such as
    * icons.
    */
   class DT_EDITQT_EXPORT UIResources
   {
   public:
      static void staticInitialize();

      static std::string LICENSE_FILE;

      static std::string ICON_FILE_NEW_MAP;
      static std::string ICON_FILE_OPEN_MAP;
      static std::string ICON_FILE_SAVE;

      static std::string ICON_EDIT_LOCAL_SPACE;
      static std::string ICON_EDIT_DUPLICATE;
      static std::string ICON_EDIT_DELETE;
      static std::string ICON_GROUND_CLAMP;
      static std::string ICON_EDIT_GROUP;
      static std::string ICON_EDIT_UNGROUP;
      static std::string ICON_EDIT_UNDO;
      static std::string ICON_EDIT_REDO;
      static std::string ICON_EDIT_RESET_TRANSLATION;
      static std::string ICON_EDIT_RESET_ROTATION;
      static std::string ICON_EDIT_RESET_SCALE;

      static std::string ICON_BRUSH_CUBE;
      static std::string ICON_BRUSH_SPHERE;
      static std::string ICON_BRUSH_RESET;

      static std::string ICON_EYE;

      static std::string ICON_TOOLMODE_NORMAL;

      static std::string ICON_VIEWMODE_TEXTURES;
      static std::string ICON_VIEWMODE_LIGHTING;
      static std::string ICON_VIEWMODE_WIREFRAME;
      static std::string ICON_VIEWMODE_TEXTURES_AND_LIGHTING;

      static std::string ICON_VIEWCONTROL_CAMERA_SPEED;
      static std::string ICON_VIEWCONTROL_CLIP_DISTANCE;

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

      static std::string ICON_EDITOR_SKELETAL_MESH;
      static std::string ICON_EDITOR_PARTICLE_SYSTEM;
      static std::string ICON_EDITOR_VIEWER;

      static std::string ICON_SOUND_RESOURCE;
      static std::string ICON_STATICMESH_RESOURCE;
      static std::string ICON_TEXTURE_RESOURCE;
      static std::string ICON_CHARACTER_RESOURCE;
      static std::string ICON_PARTICLE_RESOURCE;
      static std::string ICON_TERRAIN_RESOURCE;
      static std::string ICON_DIRECTOR_RESOURCE;

      static std::string ICON_SOUND_TAB;
      static std::string ICON_STATICMESH_TAB;
      static std::string ICON_TEXTURE_TAB;
      static std::string ICON_CHARACTER_TAB;
      static std::string ICON_PARTICLE_TAB;
      static std::string ICON_TERRAIN_TAB;

      static std::string ICON_ACTOR;
      static std::string SPLASH_SCREEN;
      static std::string ICON_APPLICATION;

      static std::string ICON_NO_ICON;
      static std::string ICON_UP_FOLDER;
      static std::string ICON_FOLDER;
   };

} // namespace dtEditQt

#endif // __UIResources__h
