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

#include "ParameterParser.h"

ParameterParser::ParameterParser(int argc, char** argv) : 
   APPLICATION_LIB_TOKEN("-a"),
   WORKING_DIR_TOKEN("-w"),
   DATA_DIR_TOKEN("-d"),
   CONFIG_FILE_TOKEN("-c")
{
   Parse(argc, argv);
}

void ParameterParser::Parse(int argc, char** argv)
{
   mApplicationLibraryFile = GetParameter(APPLICATION_LIB_TOKEN, argc, argv);
   mWorkingDirectory = GetParameter(WORKING_DIR_TOKEN, argc, argv);
   mDataDirectory = GetParameter(DATA_DIR_TOKEN, argc, argv);
   mConfigFile = GetParameter(CONFIG_FILE_TOKEN, argc, argv);

   //cout << "Application Library: " << mApplicationLibraryFile << endl;
   //cout << "Working Directory: " << mWorkingDirectory << endl;
   //cout << "Data Directory: " << mDataDirectory << endl;
}

std::string ParameterParser::GetParameter(const std::string &parameterToken, unsigned int arrayLength, char** paramArray) const
{
   std::string returnValue;

   for(unsigned int i = 0; i < arrayLength; i++)
   {
      if(parameterToken == paramArray[i])
      {
         if((i + 1) < arrayLength)
         {
            returnValue = paramArray[i + 1];
            break;
         }
      }
   }

   return returnValue;
}

ParameterParser::~ParameterParser()
{
}
