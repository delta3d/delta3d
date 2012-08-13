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
 * Author: Jeff P. Houde
 */

#ifndef DIRECTOR_TYPE_FACTORY
#define DIRECTOR_TYPE_FACTORY

#include <dtDirector/export.h>
#include <dtDirector/director.h>
#include <dtDirector/directorxml.h>

#include <dtUtil/objectfactory.h>

namespace dtDirector
{
   class DT_DIRECTOR_EXPORT DirectorTypeFactory
   {
   public:
      static const std::string DIRECTOR_EXTENSION_TEXT;
      static const std::string DIRECTOR_EXTENSION_BINARY;

      static DirectorTypeFactory* GetInstance();
      static void Destroy();

      /**
       * Creates an instance of the Director script class based on the
       * given script type.
       *
       * @param[in]  scriptType  The script type.
       *
       * @return     An instance of the script class of the given type.
       *             NULL If the type was not found in the registry.
       */
      dtCore::RefPtr<Director> CreateDirector(const std::string& scriptType);

      /**
       * Registers a Director script class with a script type.
       *
       * @param[in]  scriptType  The script type.
       */
      template <typename ScriptClassType>
      void RegisterScriptType(const std::string& scriptType)
      {
         mFactory->RemoveType(scriptType);
         mFactory->RegisterType<ScriptClassType>(scriptType);
      }

      /**
       * Unregisters a Director script class.
       *
       * @param[in]  scriptType  The script type.
       */
      void UnRegisterScriptType(const std::string& scriptType);

      /**
       * Retrieves a list of all registered script types.
       *
       * @param[out]  scriptTypes  The list of registered script types.
       */
      void GetScriptTypes(std::vector<std::string>& scriptTypes);

      /**
       * Loads a Director script.  An exception will occur on error.
       *
       * @param[in]  director     The pre-created director script to load into.
       * @param[in]  scriptFile   The name of the script file to load.
       * @param[in]  gm           The game manager.
       * @param[in]  map          The map.
       * @param[in]  cacheScript  True to cache the loaded script.
       * @param[in]  parent       The parent for this script.
       *
       * @return     The newly created Director script.
       */
      dtCore::RefPtr<Director> LoadScript(const std::string& scriptFile, dtGame::GameManager* gm = NULL, dtCore::Map* map = NULL, bool cacheScript = false, Director* parent = NULL);
      void LoadScript(Director* director, const std::string& scriptFile, bool cacheScript = false);

      /**
       * Saves a Director script.  An exception will occur on error.
       *
       * @param[in]  director     The director script to save.
       * @param[in]  scriptFile   The name of the script file to save.
       */
      void SaveScript(Director* director, const std::string& scriptFile);

      /**
       * Registers and Unregisters an instance of a loaded script.
       *
       * @param[in]  director  The script instance.
       */
      void RegisterScriptInstance(Director* director);
      void UnRegisterScriptInstance(Director* director);

      /**
       * Retrieves a list of all registered script instances.
       */
      const std::vector<Director*>& GetScriptInstances() const;

      /**
       * Adds a script to the cache.
       *
       * @param[in]  director  The script to add.
       *
       * @return     True if the script was cached properly.
       */
      bool AddCacheScript(Director* director);

      /**
       * Removes a script from the cache.
       *
       * @param[in]  script  The script path to remove.
       *
       * @return     True if the script was removed from the cache.
       */
      bool RemoveCacheScript(const std::string& script);

      /**
       * Retrieves a cached script if it exists.
       *
       * @param[in]  script  The script to retrieve.
       *
       * @return     The cached script, if it exists.
       */
      Director* GetCachedScript(const std::string& script);
      const Director* GetCachedScript(const std::string& script) const;

   private:
      DirectorTypeFactory();
      ~DirectorTypeFactory();

      // Object factory list for Director types.
      dtCore::RefPtr< dtUtil::ObjectFactory<std::string, Director> > mFactory;

      // A list of all loaded Director scripts.
      std::vector<Director*> mScriptInstances;

      std::vector<dtCore::RefPtr<DirectorParser> > mXMLParserList;
      int mCurrentXMLParser;

      std::map<std::string, dtCore::RefPtr<Director> > mCachedScripts;

      // Static instance of this class.
      static DirectorTypeFactory* mInstance;
   };
} // namespace dtDirector

#endif // DIRECTOR_TYPE_FACTORY
