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
#include <prefix/dtcoreprefix.h>
#include <dtCore/projectconfigreaderwriter.h>
#include <dtCore/basexml.h>
#include <dtCore/projectconfigxmlhandler.h>
#include <dtCore/exceptionenum.h>

#include <dtUtil/wrapperosgobject.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datapathutils.h>
#include <osgDB/Registry>

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#if XERCES_VERSION_MAJOR < 3
#   include <xercesc/internal/XMLGrammarPoolImpl.hpp>
#endif
#include <xercesc/sax/SAXParseException.hpp>

#include <osgDB/FileNameUtils>

#include <iostream>
#include <fstream>

XERCES_CPP_NAMESPACE_USE

namespace dtCore
{
   class ProjectConfigXMLParser : public BaseXMLParser
   {
   public:
      ProjectConfigXMLParser()
      : BaseXMLParser()
      {
         SetHandler(new ProjectConfigXMLHandler());

         mXercesParser->setFeature(XMLUni::fgSAX2CoreValidation, true);
         mXercesParser->setFeature(XMLUni::fgXercesDynamic, false);

         mXercesParser->setFeature(XMLUni::fgSAX2CoreNameSpaces, true);
         mXercesParser->setFeature(XMLUni::fgXercesSchema, true);
         mXercesParser->setFeature(XMLUni::fgXercesSchemaFullChecking, true);
         mXercesParser->setFeature(XMLUni::fgSAX2CoreNameSpacePrefixes, true);
         mXercesParser->setFeature(XMLUni::fgXercesUseCachedGrammarInParse, true);
         mXercesParser->setFeature(XMLUni::fgXercesCacheGrammarFromParse, true);

         std::string schemaFileName = dtUtil::FindFileInPathList("project_config.xsd");

         if (!dtUtil::FileUtils::GetInstance().FileExists(schemaFileName))
         {
            throw dtCore::ProjectException( "Unable to load required file \"project_config.xsd\", can not load Project Config file.", __FILE__, __LINE__);
         }

         XMLCh* schema = XMLString::transcode(schemaFileName.c_str());
         LocalFileInputSource inputSource(schema);
         //cache the schema
         mXercesParser->loadGrammar(inputSource, Grammar::SchemaGrammarType, true);
         mXercesParser->setProperty(XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, schema);
         XMLString::release(&schema);
      }

      void Parse(std::istream& fin, dtCore::RefPtr<ProjectConfig>& toFill)
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
   osgDB::ReaderWriter::ReadResult ProjectConfigReaderWriter::readObject(const std::string& fileName,const osgDB::ReaderWriter::Options* options) const
   {
      std::string ext = osgDB::getLowerCaseFileExtension(fileName);
      std::string path = osgDB::getFilePath(fileName);

      dtUtil::FileUtils& fileutils = dtUtil::FileUtils::GetInstance();

      if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

      if (!fileutils.FileExists(fileName))
      {
         return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::FILE_NOT_FOUND);
      }

      std::ifstream confStream(fileName.c_str(), std::ios_base::binary);

      if (!confStream.is_open())
      {
         return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE);
      }

      osgDB::ReaderWriter::ReadResult result = osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE;

      dtUtil::DirectoryPush dPush(path);
      result = readObject(confStream, options);
      return result;
   }

   //////////////////////////////////////////////////////////////////////////////////
   osgDB::ReaderWriter::ReadResult ProjectConfigReaderWriter::readObject(std::istream& fin, const osgDB::ReaderWriter::Options* options) const
   {
      dtCore::RefPtr<ProjectConfigXMLParser> pcxml = new ProjectConfigXMLParser();

      dtCore::RefPtr<ProjectConfig> projConfig;
      pcxml->Parse(fin, projConfig);
      // sadly, this won't work right if you call this function yourself and don't set the current
      // directory to the one where the project config file sits.
      projConfig->SetBasePath(dtUtil::FileUtils::GetInstance().CurrentDirectory());

      if (projConfig != NULL)
      {
         dtCore::RefPtr<dtUtil::WrapperOSGObject> obj = new dtUtil::WrapperOSGObject;
         obj->setUserData(projConfig);
         osgDB::ReaderWriter::ReadResult result =
                  osgDB::ReaderWriter::ReadResult(
                           obj,
                           osgDB::ReaderWriter::ReadResult::FILE_LOADED);
         return result;
      }
      return osgDB::ReaderWriter::ReadResult(osgDB::ReaderWriter::ReadResult::ERROR_IN_READING_FILE);
   }

   //////////////////////////////////////////////////////////////////////////////////
   osgDB::ReaderWriter::WriteResult ProjectConfigReaderWriter::writeObject(const osg::Object& obj, const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
   {
      std::string ext = osgDB::getLowerCaseFileExtension(fileName);
      if (!acceptsExtension(ext)) return WriteResult::FILE_NOT_HANDLED;

      if (dtUtil::FileUtils::GetInstance().FileExists(fileName))
      {
         return osgDB::ReaderWriter::WriteResult::ERROR_IN_WRITING_FILE;
      }

      std::ofstream fout(fileName.c_str());

      if (!fout.is_open())
      {
         return osgDB::ReaderWriter::WriteResult::ERROR_IN_WRITING_FILE;
      }

      return writeObject(obj, fout, options);
   }

   //////////////////////////////////////////////////////////////////////////////////
   osgDB::ReaderWriter::WriteResult ProjectConfigReaderWriter::writeObject(const osg::Object& obj, std::ostream& fout, const osgDB::ReaderWriter::Options* options) const
   {
      osgDB::ReaderWriter::WriteResult result = osgDB::ReaderWriter::WriteResult::ERROR_IN_WRITING_FILE;
      const ProjectConfig* pc = dynamic_cast<const ProjectConfig*>(obj.getUserData());

      if (pc != NULL)
      {
         dtCore::RefPtr<ProjectConfigXMLWriter> writer = new ProjectConfigXMLWriter;
         try
         {
            writer->Save(*pc, fout);
            result = osgDB::ReaderWriter::WriteResult::FILE_SAVED;
         }
         catch (const dtUtil::Exception& ex)
         {
            ex.LogException(dtUtil::Log::LOG_ERROR);
         }
         catch (...)
         {
            LOG_ERROR("Unknown exception trying to write project config file.");
         }
      }

      return result;
   }

   // now register with sgRegistry to instantiate the above
   // reader/writer.
   REGISTER_OSGPLUGIN(dtproj, ProjectConfigReaderWriter)

}



