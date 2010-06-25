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
#include <dtQt/dynamiccolorrgbacontrol.h>

#include <dtDAL/actorproxy.h>
#include <dtDAL/datatype.h>
#include <dtDAL/colorrgbaactorproperty.h>

#include <dtQt/dynamiccolorelementcontrol.h>
#include <dtQt/dynamicsubwidgets.h>
#include <dtQt/propertyeditortreeview.h>

#include <dtUtil/log.h>
#include <dtUtil/mathdefines.h>

#include <QtGui/QColorDialog>
#include <QtGui/QColor>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPainter>
#include <QtGui/QPaintDevice>
#include <QtGui/QPaintEngine>
#include <QtGui/QPushButton>
#include <QtCore/QSize>
#include <QtGui/QStyleOptionViewItem>
#include <QtGui/QWidget>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicColorRGBAControl::DynamicColorRGBAControl()
      : mTemporaryWrapper(NULL)
      , mTemporaryEditOnlyTextLabel(NULL)
      , mTemporaryColorPicker(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicColorRGBAControl::~DynamicColorRGBAControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicColorRGBAControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtDAL::PropertyContainer* newPC, dtDAL::ActorProperty* newProperty)
   {
      if (newProperty != NULL && newProperty->GetDataType() == dtDAL::DataType::RGBACOLOR)
      {
         mProperty = static_cast<dtDAL::ColorRgbaActorProperty*>(newProperty);

         DynamicAbstractControl::InitializeData(newParent, newModel, newPC, newProperty);

         // create R
         rElement = CreateElementControl(0, "Red", newModel, newPC);
         // create G
         gElement = CreateElementControl(1, "Green", newModel, newPC);
         // create B
         bElement = CreateElementControl(2, "Blue", newModel, newPC);
         // create A
         aElement = CreateElementControl(3, "Alpha", newModel, newPC);
      }
      else
      {
         std::string propertyName = (newProperty != NULL) ? newProperty->GetName() : "NULL";
         LOG_ERROR("Cannot create dynamic control because property [" +
            propertyName + "] is not the correct type.");
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicColorRGBAControl::getDisplayName()
   {
      return QString(tr(mProperty->GetLabel().c_str()));
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicColorRGBAControl::getDescription()
   {
      std::string tooltip = mProperty->GetDescription() + "  [Type: " +
         mProperty->GetDataType().GetName() + "]";
      return QString(tr(tooltip.c_str()));
   }


   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicColorRGBAControl::getValueAsString()
   {
      DynamicAbstractControl::getValueAsString();
      const osg::Vec4& vectorValue = mProperty->GetValue();

      QString display;
      display.sprintf("(R=%d, G=%d, B=%d, A=%d)",
         DynamicColorElementControl::convertColorFloatToInt(vectorValue[0]),
         DynamicColorElementControl::convertColorFloatToInt(vectorValue[1]),
         DynamicColorElementControl::convertColorFloatToInt(vectorValue[2]),
         DynamicColorElementControl::convertColorFloatToInt(vectorValue[3]));
      return display;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicColorRGBAControl::updateEditorFromModel(QWidget* widget)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicColorRGBAControl::updateModelFromEditor(QWidget* widget)
   {
      return DynamicAbstractControl::updateModelFromEditor(widget);
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget *DynamicColorRGBAControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = new QWidget(parent);
      wrapper->setFocusPolicy(Qt::StrongFocus);
      // set the background color to white so that it sort of blends in with the rest of the controls
      SetBackgroundColor(wrapper, PropertyEditorTreeView::ROW_COLOR_ODD);

      if (!mInitialized)
      {
         LOG_ERROR("Tried to add itself to the parent widget before being initialized");
         return wrapper;
      }

      QHBoxLayout* hBox = new QHBoxLayout(wrapper);
      hBox->setMargin(0);
      hBox->setSpacing(0);

      // label
      mTemporaryEditOnlyTextLabel = new SubQLabel(getValueAsString(), wrapper, this);
      // set the background color to white so that it sort of blends in with the rest of the controls
      SetBackgroundColor(mTemporaryEditOnlyTextLabel, PropertyEditorTreeView::ROW_COLOR_ODD);

      // button
      mTemporaryColorPicker = new SubQPushButton(tr("Pick ..."), wrapper, this);
      //temporaryColorPicker->setMaximumHeight(18);
      connect(mTemporaryColorPicker, SIGNAL(clicked()), this, SLOT(colorPickerPressed()));
      mTemporaryColorPicker->setToolTip(getDescription());

      // setup the horizontal layout
      hBox->addWidget(mTemporaryColorPicker);
      hBox->addSpacing(3);
      hBox->addWidget(mTemporaryEditOnlyTextLabel);
      hBox->addStretch(1);

      mTemporaryWrapper = wrapper;
      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicColorRGBAControl::isEditable()
   {
      return !mProperty->IsReadOnly();
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicColorRGBAControl::NeedsPersistentEditor()
   {
      // we want our color picker to always show up. Because it's less confusing that way.
      return true;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicColorRGBAControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      if (widget == mTemporaryWrapper)
      {
         mTemporaryWrapper = NULL;
         mTemporaryEditOnlyTextLabel = NULL;
         mTemporaryColorPicker = NULL;
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicColorRGBAControl::installEventFilterOnControl(QObject* filterObj)
   {
      if (mTemporaryEditOnlyTextLabel != NULL)
      {
         mTemporaryEditOnlyTextLabel->installEventFilter(filterObj);
      }
      if (mTemporaryColorPicker != NULL)
      {
         mTemporaryColorPicker->installEventFilter(filterObj);
      }
   }


   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicColorRGBAControl::updateData(QWidget* widget)
   {
      // this guy doesn't have any editors.  All the data is edited in child controls
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicColorRGBAControl::colorPickerPressed()
   {
      // get the current value, as floats
      osg::Vec4 vectorValue = mProperty->GetValue();

      //clamp to ensure its in the 0..1 range for Qt to use
      for (unsigned int i = 0; i < vectorValue.num_components; ++i)
      {
         dtUtil::Clamp(vectorValue[i], 0.f, 1.f);
      }

      QColor startColor;
      startColor.setRgbF(vectorValue[0], vectorValue[1], vectorValue[2], vectorValue[3]);

      // show the dialog.  Blocks for user input.
      bool ok;
      QRgb rgba = QColorDialog::getRgba(startColor.rgba(), &ok, mPropertyTree);

      // if the user pressed, OK, we set the color and assume it changed
      if (ok)
      {
         QColor result;
         result.setRgba(rgba);
         osg::Vec4d propColor;
         result.getRgbF(&propColor[0], &propColor[1], &propColor[2], &propColor[3]);

         std::string oldValue = mProperty->ToString();
         mProperty->SetValue(propColor);

         // give undo manager the ability to create undo/redo events
         emit PropertyAboutToChange(*mPropContainer, *mProperty,
            oldValue, mProperty->ToString());

         // update our label
         if (mTemporaryEditOnlyTextLabel !=  NULL)
         {
            mTemporaryEditOnlyTextLabel->setText(getValueAsString());
         }

         // notify the world (mostly the viewports) that our property changed
         emit PropertyChanged(*mPropContainer, *mProperty);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicColorRGBAControl::actorPropertyChanged(dtDAL::PropertyContainer& propCon,
            dtDAL::ActorProperty& property)
   {
      DynamicAbstractControl::actorPropertyChanged(propCon, property);

      if (mTemporaryEditOnlyTextLabel != NULL && &propCon == mPropContainer && &property == mProperty)
      {
         mTemporaryEditOnlyTextLabel->setText(getValueAsString());
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicColorElementControl* DynamicColorRGBAControl::CreateElementControl(int index, const std::string& label,
            PropertyEditorModel* newModel, dtDAL::PropertyContainer* newPC)
   {
      DynamicColorElementControl* control = new DynamicColorElementControl(mProperty, index, label);
      control->InitializeData(this, newModel, newPC, mProperty);
      mChildren.push_back(control);

      connect(control, SIGNAL(PropertyAboutToChange(dtDAL::PropertyContainer&, dtDAL::ActorProperty&,
                        const std::string&, const std::string&)),
               this, SLOT(PropertyAboutToChangePassThrough(dtDAL::PropertyContainer&, dtDAL::ActorProperty&,
                        const std::string&, const std::string&)));

      connect(control, SIGNAL(PropertyChanged(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)),
               this, SLOT(PropertyChangedPassThrough(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)));

      return control;
   }


} // namespace dtQt
