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
 * Chris Rodgers
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <prefix/dtdalprefix.h>
#include <osgDB/FileNameUtils>
#include <dtDAL/basexml.h>
#include <dtDAL/basexmlhandler.h>
#include <dtDAL/BaseXMLReaderWriter.h>
#include <dtUtil/fileutils.h>



namespace dtDAL
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   BaseXMLReaderWriter::BaseXMLReaderWriter()
   {  
   }

   /////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtDAL::BaseXMLParser> BaseXMLReaderWriter::CreateParser() const
   {
      return new dtDAL::BaseXMLParser();
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLReaderWriter::SetSchemaFile(const std::string& schemaFile)
   {
      mSchemaFile = schemaFile;
   }

   /////////////////////////////////////////////////////////////////
   const std::string& BaseXMLReaderWriter::GetSchemaFile() const
   {
      return mSchemaFile;
   }

   /////////////////////////////////////////////////////////////////////////////
   void BaseXMLReaderWriter::InitParser(dtDAL::BaseXMLParser& parser, dtDAL::BaseXMLHandler& handler) const
   {
      parser.SetSchemaFile(mSchemaFile);
      parser.SetHandler(&handler);
   }

   /////////////////////////////////////////////////////////////////////////////
   osgDB::ReaderWriter::ReadResult BaseXMLReaderWriter::BuildResult(
      const osgDB::ReaderWriter::ReadResult& result, dtDAL::BaseXMLHandler& handler) const
   {
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   osgDB::ReaderWriter::ReadResult BaseXMLReaderWriter::readObject(
      const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
   {
      std::string ext = osgDB::getLowerCaseFileExtension(fileName);

      if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

      if (!dtUtil::FileUtils::GetInstance().FileExists(fileName))
      {
         return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND);
      }

      std::ifstream confStream(fileName.c_str(), std::ios_base::binary);

      if (!confStream.is_open())
      {
         return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE);
      }

      return readObject(confStream, options);
   }

   /////////////////////////////////////////////////////////////////////////////
   osgDB::ReaderWriter::ReadResult BaseXMLReaderWriter::readObject(
      std::istream& fin, const osgDB::ReaderWriter::Options* options) const
   {
      dtCore::RefPtr<dtDAL::BaseXMLHandler> handler = CreateHandler();
      dtCore::RefPtr<dtDAL::BaseXMLParser> parser = CreateParser();

      InitParser(*parser, *handler);

      osgDB::ReaderWriter::ReadResult result = osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;
      if (parser->Parse(fin))
      {
         result = osgDB::ReaderWriter::ReadResult::FILE_LOADED;
      }

      return BuildResult(result, *handler);
   }

}
