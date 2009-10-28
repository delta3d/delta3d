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

#ifndef DELTA_GAME_LAUNCH_PARAMETER_PARSER
#define DELTA_GAME_LAUNCH_PARAMETER_PARSER

#include <string>

/*
 * This utility class is used to read in and parse an array of command line arguments
 */
class ParameterParser
{
   public:

      /*
       * Constructs the ParameterParser.
       *
       * @param argc The number of command line arguments in argv
       * @param argv An array of command line arguments
       */
      ParameterParser(int argc, char** argv);

      /*
       * Destructs the ParameterParser
       */
      virtual ~ParameterParser();

      //accessors

      /*
       * This accessor method returns the application library file specified
       * in the command line arguments with a '-a' token
       *
       * @return A string containing the name of the game library
       */
      const std::string& GetApplicationLibraryFile() const { return mApplicationLibraryFile; }

      /*
       * This accessor method returns the working directory specified
       * in the command line arguments with a '-w' token
       *
       * @return A string containing the name of the working directory
       */
      const std::string& GetWorkingDirectory() const { return mWorkingDirectory; }

      /*
       * This accessor method returns the data directory specified
       * in the command line arguments with a '-d' token
       *
       * @return A string containing the name of the data directory
       */
      const std::string& GetDataDirectory() const { return mDataDirectory; }

      /*
       * This accessor method returns the JavaLaunch configuration file specified
       * in the command line arguments with a '-c' token
       *
       * @return A string containing the name of the JavaLaunch configuration file
       */
      const std::string& GetConfigFile() const { return mConfigFile; }

   private:
      //constants
      const std::string APPLICATION_LIB_TOKEN; // -a
      const std::string WORKING_DIR_TOKEN; // -w
      const std::string DATA_DIR_TOKEN; // -d
      const std::string CONFIG_FILE_TOKEN; // -c

      //parameters
      std::string mApplicationLibraryFile;
      std::string mWorkingDirectory;
      std::string mDataDirectory;
      std::string mConfigFile;

      /*
       * This method is called in the constructor and looks for specific parameters in
       * the command line arguments.
       *
       * @param argc The number of command line arguments in argv
       * @param argv An array of command line arguments
       */
      void Parse(int argc, char** argv);

      /*
       * This method retrieves a specific parameter from the command line arguments based
       * on the provided token. Specifically, the method looks for the token in the array,
       * then returns the array value that comes after the token as the parameter (if it
       * exists).
       * @param parameterToken The token used to specify the parameter.
       * @param arrayLength The number of command line arguments in paramArray.
       * @param paramArray An array of command line arguments.
       *
       * @return A string containing the parameter specified by the given parameter token
       */
      std::string GetParameter(const std::string &parameterToken, unsigned int arrayLength, char** paramArray) const;
};

#endif
