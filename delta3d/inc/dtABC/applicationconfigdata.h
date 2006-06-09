/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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
 * @author John K. Grant
*/

#ifndef DELTA_APPLICATION_CONFIG_DATA
#define DELTA_APPLICATION_CONFIG_DATA

#include <dtABC/export.h>                   // for export symbol
#include <dtCore/deltawin.h>                // for internal class definition DeltaWin::Resolution
#include <string>                           // for data members

namespace dtABC
{
   /// defines API used to obtain values of the config file.
   struct DT_ABC_EXPORT ApplicationConfigData
   {
      ApplicationConfigData();
      ~ApplicationConfigData();

      int WINDOW_X, WINDOW_Y;
      dtCore::DeltaWin::Resolution RESOLUTION;
      bool SHOW_CURSOR, FULL_SCREEN, CHANGE_RESOLUTION;
      std::string CAMERA_NAME, SCENE_NAME, WINDOW_NAME;
      std::string SCENE_INSTANCE, WINDOW_INSTANCE;
   };
}

#endif  // DELTA_APPLICATION_CONFIG_DATA
