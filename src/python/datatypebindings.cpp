/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2004-2006 MOVES Institute 
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
 * Chris Osborn
 */

#include <dtCore/datatype.h>
#include <python/dtpython.h>

using namespace boost::python;
using namespace dtCore;

void initDataTypeBindings()
{
   class_< DataType, bases<dtUtil::Enumeration>, boost::noncopyable >( "DataType", no_init )
      .def_readonly( "UNKNOWN", &DataType::UNKNOWN )
      .def_readonly( "CHAR", &DataType::CHAR )
      .def_readonly( "UCHAR", &DataType::UCHAR )
      .def_readonly( "FLOAT", &DataType::FLOAT )
      .def_readonly( "DOUBLE", &DataType::DOUBLE )
      .def_readonly( "INT", &DataType::INT )
      .def_readonly( "UINT", &DataType::UINT )
      .def_readonly( "LONGINT", &DataType::LONGINT )
      .def_readonly( "ULONGINT", &DataType::ULONGINT )
      .def_readonly( "SHORTINT", &DataType::SHORTINT )
      .def_readonly( "USHORTINT", &DataType::USHORTINT )
      .def_readonly( "STRING", &DataType::STRING )
      .def_readonly( "BOOLEAN", &DataType::BOOLEAN )
      .def_readonly( "VEC4", &DataType::VEC4 )
      .def_readonly( "VEC3", &DataType::VEC3 )
      .def_readonly( "VEC2", &DataType::VEC2 )
      .def_readonly( "VEC4F", &DataType::VEC4F )
      .def_readonly( "VEC3F", &DataType::VEC3F )
      .def_readonly( "VEC2F", &DataType::VEC2F )
      .def_readonly( "VEC4D", &DataType::VEC4D )
      .def_readonly( "VEC3D", &DataType::VEC3D )
      .def_readonly( "VEC2D", &DataType::VEC2D )
      .def_readonly( "RGBCOLOR", &DataType::RGBCOLOR )
      .def_readonly( "RGBACOLOR", &DataType::RGBACOLOR )
      .def_readonly( "ENUMERATION", &DataType::ENUMERATION )
      .def_readonly( "STATIC_MESH", &DataType::STATIC_MESH )
      .def_readonly( "TEXTURE", &DataType::TEXTURE )
      .def_readonly( "TERRAIN", &DataType::TERRAIN )
      .def_readonly( "SOUND", &DataType::SOUND )
      .def_readonly( "PARTICLE_SYSTEM", &DataType::PARTICLE_SYSTEM )
      .def_readonly( "SKELETAL_MESH", &DataType::SKELETAL_MESH )
      .def( "IsResource", &DataType::IsResource )
      .def( "GetDisplayName", &DataType::GetDisplayName, return_value_policy< copy_const_reference >() )
      .def( "GetTypeId", &DataType::GetTypeId )
      .def( "SetTypeId", &DataType::SetTypeId )
      ;
}
