/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * Matthew W. Campbell
 * David Guthrie
 */

#ifndef DELTA_NAMED_UNSIGNED_SHORT_INT_PARAMETER
#define DELTA_NAMED_UNSIGNED_SHORT_INT_PARAMETER

#include <dtCore/namedpodparameter.h>

namespace dtCore
{
   /**
     * @class UnsignedShortIntMessageParameter
     */
   class DT_CORE_EXPORT NamedUnsignedShortIntParameter: public NamedPODParameter<unsigned short>
   {
      public:
         NamedUnsignedShortIntParameter(const dtUtil::RefString& name,
            unsigned short defaultValue = 0, bool isList = false);
      protected:
         virtual ~NamedUnsignedShortIntParameter();
   };
}

#endif //DELTA_NAMED_UNSIGNED_SHORT_INT_PARAMETER
