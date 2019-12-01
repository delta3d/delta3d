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
 * John K. Grant
 */

#ifndef DELTA_APP_XML_CONFIG_WRITER
#define DELTA_APP_XML_CONFIG_WRITER

#include <dtUtil/log.h>                     // to read the default log name.
#include <dtABC/export.h>                   // for export symbols
#include <string>                           // for data members
#include <xercesc/util/XercesDefs.hpp>      // for XMLCh definition

namespace dtABC
{
   struct ApplicationConfigData;
   
   /// A class that writes config files for the dtABC::Application
   class DT_ABC_EXPORT ApplicationConfigWriter
   {
   public:
      void operator ()(const std::string& filename, const ApplicationConfigData& data);

   private:

      /// A utility class to create xerces character types for the XML schema.
      /// Defines the API to obtain values used when parsing the config file.
      /// Also generates the xerces character types needed for string operations.
      struct SchemaModel
      {
         SchemaModel();
         ~SchemaModel();

         XMLCh* WINDOW;
         XMLCh* NAME;
         XMLCh* SCENE;
         XMLCh* CAMERA;
         XMLCh* VIEW;
         XMLCh* LOG;
         XMLCh* LIBRARY_PATH;

         XMLCh* X;
         XMLCh* Y;
         XMLCh* WIDTH;
         XMLCh* HEIGHT;

         XMLCh* PIXELDEPTH;
         XMLCh* REFRESHRATE;
         XMLCh* SHOWCURSOR;
         XMLCh* FULLSCREEN;
         XMLCh* REALIZE_UPON_CREATE;
         XMLCh* CHANGEDISPLAYRESOLUTION;
         XMLCh* VSYNC;
         XMLCh* HIDE_WINDOWS_CONSOLE;
         XMLCh* MULTI_SAMPLE;

         XMLCh* WINDOWINSTANCE;
         XMLCh* SCENEINSTANCE;
         XMLCh* CAMERAINSTANCE;

         XMLCh* GLOBAL_LOG_LEVEL;
         XMLCh* LOG_LEVEL;

         XMLCh* APP_PROPERTIES;
         XMLCh* APP_PROPERTY;
      };
   };
}

#endif  // DELTA_APP_XML_CONFIG_WRITER
