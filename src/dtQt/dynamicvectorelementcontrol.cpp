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

#include <prefix/dtqtprefix.h>
#include <dtQt/dynamicvectorelementcontrol.h>

#include <dtCore/actorproxy.h>
#include <dtCore/datatype.h>
#include <dtCore/vectoractorproperties.h>

#include <dtQt/dynamicsubwidgets.h>

#include <dtUtil/log.h>

#include <QtCore/QLocale>

#include <QtGui/QDoubleValidator>
#include <QtWidgets/QWidget>

#include <sstream>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtCore::Vec2fActorProperty* newVectorProp,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2fProp = newVectorProp;
      mVec3fProp = NULL;
      mVec4fProp = NULL;
      mVec2dProp = NULL;
      mVec3dProp = NULL;
      mVec4dProp = NULL;

      mActiveProp = newVectorProp;

      mWhichType = VEC2F;

      // determine whether we have floats or doubles
      // Doesn't matter whether this is a vec2, 3, or 4.
      mIsVecFloat = (sizeof(dtCore::Vec2fActorProperty::GetValueType::value_type) == sizeof(float));
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtCore::Vec2dActorProperty* newVectorProp,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2fProp = NULL;
      mVec3fProp = NULL;
      mVec4fProp = NULL;
      mVec2dProp = newVectorProp;
      mVec3dProp = NULL;
      mVec4dProp = NULL;

      mActiveProp = newVectorProp;

      mWhichType = VEC2D;
      // determine whether we have floats or doubles
      // Doesn't matter whether this is a vec2, 3, or 4.
      mIsVecFloat = (sizeof(dtCore::Vec2dActorProperty::GetValueType::value_type) == sizeof(float));
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtCore::Vec3fActorProperty* newVectorProp,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2fProp = NULL;
      mVec3fProp = newVectorProp;
      mVec4fProp = NULL;
      mVec2dProp = NULL;
      mVec3dProp = NULL;
      mVec4dProp = NULL;

      mActiveProp = newVectorProp;

      mWhichType = VEC3F;
      mIsVecFloat = (sizeof(dtCore::Vec3fActorProperty::GetValueType::value_type) == sizeof(float));
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtCore::Vec3dActorProperty* newVectorProp,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2fProp = NULL;
      mVec3fProp = NULL;
      mVec4fProp = NULL;
      mVec2dProp = NULL;
      mVec3dProp = newVectorProp;
      mVec4dProp = NULL;

      mActiveProp = newVectorProp;

      mWhichType = VEC3D;
      mIsVecFloat = (sizeof(dtCore::Vec3dActorProperty::GetValueType::value_type) == sizeof(float));
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtCore::Vec4fActorProperty* newVectorProp,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2fProp = NULL;
      mVec3fProp = NULL;
      mVec4fProp = newVectorProp;
      mVec2dProp = NULL;
      mVec3dProp = NULL;
      mVec4dProp = NULL;

      mActiveProp = newVectorProp;

      mWhichType = VEC4F;
      mIsVecFloat = (sizeof(dtCore::Vec4fActorProperty::GetValueType::value_type) == sizeof(float));
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtCore::Vec4dActorProperty* newVectorProp,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2fProp = NULL;
      mVec3fProp = NULL;
      mVec4fProp = NULL;
      mVec2dProp = NULL;
      mVec3dProp = NULL;
      mVec4dProp = newVectorProp;

      mActiveProp = newVectorProp;

      mWhichType = VEC4D;
      mIsVecFloat = (sizeof(dtCore::Vec4dActorProperty::GetValueType::value_type) == sizeof(float));
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::~DynamicVectorElementControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicVectorElementControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);
      if (mIsVecFloat)
      {
         mToolTipTypeLabel = "FLOAT";
      }
      else
      {
         mToolTipTypeLabel = "DOUBLE";
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicVectorElementControl::updateEditorFromModel(QWidget* widget)
   {
      if (widget != NULL && widget == mTemporaryEditControl)
      {
         std::ostringstream ss;
         ss << "Updating editor for index " << mElementIndex;
         LOGN_DEBUG("dynamicvectorelementcontrol.cpp", ss.str());
         // set the current value from our property
         const double value = getValue();

         osg::Vec3 testVect;

         QString strValue = RealToString(value, mIsVecFloat ? NUM_DECIMAL_DIGITS_FLOAT : NUM_DECIMAL_DIGITS_DOUBLE);
         mTemporaryEditControl->setText(strValue);
         mTemporaryEditControl->selectAll();
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicVectorElementControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      bool dataChanged = false;

      if (widget != NULL && widget == mTemporaryEditControl)
      {
         // Note, don't use the temporary variable here.  It can cause errors with QT.
         SubQLineEdit* editBox = static_cast<SubQLineEdit*>(widget);
         bool success = false;
         //float floatResult = -999.999f;
         double doubleResult = editBox->text().toDouble(&success);

         // set our value to our object
         if (success)
         {
            // Save the data if they are different.  Note, we also need to compare the QString value,
            // else we get epsilon differences that cause the map to be marked dirty with no edits :(
            QString proxyValue = RealToString(getValue(), mIsVecFloat ? NUM_DECIMAL_DIGITS_FLOAT : NUM_DECIMAL_DIGITS_DOUBLE);
            QString newValue = editBox->text();
            if ((doubleResult != getValue() && proxyValue != newValue) || !doPropertiesMatch())
            {
               dataChanged = setValue(doubleResult);
            }
         }
         else
         {
            LOGN_ERROR("dynamicvectorelementcontrol.cpp", "updateData() failed to convert our value successfully");
         }

         // reselect all the text when we commit.
         // Gives the user visual feedback that something happened.
         editBox->selectAll();
      }

      return dataChanged;
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget* DynamicVectorElementControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      std::ostringstream ss;
      ss << "Creating editor for index " << mElementIndex;
      LOGN_DEBUG("dynamicvectorelementcontrol.cpp", ss.str());
      // create and init the edit box
      //editBox = new QLineEdit(parent);
      mTemporaryEditControl = new SubQLineEdit (parent, this);
      QDoubleValidator* validator = new QDoubleValidator(mTemporaryEditControl);
      validator->setDecimals(mIsVecFloat ? NUM_DECIMAL_DIGITS_FLOAT : NUM_DECIMAL_DIGITS_DOUBLE);
      mTemporaryEditControl->setValidator(validator);
      mWrapper = mTemporaryEditControl;

      connect(mTemporaryEditControl, SIGNAL(returnPressed()), this, SLOT(enterPressed()));

      if (!IsInitialized())
      {
         LOGN_ERROR("dynamicvectorelementcontrol.cpp", "Tried to add itself to the parent widget before being initialized");
         return mTemporaryEditControl;
      }

      updateEditorFromModel(mTemporaryEditControl);

      mTemporaryEditControl->setToolTip(getDescription());

      return mTemporaryEditControl;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicVectorElementControl::getDisplayName()
   {
      QString name = DynamicAbstractControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }
      return QString(tr(mLabel.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicVectorElementControl::getDescription()
   {
      std::string tooltip = mActiveProp->GetDescription();

      tooltip += " [Type: " + mLabel;
      return QString(tr(tooltip.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicVectorElementControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();

      if (doPropertiesMatch())
      {
         double value = getValue();
         QString result = RealToString(value, mIsVecFloat ? NUM_DECIMAL_DIGITS_FLOAT : NUM_DECIMAL_DIGITS_DOUBLE);
         return result;
      }
      else
      {
         return "<Multiple Values...>";
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DynamicVectorElementControl::doPropertiesMatch()
   {
      std::vector<LinkedPropertyData>& linkedProperties = mParent->GetLinkedProperties();
      if (linkedProperties.empty())
      {
         return true;
      }

      // Retrieve the value of our base property.
      double baseValue = getValue();

      // Iterate through our linked properties and compare values.
      int count = (int)linkedProperties.size();
      for (int index = 0; index < count; ++index)
      {
         const LinkedPropertyData& data = linkedProperties[index];

         dtCore::ActorProperty* linkedProp = data.property;
         if (linkedProp)
         {
            double linkedValue = 0.0;

            switch (mWhichType)
            {
            case VEC2F:
               {
                  dtCore::Vec2fActorProperty* linkedProp =
                     dynamic_cast<dtCore::Vec2fActorProperty*>(data.property);
                  if (linkedProp)
                  {
                     linkedValue = linkedProp->GetValue()[mElementIndex];
                  }
               } break;
            case VEC2D:
               {
                  dtCore::Vec2dActorProperty* linkedProp =
                     dynamic_cast<dtCore::Vec2dActorProperty*>(data.property);
                  if (linkedProp)
                  {
                     linkedValue = linkedProp->GetValue()[mElementIndex];
                  }
               } break;
            case VEC3F:
               {
                  dtCore::Vec3fActorProperty* linkedProp =
                     dynamic_cast<dtCore::Vec3fActorProperty*>(data.property);
                  if (linkedProp)
                  {
                     linkedValue = linkedProp->GetValue()[mElementIndex];
                  }
               } break;
            case VEC3D:
               {
                  dtCore::Vec3dActorProperty* linkedProp =
                     dynamic_cast<dtCore::Vec3dActorProperty*>(data.property);
                  if (linkedProp)
                  {
                     linkedValue = linkedProp->GetValue()[mElementIndex];
                  }
               } break;
            case VEC4F:
               {
                  dtCore::Vec4fActorProperty* linkedProp =
                     dynamic_cast<dtCore::Vec4fActorProperty*>(data.property);
                  if (linkedProp)
                  {
                     linkedValue = linkedProp->GetValue()[mElementIndex];
                  }
               } break;
            case VEC4D:
               {
                  dtCore::Vec4dActorProperty* linkedProp =
                     dynamic_cast<dtCore::Vec4dActorProperty*>(data.property);
                  if (linkedProp)
                  {
                     linkedValue = linkedProp->GetValue()[mElementIndex];
                  }
               } break;
            }

            // If at any time, one of the linked values do not match
            // the base, then we contain multiple values.
            if (baseValue != linkedValue)
            {
               return false;
            }
         }
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   double DynamicVectorElementControl::getValue() const
   {
      double result = 0.0;

      switch (mWhichType)
      {
      case VEC2F:
         {
            osg::Vec2f vectorValue = mVec2fProp->GetValue();
            result = vectorValue[mElementIndex];
         } break;
      case VEC2D:
         {
            osg::Vec2d vectorValue = mVec2dProp->GetValue();
            result = vectorValue[mElementIndex];
         } break;
      case VEC3F:
         {
            osg::Vec3f vectorValue = mVec3fProp->GetValue();
            result = vectorValue[mElementIndex];
         } break;
      case VEC3D:
         {
            osg::Vec3d vectorValue = mVec3dProp->GetValue();
            result = vectorValue[mElementIndex];
         } break;
      case VEC4F:
         {
            osg::Vec4f vectorValue = mVec4fProp->GetValue();
            result = vectorValue[mElementIndex];
         } break;
      case VEC4D:
         {
            osg::Vec4d vectorValue = mVec4dProp->GetValue();
            result = vectorValue[mElementIndex];
         } break;
      }

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicVectorElementControl::setValue(double value)
   {
      std::string oldValue = mActiveProp->ToString();

      switch (mWhichType)
      {
      case VEC2F:
         {
            osg::Vec2f  vectorValue = mVec2fProp->GetValue();
            vectorValue[mElementIndex] = value;
            mVec2fProp->SetValue(vectorValue);
         } break;
      case VEC2D:
         {
            osg::Vec2d  vectorValue = mVec2dProp->GetValue();
            vectorValue[mElementIndex] = value;
            mVec2dProp->SetValue(vectorValue);
         } break;
      case VEC3F:
         {
            osg::Vec3f  vectorValue = mVec3fProp->GetValue();
            vectorValue[mElementIndex] = value;
            mVec3fProp->SetValue(vectorValue);
         } break;
      case VEC3D:
         {
            osg::Vec3d  vectorValue = mVec3dProp->GetValue();
            vectorValue[mElementIndex] = value;
            mVec3dProp->SetValue(vectorValue);
         } break;
      case VEC4F:
         {
            osg::Vec4f  vectorValue = mVec4fProp->GetValue();
            vectorValue[mElementIndex] = value;
            mVec4fProp->SetValue(vectorValue);
         } break;
      case VEC4D:
         {
            osg::Vec4d  vectorValue = mVec4dProp->GetValue();
            vectorValue[mElementIndex] = value;
            mVec4dProp->SetValue(vectorValue);
         } break;
      }

      std::string newValue = mActiveProp->ToString();

      // give undo manager the ability to create undo/redo events
      emit PropertyAboutToChange(*mPropContainer, *mActiveProp,
         oldValue, newValue);
      emit PropertyChanged(*mPropContainer, *mActiveProp);

      bool dataChanged = oldValue != newValue;

      // Update the value change to all linked properties as well.
      std::vector<LinkedPropertyData>& linkedProperties = mParent->GetLinkedProperties();
      int linkCount = (int)linkedProperties.size();
      for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
      {
         LinkedPropertyData& data = linkedProperties[linkIndex];

         oldValue = data.property->ToString();

         switch (mWhichType)
         {
         case VEC2F:
            {
               dtCore::Vec2fActorProperty* vecProp =
                  dynamic_cast<dtCore::Vec2fActorProperty*>(data.property);
               if (vecProp)
               {
                  osg::Vec2f  linkedValue = vecProp->GetValue();
                  linkedValue[mElementIndex] = value;
                  vecProp->SetValue(linkedValue);
               }
            } break;
         case VEC2D:
            {
               dtCore::Vec2dActorProperty* vecProp =
                  dynamic_cast<dtCore::Vec2dActorProperty*>(data.property);
               if (vecProp)
               {
                  osg::Vec2d  linkedValue = vecProp->GetValue();
                  linkedValue[mElementIndex] = value;
                  vecProp->SetValue(linkedValue);
               }
            } break;
         case VEC3F:
            {
               dtCore::Vec3fActorProperty* vecProp =
                  dynamic_cast<dtCore::Vec3fActorProperty*>(data.property);
               if (vecProp)
               {
                  osg::Vec3f  linkedValue = vecProp->GetValue();
                  linkedValue[mElementIndex] = value;
                  vecProp->SetValue(linkedValue);
               }
            } break;
         case VEC3D:
            {
               dtCore::Vec3dActorProperty* vecProp =
                  dynamic_cast<dtCore::Vec3dActorProperty*>(data.property);
               if (vecProp)
               {
                  osg::Vec3d  linkedValue = vecProp->GetValue();
                  linkedValue[mElementIndex] = value;
                  vecProp->SetValue(linkedValue);
               }
            } break;
         case VEC4F:
            {
               dtCore::Vec4fActorProperty* vecProp =
                  dynamic_cast<dtCore::Vec4fActorProperty*>(data.property);
               if (vecProp)
               {
                  osg::Vec4f  linkedValue = vecProp->GetValue();
                  linkedValue[mElementIndex] = value;
                  vecProp->SetValue(linkedValue);
               }
            } break;
         case VEC4D:
            {
               dtCore::Vec4dActorProperty* vecProp =
                  dynamic_cast<dtCore::Vec4dActorProperty*>(data.property);
               if (vecProp)
               {
                  osg::Vec4d linkedValue = vecProp->GetValue();
                  linkedValue[mElementIndex] = value;
                  vecProp->SetValue(linkedValue);
               }
            } break;
         }

         // give undo manager the ability to create undo/redo events
         emit PropertyAboutToChange(*data.propCon.get(), *data.property,
            oldValue, data.property->ToString());
         emit PropertyChanged(*data.propCon.get(), *data.property);
      }

      return dataChanged;
   }

} // namespace dtQt
