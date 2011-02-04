/* -*-c++-*-
 * Delta3D
 * Copyright 2011, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * 
 * David Guthrie
 */

#include <dtDAL/projectconfigreaderwriter.h>
#include <osgDB/Registry>

#include <iostream>

namespace dtDAL
{

   //////////////////////////////////////////////////////////////////////////////////
   ProjectConfigReaderWriter::ProjectConfigReaderWriter()
   {
       supportsExtension("dtproj","delta3d project config file.");
   }

   //////////////////////////////////////////////////////////////////////////////////
   ProjectConfigReaderWriter::~ProjectConfigReaderWriter()
   {
   }

   //////////////////////////////////////////////////////////////////////////////////
   osgDB::ReadResult ProjectConfigReaderWriter::readObject(const std::string& fileName,const osgDB::Options* options) const
   {
      if (!dtUtil::FileUtils::GetInstance().FileExists(fileName))
      {
         return osgDB::ReadResult(osgDB::ReadResult::FILE_NOT_FOUND);
      }

      std::ifstream confStream(fileName);

      if (!confStream.is_open())
      {
         return osgDB::ReadResult(osgDB::ReadResult::ERROR_IN_READING_FILE);
      }

      return readObject(confStream, options);
   }

   //////////////////////////////////////////////////////////////////////////////////
   osgDB::ReadResult ProjectConfigReaderWriter::readObject(std::istream& fin, const osgDB::Options* options) const
   {

   }

   //////////////////////////////////////////////////////////////////////////////////
   osgDB::WriteResult ProjectConfigReaderWriter::writeObject(const osg::Object& /*obj*/,const std::string& /*fileName*/,const osgDB::Options* =NULL) const
   {
   }

   //////////////////////////////////////////////////////////////////////////////////
   osgDB::WriteResult ProjectConfigReaderWriter::writeObject(const osg::Object& /*obj*/,std::ostream& /*fout*/,const osgDB::Options* =NULL) const
   {

   }

}

// now register with sgRegistry to instantiate the above
// reader/writer.
REGISTER_OSGPLUGIN(dtproj, dtDAL::ProjectConfigReaderWriter)

