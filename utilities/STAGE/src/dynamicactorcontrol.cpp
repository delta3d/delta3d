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
 * William E. Johnson II
 */
#include <prefix/stageprefix.h>
#include <dtCore/project.h>
#include <dtEditQt/dynamicactorcontrol.h>
#include <dtQt/propertyeditortreeview.h>
#include <dtCore/actoractorproperty.h>
#include <dtCore/actoridactorproperty.h>
#include <dtCore/map.h>
#include <dtCore/exceptionenum.h>
#include <dtCore/datatype.h>
#include <QtGui/QPushButton>
#include <QtGui/QHBoxLayout>

#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>

namespace dtEditQt
{

   STAGEDynamicActorControl::STAGEDynamicActorControl()
      : dtQt::DynamicActorControl()
      , mTemporaryGotoButton(NULL)
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   STAGEDynamicActorControl::~STAGEDynamicActorControl()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   QWidget* STAGEDynamicActorControl::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index)
   {
      QWidget* wrapper = dtQt::DynamicActorControl::createEditor(parent, option, index);

      mTemporaryGotoButton = new dtQt::SubQToolButton(wrapper, this);
      mTemporaryGotoButton->setText("Goto");

      connect(mTemporaryGotoButton, SIGNAL(clicked()), this, SLOT(onGotoClicked()));

      mGridLayout->addWidget(mTemporaryGotoButton,  0, 1, 1, 1);
      mGridLayout->setColumnMinimumWidth(1, mTemporaryGotoButton->sizeHint().width() / 2);
      mGridLayout->setColumnStretch(0, 2);

      return wrapper;
   }

   /////////////////////////////////////////////////////////////////////////////////
   // SLOTS
   /////////////////////////////////////////////////////////////////////////////////

   ////////////////////////////////////////////////////////////////////////////////
   void STAGEDynamicActorControl::handleSubEditDestroy(QWidget* widget, QAbstractItemDelegate::EndEditHint hint)
   {
      if (widget == mWrapper)
      {
         mTemporaryGotoButton = NULL;
      }

      DynamicAbstractControl::handleSubEditDestroy(widget, hint);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void STAGEDynamicActorControl::onGotoClicked()
   {
      NotifyParentOfPreUpdate();
      dtCore::BaseActorObject* actor = getActor();
      if (actor != NULL)
      {
         dtCore::ActorPtr actorPtr(actor);

         EditorEvents::GetInstance().emitGotoActor(actorPtr);

         std::vector< dtCore::RefPtr<dtCore::BaseActorObject> > vec;
         vec.push_back(actorPtr);

         EditorEvents::GetInstance().emitActorsSelected(vec);
      }
   }

} // namespace dtEditQt
