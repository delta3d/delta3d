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

#include <dtQt/basepropertyeditor.h>

#include <dtQt/dynamicabstractcontrol.h>
#include <dtQt/dynamicgroupcontrol.h>
#include <dtQt/dynamiclabelcontrol.h>
//#include <dtEditQt/dynamicnamecontrol.h>
#include <dtQt/propertyeditormodel.h>
#include <dtQt/propertyeditortreeview.h>

#include <dtQt/dynamiccontainercontrol.h>

#include <QtCore/QStringList>

#include <QtGui/QLabel>
#include <QtGui/QGridLayout>
#include <QtGui/QScrollArea>
#include <QtGui/QScrollBar>
#include <QtGui/QTreeWidget>
#include <QtGui/QMainWindow>
#include <QtGui/QHeaderView>
#include <QtGui/QGroupBox>
#include <QtGui/QTreeView>
#include <QtGui/QAction>
#include <QtGui/QHeaderView>

#include <dtCore/deltadrawable.h>

#include <dtDAL/actorproxy.h>
#include <dtDAL/actorproperty.h>
#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/datatype.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/map.h>

#include <dtUtil/log.h>
#include <dtUtil/tree.h>

#include <osg/Referenced>

#include <vector>
#include <cmath>

namespace dtQt
{

   ///////////////////////////////////////////////////////////////////////////////
   BasePropertyEditor::BasePropertyEditor(QMainWindow* parent)
      : QDockWidget(parent)
      , mControlFactory(new DynamicControlFactory)
   {
      LOG_INFO("Initializing BasePropertyEditor");
      propertyModel = NULL;
      propertyTree = NULL;
      mRootControl = NULL;

      lastScrollBarLocation = 0; // top
      lastHeaderPosition = 100; // rough guess on a good size.

      mBaseGroupBoxName = tr("Actor Properties");

      setWindowTitle(tr("Property Editor"));

      setupUI();
   }

   /////////////////////////////////////////////////////////////////////////////////
   BasePropertyEditor::~BasePropertyEditor()
   {
      delete mRootControl;
      mRootControl = NULL;
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicControlFactory& BasePropertyEditor::GetDynamicControlFactory()
   {
      return *mControlFactory;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void BasePropertyEditor::setupUI()
   {
      // create the main widget and the scroll and setup our editor docking window.
      mainAreaWidget = new QWidget(this);
      QGridLayout* mainAreaLayout = new QGridLayout(mainAreaWidget);
      setWidget(mainAreaWidget);

      // build the dynamic property area
      actorPropBox = new QGroupBox(mBaseGroupBoxName, mainAreaWidget);
      actorPropBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
      mainAreaLayout->addWidget(actorPropBox, 0, 0);
      dynamicControlLayout = new QGridLayout(actorPropBox);

      // create the base model and root properties for our tree
      propertyModel = new PropertyEditorModel(this);
      mRootControl = new DynamicGroupControl("root");

      // Left here.  This is the code you need here if you want to NOT recreate the
      // tree everytime.  See the comment a few methods down.
      //propertyTree = new BasePropertyEditorTreeView(propertyModel, actorPropBox);
      //propertyTree->setMinimumSize(100, 100);
      //propertyTree->setRoot(GetRootControl());
      //dynamicControlLayout->addWidget(propertyTree);

      refreshSelectedActors();
   }

   ///////////////////////////////////////////////////////////////////////////////
   void BasePropertyEditor::HandlePropertyContainersSelected(PropertyContainerRefPtrVector& propCons)
   {
      PropertyContainerRefPtrVector::const_iterator iter;

      // get the currently open tree branches and current caret position so we
      // can scroll back to it as best as we can.  Only do this if we have
      // exactly 1.  That way, it will remember the last valid display.
      if (mSelectedPC.size() == 1)
      {
         markCurrentExpansion();
      }

      // clear our selected list.
      mSelectedPC.clear();

      // copy passed in actors to our internal list.
      for (iter = propCons.begin(); iter != propCons.end(); ++iter)
      {
         dtCore::RefPtr<dtDAL::PropertyContainer> propCon = (*iter);
         mSelectedPC.push_back(propCon);
         LOG_INFO("Selected property containers found a property container");
      }

      // turn off screen updates so that we don't watchf it draw
      setUpdatesEnabled(false);
      mainAreaWidget->setUpdatesEnabled(false);
      actorPropBox->setUpdatesEnabled(false);

      refreshSelectedActors();

      // turn them back on, so it looks right
      setUpdatesEnabled(true);
      mainAreaWidget->setUpdatesEnabled(true);
      actorPropBox->setUpdatesEnabled(true);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void BasePropertyEditor::refreshSelectedActors()
   {
      CommitCurrentEdits();

      bool isMultiSelect = mSelectedPC.size() > 1;

      delete propertyTree;
      GetRootControl()->removeAllChildren(propertyModel);
      propertyTree = new PropertyEditorTreeView(propertyModel, actorPropBox);
      dynamicControlLayout->addWidget(propertyTree);
      propertyTree->setRoot(GetRootControl());

      resetGroupBoxLabel();
      // Walk our selection items.
      PropertyContainerRefPtrVector::const_iterator iter;
      for (iter = mSelectedPC.begin(); iter != mSelectedPC.end(); ++iter)
      {
         dtCore::RefPtr<dtDAL::PropertyContainer> pc = (*iter);

         // build the dynamic controls
         if (!isMultiSelect)
         {
            buildDynamicControls(*pc);
         }
         else
         {
            // create a single label entry for each multi selected proxy
            //DynamicGroupControl* parentControl = new DynamicGroupControl(myProxy->GetName());
            // TODO fix name.
            DynamicGroupControl* parentControl = new DynamicGroupControl("Property Container");

            parentControl->InitializeData(GetRootControl(), propertyModel, pc.get(), NULL);
            //parentControl->setDisplayValues(tr(myProxy->GetActorType().GetName().c_str()), "",
            //   QString(tr(myProxy->GetName().c_str())));
            GetRootControl()->addChildControl(parentControl, propertyModel);

            buildDynamicControls(*pc, parentControl);
         }
      }

      propertyTree->reset();

      // we deleted the tree, so we have  to reset some sizes
      //actorPropBox->setMinimumSize(actorPropBox->sizeHint());
      //mainAreaWidget->setMinimumSize(mainAreaWidget->sizeHint());
      propertyTree->setMinimumSize(80, 80);
      propertyTree->update();

      // Now, go back and try to re-expand items and restore our scroll position
      restorePreviousExpansion();

      propertyTree->show();
   }

   /////////////////////////////////////////////////////////////////////////////////
   QString BasePropertyEditor::GetGroupBoxLabelText(const QString& baseGroupBoxName)
   {
      if (mSelectedPC.empty())
      {
         return baseGroupBoxName;
      }
      else if (mSelectedPC.size() == 1)
      {
         // set the name in the group box.
         dtCore::RefPtr<dtDAL::PropertyContainer> selectedProxy = mSelectedPC[0];
         QString label;
         label = baseGroupBoxName + " ('" + tr("Property Container") + "' selected)";
         return label;
      }

      //  put the count of selections
      QString label = baseGroupBoxName + " (" + QString::number(mSelectedPC.size()) + " selected)";
      return label;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void BasePropertyEditor::resetGroupBoxLabel()
   {
      actorPropBox->setTitle(GetGroupBoxLabelText(mBaseGroupBoxName));
   }

   /////////////////////////////////////////////////////////////////////////////////
   void BasePropertyEditor::buildDynamicControls(dtDAL::PropertyContainer& propertyContainer, DynamicGroupControl* parentControl)
   {
      dtDAL::ActorProperty* curProp;
      std::vector<dtDAL::ActorProperty*> propList;
      DynamicAbstractControl* newControl;
      std::vector<dtDAL::ActorProperty*>::const_iterator propIter;
      int row = 0;

      DynamicGroupControl* parent = GetRootControl();
      if (parentControl != NULL)
      {
         parent = parentControl;
      }

      propertyContainer.GetPropertyList(propList);

      // create controls for the basic properties - name, type, etc...

      // for each property, create a new dynamic control and add it to a group, if appropriate.
      for (propIter = propList.begin(); propIter != propList.end(); ++propIter)
      {
         curProp = (*propIter);
         try
         {
            // first create the control.  Sometimes the controls aren't creatable, so
            // check that first before we do other work.  Excepts if it fails
            newControl = mControlFactory->CreateDynamicControl(*curProp);
            if (newControl == NULL)
            {
               LOG_ERROR("Object Factory failed to create a control for property: " + curProp->GetDataType().GetName());
            }
            else
            {
               newControl->SetTreeView(propertyTree);
               newControl->SetDynamicControlFactory(mControlFactory.get());

               connect(newControl, SIGNAL(PropertyAboutToChange(dtDAL::PropertyContainer&, dtDAL::ActorProperty&,
                                 const std::string&, const std::string&)),
                        this, SLOT(PropertyAboutToChangeFromControl(dtDAL::PropertyContainer&, dtDAL::ActorProperty&,
                                 const std::string&, const std::string&)));

               connect(newControl, SIGNAL(PropertyChanged(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)),
                        this, SLOT(PropertyChangedFromControl(dtDAL::PropertyContainer&, dtDAL::ActorProperty&)));

               // Work with the group.  Requires finding an existing group or creating one,
               // and eventually adding our new control to that group control
               const std::string& groupName = curProp->GetGroupName();
               if (!groupName.empty())
               {
                  // find our group
                  DynamicGroupControl* groupControl = parent->getChildGroupControl(QString(groupName.c_str()));

                  // if no group, then create one.
                  if (groupControl == NULL)
                  {
                     groupControl = new DynamicGroupControl(groupName);
                     groupControl->InitializeData(parent, propertyModel, &propertyContainer, NULL);
                     parent->addChildControl(groupControl, propertyModel);
                  }

                  // add our new control to the group.
                  newControl->InitializeData(groupControl, propertyModel, &propertyContainer, curProp);
                  groupControl->addChildControl(newControl, propertyModel);
               }
               else
               {
                  // there's no group, so use the root.
                  newControl->InitializeData(parent, propertyModel, &propertyContainer, curProp);
                  parent->addChildControl(newControl, propertyModel);
               }

               // the following code doesn't work.  I'm leaving it here for reference.
               // basically, it's supposed to check and create the control in such a way
               // that it's always visible regardless if the user had just clicked in the
               // control or not.  QT creates and destroys the edit controls on the fly.
               // make the new controls editor persistent if necessary.
               //if (newControl->isNeedsPersistentEditor()) {
               //    QModelIndex index = propertyModel->IndexOf(newControl, 1);
               //    propertyTree->openPersistentEditor(index);
               //}
            }
         }
         catch (dtUtil::Exception& ex)
         {
            LOG_ERROR("Failed to create a control for property: " + curProp->GetDataType().GetName() +
               " with error: " + ex.What());
         }

         ++row;
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   DynamicGroupControl* BasePropertyEditor::GetRootControl()
   {
      return mRootControl;
   }

   /////////////////////////////////////////////////////////////////////////////////
   PropertyEditorModel& BasePropertyEditor::GetPropertyEditorModel()
   {
      return *propertyModel;
   }

   /////////////////////////////////////////////////////////////////////////////////
   void BasePropertyEditor::markCurrentExpansion()
   {
      // clear out previous marks
      expandedTreeNames.clear();

      if (propertyTree != NULL && GetRootControl() != NULL)
      {
         // start recursion
         recurseMarkCurrentExpansion(GetRootControl(), expandedTreeNames);

         // also store the last location of the scroll bar... so that they go back
         // to where they were next time.
         lastScrollBarLocation = propertyTree->verticalScrollBar()->value();//sliderPosition();
         lastHeaderPosition = propertyTree->header()->sectionSize(0);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void BasePropertyEditor::recurseMarkCurrentExpansion(DynamicAbstractControl* parent,
      dtUtil::tree<QString>& currentTree)
   {
      for (int i = 0; i < parent->getChildCount(); ++i)
      {
         DynamicAbstractControl* child = parent->getChild(i);

         // if we have children, then we could potentially be expanded...
         if (child->getChildCount() > 0)
         {
            QModelIndex index = propertyModel->IndexOf(child);
            if (propertyTree->isExpanded(index))
            {
               // add it to our list
               dtUtil::tree<QString>& insertedItem = currentTree.
                  insert(child->getDisplayName()).tree_ref();

               // recurse on the child with the new tree
               recurseMarkCurrentExpansion(child, insertedItem);
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void BasePropertyEditor::restorePreviousExpansion()
   {
      recurseRestorePreviousExpansion(GetRootControl(), expandedTreeNames);

      // Put the scroll bar back where it was last time
      propertyTree->verticalScrollBar()->setValue(lastScrollBarLocation);//setSliderPosition(lastScrollBarLocation);
      // reset the little header scroll bar
      propertyTree->header()->resizeSection(0, lastHeaderPosition);
      lastScrollBarLocation = propertyTree->verticalScrollBar()->value();//sliderPosition();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void BasePropertyEditor::recurseRestorePreviousExpansion(DynamicAbstractControl* parent,
      dtUtil::tree<QString>& currentTree)
   {
      // walk through the children...
      for (dtUtil::tree<QString>::const_iterator iter = currentTree.in(); iter != currentTree.end(); ++iter)
      {
         QString name = (*iter);

         // Try to find a control with this name in our propertyModel
         for (int i = 0; i < parent->getChildCount(); ++i)
         {
            DynamicAbstractControl* child = parent->getChild(i);
            // found a match!  expand it
            if (child->getDisplayName() == name)
            {
               QModelIndex childIndex = propertyModel->IndexOf(child);
               propertyTree->setExpanded(childIndex, true);

               // recurse over the children of this object
               recurseRestorePreviousExpansion(child, iter.tree_ref());
            }
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void BasePropertyEditor::ActorPropertyChanged(dtDAL::PropertyContainer& propCon,
      dtDAL::ActorProperty& property)
   {
      propertyTree->viewport()->update();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void BasePropertyEditor::ProxyNameChanged(dtDAL::ActorProxy& propCon, std::string oldName)
   {
      resetGroupBoxLabel();
      propertyTree->viewport()->update();
   }

} // namespace dtQt
