/* -*-c++-*-
 * testAPP - Using 'The MIT License'
 * Copyright (C) 2014, Caper Holdings LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/propertycontrolfactory.h>
#include <dtQt/propertycontrolarray.h>
#include <dtQt/propertycontrolbool.h>
#include <dtQt/propertycontrolbitmask.h>
#include <dtQt/propertycontrolcolor.h>
#include <dtQt/propertycontroldouble.h>
#include <dtQt/propertycontrolenum.h>
#include <dtQt/propertycontrolimage.h>
#include <dtQt/propertycontrolint.h>
#include <dtQt/propertycontrolrange.h>
#include <dtQt/propertycontrolresource.h>
#include <dtQt/propertycontrolstring.h>
#include <dtQt/propertycontrolvec4.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   PropertyControlFactory::PropertyControlFactory()
   {
      using namespace dtCore;

      // register all the data types with the dynamic control factory
      /*RegisterControlForDataType<DynamicLongControl>(DataType::LONGINT);
      RegisterControlForDataType<DynamicVecNControl<Vec2ActorProperty> >(DataType::VEC2);
      RegisterControlForDataType<DynamicVecNControl<Vec2fActorProperty> >(DataType::VEC2F);
      RegisterControlForDataType<DynamicVecNControl<Vec2dActorProperty> >(DataType::VEC2D);
      RegisterControlForDataType<DynamicVecNControl<Vec3ActorProperty> >(DataType::VEC3);
      RegisterControlForDataType<DynamicVecNControl<Vec3fActorProperty> >(DataType::VEC3F);
      RegisterControlForDataType<DynamicVecNControl<Vec3dActorProperty> >(DataType::VEC3D);
      RegisterControlForDataType<DynamicVecNControl<Vec4ActorProperty> >(DataType::VEC4);
      RegisterControlForDataType<DynamicVecNControl<Vec4fActorProperty> >(DataType::VEC4F);
      RegisterControlForDataType<DynamicVecNControl<Vec4dActorProperty> >(DataType::VEC4D);
      RegisterControlForDataType<DynamicContainerControl>(DataType::CONTAINER);
      RegisterControlForDataType<DynamicContainerSelectorControl>(DataType::CONTAINER_SELECTOR);
      RegisterControlForDataType<DynamicActorControl>(DataType::ACTOR);
      RegisterControlForDataType<DynamicGameEventControl>(DataType::GAME_EVENT);*/
      RegisterControlForDataType<PropertyControlBool>(DataType::BOOLEAN);
      RegisterControlForDataType<PropertyControlBitMask>(DataType::BIT_MASK);
      RegisterControlForDataType<PropertyControlInt>(DataType::INT);
      RegisterControlForDataType<PropertyControlDouble>(DataType::FLOAT);
      RegisterControlForDataType<PropertyControlDouble>(DataType::DOUBLE);
      RegisterControlForDataType<PropertyControlColor>(DataType::RGBACOLOR);
      RegisterControlForDataType<PropertyControlString>(DataType::STRING);
      RegisterControlForDataType<PropertyControlEnum>(DataType::ENUMERATION);
      RegisterControlForDataType<PropertyControlVec4>(DataType::VEC4F);
      //RegisterControlForDataType<PropertyControlRange>(DataType::RANGE);
      RegisterControlForDataType<PropertyControlArray>(DataType::ARRAY);
      //RegisterControlForDataType<PropertyControlContainer>(DataType::PROPERTY_CONTAINER);
      
      // Resource Types
      RegisterControlForDataType<PropertyControlImage>(DataType::TEXTURE);
      RegisterControlForDataType<PropertyControlStaticMesh>(DataType::STATIC_MESH);
      RegisterControlForDataType<PropertyControlSkeletalMesh>(DataType::SKELETAL_MESH);
      RegisterControlForDataType<PropertyControlTerrain>(DataType::TERRAIN);
      RegisterControlForDataType<PropertyControlSound>(DataType::SOUND);
      RegisterControlForDataType<PropertyControlParticleSystem>(DataType::PARTICLE_SYSTEM);
      RegisterControlForDataType<PropertyControlPrefab>(DataType::PREFAB);
      RegisterControlForDataType<PropertyControlShader>(DataType::SHADER);
      RegisterControlForDataType<PropertyControlDirector>(DataType::DIRECTOR);
   }

   PropertyControlFactory::~PropertyControlFactory()
   {
   }
   
   dtCore::DataType* PropertyControlFactory::GetDataTypeByName(const std::string& typeName)
   {
      return dtCore::DataType::GetValueForName(typeName);
   }

}
