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

/** \file dtUtil/xercesparser.h
  * \author John K. Grant
  */
#ifndef DELTA_XERCES_PARSER_INC
#define DELTA_XERCES_PARSER_INC

#include <string>
#include <xercesc/sax2/ContentHandler.hpp>
#include <dtUtil/export.h>

XERCES_CPP_NAMESPACE_BEGIN
   class SAX2XMLReader;
XERCES_CPP_NAMESPACE_END

namespace dtUtil
{
   /** \brief A class to unify the usage of Xerces parsing.
     * This class uses the Xerces SAX2 parser.  Client code
     * must provide a Xerces ContentHandler instance, and the
     * XML data file to be parsed.
     */
   class DT_UTIL_EXPORT XercesParser
   {
   public:
      XercesParser();
      ~XercesParser();

      static void StaticInit();
      static void StaticShutdown();


      /** \brief The function that parses the file.
        * This function uses the Xerces SAX2Reader to parse a XML document.
        * \param data the file to be parsed.
        * \param handler the object to handle content within the file to be parsed.
        * \param schema the file that defines the schema requirements.
        * \return if parsing the file threw an exception, return is false.  Otherwise, true.
        * \throw SAXParseException if a parsing error occurs.
        */
      bool Parse(const std::string& data,
                 XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler& handler,
                 const std::string& schema="");
   private:
      XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader* mParser;
   };
}

#endif  // DELTA_XERCES_PARSER_INC
