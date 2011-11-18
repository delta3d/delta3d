/* -*-c++-*-
 * Delta3D
 * Copyright 2011, Alion Science and Technology
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * 
 * David Guthrie
 */

#ifndef XERCESBININPUTSTREAMIFSTREAM_H_
#define XERCESBININPUTSTREAMIFSTREAM_H_

#include <dtUtil/export.h>
#include <xercesc/util/BinInputStream.hpp>

#include <iosfwd>

#if XERCES_VERSION_MAJOR < 3 
	typedef unsigned int XMLFilePos;
	typedef unsigned int XMLSize_tt; 
#else
	typedef XMLSize_t XMLSize_tt;
#endif

namespace dtUtil
{
   /**
    * Wraps an std::ifstream for xerces to use.
    */
   class DT_UTIL_EXPORT XercesBinInputStreamIStream : public xercesc::BinInputStream
   {
   public:

      XercesBinInputStreamIStream(std::istream& stream);

      virtual XMLFilePos curPos() const;

      virtual XMLSize_tt readBytes(XMLByte* const toFill, const XMLSize_tt maxToRead);

      virtual const XMLCh* getContentType() const { return 0; }
   private:
      std::istream& mStream;
   };
}

#endif /* XERCESBININPUTSTREAMIFSTREAM_H_ */
