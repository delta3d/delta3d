/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
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
*/

/** \file dtUtil/xerceserrorhandler.h
  * \author John K. Grant
  */

#ifndef DTUTIL_XERCES_ERROR_HANDLER_INC
#define DTUTIL_XERCES_ERROR_HANDLER_INC

#include <dtUtil/export.h>

#include <xercesc/sax/ErrorHandler.hpp>       // for base class
#include <xercesc/sax/SAXParseException.hpp>  // for base class

namespace dtUtil
{
   /** \brief Logs Xerces parsing errors.
     * Used with the Xerces SAX2XMLReader.  Create an instance of this class when creating
     * SAX2XMLReader and set it as the error handler with the parser's 'setErrorHandler' method.
     * This class is used by classes that parse XML files.  It is not needed by client code.
     */
   class DT_UTIL_EXPORT XercesErrorHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ErrorHandler
   {
   public:
      XercesErrorHandler();
      ~XercesErrorHandler();

      virtual void warning(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e);

      /// \throw SAXParseException Upon errors
      virtual void error(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e);

      /// \throw SAXParseException Upon fatal errors
      virtual void fatalError(const XERCES_CPP_NAMESPACE_QUALIFIER SAXParseException& e);

      virtual void resetErrors();
   };
}

#endif // DTUTIL_XERCES_ERROR_HANDLER_INC
