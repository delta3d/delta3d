/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006-2011, Alion Science and Technology, BMH Operation.
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

#ifndef DELTA_BASE_XML_HANDLER
#define DELTA_BASE_XML_HANDLER

#include <string>
#include <stack>

#include <dtCore/export.h>
#include <dtCore/refptr.h>
#include <osg/Referenced>

#include <xercesc/sax2/ContentHandler.hpp>
#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/EntityResolver.hpp>

namespace dtUtil
{
   class Log;
}

namespace dtCore
{
   /**
    * @class BaseXMLHandler
    * @brief The SAX2 content handler for handling xml.
    */
   class DT_CORE_EXPORT BaseXMLHandler: public xercesc::ContentHandler, public xercesc::ErrorHandler,
                                   public xercesc::EntityResolver, public osg::Referenced
   {
   public:

      //A string using the xerces multibyte character.  This allows
      //for easier coding.
      typedef std::basic_string<XMLCh> xmlCharString;

      ///Constructor
      BaseXMLHandler();

      /**
       * @see DocumentHandler#startDocument
       */
      virtual void startDocument();

      /**
       * @see DocumentHandler#endDocument
       */
      virtual void endDocument();

      /**
       * Any previously held onto map created during parsing will be deleted.
       * @see DocumentHandler#resetDocument
       */
      virtual void resetDocument();

      /**
       * @see DocumentHandler#startElement
       */
      virtual void startElement( const XMLCh*  const  uri,
                                 const XMLCh*  const  localname,
                                 const XMLCh*  const  qname,
                                 const xercesc::Attributes& attrs );
      virtual void ElementStarted( const XMLCh*  const  uri,
                                 const XMLCh*  const  localname,
                                 const XMLCh*  const  qname,
                                 const xercesc::Attributes& attrs);

      /**
       * @see DocumentHandler#endElement
       */
      virtual void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
      virtual void ElementEnded(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);

      /**
       * @see DocumentHandler#characters
       */
#if XERCES_VERSION_MAJOR < 3
      virtual void characters(const XMLCh* const chars, const unsigned int length);
      virtual void ignorableWhitespace(const XMLCh* const, const unsigned int);
#else
      virtual void characters(const XMLCh* const chars, const XMLSize_t length);
      virtual void ignorableWhitespace(const XMLCh* const, const XMLSize_t);
#endif

      /**
       * Because the sax parser may give a contiguous set of characters in multiple calls to characters, one should
       * override this function instead of "characters" get all the data
       */
      virtual void CombinedCharacters(const XMLCh* const /*chars*/, size_t /*length*/) {}

      /**
       * @see DocumentHandler#processingInstruction
       */
      virtual void processingInstruction(const   XMLCh* const target, const XMLCh* const   data);

      /**
       * @see Locator
       */
      virtual void setDocumentLocator(const xercesc::Locator* const locator);

      /**
       * @see DocumentHandler#startPrefixMapping
       */
      virtual void startPrefixMapping(const XMLCh* const prefix, const XMLCh* const uri);

      /**
       * @see DocumentHandler#endPrefixMapping
       */
      virtual void endPrefixMapping(const XMLCh* const prefix);

      /**
       * @see DocumentHandler#skippedEntity
       */
      virtual void skippedEntity(const XMLCh* const name);

      //@}

      /** @name Default implementation of the EntityResolver interface. */

      //@{
      /**
       * @see EntityResolver#resolveEntity
       */
      virtual xercesc::InputSource* resolveEntity(
         const XMLCh* const publicId,
         const XMLCh* const systemId);

      //@}

      /** @name Default implementation of the ErrorHandler interface */
      //@{
      /**
       * @see xercesc::ErrorHandler#warning
       * @see xercesc::SAXParseException#SAXParseException
       */
      virtual void error(const xercesc::SAXParseException& exc);

      /**
       * @see xercesc::ErrorHandler#fatalError
       * @see xercesc::SAXParseException#SAXParseException
       */
      virtual void fatalError(const xercesc::SAXParseException& exc);


      /**
       * @see xercesc::ErrorHandler#warning
       * @see xercesc::SAXParseException#SAXParseException
       */
      virtual void warning(const xercesc::SAXParseException& exc);

      /**
       * @see xercesc::ErrorHandler#resetErrors
       */
      virtual void resetErrors();

      virtual void Reset();

      /**
       * Returns whether the data this handler wanted to parse has been reached
       * or not.  The default case returns true when the end of the document
       * has been reached, but if you only need to read to a certain piece of
       * information, you can override this function for your case.
       * @see MapHeaderHandler::HandledDesiredData
       */
      virtual bool HandledDesiredData() const;

      dtUtil::Log* mLogger;

   protected: // This class is referenced counted, but this causes an error...

      virtual ~BaseXMLHandler();

      // -----------------------------------------------------------------------
      //  Unimplemented constructors and operators
      // -----------------------------------------------------------------------
      BaseXMLHandler(const BaseXMLHandler&);
      BaseXMLHandler& operator=(const BaseXMLHandler&);

      virtual std::string MakeErrorString(const xercesc::SAXParseException &exc);

      std::stack<xmlCharString> mElements;

      xmlCharString mCombinedCharacters;

      int mErrorCount;
      int mFatalErrorCount;
      int mWarningCount;
      bool mReachedEnd;
   };
}
#endif
