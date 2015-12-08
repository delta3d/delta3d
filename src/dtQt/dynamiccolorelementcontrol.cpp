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

//#include <osg/Vec3>
#include <prefix/dtqtprefix.h>
#include <dtQt/dynamiccolorelementcontrol.h>

#include <dtCore/actorproxy.h>
#include <dtCore/colorrgbaactorproperty.h>
#include <dtCore/datatype.h>

#include <dtQt/dynamicsubwidgets.h>

#include <dtUtil/log.h>

#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicColorElementControl::DynamicColorElementControl(dtCore::ColorRgbaActorProperty* newColorRGBA,
      int whichIndex, const std::string& newLabel)
      : mLabel(newLabel)
      , mElementIndex(whichIndex)
      , mTemporaryEditControl(NULL)
   {

      //mColorRGB = NULL;
      mColorRGBA = newColorRGBA;
      mWhichType = RGBA;
   }

   ///////////////////////////////////////////////////////////////////////////////
   //DynamicColorElementControl::DynamicColorElementControl(dtCore::ColorRgbActorProperty* colorRGB,
   //        int whichIndex, const std::string& newLabel)
   //   : mLabel(newLabel)
   //   , mElementIndex(whichIndex)
   //{
   //    mColorRGB = colorRGBA;
   //    mColorRGBA = NULL;
   //    mWhichType = RGB;
   //}

   /////////////////////////////////////////////////////////////////////////////////
   DynamicColorElementControl::~DynamicColorElementControl()
   {
   }


   /////////////////////////////////////////////////////////////////////////////////
   void DynamicColorElementControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtCore::PropertyContainer* newPC, dtCore::ActorProperty* newProperty)
   {
      // NOTE - This object sets it's property in the contructor.
      DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicColorElementControl::updateEditorFromModel(QWidget* widget)
   {
      if (widget == mWrapper && mTemporaryEditControl)
      {
         // set the current value from our property
         int value = getValue();
         mTemporaryEditControl->setValue(value);
      }

      DynamicAbstractControl::updateEditorFromModel(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicColorElementControl::updateModelFromEditor(QWidget* widget)
   {
      DynamicAbstractControl::updateModelFromEditor(widget);

      bool dataChanged = false;

      if (widget == mWrapper && mTemporaryEditControl)
      {
         int result = mTemporaryEditControl->value();

         // set our value to our object
         if (result != getValue())
         {
            dataChanged = setValue(result);
         }
      }

      return dataChanged;
   }


   /////////////////////////////////////////////////////////////////////////////////
   QWidget* DynamicColorElementControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      // create and init the edit box
      mTemporaryEditControl = new SubQSpinBox(parent, this);
      mTemporaryEditControl->setMinimum(-512);
      mTemporaryEditControl->setMaximum(512);
      mTemporaryEditControl->setToolTip(getDescription());
      mWrapper = mTemporaryEditControl;

      if (!IsInitialized())
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return mTemporaryEditControl;
      }

      updateEditorFromModel(mWrapper);

      return mTemporaryEditControl;
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicColorElementControl::getDisplayName()
   {
      QString name = DynamicAbstractControl::getDisplayName();
      if (!name.isEmpty())
      {
         return name;
      }
      return QString(tr(mLabel.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicColorElementControl::getDescription()
   {
      std::string tooltip;

      if (mWhichType == RGBA)
      {
         tooltip = mColorRGBA->GetDescription();
      }// else { // == RGB
      //    tooltip = colorRGB->getDescription();
      //}

      tooltip += " " + mLabel;
      return QString(tr(tooltip.c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicColorElementControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();

      if (doPropertiesMatch())
      {
         int value = getValue();
         return QString::number(value);
      }
      else
      {
         return "<Multiple Values...>";
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   bool DynamicColorElementControl::doPropertiesMatch()
   {
      const std::vector<LinkedPropertyData>& linkedProperties = mParent->GetLinkedProperties();

      osg::Vec4 baseValue = mColorRGBA->GetValue();

      int linkCount = (int)linkedProperties.size();
      for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
      {
         const LinkedPropertyData& data = linkedProperties[linkIndex];

         dtCore::ColorRgbaActorProperty* linkedProp =
            dynamic_cast<dtCore::ColorRgbaActorProperty*>(data.property);

         if (linkedProp)
         {
            osg::Vec4 linkedValue = linkedProp->GetValue();

            // Compare the two property values for the specific vector index
            // that this element represents.
            if (baseValue[mElementIndex] != linkedValue[mElementIndex])
            {
               return false;
            }
         }
      }

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   int DynamicColorElementControl::getValue()
   {
      int result = 0;

      if (mWhichType == RGBA)
      {
         osg::Vec4 vectorValue = mColorRGBA->GetValue();
         double intermediate = (double) vectorValue[mElementIndex];
         result = DynamicColorElementControl::convertColorFloatToInt(intermediate);
      }
      //else { // == RGB
      //    osg::Vec3 vectorValue = mColorRGB->getValue();
      //    result = vectorValue[mElementIndex];
      //}

      return result;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicColorElementControl::setValue(int value)
   {
      if (mWhichType == RGBA)
      {
         std::string oldValue  = mColorRGBA->ToString();
         osg::Vec4 vectorValue = mColorRGBA->GetValue();
         double intermediate = DynamicColorElementControl::convertColorIntToFloat(value);
         vectorValue[mElementIndex] = (double)intermediate;
         mColorRGBA->SetValue(vectorValue);

         // give undo manager the ability to create undo/redo events
         emit PropertyAboutToChange(*mPropContainer, *mColorRGBA,
            oldValue, mColorRGBA->ToString());
         emit PropertyChanged(*mPropContainer, *mColorRGBA);

         // Copy the value change to all linked properties as well.
         std::vector<LinkedPropertyData>& linkedProperties = getParent()->GetLinkedProperties();

         int linkCount = (int)linkedProperties.size();
         for (int linkIndex = 0; linkIndex < linkCount; ++linkIndex)
         {
            LinkedPropertyData& data = linkedProperties[linkIndex];

            dtCore::ColorRgbaActorProperty* linkedProp =
               dynamic_cast<dtCore::ColorRgbaActorProperty*>(data.property);

            if (linkedProp)
            {
               oldValue = linkedProp->ToString();
               osg::Vec4 linkedValue = linkedProp->GetValue();
               linkedValue[mElementIndex] = (double)intermediate;
               linkedProp->SetValue(linkedValue);

               // give undo manager the ability to create undo/redo events
               emit PropertyAboutToChange(*data.propCon.get(), *linkedProp,
                  oldValue, linkedProp->ToString());
               emit PropertyChanged(*data.propCon.get(), *linkedProp);
            }
         }
      }
      //else { // == RGB
      //    osg::Vec3 vectorValue = mColorRGB->getValue();
      //    vectorValue[mElementIndex] = value;
      //    mColorRGB->setValue(vectorValue);
      //}

      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   bool DynamicColorElementControl::updateData(QWidget* widget)
   {
      if (!IsInitialized() || widget == NULL)
      {
         LOG_ERROR("Tried to updateData before being initialized");
         return false;
      }

      return updateModelFromEditor(widget);
   }

   // STATIC methods

   /////////////////////////////////////////////////////////////////////////////////
   int DynamicColorElementControl::convertColorFloatToInt(float value)
   {
      // convert 0 to 1 to 0 to 255 with a round for display
      int result = (int) (value * 255 + 0.5);
      return result;
   }

   float DynamicColorElementControl::convertColorIntToFloat(int value)
   {
      // convert 0 to 255 back to 0 to 1.
      float result = (float) (value / 255.0);
      return result;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicColorElementControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      // we have to check - sometimes the destructor won't get called before the
      // next widget is created.  Then, when it is called, it sets the NEW editor to NULL!
      if (widget == mWrapper)
      {
         mTemporaryEditControl = NULL;
      }

      DynamicAbstractControl::handleSubEditDestroy(widget, hint);
   }
} // namespace dtQt
