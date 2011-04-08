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

#include <prefix/dtdalprefix.h>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable : 4267) // for warning C4267: 'argument' : conversion from 'size_t' to 'const unsigned int', possible loss of data
#endif

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#if XERCES_VERSION_MAJOR < 3
#   include <xercesc/internal/XMLGrammarPoolImpl.hpp>
#endif
#include <xercesc/sax/SAXParseException.hpp>

#ifdef _MSC_VER
#   pragma warning(pop)
#endif

#include <osgDB/FileNameUtils>

#include <dtDAL/basexml.h>
#include <dtDAL/basexmlhandler.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/mapxmlconstants.h>

#include <dtUtil/xercesbininputstreamistream.h>

#include <dtUtil/datapathutils.h>
#include <dtUtil/fileutils.h>
#include <dtUtil/datetime.h>
#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>

#include <iostream>
#include <fstream>

XERCES_CPP_NAMESPACE_USE

namespace dtDAL
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
         throw dtDAL::XMLLoadParsingException( "Ran out of memory parsing save file.", __FILE__, __LINE__);
      }
      catch (const XMLException& toCatch)
      {
         mParsing = false;
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during parsing! %s :\n",
                  dtUtil::XMLStringConverter(toCatch.getMessage()).c_str());
         throw dtDAL::XMLLoadParsingException( "Error while parsing XML file. See log for more information.", __FILE__, __LINE__);
      }
      catch (const SAXParseException& toCatch)
      {
         mParsing = false;
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "Error during sax parsing! %s :\n",
                  dtUtil::XMLStringConverter(toCatch.getMessage()).c_str());
         //this will already by logged by the code above
         throw dtDAL::XMLLoadParsingException( "Error while parsing XML file. See log for more information.", __FILE__, __LINE__);
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
   void BaseXMLParser::SetParsing(bool parsing)
   {
      mParsing = parsing;
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
      mFormatter << string.c_str();
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLWriter::AddCharacters(const std::string& string)
   {
      mLastCharWasLF = false;
      XMLCh * stringX = XMLString::transcode(string.c_str());
      mFormatter << stringX;
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