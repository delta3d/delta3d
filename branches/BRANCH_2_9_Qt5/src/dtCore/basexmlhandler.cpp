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

#include <prefix/dtcoreprefix.h>

#include <typeinfo>

#include <dtCore/basexmlhandler.h>

#include <dtUtil/xercesutils.h>
#include <dtUtil/log.h>

#include <dtUtil/deprecationmgr.h>
DT_DISABLE_WARNING_ALL_START

#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/sax/SAXParseException.hpp>

DT_DISABLE_WARNING_END

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

namespace  dtCore
{
   /////////////////////////////////////////////////////////////////
   BaseXMLHandler::BaseXMLHandler()
   {
      mLogger = &dtUtil::Log::GetInstance ( "BaseXMLHandler.cpp" );
      //mLogger->SetLogLevel(dtUtil::Log::LOG_DEBUG);
      mLogger->LogMessage(DT_LOG_SOURCE, "Creating Map Content Handler.", dtUtil::Log::LOG_INFO);
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
         mLogger->LogMessage(DT_LOG_SOURCE, "Parsing Map Document Started.", dtUtil::Log::LOG_DEBUG);

      Reset();
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLHandler::endDocument()
   {
      mReachedEnd = true;
      mLogger->LogMessage(DT_LOG_SOURCE, "Parsing Map Document Ended.", dtUtil::Log::LOG_DEBUG);
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
         mLogger->LogMessage(DT_LOG_SOURCE, "Found element " + dtUtil::XMLStringConverter(localname).ToString(),
                             dtUtil::Log::LOG_DEBUG);
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
         mLogger->LogMessage(DT_LOG_SOURCE,
            "Attempting to pop elements off of stack and the stack is empty."
            "it should at least contain element " +
             dtUtil::XMLStringConverter(localname).ToString() + ".",
             dtUtil::Log::LOG_ERROR);
         return;
      }

      const XMLCh* lname = mElements.top().c_str();

      if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
      {
         mLogger->LogMessage(DT_LOG_SOURCE,
            "Ending element: \"" + dtUtil::XMLStringConverter(lname).ToString() + "\"",
            dtUtil::Log::LOG_DEBUG);
      }

      if (XMLString::compareString(lname, localname) != 0)
      {
         mLogger->LogMessage(DT_LOG_SOURCE,
            "Attempting to pop mElements off of stack and the element "
            "at the top (" + dtUtil::XMLStringConverter(lname).ToString() + ") is not the same as the element ending (" +
            dtUtil::XMLStringConverter(localname).ToString() + ").", dtUtil::Log::LOG_ERROR);
      }

      CombinedCharacters(mCombinedCharacters.c_str(), mCombinedCharacters.size());
      mCombinedCharacters.clear();

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

      if (!mElements.empty())
      {
         xmlCharString& topEl = mElements.top();

         mCombinedCharacters.append(chars, length);

         if (mLogger->IsLevelEnabled(dtUtil::Log::LOG_DEBUG))
         {
            mLogger->LogMessage(DT_LOG_SOURCE,
               "Found characters for element '" +
               dtUtil::XMLStringConverter(topEl.c_str()).ToString() + "' '" + dtUtil::XMLStringConverter(chars).ToString() + "'",
               dtUtil::Log::LOG_DEBUG);
         }
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
      mLogger->LogMessage(DT_LOG_SOURCE, "ERROR: " + MakeErrorString(exc), dtUtil::Log::LOG_ERROR);
      throw exc;
   }
   /////////////////////////////////////////////////////////////////
   void BaseXMLHandler::fatalError(const xercesc::SAXParseException& exc)
   {
      mLogger->LogMessage(DT_LOG_SOURCE, "FATAL ERROR: " + MakeErrorString(exc), dtUtil::Log::LOG_ERROR);
      throw exc;
   }

   /////////////////////////////////////////////////////////////////
   void BaseXMLHandler::warning(const xercesc::SAXParseException& exc)
   {
      mLogger->LogMessage(DT_LOG_SOURCE, "WARNING: " + MakeErrorString(exc), dtUtil::Log::LOG_WARNING);
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
      mCombinedCharacters.clear();

      resetErrors();
      mReachedEnd = false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   bool BaseXMLHandler::HandledDesiredData() const
   {
      return mReachedEnd;
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
