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

#ifndef DELTA_BASE_PROPERTY_CONTROL_FACTORY_H
#define DELTA_BASE_PROPERTY_CONTROL_FACTORY_H

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <dtQt/export.h>
#include <dtQt/basepropertycontrol.h>
#include <dtCore/datatype.h>
#include <dtCore/refptr.h>
#include <dtCore/propertycontainer.h>
#include <dtUtil/objectfactory.h>
#include <osg/Referenced>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
namespace dtQt
{
   class BasePropertyControl;



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   class AbstractPropertyControlFactory : public osg::Referenced
   {
   public:
      virtual BasePropertyControl* CreateControl(const std::string& dataType) = 0;

   protected:
      virtual ~AbstractPropertyControlFactory()
      {}
   };



   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   template <typename T_DataType>
   class BasePropertyControlFactory : public AbstractPropertyControlFactory
   {
   public:
      typedef AbstractPropertyControlFactory BaseClass;

      typedef T_DataType Type_DataType;

      BasePropertyControlFactory()
         : mControlFactory(new dtUtil::ObjectFactory<T_DataType*, BasePropertyControl>)
      {
      }

      virtual BasePropertyControl* CreateControl(T_DataType& dataType)
      {
         BasePropertyControl* control = mControlFactory->CreateObject(&dataType);
         control->Init();

         return control;
      }

      /*virtual*/ BasePropertyControl* CreateControl(const std::string& dataTypeName)
      {
         BasePropertyControl* control = NULL;

         T_DataType* dataType = GetDataTypeByName(dataTypeName);

         if (dataType != NULL)
         {
            control = mControlFactory->CreateObject(dataType);
         }

         return control;
      }

      virtual T_DataType* GetDataTypeByName(const std::string& typeName) = 0;

      template <typename T_ControlType>
      void RegisterControlForDataType(T_DataType& dataType)
      {
         mControlFactory->RemoveType(&dataType);
         mControlFactory->template RegisterType<T_ControlType>(&dataType);
      }

   protected:
      virtual ~BasePropertyControlFactory()
      {}

      dtCore::RefPtr<dtUtil::ObjectFactory<T_DataType*, BasePropertyControl> > mControlFactory;
   };

}

#endif
