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

#include <prefix/stageprefix.h>
#include <dtEditQt/propertyeditor.h>

#include <dtCore/deltadrawable.h>

#include <dtEditQt/dynamicactorcontrol.h>
#include <dtEditQt/dynamicgrouppropertycontrol.h>
#include <dtEditQt/dynamicresourcecontrol.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/viewportmanager.h>

#include <dtCore/actorproperty.h>
#include <dtCore/actorproxy.h>
#include <dtCore/datatype.h>
#include <dtCore/actorfactory.h>
#include <dtCore/map.h>

#include <dtQt/dynamicabstractcontrol.h>
#include <dtQt/dynamiccontainercontrol.h>
#include <dtQt/dynamicgroupcontrol.h>
#include <dtQt/dynamiclabelcontrol.h>
#include <dtQt/propertyeditormodel.h>
#include <dtQt/propertyeditortreeview.h>

#include <dtUtil/log.h>
#include <dtUtil/utiltree.h>

#include <QtCore/QStringList>
#include <QtWidgets/QAction>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QScrollArea>
#include <QtWidgets/QScrollBar>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QTreeWidget>

#include <osg/Referenced>

#include <vector>
#include <cmath>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   PropertyEditor::PropertyEditor(QMainWindow* parent)
      : dtQt::BasePropertyEditor(parent)
   {
      dtQt::DynamicControlFactory& dcfactory = GetDynamicControlFactory();

      size_t datatypeCount = dtCore::DataType::EnumerateType().size();

      for (size_t i = 0; i < datatypeCount; ++i)
      {
         dtCore::DataType* dt = dtCore::DataType::EnumerateType()[i];
         if (dt->IsResource())
         {
            dcfactory.RegisterControlForDataType<STAGEDynamicResourceControl>(*dt);
         }
      }

      dcfactory.RegisterControlForDataType<STAGEDynamicActorControl>(dtCore::DataType::ACTOR);
      dcfactory.RegisterControlForDataType<STAGEDynamicGroupPropertyControl>(dtCore::DataType::GROUP);
   }

   /////////////////////////////////////////////////////////////////////////////////
   PropertyEditor::~PropertyEditor()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   QString PropertyEditor::GetGroupBoxLabelText(const QString& baseGroupBoxName)
   {
      std::vector<dtCore::PropertyContainer*> selectedActors;
      GetSelectedPropertyContainers(selectedActors);

      if (selectedActors.size() == 1)
      {
         // set the name in the group box.
         dtCore::BaseActorObject* selectedProxy = dynamic_cast<dtCore::BaseActorObject*>(selectedActors[0]);

         if (selectedProxy != NULL)
         {
            QString label;
            if (selectedProxy == EditorData::GetInstance().getCurrentMap()->GetEnvironmentActor())
            {
               label = baseGroupBoxName + " ('" + tr(selectedProxy->GetName().c_str()) + " *Environment Actor*' selected)";
            }
            else
            {
               label = baseGroupBoxName + " ('" + tr(selectedProxy->GetName().c_str()) + "' selected)";
            }
            return label;
         }

      }

      return BaseClass::GetGroupBoxLabelText(baseGroupBoxName);
   }

   ////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::buildDynamicControls()
   {
      //dtQt::DynamicGroupControl* parent = GetRootControl();

      std::vector<dtCore::PropertyContainer*> propCons;
      GetSelectedPropertyContainers(propCons);

      if (propCons.empty())
      {
         return;
      }

      dtCore::BaseActorObject* proxy = dynamic_cast<dtCore::BaseActorObject*>(propCons[0]);

      //if (proxy != NULL)
      //{
      //   dtQt::PropertyEditorModel* propertyEditorModel = &GetPropertyEditorModel();
      //   // create the basic actor group
      //   dtQt::DynamicGroupControl* baseGroupControl = new dtQt::DynamicGroupControl("Actor Information");
      //   baseGroupControl->InitializeData(parent, propertyEditorModel, NULL, NULL);
      //   parent->addChildControl(baseGroupControl, propertyEditorModel);
      //
      //   // name of actor
      //   DynamicNameControl* nameControl = new DynamicNameControl();
      //   nameControl->InitializeData(baseGroupControl, propertyEditorModel, proxy, NULL);
      //   baseGroupControl->addChildControl(nameControl, propertyEditorModel);

      //   // Category of actor
      //   dtQt::DynamicLabelControl* categoryLabelControl = new dtQt::DynamicLabelControl();
      //   categoryLabelControl->InitializeData(baseGroupControl, propertyEditorModel, proxy, NULL);
      //   categoryLabelControl->setDisplayValues("Actor Category", "The category of the Actor - visible in the Actor Browser",
      //      QString(tr(proxy->GetActorType().GetCategory().c_str())));
      //   baseGroupControl->addChildControl(categoryLabelControl, propertyEditorModel);

      //   // Type of actor
      //   dtQt::DynamicLabelControl* typeLabelControl = new dtQt::DynamicLabelControl();
      //   typeLabelControl->InitializeData(baseGroupControl, propertyEditorModel, proxy, NULL);
      //   typeLabelControl->setDisplayValues("Actor Type", "The actual type of the actor as defined in the by the imported library",
      //      QString(tr(proxy->GetActorType().GetName().c_str())));
      //   baseGroupControl->addChildControl(typeLabelControl, propertyEditorModel);

      //   // Class of actor
      //   dtQt::DynamicLabelControl* classLabelControl = new dtQt::DynamicLabelControl();
      //   classLabelControl->InitializeData(baseGroupControl, propertyEditorModel, proxy, NULL);
      //   classLabelControl->setDisplayValues("Actor Class", "The Delta3D C++ class name for this actor - useful if you are trying to reference this actor in code",
      //      QString(tr(proxy->GetClassName().c_str())));
      //   baseGroupControl->addChildControl(classLabelControl, propertyEditorModel);
      //}

      std::vector<dtCore::ActorProperty*> propList;
      GetNestedPropertyList(*proxy, propList);

      ViewportManager::GetInstance().emitModifyPropList(*proxy, propList);

      BaseClass::buildDynamicControls();
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::HandleActorsSelected(ActorRefPtrVector& actors)
   {
      PropertyContainerRefPtrVector pcs;
      pcs.reserve(actors.size());
      for (size_t i = 0; i < actors.size(); ++i)
      {
         pcs.push_back(actors[i].get());
      }

      BaseClass::HandlePropertyContainersSelected(pcs);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::ActorPropertyChanged(dtCore::ActorPtr proxy, ActorPropertyRefPtr property)
   {
      BaseClass::ActorPropertyChanged(*proxy, *property);
   }


   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::closeEvent(QCloseEvent* e)
   {
      if(EditorActions::GetInstance().mActionWindowsPropertyEditor != NULL)
      {
         EditorActions::GetInstance().mActionWindowsPropertyEditor->setChecked(false);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::PropertyAboutToChangeFromControl(dtCore::PropertyContainer& propCon, dtCore::ActorProperty& prop,
            const std::string& oldValue, const std::string& newValue)
   {
      dtCore::BaseActorObject* actor = dynamic_cast<dtCore::BaseActorObject*>(&propCon);
     if (actor != nullptr)
      {
         EditorEvents::GetInstance().emitActorPropertyAboutToChange(actor, &prop, oldValue, newValue);
      }
      else
      {
         EditorEvents::GetInstance().emitActorPropertyAboutToChange(NULL, &prop, oldValue, newValue);
      }

   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::PropertyChangedFromControl(dtCore::PropertyContainer& propCon, dtCore::ActorProperty& prop)
   {
      BasePropertyEditor::PropertyChangedFromControl(propCon, prop);

      dtCore::BaseActorObject* actor = dynamic_cast<dtCore::BaseActorObject*>(&propCon);
      if (actor != NULL)
      {
         EditorEvents::GetInstance().emitActorPropertyChanged(actor, &prop);
      }
      else
      {
         EditorEvents::GetInstance().emitActorPropertyChanged(NULL, &prop);
      }
   }


} // namespace dtEditQt

