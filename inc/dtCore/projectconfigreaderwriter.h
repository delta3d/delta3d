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

#ifndef PROJECTCONFIGREADERWRITER_H_
#define PROJECTCONFIGREADERWRITER_H_


#include <dtCore/export.h>

#include <osgDB/ReaderWriter>

#include <dtCore/projectconfig.h>

#include <iosfwd>

namespace dtCore
{

   class DT_CORE_EXPORT ProjectConfigReaderWriter : public osgDB::ReaderWriter
   {
   public:
      ProjectConfigReaderWriter();
      virtual ~ProjectConfigReaderWriter();

      virtual osgDB::ReaderWriter::ReadResult readObject(const std::string& /*fileName*/,const osgDB::ReaderWriter::Options*) const;
      /**
       * You need to set the current directory to the one where the file exists before calling this
       * because relative paths in the file are assumed to be relative to the file.
       */
      virtual osgDB::ReaderWriter::ReadResult readObject(std::istream& /*fin*/,const osgDB::ReaderWriter::Options*) const;

      virtual osgDB::ReaderWriter::WriteResult writeObject(const osg::Object& /*obj*/,const std::string& /*fileName*/,const osgDB::ReaderWriter::Options*) const;
      virtual osgDB::ReaderWriter::WriteResult writeObject(const osg::Object& /*obj*/,std::ostream& /*fout*/,const osgDB::ReaderWriter::Options* =NULL) const;

      static void LoadProjectConfig(const std::string& path, dtCore::RefPtr<ProjectConfig>& projectPtr);
      static void SaveProjectConfig(const std::string& path, const ProjectConfig& projectPtr);

   private:

   };

}

#endif /* PROJECTCONFIGREADERWRITER_H_ */
