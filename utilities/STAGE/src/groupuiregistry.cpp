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
#include <prefix/dtstageprefix-src.h>
#include <dtEditQt/groupuiregistry.h>
#include <dtEditQt/groupuiplugin.h>
#include <dtUtil/log.h>

namespace dtEditQt 
{
   ///////////////////////////////////////////////////////////////////////////////
   GroupUIRegistry::GroupUIRegistry(){}
   ///////////////////////////////////////////////////////////////////////////////
   GroupUIRegistry::~GroupUIRegistry()
   {}
         
   ///////////////////////////////////////////////////////////////////////////////
   void GroupUIRegistry::RegisterPlugin(GroupUIPlugin& plugin)
   {
      LOG_ALWAYS("Got a GroupUIPlugin with name \"" + plugin.GetName() + "\".");
      mPlugins.insert(std::make_pair(plugin.GetName(), &plugin));
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   void GroupUIRegistry::UnregisterPlugin(const std::string& pluginName)
   {
      std::map<std::string, GroupUIPlugin* >::iterator i = mPlugins.find(pluginName);
      if (i != mPlugins.end())
      {
         //The instances are held onto internally by QT, so we don't delete them.
         mPlugins.erase(i);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   GroupUIPlugin* GroupUIRegistry::GetPlugin(const std::string& pluginName)
   {
      std::map<std::string, GroupUIPlugin* >::iterator i = mPlugins.find(pluginName);
      if (i != mPlugins.end())
      {
         return i->second;
      }
      return NULL;
   }
   
   ///////////////////////////////////////////////////////////////////////////////
   const GroupUIPlugin* GroupUIRegistry::GetPlugin(const std::string& pluginName) const
   {
      std::map<std::string, GroupUIPlugin* >::const_iterator i = mPlugins.find(pluginName);
      if (i != mPlugins.end())
      {
         return i->second;
      }
      return NULL;
   }
         
   ///////////////////////////////////////////////////////////////////////////////
   //disallow copies and assignments.
   GroupUIRegistry::GroupUIRegistry(const GroupUIRegistry& toCopy) {}
   GroupUIRegistry& GroupUIRegistry::operator=(const GroupUIRegistry& toCopy) { return *this; }
}
