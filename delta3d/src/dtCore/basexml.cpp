/*
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

#include <prefix/dtcoreprefix.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>

#include <dtUtil/deprecationmgr.h>
DT_DISABLE_WARNING_ALL_START

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <osgDB/FileNameUtils>

DT_DISABLE_WARNING_END


#include <dtCore/basexml.h>
#include <dtCore/basexmlhandler.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/mapxmlconstants.h>

#include <dtUtil/xercesbininputstreamistream.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datetime.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>

#include <iostream>
#include <fstream>

XERCES_CPP_NAMESPACE_USE

namespace dtCore
{

   static const std::string logName("basexml.cpp");

   InputSourcefStream::InputSourcefStream(std::istream& stream)
      : mStream(stream)
   {
   }

   xercesc::BinInputStream* InputSourcefStream::makeStream() const
   {
      return new dtUtil::XercesBinInputStreamIStream(mStream);
   }

   /////////////////////////////////////////////////////////////////
   BaseXMLParser::BaseXMLParser()
      : mParsing(false)
   {
      mLogger = &dtUtil::Log::GetInstance(logName);

      mXercesParser = XMLReaderFactory::createXMLReader();
   }

   /////////////////////////////////////////////////////////////////
   BaseXMLParser::~BaseXMLParser()
   {
      delete mXercesParser;
   }

   /////////////////////////////////////////////////////////////////
   bool BaseXMLParser::Parse(std::istream& stream)
   {
      try
      {
         mParsing = true;
         mXercesParser->setContentHandler(mHandler.get());
         mXercesParser->setErrorHandler(mHandler.get());
         //mXercesParser->parse(path.c_str());
         InputSourcefStream xerStream(stream);
         mXercesParser->parse(xerStream);
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__, "Parsing complete.\n");
         mParsing = false;
         return true;
      }
      catch (const OutOfMemoryException&)
      {
         mParsing = false;
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Ran out of memory parsing!");
         throw dtCore::XMLLoadParsingException( "Ran out of memory parsing save file.", __FILE__, __LINE__);
      }
      catch (const XMLException& toCatch)
      {
         mParsing = false;
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! %s :\n",
                  dtUtil::XMLStringConverter(toCatch.getMessage()).c_str());
         throw dtCore::XMLLoadParsingException( "Error while parsing XML file. See log for more information.", __FILE__, __LINE__);
      }
      catch (const SAXParseException& toCatch)
      {
         mParsing = false;
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during sax parsing! %s :\n",
                  dtUtil::XMLStringConverter(toCatch.getMessage()).c_str());
         //this will already by logged by the code above
         throw dtCore::XMLLoadParsingException( "Error while parsing XML file. See log for more information.", __FILE__, __LINE__);
      }
      return false;
   }

   /////////////////////////////////////////////////////////////////
   bool BaseXMLParser::Parse(const std::string& path)
   {
      std::ifstream fileStream(path.c_str(), std::ios_base::binary | std::ios_base::in);
      return Parse(fileStream);
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLParser::SetHandler(BaseXMLHandler* handler)
   {
      mHandler = handler;
   }

   /////////////////////////////////////////////////////////////////
   BaseXMLHandler* BaseXMLParser::GetHandler()
   {
      return mHandler;
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLParser::SetSchemaFile(const std::string& schemaFile)
   {
      mSchemaFile = schemaFile;

      // NOTE: The following implementation was copied from dtUtil::XercesParser.

      if (!schemaFile.empty())
      {
         std::string schema = dtUtil::FindFileInPathList(schemaFile);

         if (schema.empty())
         {
            LOG_WARNING("Scheme file, " + schemaFile + ", not found, check your DELTA_DATA environment variable, schema checking disabled.")
         }
         else // turn on schema checking
         {
            // In some cases, schema will contain a url that is
            // relative to the current working directory which
            // may cause problems with xerces correctly finding it
            schema = osgDB::getRealPath(schema);

            mXercesParser->setFeature(XMLUni::fgXercesSchema, true);                  // enables schema checking.
            mXercesParser->setFeature(XMLUni::fgSAX2CoreValidation, true);            // posts validation errors.
            mXercesParser->setFeature(XMLUni::fgXercesValidationErrorAsFatal, true);  // does not allow parsing if schema is not fulfilled.
            mXercesParser->loadGrammar(schema.c_str(), Grammar::SchemaGrammarType, true);
            XMLCh* SCHEMA = XMLString::transcode(schema.c_str());
            mXercesParser->setFeature(XMLUni::fgXercesSchema, true);
            mXercesParser->setProperty(XMLUni::fgXercesSchemaExternalNoNameSpaceSchemaLocation, SCHEMA);
            XMLString::release(&SCHEMA);
         }
      }
   }

   /////////////////////////////////////////////////////////////////
   const std::string& BaseXMLParser::GetSchemaFile() const
   {
      return mSchemaFile;
   }


   /////////////////////////////////////////////////////////////////
   void BaseXMLParser::SetParsing(bool parsing)
   {
      mParsing = parsing;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool BaseXMLParser::ParseFileByToken(std::istream& stream, BaseXMLHandler* handler) const
   {
      mXercesParser->setContentHandler(handler);
      mXercesParser->setErrorHandler(handler);

      XMLPScanToken token;
      InputSourcefStream xerStream(stream);

      if (!mXercesParser->parseFirst(xerStream, token))
      {
         return false;
      }

      while (mXercesParser->parseNext(token))
      {
         if (handler->HandledDesiredData())
         {
            //finished parsing the header data
            break;
         }
      }

      if (handler->HandledDesiredData() == false)
      {
         return false;
      }

      mXercesParser->parseReset(token);

      return true;
   }

   /////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////

   //////////////////////////////////////////////////
   BaseXMLWriter::BaseXMLWriter()
      : mFormatter("UTF-8", NULL, &mFormatTarget, XMLFormatter::NoEscapes, XMLFormatter::DefaultUnRep)
   {
      mLogger = &dtUtil::Log::GetInstance(logName);
   }

   /////////////////////////////////////////////////////////////////
   BaseXMLWriter::~BaseXMLWriter()
   {
   }

   //////////////////////////////////////////////////////////////////////////
   BaseXMLWriter::FormatTarget::FormatTarget()
      : mOutStream(NULL)
   {
      mLogger = &dtUtil::Log::GetInstance(logName);
   }

   /////////////////////////////////////////////////////////////////
   BaseXMLWriter::FormatTarget::~FormatTarget()
   {
      SetOutputStream(&std::cout);
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLWriter::FormatTarget::SetOutputStream(std::ostream* stream)
   {
      mOutStream = stream;
      if (mOutStream == NULL)
      {
         mOutStream = &std::cout;
      }
   }

   /////////////////////////////////////////////////////////////////
#if XERCES_VERSION_MAJOR < 3
   void BaseXMLWriter::FormatTarget::writeChars(
      const XMLByte* const toWrite,
      const unsigned int count,
      xercesc::XMLFormatter* const formatter)
#else
   void BaseXMLWriter::FormatTarget::writeChars(
      const XMLByte* const toWrite,
      const XMLSize_t count,
      xercesc::XMLFormatter* const formatter)
#endif
   {
      mOutStream->write((char *) toWrite, size_t(count));
      mOutStream->flush();
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLWriter::FormatTarget::flush()
   {
      mOutStream->flush();
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLWriter::WriteHeader()
   {
      mFormatter << MapXMLConstants::BEGIN_XML_DECL << mFormatter.getEncodingName() << MapXMLConstants::END_XML_DECL << chLF;
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLWriter::BeginElement(const std::string& name,
      const dtCore::AttributeMap& attrs, bool closeImmediately)
   {
      std::string attrsStr(attrs.ToString());
      BeginElement(name, attrsStr, closeImmediately);
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLWriter::BeginElement(const std::string& name,
      const std::string& attributes, bool closeImmediately)
   {
      XMLCh* attrsX = attributes.empty() ? NULL : XMLString::transcode(attributes.c_str());
      XMLCh* nameX = XMLString::transcode(name.c_str());

      BeginElement(nameX, attrsX, closeImmediately);

      XMLString::release(&nameX);
      if (attrsX != NULL)
      {
         XMLString::release(&attrsX);
      }
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLWriter::BeginElement(const XMLCh* name, const XMLCh* attributes, bool closeImmediately)
   {
      xmlCharString s(name);
      mElements.push(name);
      AddIndent();

      mFormatter << chOpenAngle << name;
      if (attributes != NULL)
         mFormatter << chSpace << attributes;

      if (closeImmediately)
      {
         mFormatter << chForwardSlash << chCloseAngle << chLF;
         mLastCharWasLF = true;
         mElements.pop();
      }
      else
      {
         mFormatter << chCloseAngle;
         mLastCharWasLF = false;
      }
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLWriter::EndElement()
   {
      const xmlCharString& name = mElements.top();
      if (mLastCharWasLF)
      {
         AddIndent();
      }

      mFormatter << chOpenAngle << chForwardSlash << name.c_str() << chCloseAngle << chLF;
      mLastCharWasLF = true;
      mElements.pop();
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLWriter::AddCharacters(const xmlCharString& string)
   {
      mLastCharWasLF = false;
      mFormatter.setEscapeFlags(XMLFormatter::StdEscapes);
      mFormatter << string.c_str();
      mFormatter.setEscapeFlags(XMLFormatter::NoEscapes);
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLWriter::AddCharacters(const std::string& string)
   {
      mLastCharWasLF = false;
      XMLCh * stringX = XMLString::transcode(string.c_str());
      mFormatter.setEscapeFlags(XMLFormatter::StdEscapes);
      mFormatter << stringX;
      mFormatter.setEscapeFlags(XMLFormatter::NoEscapes);
      XMLString::release(&stringX);
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLWriter::AddIndent()
   {
      if (!mLastCharWasLF)
         mFormatter << chLF;

      mLastCharWasLF = false;

      size_t indentCount = mElements.size() - 1;

      if (mElements.empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__, "Invalid end element when saving a map: ending with no beginning.");

         indentCount = 0;
      }
      for (size_t x = 0; x < indentCount; x++)
      {
         for (int y = 0; y < BaseXMLWriter::indentSize; y++)
         {
            mFormatter << chSpace;
         }
      }
   }
}

//////////////////////////////////////////////////////////////////////////
