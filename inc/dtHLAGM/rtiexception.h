/* -*-c++-*-
 * Delta3D
 * Copyright 2012, MASA Group Inc.
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
 */

#ifndef RTIEXCEPTION_H_
#define RTIEXCEPTION_H_

#include <dtHLAGM/export.h>
#include <dtUtil/exception.h>

namespace dtHLAGM
{

   /**
    * Delta3D Exception to wrap the internal rti exceptions.
    */
   class DT_HLAGM_EXPORT RTIException: public dtUtil::Exception
   {
   public:
      RTIException(const std::string& message, const std::string& filename, unsigned int linenum);
      virtual ~RTIException();
   };

}
#endif /* RTIEXCEPTION_H_ */
