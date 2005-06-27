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

            static const std::string ICON_FILE_NEW_MAP;
            static const std::string ICON_FILE_OPEN_MAP;
            static const std::string ICON_FILE_SAVE;

            static const std::string ICON_EDIT_DUPLICATE;
            static const std::string ICON_EDIT_DELETE;
            static const std::string ICON_GROUND_CLAMP;
            static const std::string ICON_EDIT_UNDO;
            static const std::string ICON_EDIT_REDO;

            static const std::string ICON_TOOLMODE_CAMERA;
            static const std::string ICON_TOOLMODE_SELECT;
            static const std::string ICON_TOOLMODE_TRANSLATE;
            static const std::string ICON_TOOLMODE_ROTATE;

            static const std::string ICON_VIEWMODE_TEXTURES;
            static const std::string ICON_VIEWMODE_LIGHTING;
            static const std::string ICON_VIEWMODE_WIREFRAME;
            static const std::string ICON_VIEWMODE_TEXTURES_AND_LIGHTING;

            static const std::string ICON_TINY_FOLDER;
            static const std::string ICON_TINY_FOLDER_OPEN;

            static const std::string LARGE_ICON_EDIT_DUPLICATE;
            static const std::string LARGE_ICON_EDIT_DELETE;
            static const std::string LARGE_ICON_EDIT_GOTO;
            static const std::string LARGE_ICON_EDIT_IMPORT;
            static const std::string LARGE_ICON_EDIT_REFRESH;
            static const std::string LARGE_ICON_EDIT_CATEGORY;

            static const std::string ICON_SOUND_STOP;
            static const std::string ICON_SOUND_PLAY;

            static const std::string SPLASH_SCREEN;

       private:

            static const std::string mPrefix;
    };
   
}

#endif
