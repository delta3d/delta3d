/* -*-c++-*-
* Delta3D Simulation Training And Game Editor (STAGE)
* STAGE - dynamicfloatcontrol (.h & .cpp) - Using 'The MIT License'
* Copyright (C) 2005-2008, Alion Science and Technology Corporation
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
*
* This software was developed by Alion Science and Technology Corporation under
* circumstances in which the U. S. Government may have rights in the software.
*
* Jeffrey Houde
*/
#ifndef DELTA_DYNAMICCONTAINERCONTROL
#define DELTA_DYNAMICCONTAINERCONTROL

#include <dtQt/export.h>
#include <dtQt/dynamicabstractparentcontrol.h>
#include <dtQt/dynamicsubwidgets.h>

namespace dtDAL
{
    class ContainerActorProperty;
}

namespace dtQt
{
   /**
   * @class DynamicContainerControl
   * @brief This is the dynamic control for the float data type - used in the property editor
   */
   class DT_QT_EXPORT DynamicContainerControl : public DynamicAbstractParentControl
   {
      Q_OBJECT
   public:
      /**
      * Constructor
      */
      DynamicContainerControl();

      /**
      * Destructor
      */
      ~DynamicContainerControl();

      /**
      * @see DynamicAbstractControl#InitializeData
      */
      virtual void InitializeData(DynamicAbstractControl* newParent, PropertyEditorModel* model,
         dtDAL::PropertyContainer* newPC, dtDAL::ActorProperty* property);

      /**
      * @see DynamicAbstractControl#getDisplayName
      */
      virtual const QString getDisplayName();

      /**
      * @see DynamicAbstractControl#getDescription
      */
      virtual const QString getDescription();

      /**
      * @see DynamicAbstractControl#getValueAsString
      */
      virtual const QString getValueAsString();

      /**
      * @see DynamicAbstractControl#isEditable
      */
      virtual bool isEditable();


   public slots:

      /**
      * Signal when the data is updated.
      */
      virtual bool updateData(QWidget* widget);


   private:

      dtCore::RefPtr<dtDAL::ContainerActorProperty> mProperty;
   };
}

#endif
