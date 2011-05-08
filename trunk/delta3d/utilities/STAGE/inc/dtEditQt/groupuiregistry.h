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

#ifndef DELTA_GROUP_UI_REGISTRY
#define DELTA_GROUP_UI_REGISTRY

#include <dtEditQt/export.h>

#include <map>
#include <osg/Referenced>
#include <dtCore/refptr.h>

namespace dtEditQt
{
   class BaseUIPlugin;

   class DT_EDITQT_EXPORT GroupUIRegistry : public osg::Referenced
   {
   public:
      GroupUIRegistry();

      /**
       * Registers a plugin.
       * @param plugin the plugin to register.
       */
      void RegisterPlugin(BaseUIPlugin& plugin);

      /**
       * Unregisters a plugin by name.
       * @param pluginName the name of the plugin to unregister.
       */
      void UnregisterPlugin(const std::string& pluginName);

      ///@return a pointer to the plugin with the give name or NULL if none exists.
      BaseUIPlugin* GetPlugin(const std::string& pluginName);

      ///@return a const pointer to the plugin with the give name or NULL if none exists.
      const BaseUIPlugin* GetPlugin(const std::string& pluginName) const;

   private:
      ///private destructor since it's referenced.
      ~GroupUIRegistry();
      // disallow copies and assignments.
      GroupUIRegistry(const GroupUIRegistry& toCopy);
      GroupUIRegistry& operator=(const GroupUIRegistry& toCopy);

      std::map<std::string, BaseUIPlugin*> mPlugins;
   };

} // namespace dtEditQt

#endif // DELTA_GROUP_UI_REGISTRY
