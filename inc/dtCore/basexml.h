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
 * David Guthrie
 * Extracted base functionality from mapxml by Jeff P. Houde
 */

#ifndef DELTA_BASEXML
#define DELTA_BASEXML

#include <string>
#include <stack>
#include <map>
#include <set>
#include <vector>

#include <osg/Referenced>

#include <dtCore/refptr.h>
#include <dtCore/export.h>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <xercesc/sax/InputSource.hpp>

#include <iosfwd>

namespace dtUtil
{
   class AttributeSearch;
   class Log;
}

namespace dtCore
{
   class BaseXMLHandler;

   class DT_CORE_EXPORT InputSourcefStream : public xercesc::InputSource
   {
   public:
      InputSourcefStream(std::istream& stream);

      virtual xercesc::BinInputStream* makeStream() const;

      std::istream& mStream;
   };

   typedef dtUtil::AttributeSearch AttributeMap;

   /**
    * @class BaseXMLParser
    * @brief front end class for parsing an XML file.
    */
   class DT_CORE_EXPORT BaseXMLParser: public osg::Referenced
   {
   public:

      /**
       * Constructor.
       */
      BaseXMLParser();

      /**
       * Parses the given XML file.
       *
       * @param[in]  path  The path to the xml file to parse.
       */
      virtual bool Parse(const std::string& path);

      virtual bool Parse(std::istream& stream);

      ///@return true if the map is currently being parsed.
      bool IsParsing() const { return mParsing; }

      void SetHandler(BaseXMLHandler* handler);
      BaseXMLHandler* GetHandler();

      void SetSchemaFile(const std::string& schemaFile);
      const std::string& GetSchemaFile() const;

   protected:

      void SetParsing(bool parsing);

      /**
       * Parses the given XML file token by token until the handler has the
       * information it wants or the end of the file has been reached.
       *
       * @param[in]  stream  the stream to parse
       * @param[in]  handler   The XML handler that determines what information to parse.
       * @return true if the file was parsed successfully
       */
      bool ParseFileByToken(std::istream& stream, BaseXMLHandler* handler) const;

      virtual ~BaseXMLParser();

   public:

      dtUtil::Log* mLogger;

   private:
      BaseXMLParser(const BaseXMLParser& copyParser);
      BaseXMLParser& operator=(const BaseXMLParser& assignParser);

      bool mParsing;
      std::string mSchemaFile;
      dtCore::RefPtr<BaseXMLHandler> mHandler;

   protected:
      xercesc::SAX2XMLReader* mXercesParser;
   };


   /**
    * @class BaseXMLWriter
    * @brief Writes a map out to an XML file
    */
   class DT_CORE_EXPORT BaseXMLWriter: public osg::Referenced
   {
   public:

      /**
       * Constructor.
       */
      BaseXMLWriter();

      /**
       * @note  Create your own Save functions in your inherited classes.
       */

      dtUtil::Log* mLogger;

   protected:
      virtual ~BaseXMLWriter(); ///Protected destructor so that this could be subclassed.

      class DT_CORE_EXPORT FormatTarget: public xercesc::XMLFormatTarget
      {
         public:
            FormatTarget();
            virtual ~FormatTarget();

            void SetOutputStream(std::ostream* stream);
            std::ostream* GetOutputStream() const { return mOutStream; }

#if XERCES_VERSION_MAJOR < 3
            virtual void writeChars(
               const XMLByte* const toWrite,
               const unsigned int count,
               xercesc::XMLFormatter* const formatter);
#else
            virtual void writeChars(
               const XMLByte* const toWrite,
               const XMLSize_t count,
               xercesc::XMLFormatter* const formatter);
#endif
            virtual void flush();

         private:
            std::ostream* mOutStream;
            dtUtil::Log*  mLogger;
      };

      typedef std::basic_string<XMLCh> xmlCharString;
      static const int indentSize = 4;

      bool mLastCharWasLF;
      std::stack<xmlCharString> mElements;

      FormatTarget mFormatTarget;
      xercesc::XMLFormatter mFormatter;

   public:

      ///writes out a standard xml header with encoding.
      virtual void WriteHeader();

      ///writes out the open tags for a new element including indentation or the complete self-closing element if closeImmediatly is true.
      virtual void BeginElement(const std::string& name, const AttributeMap& attrs, bool closeImmediately = false);
      virtual void BeginElement(const std::string& name, const std::string& attributes = "", bool closeImmediately = false);
      virtual void BeginElement(const XMLCh* const name, const XMLCh* const attributes = NULL, bool closeImmediately = false);
      ///writes out the end element tag including indentation if necessary.
      virtual void EndElement();

      virtual void AddCharacters(const xmlCharString& string);
      virtual void AddCharacters(const std::string& string);

      void AddIndent();

      //disable copy constructor
      BaseXMLWriter(const BaseXMLWriter& /*toCopy*/): mFormatter("UTF-8", NULL, &mFormatTarget, xercesc::XMLFormatter::NoEscapes, xercesc::XMLFormatter::DefaultUnRep) {}
      //disable operator =
      BaseXMLWriter& operator=(const BaseXMLWriter& /*assignTo*/) { return *this;}
   };

}

#endif
