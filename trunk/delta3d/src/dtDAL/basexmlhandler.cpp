/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006, Alion Science and Technology, BMH Operation.
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
 * Extracted base functionality from mapcontenthandler by Jeff P. Houde
 */

#include <prefix/dtdalprefix.h>

#include <typeinfo>

#include <dtDAL/basexmlhandler.h>


#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>

#ifdef _MSC_VER
#   pragma warning(push)
#   pragma warning(disable : 4267) // for warning C4267: 'argument' : conversion from 'size_t' to 'const unsigned int', possible loss of data
#endif

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#ifdef _MSC_VER
#   pragma warning(pop)
#endif

XERCES_CPP_NAMESPACE_USE

#include <sstream>


// Default iimplementation of char_traits<XMLCh>, needed for gcc3.3
#if (__GNUC__ == 3 && __GNUC_MINOR__ <= 3)
/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace std
{
   template<>
   struct char_traits<unsigned short>
   {
      typedef unsigned short char_type;

      static void
         assign(char_type& __c1, const char_type& __c2)
      { __c1 = __c2; }

      static int
         compare(const char_type* __s1, const char_type* __s2, size_t __n)
      {
         for (;__n > 0; ++__s1, ++__s2, --__n) {
            if (*__s1 < *__s2) return -1;
            if (*__s1 > *__s2) return +1;
         }
         return 0;
      }

      static size_t
         length(const char_type* __s)
      { size_t __n = 0; while (*__s++) ++__n; return __n; }

      static char_type*
         copy(char_type* __s1, const char_type* __s2, size_t __n)
      {  return static_cast<char_type*>(memcpy(__s1, __s2, __n * sizeof(char_type))); }

   };
}
/// @endcond
#endif

namespace  dtDAL
{
   /////////////////////////////////////////////////////////////////
   BaseXMLHandler::BaseXMLHandler()
   {
      mLogger = &dtUtil::Log::GetInstance ( "BaseXMLHandler.cpp" );
      //mLogger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
      mLogger->LogMessage(dtUtil::Log::LOG_INFO, __FUNCTION__,  __LINE__, "Creating Map Content Handler.\n");
   }

   /////////////////////////////////////////////////////////////////
   BaseXMLHandler::~BaseXMLHandler() {}

   //////////////////////////////////////////////////////////////////////////
   BaseXMLHandler::BaseXMLHandler(const BaseXMLHandler&) {}

   //////////////////////////////////////////////////////////////////////////
   BaseXMLHandler& BaseXMLHandler::operator=(const BaseXMLHandler&) { return *this;}

   /////////////////////////////////////////////////////////////////
   void BaseXMLHandler::startDocument()
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
         "Parsing Map Document Started.");

      Reset();
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLHandler::endDocument()
   {
      mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
         "Parsing Map Document Ended.\n");
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLHandler::resetDocument()
   {
      Reset();
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLHandler::startElement (
      const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname,
      const xercesc::Attributes& attrs)
   {
      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__, __LINE__,
            "Found element %s", dtUtil::XMLStringConverter(localname).c_str());
      }

      if (!mCombinedCharacters.empty())
      {
         CombinedCharacters(mCombinedCharacters.c_str(), mCombinedCharacters.size());
         mCombinedCharacters.clear();
      }

      ElementStarted(uri, localname, qname, attrs);

      mElements.push(xmlCharString(localname));
   }

   //////////////////////////////////////////////////////////////////////////
   void BaseXMLHandler::ElementStarted(
      const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname,
      const xercesc::Attributes& attrs)
   {

   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLHandler::endElement( const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname)
   {
      if (mElements.empty())
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Attempting to pop elements off of stack and the stack is empty."
            "it should at least contain element %s.",
            dtUtil::XMLStringConverter(localname).c_str());
         return;
      }

      const XMLCh* lname = mElements.top().c_str();

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
            "Ending element: \"%s\"", dtUtil::XMLStringConverter(lname).c_str());
      }

      if (XMLString::compareString(lname, localname) != 0)
      {
         mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__, __LINE__,
            "Attempting to pop mElements off of stack and the element "
            "at the top (%s) is not the same as the element ending (%s).",
            dtUtil::XMLStringConverter(lname).c_str(), dtUtil::XMLStringConverter(localname).c_str());
      }

      if (!mCombinedCharacters.empty())
      {
         CombinedCharacters(mCombinedCharacters.c_str(), mCombinedCharacters.size());
         mCombinedCharacters.clear();
      }

      ElementEnded(uri, localname, qname);

      mElements.pop();
   }

   //////////////////////////////////////////////////////////////////////////
   void BaseXMLHandler::ElementEnded( const XMLCh* const uri,
      const XMLCh* const localname,
      const XMLCh* const qname)
   {
      mCombinedCharacters.clear();
   }


   /////////////////////////////////////////////////////////////////
   /////////////////////////////////////////////////////////////////
#if XERCES_VERSION_MAJOR < 3
   void BaseXMLHandler::ignorableWhitespace(const XMLCh* const chars, const unsigned int length)
#else
   void BaseXMLHandler::ignorableWhitespace(const XMLCh* const chars, const XMLSize_t length)
#endif
   {
      characters(chars, length);
   }

   /////////////////////////////////////////////////////////////////
#if XERCES_VERSION_MAJOR < 3
   void BaseXMLHandler::characters(const XMLCh* const chars, const unsigned int length)
#else
   void BaseXMLHandler::characters(const XMLCh* const chars, const XMLSize_t length)
#endif
   {

      xmlCharString& topEl = mElements.top();

      mCombinedCharacters.append(chars, length);

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(dtUtil::Log::LOG_DEBUG, __FUNCTION__,  __LINE__,
            "Found characters for element \"%s\" \"%s\"", dtUtil::XMLStringConverter(topEl.c_str()).c_str(), dtUtil::XMLStringConverter(chars).c_str());
      }
   }

   //////////////////////////////////////////////////////////////////////////
   void BaseXMLHandler::processingInstruction(const XMLCh* const target, const XMLCh* const data) {}

   //////////////////////////////////////////////////////////////////////////
   void BaseXMLHandler::setDocumentLocator(const xercesc::Locator* const locator) {}

   //////////////////////////////////////////////////////////////////////////
   void BaseXMLHandler::startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri) {}

   //////////////////////////////////////////////////////////////////////////
   void BaseXMLHandler::endPrefixMapping(const XMLCh* const prefix) {}

   //////////////////////////////////////////////////////////////////////////
   void BaseXMLHandler::skippedEntity(const XMLCh* const name) {}

   //////////////////////////////////////////////////////////////////////////
   InputSource* BaseXMLHandler::resolveEntity(const XMLCh* const publicId, const XMLCh* const systemId)
   {
      return NULL;
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLHandler::error(const xercesc::SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "ERROR: " + MakeErrorString(exc));
      throw exc;
   }
   /////////////////////////////////////////////////////////////////
   void BaseXMLHandler::fatalError(const xercesc::SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_ERROR, __FUNCTION__,  __LINE__, "FATAL ERROR: " + MakeErrorString(exc));
      throw exc;
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLHandler::warning(const xercesc::SAXParseException& exc)
   {
      mLogger->LogMessage(dtUtil::Log::LOG_WARNING, __FUNCTION__,  __LINE__, "WARNING: " + MakeErrorString(exc));
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLHandler::resetErrors()
   {
      mErrorCount = 0;
      mFatalErrorCount = 0;
      mWarningCount = 0;
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLHandler::Reset()
   {
      while (!mElements.empty()) mElements.pop();

      resetErrors();
   }

   ////////////////////////////////////////////////////////////////////////////////
   std::string BaseXMLHandler::MakeErrorString(const xercesc::SAXParseException &exc)
   {
      const std::string publicID = exc.getPublicId() ? dtUtil::XMLStringConverter(exc.getPublicId()).ToString(): "n/a";
      const std::string systemID = exc.getSystemId() ? dtUtil::XMLStringConverter(exc.getSystemId()).ToString(): "n/a";

      std::stringstream ss;

      ss << dtUtil::XMLStringConverter(exc.getMessage()).c_str() << 
         ". Ln:" << exc.getLineNumber() << " Col:" << exc.getColumnNumber() << 
         " PublicID:" << publicID << " SystemID:" << systemID;

      return ss.str();
   }
}

//////////////////////////////////////////////////////////////////////////
