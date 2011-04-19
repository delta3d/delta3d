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
#include <dtEditQt/dynamicgameeventcontrol.h>
#include <dtEditQt/dynamicgrouppropertycontrol.h>
#include <dtEditQt/dynamicnamecontrol.h>
#include <dtEditQt/dynamicresourcecontrol.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/viewportmanager.h>

#include <dtDAL/actorproperty.h>
#include <dtDAL/actorproxy.h>
#include <dtDAL/datatype.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/map.h>

#include <dtQt/dynamicabstractcontrol.h>
#include <dtQt/dynamiccontainercontrol.h>
#include <dtQt/dynamicgroupcontrol.h>
#include <dtQt/dynamiclabelcontrol.h>
#include <dtQt/propertyeditormodel.h>
#include <dtQt/propertyeditortreeview.h>

#include <dtUtil/log.h>
#include <dtUtil/tree.h>

#include <QtCore/QStringList>
#include <QtGui/QAction>
#include <QtGui/QGridLayout>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QMainWindow>
#include <QtGui/QScrollArea>
#include <QtGui/QScrollBar>
#include <QtGui/QTreeView>
#include <QtGui/QTreeWidget>

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

      size_t datatypeCount = dtDAL::DataType::EnumerateType().size();

      for (size_t i = 0; i < datatypeCount; ++i)
      {
         dtDAL::DataType* dt = dtDAL::DataType::EnumerateType()[i];
         if (dt->IsResource())
         {
            dcfactory.RegisterControlForDataType<DynamicResourceControl>(*dt);
         }
      }

      dcfactory.RegisterControlForDataType<DynamicActorControl>(dtDAL::DataType::ACTOR);
      dcfactory.RegisterControlForDataType<DynamicGameEventControl>(dtDAL::DataType::GAME_EVENT);
      dcfactory.RegisterControlForDataType<DynamicGroupPropertyControl>(dtDAL::DataType::GROUP);
   }

   /////////////////////////////////////////////////////////////////////////////////
   PropertyEditor::~PropertyEditor()
   {
   }

   /////////////////////////////////////////////////////////////////////////////////
   QString PropertyEditor::GetGroupBoxLabelText(const QString& baseGroupBoxName)
   {
      std::vector<dtDAL::PropertyContainer*> selectedActors;
      GetSelectedPropertyContainers(selectedActors);

      if (selectedActors.size() == 1)
      {
         // set the name in the group box.
         dtDAL::BaseActorObject* selectedProxy = dynamic_cast<dtDAL::BaseActorObject*>(selectedActors[0]);

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

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::buildDynamicControls(dtDAL::PropertyContainer& propCon, dtQt::DynamicGroupControl* parentControl)
   {
      dtQt::DynamicGroupControl* parent = GetRootControl();
      if (parentControl != NULL)
      {
         parent = parentControl;
      }

      dtDAL::BaseActorObject* proxy = dynamic_cast<dtDAL::BaseActorObject*>(&propCon);

      if (proxy != NULL)
      {
         dtQt::PropertyEditorModel* propertyEditorModel = &GetPropertyEditorModel();
         // create the basic actor group
         dtQt::DynamicGroupControl* baseGroupControl = new dtQt::DynamicGroupControl("Actor Information");
         baseGroupControl->InitializeData(parent, propertyEditorModel, NULL, NULL);
         parent->addChildControl(baseGroupControl, propertyEditorModel);

         // name of actor
         DynamicNameControl* nameControl = new DynamicNameControl();
         nameControl->InitializeData(baseGroupControl, propertyEditorModel, &propCon, NULL);
         baseGroupControl->addChildControl(nameControl, propertyEditorModel);

         // Category of actor
         dtQt::DynamicLabelControl* labelControl = new dtQt::DynamicLabelControl();
         labelControl->InitializeData(baseGroupControl, propertyEditorModel, &propCon, NULL);
         labelControl->setDisplayValues("Actor Category", "The category of the Actor - visible in the Actor Browser",
            QString(tr(proxy->GetActorType().GetCategory().c_str())));
         baseGroupControl->addChildControl(labelControl, propertyEditorModel);

         // Type of actor
         labelControl = new dtQt::DynamicLabelControl();
         labelControl->InitializeData(baseGroupControl, propertyEditorModel, &propCon, NULL);
         labelControl->setDisplayValues("Actor Type", "The actual type of the actor as defined in the by the imported library",
            QString(tr(proxy->GetActorType().GetName().c_str())));
         baseGroupControl->addChildControl(labelControl, propertyEditorModel);

         // Class of actor
         labelControl = new dtQt::DynamicLabelControl();
         labelControl->InitializeData(baseGroupControl, propertyEditorModel, &propCon, NULL);
         labelControl->setDisplayValues("Actor Class", "The Delta3D C++ class name for this actor - useful if you are trying to reference this actor in code",
            QString(tr(proxy->GetClassName().c_str())));
         baseGroupControl->addChildControl(labelControl, propertyEditorModel);
      }

      std::vector<dtDAL::ActorProperty*> propList;
      propCon.GetPropertyList(propList);

      ViewportManager::GetInstance().emitModifyPropList(propCon, propList);

      BaseClass::buildDynamicControls(propCon, propList, parentControl);
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::HandleActorsSelected(ActorProxyRefPtrVector& actors)
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
   void PropertyEditor::ActorPropertyChanged(ActorProxyRefPtr proxy, ActorPropertyRefPtr property)
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
   void PropertyEditor::PropertyAboutToChangeFromControl(dtDAL::PropertyContainer& propCon, dtDAL::ActorProperty& prop,
            const std::string& oldValue, const std::string& newValue)
   {
      dtDAL::BaseActorObject* proxy = dynamic_cast<dtDAL::BaseActorObject*>(&propCon);
      if (proxy != NULL)
      {
         EditorEvents::GetInstance().emitActorPropertyAboutToChange(proxy, &prop, oldValue, newValue);
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void PropertyEditor::PropertyChangedFromControl(dtDAL::PropertyContainer& propCon, dtDAL::ActorProperty& prop)
   {
      BasePropertyEditor::PropertyChangedFromControl(propCon, prop);

      dtDAL::BaseActorObject* proxy = dynamic_cast<dtDAL::BaseActorObject*>(&propCon);
      if (proxy != NULL)
      {
         EditorEvents::GetInstance().emitActorPropertyChanged(proxy, &prop);
      }
   }


} // namespace dtEditQt

