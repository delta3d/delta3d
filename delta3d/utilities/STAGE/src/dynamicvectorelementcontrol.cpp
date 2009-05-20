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

#include <prefix/dtstageprefix-src.h>
#include <dtEditQt/dynamicvectorelementcontrol.h>
#include <dtEditQt/dynamicsubwidgets.h>
#include <dtEditQt/editorevents.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/datatype.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtUtil/log.h>

#include <QtCore/QLocale>
#include <QtGui/QWidget>
#include <QtGui/QDoubleValidator>
#include <sstream>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec2ActorProperty* newVectorProp,
      int whichIndex, const std::string &newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2Prop  = newVectorProp;
      mVec3Prop  = NULL;
      mVec4Prop  = NULL;
      mVec2fProp = NULL;
      mVec3fProp = NULL;
      mVec4fProp = NULL;
      mVec2dProp = NULL;
      mVec3dProp = NULL;
      mVec4dProp = NULL;
      mWhichType = VEC2;
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec2fActorProperty* newVectorProp,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2Prop  = NULL;
      mVec3Prop  = NULL;
      mVec4Prop  = NULL;
      mVec2fProp = newVectorProp;
      mVec3fProp = NULL;
      mVec4fProp = NULL;
      mVec2dProp = NULL;
      mVec3dProp = NULL;
      mVec4dProp = NULL;
      mWhichType = VEC2F;
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec2dActorProperty* newVectorProp,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2Prop  = NULL;
      mVec3Prop  = NULL;
      mVec4Prop  = NULL;
      mVec2fProp = NULL;
      mVec3fProp = NULL;
      mVec4fProp = NULL;
      mVec2dProp = newVectorProp;
      mVec3dProp = NULL;
      mVec4dProp = NULL;
      mWhichType = VEC2D;
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec3ActorProperty* newVectorProp,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2Prop  = NULL;
      mVec3Prop  = newVectorProp;
      mVec4Prop  = NULL;
      mVec2fProp = NULL;
      mVec3fProp = NULL;
      mVec4fProp = NULL;
      mVec2dProp = NULL;
      mVec3dProp = NULL;
      mVec4dProp = NULL;
      mWhichType = VEC3;
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec3fActorProperty* newVectorProp,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2Prop  = NULL;
      mVec3Prop  = NULL;
      mVec4Prop  = NULL;
      mVec2fProp = NULL;
      mVec3fProp = newVectorProp;
      mVec4fProp = NULL;
      mVec2dProp = NULL;
      mVec3dProp = NULL;
      mVec4dProp = NULL;
      mWhichType = VEC3F;
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec3dActorProperty* newVectorProp,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2Prop  = NULL;
      mVec3Prop  = NULL;
      mVec4Prop  = NULL;
      mVec2fProp = NULL;
      mVec3fProp = NULL;
      mVec4fProp = NULL;
      mVec2dProp = NULL;
      mVec3dProp = newVectorProp;
      mVec4dProp = NULL;
      mWhichType = VEC3D;
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec4ActorProperty* newVectorProp,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2Prop  = NULL;
      mVec3Prop  = NULL;
      mVec4Prop  = newVectorProp;
      mVec2fProp = NULL;
      mVec3fProp = NULL;
      mVec4fProp = NULL;
      mVec2dProp = NULL;
      mVec3dProp = NULL;
      mVec4dProp = NULL;
      mWhichType = VEC4;
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec4fActorProperty* newVectorProp,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2Prop  = NULL;
      mVec3Prop  = NULL;
      mVec4Prop  = NULL;
      mVec2fProp = NULL;
      mVec3fProp = NULL;
      mVec4fProp = newVectorProp;
      mVec2dProp = NULL;
      mVec3dProp = NULL;
      mVec4dProp = NULL;
      mWhichType = VEC4F;
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::DynamicVectorElementControl(dtDAL::Vec4dActorProperty* newVectorProp,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {
      mVec2Prop  = NULL;
      mVec3Prop  = NULL;
      mVec4Prop  = NULL;
      mVec2fProp = NULL;
      mVec3fProp = NULL;
      mVec4fProp = NULL;
      mVec2dProp = NULL;
      mVec3dProp = NULL;
      mVec4dProp = newVectorProp;
      mWhichType = VEC4D;
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicVectorElementControl::~DynamicVectorElementControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicVectorElementControl::initializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtDAL::ActorProxy* newProxy, dtDAL::ActorProperty* newProperty)
   {
      DynamicAbstractControl::initializeData(newParent, newModel, newProxy, newProperty);

      // determine whether we have floats or doubles
      // Doesn't matter whether this is a vec2, 3, or 4.
      osg::Vec3 testVect;
      mIsVecFloat = (sizeof(testVect.x()) == sizeof(float));
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
         const bool isVecFloat = (sizeof(testVect.x()) == sizeof(float));

         QString strValue = QString::number(value, 'f', isVecFloat ? NUM_DECIMAL_DIGITS_FLOAT : NUM_DECIMAL_DIGITS_DOUBLE);
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
         std::ostringstream ss;
         ss << "Updating model for index " << mElementIndex;
         LOGN_DEBUG("dynamicvectorelementcontrol.cpp", ss.str());

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
            QString proxyValue = QString::number(getValue(), 'f', NUM_DECIMAL_DIGITS_DOUBLE);
            QString newValue = editBox->text();
            if (doubleResult != getValue() && proxyValue != newValue)
            {
               setValue(doubleResult);
               dataChanged = true;
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

      // notify the world (mostly the viewports) that our property changed
      if (dataChanged)
      {
         std::ostringstream ss;
         ss << "Updating model (data changed) for index " << mElementIndex;
         LOGN_DEBUG("dynamicvectorelementcontrol.cpp", ss.str());

         switch (mWhichType)
         {
         case VEC2:  EditorEvents::GetInstance().emitActorPropertyChanged(mProxy, mVec2Prop);  break;
         case VEC2F: EditorEvents::GetInstance().emitActorPropertyChanged(mProxy, mVec2fProp); break;
         case VEC2D: EditorEvents::GetInstance().emitActorPropertyChanged(mProxy, mVec2dProp); break;
         case VEC3:  EditorEvents::GetInstance().emitActorPropertyChanged(mProxy, mVec3Prop);  break;
         case VEC3F: EditorEvents::GetInstance().emitActorPropertyChanged(mProxy, mVec3fProp); break;
         case VEC3D: EditorEvents::GetInstance().emitActorPropertyChanged(mProxy, mVec3dProp); break;
         case VEC4:  EditorEvents::GetInstance().emitActorPropertyChanged(mProxy, mVec4Prop);  break;
         case VEC4F: EditorEvents::GetInstance().emitActorPropertyChanged(mProxy, mVec4fProp); break;
         case VEC4D: EditorEvents::GetInstance().emitActorPropertyChanged(mProxy, mVec4dProp); break;
         }
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
      validator->setDecimals(NUM_DECIMAL_DIGITS_DOUBLE);
      mTemporaryEditControl->setValidator(validator);

      if (!mInitialized)
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
      return QString(tr(mLabel.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicVectorElementControl::getDescription()
   {
      std::string tooltip;

      switch (mWhichType)
      {
      case VEC2:  tooltip = mVec2Prop->GetDescription();  break;
      case VEC2F: tooltip = mVec2fProp->GetDescription(); break;
      case VEC2D: tooltip = mVec2dProp->GetDescription(); break;
      case VEC3:  tooltip = mVec3Prop->GetDescription();  break;
      case VEC3F: tooltip = mVec3fProp->GetDescription(); break;
      case VEC3D: tooltip = mVec3dProp->GetDescription(); break;
      case VEC4:  tooltip = mVec4Prop->GetDescription();  break;
      case VEC4F: tooltip = mVec4fProp->GetDescription(); break;
      case VEC4D: tooltip = mVec4dProp->GetDescription(); break;
      }

      tooltip += " [Type: " + mLabel;
      return QString(tr(tooltip.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicVectorElementControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();

      double value = getValue();
      return QString::number(value, 'f', NUM_DECIMAL_DIGITS_DOUBLE);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicVectorElementControl::isEditable()
   {
      switch (mWhichType)
      {
      case VEC2:  return !mVec2Prop->IsReadOnly();
      case VEC2F: return !mVec2fProp->IsReadOnly();
      case VEC2D: return !mVec2dProp->IsReadOnly();
      case VEC3:  return !mVec3Prop->IsReadOnly();
      case VEC3F: return !mVec3fProp->IsReadOnly();
      case VEC3D: return !mVec3dProp->IsReadOnly();
      case VEC4:  return !mVec4Prop->IsReadOnly();
      case VEC4F: return !mVec4fProp->IsReadOnly();
      case VEC4D: return !mVec4dProp->IsReadOnly();
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   double DynamicVectorElementControl::getValue()
   {
      double result = 0.0;

      switch (mWhichType)
      {
      case VEC2:
         {
            osg::Vec2 vectorValue = mVec2Prop->GetValue();
            result = vectorValue[mElementIndex];
         } break;
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
      case VEC3:
         {
            osg::Vec3 vectorValue = mVec3Prop->GetValue();
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
      case VEC4:
         {
            osg::Vec4 vectorValue = mVec4Prop->GetValue();
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
   void DynamicVectorElementControl::setValue(double value)
   {
      switch (mWhichType)
      {
      case VEC2:
         {
            std::string oldValue    = mVec2Prop->ToString();
            osg::Vec2   vectorValue = mVec2Prop->GetValue();
            vectorValue[mElementIndex] = value;
            mVec2Prop->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(mProxy, mVec2Prop,
               oldValue, mVec2Prop->ToString());
         } break;
      case VEC2F:
         {
            std::string oldValue    = mVec2fProp->ToString();
            osg::Vec2f  vectorValue = mVec2fProp->GetValue();
            vectorValue[mElementIndex] = value;
            mVec2fProp->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(mProxy, mVec2fProp,
               oldValue, mVec2fProp->ToString());
         } break;
      case VEC2D:
         {
            std::string oldValue    = mVec2dProp->ToString();
            osg::Vec2d  vectorValue = mVec2dProp->GetValue();
            vectorValue[mElementIndex] = value;
            mVec2dProp->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(mProxy, mVec2dProp,
               oldValue, mVec2dProp->ToString());
         } break;
      case VEC3:
         {
            std::string oldValue    = mVec3Prop->ToString();
            osg::Vec3   vectorValue = mVec3Prop->GetValue();
            vectorValue[mElementIndex] = value;
            mVec3Prop->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(mProxy, mVec3Prop,
               oldValue, mVec3Prop->ToString());
         } break;
      case VEC3F:
         {
            std::string oldValue    = mVec3fProp->ToString();
            osg::Vec3f  vectorValue = mVec3fProp->GetValue();
            vectorValue[mElementIndex] = value;
            mVec3fProp->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(mProxy, mVec3fProp,
               oldValue, mVec3fProp->ToString());
         } break;
      case VEC3D:
         {
            std::string oldValue    = mVec3dProp->ToString();
            osg::Vec3d  vectorValue = mVec3dProp->GetValue();
            vectorValue[mElementIndex] = value;
            mVec3dProp->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(mProxy, mVec3dProp,
               oldValue, mVec3dProp->ToString());
         } break;
      case VEC4:
         {
            std::string oldValue    = mVec4Prop->ToString();
            osg::Vec4   vectorValue = mVec4Prop->GetValue();
            vectorValue[mElementIndex] = value;
            mVec4Prop->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(mProxy, mVec4Prop,
               oldValue, mVec4Prop->ToString());
         } break;
      case VEC4F:
         {
            std::string oldValue    = mVec4fProp->ToString();
            osg::Vec4f  vectorValue = mVec4fProp->GetValue();
            vectorValue[mElementIndex] = value;
            mVec4fProp->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(mProxy, mVec4fProp,
               oldValue, mVec4fProp->ToString());
         } break;
      case VEC4D:
         {
            std::string oldValue    = mVec4dProp->ToString();
            osg::Vec4d  vectorValue = mVec4dProp->GetValue();
            vectorValue[mElementIndex] = value;
            mVec4dProp->SetValue(vectorValue);

            // give undo manager the ability to create undo/redo events
            EditorEvents::GetInstance().emitActorPropertyAboutToChange(mProxy, mVec4dProp,
               oldValue, mVec4dProp->ToString());
         } break;
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicVectorElementControl::updateData(QWidget* widget)
   {
      if (!mInitialized || widget == NULL)
      {
         LOG_ERROR("Tried to updateData before being initialized");
         return false;
      }

      return updateModelFromEditor(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicVectorElementControl::actorPropertyChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
      dtCore::RefPtr<dtDAL::ActorProperty> property)
   {
      DynamicAbstractControl::actorPropertyChanged(proxy, property);

      if (mTemporaryEditControl != NULL && proxy == mProxy)
      {
         switch (mWhichType)
         {
         case VEC2:
            if (property == mVec2Prop)
            {
               updateEditorFromModel(mTemporaryEditControl);
            } break;
         case VEC2F:
            if (property == mVec2fProp)
            {
               updateEditorFromModel(mTemporaryEditControl);
            } break;
         case VEC2D:
            if (property == mVec2dProp)
            {
               updateEditorFromModel(mTemporaryEditControl);
            } break;
         case VEC3:
            if (property == mVec3Prop)
            {
               updateEditorFromModel(mTemporaryEditControl);
            } break;
         case VEC3F:
            if (property == mVec3fProp)
            {
               updateEditorFromModel(mTemporaryEditControl);
            } break;
         case VEC3D:
            if (property == mVec3dProp)
            {
               updateEditorFromModel(mTemporaryEditControl);
            } break;
         case VEC4:
            if (property == mVec4Prop)
            {
               updateEditorFromModel(mTemporaryEditControl);
            } break;
         case VEC4F:
            if (property == mVec4fProp)
            {
               updateEditorFromModel(mTemporaryEditControl);
            } break;
         case VEC4D:
            if (property == mVec4dProp)
            {
               updateEditorFromModel(mTemporaryEditControl);
            } break;
         }
      }
   }

} // namespace dtEditQt
