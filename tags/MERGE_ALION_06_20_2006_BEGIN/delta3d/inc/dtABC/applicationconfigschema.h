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

#ifndef DELTA_APPLICATION_CONFIG_SCHEMA
#define DELTA_APPLICATION_CONFIG_SCHEMA

#include <dtABC/export.h>                   // for export symbol
#include <string>                           // for data members

namespace dtABC
{
   /// defines API used to model the XML schema for the config file.
   struct DT_ABC_EXPORT ApplicationConfigSchema
   {
   public:
      static const std::string WINDOW;
      static const std::string NAME;
      static const std::string SCENE;
      static const std::string CAMERA;

      static const std::string X;
      static const std::string Y;
      static const std::string WIDTH;
      static const std::string HEIGHT;

      static const std::string PIXELDEPTH;
      static const std::string REFRESHRATE;
      static const std::string SHOWCURSOR;
      static const std::string FULLSCREEN;
      static const std::string CHANGEDISPLAYRESOLUTION;

      static const std::string WINDOWINSTANCE;
      static const std::string SCENEINSTANCE;
   };
}

#endif  // DELTA_APPLICATION_CONFIG_SCHEMA
