/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2008 MOVES Institute
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
 * Author:
 * Edited By: Jeff P. Houde
 */
#ifndef DIRECTORQT_PLUGIN_INTERFACE
#define DIRECTORQT_PLUGIN_INTERFACE

#include <dtDirectorQt/export.h>
#include <list>
#include <string>

namespace dtDirector
{
   /**
      Abstract interface class for Director plugins
    */
   class DT_DIRECTOR_QT_EXPORT Plugin
   {
      public:

      virtual ~Plugin() {}

      /** Is called after instantiation */
      virtual void Create() {}

      /** Is called before destruction */
      virtual void Destroy() {}

   };

   /**
     A plugin factory is used by the plugin manager to identify available
     plugins and instantiate them.
   */
   class DT_DIRECTOR_QT_EXPORT PluginFactory
   {
   public:

      PluginFactory() {}

      virtual ~PluginFactory() {}

      /** construct the plugin and return a pointer to it */
      virtual Plugin* Create() = 0;

      /** delete the plugin */
      virtual void Destroy() = 0;

      /** get the name of the plugin */
      virtual std::string GetName() = 0;

      /** get a description of the plugin */
      virtual std::string GetDescription() = 0;

      /**
        fill list with names of all plugins this plugin depends on.
        WARNING: circular dependencies are not handled and
        will cause a crash!
      */
      virtual void GetDependencies(std::list<std::string>&) {};

      /**
         get the version of Director that the plugin is compiled against
         Only plugins compiled against the current version of Director
         are started
      */
      virtual std::string GetExpectedDirectorVersion()
      {
         // should be replaced by SVN to give version number
         return "$Revision$";
      }

      /** Should plugin be started autmatically? */
      virtual bool IsSystemPlugin()
      {
         return false;
      }

   protected:
   };

}

#endif //DIRECTORQT_PLUGIN_INTERFACE
