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
#include <dtDAL/export.h>

#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/framework/XMLFormatter.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>

namespace dtUtil
{
   class Log;
}

namespace dtDAL
{
   class BaseXMLHandler;

   /**
    * @class BaseXMLParser
    * @brief front end class for parsing an XML file.
    */
   class DT_DAL_EXPORT BaseXMLParser: public osg::Referenced
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

      ///@return true if the map is currently being parsed.
      bool IsParsing() const { return mParsing; }

   protected:

      virtual ~BaseXMLParser();

   public:

      /**
       * this is called automatically on startup.
       */
      static void StaticInit();

      /**
       * this is called automatically on shutdown.
       */
      static void StaticShutdown();

      dtUtil::Log* mLogger;

   private:
      BaseXMLParser(const BaseXMLParser& copyParser);
      BaseXMLParser& operator=(const BaseXMLParser& assignParser);

   protected:
      dtCore::RefPtr<BaseXMLHandler> mHandler;
      xercesc::SAX2XMLReader* mXercesParser;
      bool mParsing;
   };


   /**
    * @class BaseXMLWriter
    * @brief Writes a map out to an XML file
    */
   class DT_DAL_EXPORT BaseXMLWriter: public osg::Referenced
   {
   public:

      /**
       * Constructor.
       */
      BaseXMLWriter();

      /**
       * @Note  Create your own Save functions in your inherited classes.
       */
         
      dtUtil::Log* mLogger;

   protected:
      virtual ~BaseXMLWriter(); ///Protected destructor so that this could be subclassed.

      class DT_DAL_EXPORT FormatTarget: public xercesc::XMLFormatTarget
      {
         public:
            FormatTarget();
            virtual ~FormatTarget();

                  void SetOutputFile(FILE* newFile);
            const FILE* GetOutputFile(FILE* newFile) const { return mOutFile; }

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
            FILE* mOutFile;
            dtUtil::Log*  mLogger;
      };
         
      typedef std::basic_string<XMLCh> xmlCharString;
      static const int indentSize = 4;

      bool mLastCharWasLF;
      std::stack<xmlCharString> mElements;

      FormatTarget mFormatTarget;
      xercesc::XMLFormatter mFormatter;

   public:

      //writes out the open tags for a new element including indentation.
      virtual void BeginElement(const XMLCh* const name, const XMLCh* const attributes = NULL);
      //writes out the end element tag including indentation if necessary.
      virtual void EndElement();

      virtual void AddCharacters(const xmlCharString& string);
      virtual void AddCharacters(const std::string& string);

      void AddIndent();
         
      //disable copy constructor
      BaseXMLWriter(const BaseXMLWriter& toCopy): mFormatter("UTF-8", NULL, &mFormatTarget, xercesc::XMLFormatter::NoEscapes, xercesc::XMLFormatter::DefaultUnRep) {}
      //disable operator =
      BaseXMLWriter& operator=(const BaseXMLWriter& assignTo) { return *this;}
   };

}

#endif
