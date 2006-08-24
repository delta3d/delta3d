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
 * @author John K. Grant
 * @author David Guthrie
 */
#include <dtABC/applicationconfigdata.h>
#include <dtUtil/log.h>

namespace dtABC
{
   ApplicationConfigData::ApplicationConfigData() :
      WINDOW_X(0), WINDOW_Y(0),
      RESOLUTION(),
      SHOW_CURSOR(false), FULL_SCREEN(false), CHANGE_RESOLUTION(false),
      CAMERA_NAME(""), SCENE_NAME(""), WINDOW_NAME(""),
      SCENE_INSTANCE(""), WINDOW_INSTANCE("")
   {
   }
   
   ApplicationConfigData::~ApplicationConfigData()
   {
   }
}
