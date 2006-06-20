/*
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
 * @author David Guthrie
 */
#ifndef DELTA_HLAGM_EXCEPTION_ENUM
#define DELTA_HLAGM_EXCEPTION_ENUM

#include <dtUtil/enumeration.h>
#include "dtHLAGM/export.h"

namespace dtHLAGM
{

   class DT_HLAGM_EXPORT ExceptionEnum : public dtUtil::Enumeration
   {
      DECLARE_ENUM(ExceptionEnum);
      public:
         ///Used when an exception was received from the XML parser.
         static ExceptionEnum XML_INTERNAL_EXCEPTION;

         ///Used when the parser encounters errors or the parser is unable to setup the configuration.
         static ExceptionEnum XML_CONFIG_EXCEPTION;
      protected:
         ExceptionEnum(const std::string &name) : dtUtil::Enumeration(name)
         {
            AddInstance(this);
         }
   };

}

#endif /*DELTA_HLAGM_EXCEPTION_ENUM*/
