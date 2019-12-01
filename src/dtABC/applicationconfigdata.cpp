/* -*-c++-*- 
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
 * John K. Grant
 * David Guthrie
 */
#include <dtABC/applicationconfigdata.h>

namespace dtABC
{
   ApplicationConfigData::ApplicationConfigData()
   : WINDOW_X(0)
   , WINDOW_Y(0)
   , RESOLUTION()
   , SHOW_CURSOR(false)
   , FULL_SCREEN(false)
   , CHANGE_RESOLUTION(false)
   , REALIZE_UPON_CREATE(false)
   , VSYNC(true)
   , HIDE_WINDOWS_CONSOLE(false)
   , CAMERA_NAME("")
   , VIEW_NAME("")
   , SCENE_NAME("")
   , WINDOW_NAME("")
   , CAMERA_INSTANCE("")
   , SCENE_INSTANCE("")
   , WINDOW_INSTANCE("")
   , VIEWPORT_X(0)
   , VIEWPORT_Y(0)
   , VIEWPORT_W(-1)
   , VIEWPORT_H(-1)
   , MULTI_SAMPLE(0)
   , GLOBAL_LOG_LEVEL("Warn")
   {
   }
   
   ApplicationConfigData::~ApplicationConfigData()
   {
   }
}
