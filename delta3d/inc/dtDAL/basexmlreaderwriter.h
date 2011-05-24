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
#include <dtDAL/export.h>
#include <osgDB/ReadFile>



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
   class DT_DAL_EXPORT BaseXMLReaderWriter : public osgDB::ReaderWriter
   {
   public:
      BaseXMLReaderWriter();

      virtual dtCore::RefPtr<BaseXMLHandler> CreateHandler() const = 0;

      /**
       * Method that creates the internal parser. This can be overridden
       * to return a custom parser.
       * By default this method will create a simple parser.
       * @return The parser instance to be used for all calls to this plug-in.
       */
      virtual dtCore::RefPtr<dtDAL::BaseXMLParser> CreateParser() const;

      /**
       * Convenience method for setting up the newly parser.
       * Override this method for custom parser setup.
       * By default for minimal setup, this will set the
       * schema file and handler on the parser.
       */
      virtual void InitParser(dtDAL::BaseXMLParser& parser, dtDAL::BaseXMLHandler& handler) const;

      /**
       * Set the schema file the internal parser should use.
       */
      void SetSchemaFile(const std::string& schemaFile);
      const std::string& GetSchemaFile() const;

      /**
       * Override method for adding any special data to the result.
       * The default implementation returns the original result.
       * @param result The original result of the whole read/write operation.
       * @param handler The handler that was involved with the parsing.
       * @return Modified result.
       */
      virtual osgDB::ReaderWriter::ReadResult BuildResult(
         const osgDB::ReaderWriter::ReadResult& result, dtDAL::BaseXMLHandler& handler) const;

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
      osgDB::ReaderWriter::ReadResult readObject(const std::string& fileName,
         const osgDB::ReaderWriter::Options* options = NULL) const;

      /**
       * Override method primarily responsible for processing file data.
       * @param fin File already loaded in the form of an input stream.
       * @param options OSG options struct that may hold additional data
       *        required by this plug-in, or any of its derived classes.
       * @return FILE_LOADED or ERROR_IN_READING_FILE.
       */
      osgDB::ReaderWriter::ReadResult readObject(std::istream& fin,
         const osgDB::ReaderWriter::Options* options = NULL) const;

   private:
      std::string mSchemaFile;
   };

}

#endif
