/*
 * defaultgameentrypoint.h
 *
 *  Created on: Sep 28, 2015
 *      Author: david
 */

#ifndef INC_DTGAME_DEFAULTGAMEENTRYPOINT_H_
#define INC_DTGAME_DEFAULTGAMEENTRYPOINT_H_

#include <dtGame/export.h>
#include <dtGame/gameentrypoint.h>
#include <dtUtil/getsetmacros.h>
#include <osg/ArgumentParser>
#include <dtGame/gamemanager.h>
#include <memory>

namespace dtGame
{

   class DT_GAME_EXPORT DefaultGameEntryPoint: public GameEntryPoint
   {
   public:
      DefaultGameEntryPoint();
      ~DefaultGameEntryPoint() /*override*/;

      /**
       * Parses a default command line.
       */
      void Initialize(dtABC::BaseABC& app, int argc, char **argv) override;
      void OnStartup(dtABC::BaseABC& app, dtGame::GameManager& gameManager) override;

      DT_DECLARE_ACCESSOR(std::string, ProjectPath)
      DT_DECLARE_ACCESSOR(std::string, MapName)
      DT_DECLARE_ACCESSOR(std::string, BaseMapName)
      /**
       * If this is true, it will still parse the command line options if you them, but any map names
       * set aren't loaded.  This allows a subclass to handle the maps on its own.
       */
      DT_DECLARE_ACCESSOR(bool, MapIsRequired)
   protected:
      osg::ArgumentParser* GetOrCreateArgParser(int argc, char **argv);
      osg::ArgumentParser* GetArgParser();
      void ParseDefaultCommandLineOptions();
      virtual void SetupProjectContext();
      void ValidateMap(const std::string& mapName);
      /**
       * This should load the maps passed in, but it won't be called if "MapIsRequired" is false.
       * By default, the vector will have BaseMap, then Map in it, so a subclass can add maps wherever.
       */
      virtual void LoadMaps(dtGame::GameManager& gameManager, dtGame::GameManager::NameVector& mapNames);
   private:
      std::unique_ptr<osg::ArgumentParser> mArgParser;
      // have to hold onto an argc because the arg parser keeps a pointer to it.
      int mArgc;
   };

} /* namespace dtGame */

#endif /* INC_DTGAME_DEFAULTGAMEENTRYPOINT_H_ */
