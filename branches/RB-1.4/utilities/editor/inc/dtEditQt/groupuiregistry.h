/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine 
 * Simulation, Training, and Game Editor (STAGE)
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation 
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free 
 * Software Foundation; either version 2 of the License, or (at your option) 
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
 * David Guthrie
 */

#ifndef DELTA_GROUP_UI_REGISTRY
#define DELTA_GROUP_UI_REGISTRY

#include <map>
#include <osg/Referenced>
#include <dtCore/refptr.h>


namespace dtEditQt 
{
   class GroupUIPlugin;

   class GroupUIRegistry : public osg::Referenced
   {
      public:
         GroupUIRegistry();
         
         /**
          * Registers a plugin.
          * @param plugin the plugin to register.
          */
         void RegisterPlugin(GroupUIPlugin& plugin);
         /**
          * Unregisters a plugin by name.
          * @param pluginName the name of the plugin to unregister.
          */
         void UnregisterPlugin(const std::string& pluginName);

         ///@return a pointer to the plugin with the give name or NULL if none exists.
         GroupUIPlugin* GetPlugin(const std::string& pluginName);
         ///@return a const pointer to the plugin with the give name or NULL if none exists.
         const GroupUIPlugin* GetPlugin(const std::string& pluginName) const;
         
      private:
         ///private destructor since it's referenced.
         ~GroupUIRegistry();
         //disallow copies and assignments.
         GroupUIRegistry(const GroupUIRegistry& toCopy);
         GroupUIRegistry& operator=(const GroupUIRegistry& toCopy);
         
         std::map<std::string, GroupUIPlugin* > mPlugins;
   };
}

#endif