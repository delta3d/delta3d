/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
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
 * Curtiss Murphy
 */
#ifndef DELTA_DYNAMICVEC2CONTROL
#define DELTA_DYNAMICVEC2CONTROL

#include <dtQt/dynamicabstractparentcontrol.h>
#include <dtQt/dynamicvectorelementcontrol.h>
#include <dtCore/refptr.h>

#include <QtCore/QString>

namespace dtQt
{


   // this has to be done here because Qt doesn't support template q objects, so the one slot
   // is implemented in a super class.
   class DynamicNoUpdateParentControl: public DynamicAbstractParentControl
   {
      Q_OBJECT;
   public slots:
      virtual bool updateData(QWidget* widget)
      {
         return false;
      }
   };

   /**
    * @class DynamicVecNControl
    * @brief This is the dynamic control for the N dimensional vector data type -
    * used in the property editor
    * @Note It adds a group of child elements to the tree, since you can't edit multiple things
    * in one control easily.
    */
   template <typename PropertyType>
   class DynamicVecNControl : public DynamicNoUpdateParentControl
   {
   public:
      typedef typename PropertyType::GetValueType PropertyGetValueType;
      /**
       * Constructor
       */
      DynamicVecNControl();

      /**
       * Destructor
       */
      ~DynamicVecNControl();

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

   protected:

      DynamicVectorElementControl* CreateElementControl(PropertyType* prop, int index, const std::string& label,
               PropertyEditorModel* newModel, dtDAL::PropertyContainer* newPC);
   private:
      DynamicVectorElementControl* xElement;
      DynamicVectorElementControl* yElement;
      DynamicVectorElementControl* zElement;
      DynamicVectorElementControl* wElement;

      // the tool tip type label indicates that the vector is a float or a double
      std::string toolTipTypeLabel;

      dtCore::RefPtr<PropertyType> mProperty;
   };

   ///////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   DynamicVecNControl<PropertyType>::DynamicVecNControl()
      : xElement(NULL)
      , yElement(NULL)
      , zElement(NULL)
      , wElement(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   DynamicVecNControl<PropertyType>::~DynamicVecNControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   DynamicVectorElementControl* DynamicVecNControl<PropertyType>::CreateElementControl(PropertyType* prop, int index, const std::string& label,
            PropertyEditorModel* newModel, dtDAL::PropertyContainer* newPC)
   {
      DynamicVectorElementControl* control = new DynamicVectorElementControl(prop, index, label);
      control->InitializeData(this, newModel, newPC, prop);
      mChildren.push_back(control);

      connect(control, SIGNAL(PropertyAboutToChange(dtDAL::PropertyContainer&, dtDAL::ActorProperty&,
                        const std::string&, const std::string&)),
               this, SLOT(PropertyAboutToChangePassThrough(dtDAL::PropertyContainer&, dtDAL::ActorProperty&,
                        const std::string&, const std::string&)));

      connect(control, SIGNAL(PropertyChanged(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)),
               this, SLOT(PropertyChangedPassThrough(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)));

      return control;
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   void DynamicVecNControl<PropertyType>::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtDAL::PropertyContainer* newPC, dtDAL::ActorProperty* newProperty)
   {
      DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);

      mProperty = static_cast<PropertyType*>(newProperty);

      // create X
      xElement = CreateElementControl(mProperty.get(), 0, "X", newModel, newPC);
      // create Y
      yElement = CreateElementControl(mProperty.get(), 1, "Y", newModel, newPC);

      // Static if
      if (PropertyGetValueType::num_components > 2)
      {
         zElement = CreateElementControl(mProperty.get(), 2, "Z", newModel, newPC);
      }

      // Static if
      if (PropertyGetValueType::num_components > 3)
      {
         wElement = CreateElementControl(mProperty.get(), 3, "W", newModel, newPC);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   const QString DynamicVecNControl<PropertyType>::getDisplayName()
   {
         return tr(mProperty->GetLabel().c_str());
    }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   const QString DynamicVecNControl<PropertyType>::getDescription()
   {
      std::string tooltip = mProperty->GetDescription() + "  [Type: " +
         mProperty->GetDataType().GetName() + "]";
      return tr(tooltip.c_str());
   }

   /////////////////////////////////////////////////////////////////////////////////
   template <typename PropertyType>
   const QString DynamicVecNControl<PropertyType>::getValueAsString()
   {
      typedef typename PropertyGetValueType::value_type VecNValueType;
      DynamicAbstractControl::getValueAsString();

      const unsigned int NUM_DECIMAL_DIGITS =
         sizeof(VecNValueType) == sizeof(float)
         ? NUM_DECIMAL_DIGITS_FLOAT : NUM_DECIMAL_DIGITS_DOUBLE;

      PropertyGetValueType vectorValue = mProperty->GetValue();

      QString result = "(X="  + QString::number(vectorValue.x(), 'f', NUM_DECIMAL_DIGITS) +
         ", Y=" + QString::number(vectorValue.y(), 'f', NUM_DECIMAL_DIGITS) ;

      // Static if
      if (PropertyGetValueType::num_components > 2)
      {
         //Had to use [] access because the code still must parse for vecs without a .z()
         result += ", Z=" + QString::number(vectorValue[2], 'f', NUM_DECIMAL_DIGITS) ;
      }

      // Static if
      if (PropertyGetValueType::num_components > 3)
      {
         //Had to use [] access because the code still must parse for vecs without a .w()
         result += ", W=" + QString::number(vectorValue[3], 'f', NUM_DECIMAL_DIGITS);
      }

      result += ")";
      return result;
   }

   template <typename PropertyType>
   bool DynamicVecNControl<PropertyType>::isEditable()
   {
      return false;
   }

} // namespace dtEditQt

#endif // DELTA_DYNAMICVEC2CONTROL
