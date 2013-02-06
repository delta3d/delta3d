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

/** \file dtUtil/xerceswriter.h
  * \author John K. Grant
  */
#ifndef DELTA_XERCESWRITER_INC
#define DELTA_XERCESWRITER_INC

#include <osg/Referenced>   // for base class

#include <string>
#include <vector>

#include <xercesc/util/XercesDefs.hpp>
#include <dtUtil/export.h>

XERCES_CPP_NAMESPACE_BEGIN
   class DOMElement;
   class DOMDocument;
   class DOMImplementation;
XERCES_CPP_NAMESPACE_END

namespace dtUtil
{
   /// A class that manages one XML DOM document.
   class DT_UTIL_EXPORT XercesWriter : public osg::Referenced
   {
   public:
      /// Initializes the xerces system.
      XercesWriter();

   protected:
      /// Does <b>NOT</b> destroy the xerces system.
      ~XercesWriter();

   public:
      /// Create a new document for the instance to use.
      /// @param rootname the name of the root XML node.
      void CreateDocument(const std::string& rootname);

      XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* GetDocument() { return mDocument; }
      const XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* GetDocument() const { return mDocument; }

      /// This is the call that serializes the XML Tree
      /// @param the filename
      void WriteFile(const std::string& file);

   private:
      XercesWriter(const XercesWriter&); /// not implemented because copying these members would be bad
      XercesWriter& operator =(const XercesWriter&); /// not implemented because copying these members would be bad

      XERCES_CPP_NAMESPACE_QUALIFIER DOMImplementation* mImplementation;
      XERCES_CPP_NAMESPACE_QUALIFIER DOMDocument* mDocument;
   };
}

#endif  // DELTA_XERCESWRITER_INC
