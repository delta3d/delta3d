/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2006 MOVES Institute
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
 * @author Erik Johnson 05/16/2006
 */

#ifndef __DELTA_CORE_EXCEPTIONENUM_H__
#define __DELTA_CORE_EXCEPTIONENUM_H__

#include <dtUtil/enumeration.h>
#include <dtCore/export.h>

namespace dtCore
{
   /**
    * Exceptions for the dtCore namespace
    */
   class DT_CORE_EXPORT ExceptionEnum : public dtUtil::Enumeration
   {
      DECLARE_ENUM(ExceptionEnum);
      public:

         static ExceptionEnum INVALID_PARAMETER;
   
   protected:
      ExceptionEnum(const std::string &name):Enumeration(name)
      {
         AddInstance(this);
      }
   
   };
}//namespace dtCore

#endif // __DELTA_CORE_EXCEPTIONENUM_H__
