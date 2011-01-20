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
#include <dtQt/dynamicabstractcontrol.h>

#include <dtDAL/actorproxy.h>
#include <dtDAL/datatype.h>
#include <dtDAL/vectoractorproperties.h>

#include <dtQt/dynamicabstractparentcontrol.h>
#include <dtQt/dynamicpropertycontainercontrol.h>
#include <dtQt/dynamicarraycontrol.h>
#include <dtQt/dynamicboolcontrol.h>
#include <dtQt/dynamiccolorrgbacontrol.h>
#include <dtQt/dynamiccontainercontrol.h>
#include <dtQt/dynamicdoublecontrol.h>
#include <dtQt/dynamicenumcontrol.h>
#include <dtQt/dynamicfloatcontrol.h>
#include <dtQt/dynamicintcontrol.h>
#include <dtQt/dynamiclabelcontrol.h>
#include <dtQt/dynamiclongcontrol.h>
#include <dtQt/dynamicstringcontrol.h>
#include <dtQt/dynamicvecncontrol.h>
#include <dtQt/propertyeditortreeview.h>

#include <QtCore/QSize>

#include <QtGui/QColor>
#include <QtGui/QPainter>
#include <QtGui/QPalette>
#include <QtGui/QStyleOptionViewItem>
#include <QtGui/QWidget>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   DynamicControlFactory::DynamicControlFactory()
   : mControlFactory(new dtUtil::ObjectFactory<dtDAL::DataType*, DynamicAbstractControl>)
   {
      // register all the data types with the dynamic control factory
      RegisterControlForDataType<DynamicStringControl>(dtDAL::DataType::STRING);
      RegisterControlForDataType<DynamicFloatControl>(dtDAL::DataType::FLOAT);
      RegisterControlForDataType<DynamicDoubleControl>(dtDAL::DataType::DOUBLE);
      RegisterControlForDataType<DynamicIntControl>(dtDAL::DataType::INT);
      RegisterControlForDataType<DynamicLongControl>(dtDAL::DataType::LONGINT);
      RegisterControlForDataType<DynamicBoolControl>(dtDAL::DataType::BOOLEAN);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec2ActorProperty> >(dtDAL::DataType::VEC2);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec2fActorProperty> >(dtDAL::DataType::VEC2F);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec2dActorProperty> >(dtDAL::DataType::VEC2D);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec3ActorProperty> >(dtDAL::DataType::VEC3);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec3fActorProperty> >(dtDAL::DataType::VEC3F);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec3dActorProperty> >(dtDAL::DataType::VEC3D);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec4ActorProperty> >(dtDAL::DataType::VEC4);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec4fActorProperty> >(dtDAL::DataType::VEC4F);
      RegisterControlForDataType<DynamicVecNControl<dtDAL::Vec4dActorProperty> >(dtDAL::DataType::VEC4D);
      RegisterControlForDataType<DynamicEnumControl>(dtDAL::DataType::ENUMERATION);
      RegisterControlForDataType<DynamicColorRGBAControl>(dtDAL::DataType::RGBACOLOR);
      RegisterControlForDataType<DynamicArrayControl>(dtDAL::DataType::ARRAY);
      RegisterControlForDataType<DynamicContainerControl>(dtDAL::DataType::CONTAINER);
      RegisterControlForDataType<DynamicPropertyContainerControl>(dtDAL::DataType::PROPERTY_CONTAINER);
   }

   ////////////////////////////////////////////////////////////////////////////////
   DynamicAbstractControl* DynamicControlFactory::CreateDynamicControl(const dtDAL::ActorProperty& prop)
   {
      return mControlFactory->CreateObject(&prop.GetPropertyType());
   }

   ///////////////////////////////////////////////////////////////////////////////
   DynamicControlFactory::~DynamicControlFactory()
   {
   }

   ///////////////////////////////////////////////////////////////////////////////
   ///////////////////////////////////////////////////////////////////////////////
   DynamicAbstractControl::DynamicAbstractControl()
      : mInitialized(false)
      , mParent(NULL)
      , mModel(NULL)
      , mPropertyTree(NULL)
      //, mNewCommitEmitter(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicAbstractControl::~DynamicAbstractControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicControlFactory* DynamicAbstractControl::GetDynamicControlFactory()
   {
      return mControlFactory.get();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::SetDynamicControlFactory(DynamicControlFactory* factory)
   {
      mControlFactory = factory;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::updateEditorFromModel(QWidget* widget)
   {
      // do nothing here, but allows controls without an editor to not have to override it
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicAbstractControl::updateModelFromEditor(QWidget* widget)
   {
      // Notify the parent that the control is about to be updated.
      NotifyParentOfPreUpdate();
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::InitializeData(DynamicAbstractControl* newParent,
      PropertyEditorModel* newModel, dtDAL::PropertyContainer* newPC, dtDAL::ActorProperty* newProperty)
   {
      mInitialized = true;

      mParent = newParent;
      mPropContainer  = newPC;
      mModel  = newModel;

      // Set the tooltip description on the control.
      if (mModel)
      {
         mModel->setDescription(this);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget* DynamicAbstractControl::createEditor(QWidget* parent,
      const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      // do nothing.  This method allows controls with no editor to not have to override it.
      return NULL;
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicAbstractControl* DynamicAbstractControl::getParent()
   {
      return mParent;
   }

   /////////////////////////////////////////////////////////////////////////////////
   int DynamicAbstractControl::getChildIndex(DynamicAbstractControl* child)
   {
      return 0;
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicAbstractControl* DynamicAbstractControl::getChild(int index)
   {
      return NULL;
   }

   /////////////////////////////////////////////////////////////////////////////////
   int DynamicAbstractControl::getChildCount()
   {
      return 0;
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::PropertyAboutToChangePassThrough(dtDAL::PropertyContainer& proxy, dtDAL::ActorProperty& prop,
            std::string oldValue, std::string newValue)
   {
      emit PropertyAboutToChange(proxy, prop, oldValue, newValue);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::PropertyChangedPassThrough(dtDAL::PropertyContainer& proxy, dtDAL::ActorProperty& prop)
   {
      emit PropertyChanged(proxy, prop);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::OnChildPreUpdate(DynamicAbstractControl* child)
   {
      NotifyParentOfPreUpdate();
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicAbstractControl::getDisplayName()
   {
      return QString("");
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicAbstractControl::getDescription()
   {
      return QString("");
   }

   /////////////////////////////////////////////////////////////////////////////////
   const QString DynamicAbstractControl::getValueAsString()
   {
      NotifyParentOfPreUpdate();
      return QString("");
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicAbstractControl::isEditable()
   {
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicAbstractControl::isControlDoesCustomPainting(int column)
   {
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::paintColumn(int column, QPainter* painter, const QStyleOptionViewItem& opt)
   {
      // do nothing
   }

   /////////////////////////////////////////////////////////////////////////////////
   QSize DynamicAbstractControl::sizeHint(int column, const QStyleOptionViewItem& opt)
   {
      // do nothing
      return QSize();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::SetTreeView(PropertyEditorTreeView* newPropertyTree)
   {
      mPropertyTree = newPropertyTree;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   bool DynamicAbstractControl::NeedsPersistentEditor()
   {
      return false;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::SetBackgroundColor(QWidget* widget, QColor color)
   {
      QPalette palette(widget->palette());
      palette.setColor(QPalette::Active,   QPalette::Background, color);
      palette.setColor(QPalette::Inactive, QPalette::Background, color);
      palette.setColor(QPalette::Disabled, QPalette::Background, color) ;
      widget->setPalette(palette);
      //4.1 only
      widget->setAutoFillBackground(true);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void DynamicAbstractControl::InstallEventFilterOnControl(QObject* filterObj)
   {
      QObject::installEventFilter(filterObj);
   }

} // namespace dtEditQt
