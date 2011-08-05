/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Christopher DuBuc
 */

#ifndef DELTA_GAME_LAUNCH_LAUNCHER
#define DELTA_GAME_LAUNCH_LAUNCHER


#include <dtGame/gameapplication.h>

class ParameterParser;

/*
 * This class encapsulates the code needed for launching a Delta3D simulation
 * constructed as a dtGame::GameApplication library. It provides a standard
 * method for launching a library whether called from native or Java code.
 */
class Launcher
{
   private:
      int mArgc;
      char** mArgv;
      ParameterParser* mParser; //used to parse the command line arguments

   public:

      /*
       * Constructs the Launcher object
       *
       * @param argc The number of command line arguments in the argv array
       * @param argv An array of command line arguments passed in to the dtGameLaunch
       *    library from either a native or Java executable; These arguments will
       *    also be passed on to the game library.
       */
      Launcher(int argc, char** argv);

      /*
       * This method sets the working directory for the main process. This is
       * useful because Java Web Start will launch with the JWS cache as the
       * working directory, which is obfuscated from the user and should not b
       * manipulated directly.
       *
       * @param workingDirectory The working directory to be set as the root
       *    application directory.
       * @return A boolean value specifying whether the working directory was
       *    set by the method.
       */
      bool SetWorkingDirectory(const char* workingDirectory);

      /*
       * Overload for SetWorkingDirectory() which takes a string parameter instead
       * of a char*.
       *
       * @param workingDirectory The working directory to be set as the root
       *    application directory.
       *
       * @return A boolean value specifying whether the the working directory was
       *    set by the method.
       */
      bool SetWorkingDirectory(const std::string &workingDirectory);

      /*
       * This method prepends the working directory to the environment path
       * for the main process. This allows the application to find any needed
       * code libraries (.dll/.so's) that need to be dynamically loaded.
       *
       * @param workingDirectory The directory where code libraries (.dll/.so's) can
       *    be found.
       */
      void PrependWorkingDirToPath(const char* workingDirectory);

      /*
       * Overload for PrependWorkingDirToPath that takes a string parameter
       * instead of a char*.
       *
       * @param workingDirectory The directory where code libraries (.dll/.so's) can
       *    be found
       */
      void PrependWorkingDirToPath(const std::string &workingDirectory);

      /*
       * This method launches the game library via dtGame::GameApplication
       */
      void Launch();

      /*
       * Destructs the Launcher object
       */
      virtual ~Launcher();
};

#endif
