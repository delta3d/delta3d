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
#include <dtDAL/basexml.h>
#include <dtDAL/projectconfigxmlhandler.h>
#include <osgDB/Registry>

#include <iostream>

namespace dtDAL
{
   class WrapperOSGObject : public osg::Object
   {
   public:
      WrapperOSGObject() : osg::Object() {}
      explicit WrapperOSGObject(bool threadSafeRefUnref) : osg::Object(threadSafeRefUnref) {}

      WrapperOSGObject(const osg::Object& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
      : osg::Object(obj, copyop)
      {}

      META_Object("dtDAL", WrapperOSGObject);
   };

   class ProjectConfigXMLParser : public BaseXMLParser
   {
   public:
      ProjectConfigXMLParser()
      : BaseXMLParser()
      {
         SetHandler(new ProjectConfigXMLHandler());
      }

      void Parse(std::istream& fin, dtCore::RefPtr<ProjectConfig> toFill)
      {
         if (BaseXMLParser::Parse(fin))
         {
            toFill = &(static_cast<ProjectConfigXMLHandler*>(GetHandler())->GetProjectConfig());
         }
         else
         {
            toFill = NULL;
         }
      }
   };

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
   osgDB::ReaderWriter::ReadResult ProjectConfigReaderWriter::readObject(const std::string& fileName,const osgDB::Options* options) const
   {
      if (!dtUtil::FileUtils::GetInstance().FileExists(fileName))
      {
         return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND);
      }

      std::ifstream confStream(fileName);

      if (!confStream.is_open())
      {
         return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE);
      }

      return readObject(confStream, options);
   }

   //////////////////////////////////////////////////////////////////////////////////
   osgDB::ReaderWriter::ReadResult ProjectConfigReaderWriter::readObject(std::istream& fin, const osgDB::Options* options) const
   {
      dtCore::RefPtr<ProjectConfigXMLParser> pcxml = new ProjectConfigXMLParser();

      dtCore::RefPtr<ProjectConfig> projConfig;
      pcxml->Parse(fin, projConfig);

      if (projConfig != NULL)
      {
         dtCore::RefPtr<WrapperOSGObject> obj = new WrapperOSGObject;
         obj->setUserData(ProjectConfig);
         osgDB::ReaderWriter::ReadResult result =
                  osgDB::ReaderWriter::ReadResult(
                           obj,
                           osgDB::ReaderWriter::ReadResult::FILE_LOADED);
         return result;
      }
      return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE);
   }

   //////////////////////////////////////////////////////////////////////////////////
   osgDB::ReaderWriter::WriteResult ProjectConfigReaderWriter::writeObject(const osg::Object& /*obj*/,const std::string& /*fileName*/,const osgDB::Options* =NULL) const
   {
   }

   //////////////////////////////////////////////////////////////////////////////////
   osgDB::ReaderWriter::WriteResult ProjectConfigReaderWriter::writeObject(const osg::Object& /*obj*/,std::ostream& /*fout*/,const osgDB::Options* =NULL) const
   {
   }

}

// now register with sgRegistry to instantiate the above
// reader/writer.
REGISTER_OSGPLUGIN(dtproj, dtDAL::ProjectConfigReaderWriter)

