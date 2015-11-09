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

#include <prefix/dtcoreprefix.h>
#include <dtCore/systemcomponenttype.h>

namespace dtCore
{

   dtCore::RefPtr<const SystemComponentType>  SystemComponentType::BaseSystemComponentType(new SystemComponentType("Base", "SystemComponents", "Base type for system components.", NULL));

   SystemComponentType::SystemComponentType(const std::string& name,
         const std::string& component,
         const std::string& desc,
         const SystemComponentType* parentType)
   : ActorType(name, component, desc, parentType)
   {
   }

   SystemComponentType::~SystemComponentType()
   {
   }

} /* namespace dtCore */
