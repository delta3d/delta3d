/*
 * Delta3D Open Source Game and Simulation Engine
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

/** \file serializer.h
 *  Utility functions for serialization.
 *  John K. Grant
 */

#ifndef _DTUTIL_SERIALIZER_H_
#define _DTUTIL_SERIALIZER_H_

////////////////////////////////////////////////////////////////////////////////

#include <string>
#include <dtUtil/export.h>
#include <xercesc/util/XercesDefs.hpp>

XERCES_CPP_NAMESPACE_BEGIN
  class DOMElement;
  class DOMDocument;
XERCES_CPP_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////

namespace dtUtil
{
   /** A place to implement functions for serialization.
    */
   struct DT_UTIL_EXPORT Serializer
   {
      /** Creates an XML Node.
       * @param value The important value to be stored.
       * @param name The name of the XML Node to be created.
       * @param doc The Xerces DOMDocument required for creating new XML Nodes.
       */
      static XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* ToInt(const int value, const char* name, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc);

      /** Creates an XML Node.
       * @param value The important value to be stored.
       * @param name The name of the XML Node to be created.
       * @param doc The Xerces DOMDocument required for creating new XML Nodes.
       */
      static XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* ToFloat(const float value, const char* name, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc);

      /** Creates an XML Node.
       * @param value The important value to be stored.
       * @param name The name of the XML Node to be created.
       * @param doc The Xerces DOMDocument required for creating new XML Nodes.
       */
      static XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* ToDouble(const double value, const char* name, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc);

      /** Creates an XML Node.
       * @param value The important value to be stored.
       * @param name The name of the XML Node to be created.
       * @param doc The Xerces DOMDocument required for creating new XML Nodes.
       */
      static XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* ToBool(const bool value, const char* name, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc);

      /** Creates an XML Node.
       * @param value The important value to be stored.
       * @param name The name of the XML Node to be created.
       * @param doc The Xerces DOMDocument required for creating new XML Nodes.
       */
      static XERCES_CPP_NAMESPACE_QUALIFIER DOMElement* ToString(const std::string& value, const char* name, XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* doc);
   };
}

////////////////////////////////////////////////////////////////////////////////

#endif // _DTUTIL_SERIALIZER_H_
