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
 * David Guthrie
 */
#include <prefix/stageprefix.h>
#include <dtEditQt/groupuiregistry.h>
#include <dtEditQt/groupuiplugin.h>
#include <dtUtil/log.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   GroupUIRegistry::GroupUIRegistry() {}

   ///////////////////////////////////////////////////////////////////////////////
   GroupUIRegistry::~GroupUIRegistry() {}

   ///////////////////////////////////////////////////////////////////////////////
   void GroupUIRegistry::RegisterPlugin(BaseUIPlugin& plugin)
   {
      LOG_DEBUG("Got a GroupUIPlugin with name \"" + plugin.GetName() + "\".");
      mPlugins.insert(std::make_pair(plugin.GetName(), &plugin));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void GroupUIRegistry::UnregisterPlugin(const std::string& pluginName)
   {
      std::map<std::string, BaseUIPlugin*>::iterator i = mPlugins.find(pluginName);
      if (i != mPlugins.end())
      {
         // The instances are held onto internally by QT, so we don't delete them.
         mPlugins.erase(i);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   BaseUIPlugin* GroupUIRegistry::GetPlugin(const std::string& pluginName)
   {
      std::map<std::string, BaseUIPlugin*>::iterator i = mPlugins.find(pluginName);
      if (i != mPlugins.end())
      {
         return i->second;
      }
      return NULL;
   }

   ///////////////////////////////////////////////////////////////////////////////
   const BaseUIPlugin* GroupUIRegistry::GetPlugin(const std::string& pluginName) const
   {
      std::map<std::string, BaseUIPlugin*>::const_iterator i = mPlugins.find(pluginName);
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

} // namespace dtEditQt
