/* -*-c++-*-
 * delta3d
 * Copyright 2014, David Guthrie
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

#ifndef SYSTEMCOMPONENTTYPE_H_
#define SYSTEMCOMPONENTTYPE_H_

#include <dtCore/actortype.h>
#include <dtCore/refptr.h>
#include <string>

namespace dtCore
{

   class DT_CORE_EXPORT SystemComponentType: public dtCore::ActorType
   {
   public:
      static dtCore::RefPtr<const SystemComponentType> BaseSystemComponentType;

      SystemComponentType(const std::string& name,
            const std::string& component = "SystemComponents",
            const std::string& desc="",
            const SystemComponentType* parentType = BaseSystemComponentType);
   protected:
      virtual ~SystemComponentType();
   };

} /* namespace dtCore */

#endif /* SYSTEMCOMPONENTTYPE_H_ */
