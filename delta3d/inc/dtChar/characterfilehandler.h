/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2007 MOVES Institute 
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
 * Erik Johnson
 */
#ifndef DELTA_CHARACTER_FILE_HANDLER
#define DELTA_CHARACTER_FILE_HANDLER

#include <dtChar/export.h>
#include <dtUtil/macros.h>
#include <xercesc/sax2/ContentHandler.hpp>  // for a base class
#include <vector>
#include <string>

namespace dtChar
{
   /** Simple Xerces XML handler that will store the data read from a character
     * definition .xml file.
     * into a series of containers of strings.
     * Usage:
     * @code
     *  dtUtil::XercesParser parse;
     *  dtChar::CharacterHandler handler;
     *  parser.Parse(filename, handler);
     *
     *  std::string skeletonFilename = handler.mSkeletonFilename;
     *  ...
     * @endcode
     * @code
     *   <character>
     *      <skeleton filename="skel.csf" />
     *      <animation filename="anim1.xaf" />
     *      <animation ...
     *      <mesh filename="mesh1.cmf" />
     *      <mesh ...
     *      <material filename="mat1.crf" />
     *      <material ...
     *   </character>
     * @endcode
     */
   class DT_CHAR_EXPORT CharacterFileHandler : public XERCES_CPP_NAMESPACE_QUALIFIER ContentHandler
   {
   public:
   	CharacterFileHandler();
   	~CharacterFileHandler();

      virtual void characters(const XMLCh* const chars, const unsigned int length) {};
      virtual void endDocument() {};
      virtual void endElement(const XMLCh* const uri,const XMLCh* const localname,const XMLCh* const qname) {};
      virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length) {};
      virtual void processingInstruction(const XMLCh* const target, const XMLCh* const data) {};
      virtual void setDocumentLocator(const XERCES_CPP_NAMESPACE_QUALIFIER Locator* const locator) {};
      virtual void startDocument() {};

      virtual void startElement(const XMLCh* const uri,const XMLCh* const localname,
                                const XMLCh* const qname, const XERCES_CPP_NAMESPACE_QUALIFIER Attributes& attrs);

      virtual void startPrefixMapping(const	XMLCh* const prefix,const XMLCh* const uri) {};
      virtual void endPrefixMapping(const XMLCh* const prefix) {};
      virtual void skippedEntity(const XMLCh* const name) {};


#ifdef DELTA_WIN32
      //need these definitions to properly export a std::vector<std::string>
      template class DT_CHAR_EXPORT std::allocator<std::string>;
      template class DT_CHAR_EXPORT std::vector<std::string>;
#endif

      std::string mName;                            ///<The name of this animated entity
      std::vector<std::string> mAnimationFilenames; ///<Container of animation filenames
      std::vector<std::string> mMaterialFilenames;  ///<Container of material filenames
      std::vector<std::string> mMeshFilenames;      ///<Container of mesh filenames
      std::string mSkeletonFilename;                ///<The one skeleton filename
   };
}
#endif // DELTA_CHARACTER_FILE_HANDLER