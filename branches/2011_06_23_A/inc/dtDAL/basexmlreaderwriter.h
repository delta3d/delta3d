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

#ifndef DELTA_BASEXML_READERWRITER
#define DELTA_BASEXML_READERWRITER

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <osgDB/FileNameUtils>
#include <osgDB/ReadFile>
#include <dtDAL/basexml.h>
#include <dtDAL/basexmlhandler.h>
#include <dtDAL/basexmlreaderwriter.h>
#include <dtUtil/fileutils.h>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace dtDAL
{
   class BaseXMLHandler;
   class BaseXMLParser;
}



namespace dtDAL
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   template <class T_Object>
   class WriteWrapperOSGObject : public osg::Object
   {
   public:
      typedef osg::Object BaseClass;

      WriteWrapperOSGObject(const T_Object& obj)
         : mObject(&obj)
      {}

      const T_Object* const GetObject() const
      {
         return mObject;
      }

      META_Object("dtDAL", WriteWrapperOSGObject);

   private:
      const T_Object* const mObject;

      WriteWrapperOSGObject()
         : mObject(NULL)
      {}

      WriteWrapperOSGObject(const osg::Object& obj,const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
         : BaseClass(obj, copyop)
         , mObject(NULL)
      {}
   };



#define TEMPLATE_PARAMS_BASE_XML_READERWRITER typename T_Object, typename T_Handler, typename T_Writer
#define TEMPLATE_BASE_XML_READERWRITER template<TEMPLATE_PARAMS_BASE_XML_READERWRITER>

   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   class BaseXMLReaderWriter : public osgDB::ReaderWriter
   {
   public:
      BaseXMLReaderWriter();

      /**
       * Method that creates the internal parser. This can be overridden
       * to return a custom parser.
       * By default this method will create a simple parser.
       * @return The parser instance to be used for a single call to this plug-in.
       */
      virtual dtCore::RefPtr<dtDAL::BaseXMLParser> CreateParser() const;

      /**
       * Method that creates the internal handler for custom parsing.
       * Sub-classes must override this method.
       * @return New instance of a custom handler.
       */
      virtual typename dtCore::RefPtr<T_Handler> CreateHandler() const;

      /**
       * Method that creates the internal writer. This can be overridden
       * to return a custom writer.
       * By default this method will create a simple writer.
       * @return The writer instance to be used for a single call to this plug-in.
       */
      virtual typename dtCore::RefPtr<T_Writer> CreateWriter() const;

      /**
       * Convenience method for setting up the new parser.
       * Override this method for custom parser setup.
       * By default for minimal setup, this will set the
       * schema file and handler on the parser.
       */
      virtual void InitParser(dtDAL::BaseXMLParser& parser, T_Handler& handler) const;

      /**
       * Convenience method for setting up the new writer.
       * Override this method for custom writer setup.
       * Default implementation will do minimal setup.
       */
      virtual void InitWriter(T_Writer& writer) const;

      /**
       * Set the schema file the internal parser should use.
       */
      void SetSchemaFile(const std::string& schemaFile);
      const std::string& GetSchemaFile() const;

      /**
       * Override method for adding any special data to the Read Result.
       * The default implementation returns the original Read Result.
       * @param result The original result of the whole read operation.
       * @param handler The handler that was involved with the parsing.
       * @return Modified Read Result.
       */
      virtual osgDB::ReaderWriter::ReadResult BuildResult(
         const osgDB::ReaderWriter::ReadResult& result, T_Handler& handler) const;

      /**
       * Override method for adding any special data to the Write Result.
       * The default implementation returns the original Write Result.
       * @param result The original result of the whole write operation.
       * @return Modified Write Result.
       */
      virtual osgDB::ReaderWriter::WriteResult BuildResult(
         const osgDB::ReaderWriter::WriteResult& result) const;

      //////////////////////////////////////////////////////////////////////////
      // INHERITED OSG OVERRIDE METHODS
      //////////////////////////////////////////////////////////////////////////

      /**
       * Override method for loading a file referred to by a file path.
       * This method handles loading a file as an input stream, and
       * subsequently calls the input stream overload version of this method.
       * @param fileName Path pointing to the file to load.
       * @param options OSG options struct that may hold additional data
       *        required by this plug-in, or any of its derived classes.
       * @return FILE_LOADED, FILE_NOT_FOUND or ERROR_IN_READING_FILE.
       */
      virtual osgDB::ReaderWriter::ReadResult readObject(const std::string& fileName,
         const osgDB::ReaderWriter::Options* options = NULL) const;

      /**
       * Override method primarily responsible for processing file data.
       * @param fin File already loaded in the form of an input stream.
       * @param options OSG options struct that may hold additional data
       *        required by this plug-in, or any of its derived classes.
       * @return FILE_LOADED or ERROR_IN_READING_FILE.
       */
      virtual osgDB::ReaderWriter::ReadResult readObject(std::istream& fin,
         const osgDB::ReaderWriter::Options* options = NULL) const;

      /**
       * Override method primarily responsible for writing data.
       * @param obj Object to be written.
       * @param fileName File to write the object to.
       * @param options Optional options; not used in default implementation.
       * @return FILE_SAVED or ERROR_IN_READING_FILE.
       */
      virtual osgDB::ReaderWriter::WriteResult writeObject(
         const osg::Object& obj,const std::string& fileName,
         const osgDB::ReaderWriter::Options* options = NULL) const;

      /**
       * Override method primarily responsible for writing data.
       * @param obj Object to be written.
       * @param fout File output stream to which to write the object.
       * @param options Optional options; not used in default implementation.
       * @return FILE_SAVED or ERROR_IN_READING_FILE.
       */
      virtual osgDB::ReaderWriter::WriteResult writeObject(
         const osg::Object& obj, std::ostream& fout,
         const osgDB::ReaderWriter::Options* options = NULL) const;

   private:
      std::string mSchemaFile;
   };


#define TEMP_BASEXMLREADERWRITER BaseXMLReaderWriter<TEMPLATE_PARAMS_BASE_XML_READERWRITER>

   /////////////////////////////////////////////////////////////////////////////
   // IMPLEMENTATION CODE
   /////////////////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   TEMP_BASEXMLREADERWRITER::BaseXMLReaderWriter()
   {  
   }

   /////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   dtCore::RefPtr<dtDAL::BaseXMLParser> TEMP_BASEXMLREADERWRITER::CreateParser() const
   {
      return new dtDAL::BaseXMLParser();
   }

   /////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   typename dtCore::RefPtr<T_Handler> TEMP_BASEXMLREADERWRITER::CreateHandler() const
   {
      return new T_Handler();
   }

   /////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   typename dtCore::RefPtr<T_Writer> TEMP_BASEXMLREADERWRITER::CreateWriter() const
   {
      return new T_Writer();
   }

   /////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   void TEMP_BASEXMLREADERWRITER::SetSchemaFile(const std::string& schemaFile)
   {
      mSchemaFile = schemaFile;
   }

   /////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   const std::string& TEMP_BASEXMLREADERWRITER::GetSchemaFile() const
   {
      return mSchemaFile;
   }

   /////////////////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   void TEMP_BASEXMLREADERWRITER::InitParser(dtDAL::BaseXMLParser& parser, T_Handler& handler) const
   {
      parser.SetSchemaFile(mSchemaFile);
      parser.SetHandler(&handler);
   }

   /////////////////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   void TEMP_BASEXMLREADERWRITER::InitWriter(T_Writer& writer) const
   {
      // OVERRIDE:
   }

   /////////////////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   osgDB::ReaderWriter::ReadResult TEMP_BASEXMLREADERWRITER::BuildResult(
      const ReadResult& result, T_Handler& handler) const
   {
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   osgDB::ReaderWriter::WriteResult TEMP_BASEXMLREADERWRITER::BuildResult(
      const WriteResult& result) const
   {
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   osgDB::ReaderWriter::ReadResult TEMP_BASEXMLREADERWRITER::readObject(
      const std::string& fileName, const osgDB::ReaderWriter::Options* options) const
   {
      std::string ext = osgDB::getLowerCaseFileExtension(fileName);

      if (!acceptsExtension(ext)) return ReadResult::FILE_NOT_HANDLED;

      if (!dtUtil::FileUtils::GetInstance().FileExists(fileName))
      {
         return ReadResult::FILE_NOT_FOUND;
      }

      std::ifstream stream(fileName.c_str(), std::ios_base::binary);

      if (!stream.is_open())
      {
         return ReadResult::ERROR_IN_READING_FILE;
      }

      return readObject(stream, options);
   }

   /////////////////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   osgDB::ReaderWriter::ReadResult TEMP_BASEXMLREADERWRITER::readObject(
      std::istream& fin, const osgDB::ReaderWriter::Options* options) const
   {
      dtCore::RefPtr<T_Handler> handler = CreateHandler();
      dtCore::RefPtr<dtDAL::BaseXMLParser> parser = CreateParser();

      InitParser(*parser, *handler);

      ReadResult result = ReadResult::ERROR_IN_READING_FILE;
      if (parser->Parse(fin))
      {
         result = ReadResult::FILE_LOADED;
      }

      return BuildResult(result, *handler);
   }

   /////////////////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   osgDB::ReaderWriter::WriteResult TEMP_BASEXMLREADERWRITER::writeObject(
      const osg::Object& obj, const std::string& fileName,
      const osgDB::ReaderWriter::Options* options) const
   {
      std::string ext = osgDB::getLowerCaseFileExtension(fileName);

      if (!acceptsExtension(ext)) return WriteResult::FILE_NOT_HANDLED;

      std::ofstream stream(fileName.c_str(), std::ios_base::trunc|std::ios_base::binary);
      if (!stream.is_open())
      {
         return WriteResult::ERROR_IN_WRITING_FILE;
      }

      return writeObject(obj, stream, options); 
   }

   /////////////////////////////////////////////////////////////////////////////
   TEMPLATE_BASE_XML_READERWRITER
   osgDB::ReaderWriter::WriteResult TEMP_BASEXMLREADERWRITER::writeObject(
      const osg::Object& obj, std::ostream& fout, const Options*) const 
   {
      dtCore::RefPtr<T_Writer> writer = CreateWriter();

      WriteResult result = WriteResult::ERROR_IN_WRITING_FILE;

      const WriteWrapperOSGObject<T_Object>* wrapperObj
         = dynamic_cast<const WriteWrapperOSGObject<T_Object>* >(&obj);

      // Is the object wrapped to satisfy the osg::Object parameter?
      if (wrapperObj != NULL)
      {
         if (writer->Write(*wrapperObj->GetObject(), fout))
         {
            result = WriteResult::FILE_SAVED; 
         }
      }
      else // ...otherwise try a direct cast...
      {
         const T_Object* castObj = dynamic_cast<const T_Object* >(&obj);
         if (castObj != NULL)
         {
            if (writer->Write(*castObj, fout))
            {
               result = WriteResult::FILE_SAVED; 
            }
         }
         else
         {
            std::ostringstream oss;
            oss << "Could not cast object \"" << obj.getName()
               << "\" to the proper type for writing.\n" << std::endl;
            LOG_ERROR(oss.str());
         }
      }

      return BuildResult(result);
   }

// This macro should not be needed outside of this file.
#undef TEMP_BASEXMLREADERWRITER
#undef TEMPLATE_BASE_XML_READERWRITER
#undef TEMPLATE_PARAMS_BASE_XML_READERWRITER

}

#endif
