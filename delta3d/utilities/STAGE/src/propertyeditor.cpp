/*
* Delta3D Open Source Game and Simulation Engine
* Simulation, Training, and Game Editor (STAGE)
* Copyright (C) 2005, BMH Associates, Inc.
*
* This program is free software; you can redistribute it and/or modify it under
* the terms of the GNU General Public License as published by the Free
* Software Foundation; either version 2 of the License, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU General Public License for more
* details.
*
* You should have received a copy of the GNU General Public License
* along with this library; if not, write to the Free Software Foundation, Inc.,
* 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*
* Curtiss Murphy
*/
#include <prefix/dtstageprefix-src.h>

#include <dtEditQt/propertyeditor.h>

#include <dtEditQt/actortypetreewidget.h>
#include <dtEditQt/dynamicabstractcontrol.h>
#include <dtEditQt/dynamicabstractparentcontrol.h>
#include <dtEditQt/dynamicactorcontrol.h>
#include <dtEditQt/dynamicboolcontrol.h>
#include <dtEditQt/dynamiccolorrgbacontrol.h>
#include <dtEditQt/dynamicenumcontrol.h>
#include <dtEditQt/dynamicfloatcontrol.h>
#include <dtEditQt/dynamicdoublecontrol.h>
#include <dtEditQt/dynamicgroupcontrol.h>
#include <dtEditQt/dynamicgrouppropertycontrol.h>
#include <dtEditQt/dynamicintcontrol.h>
#include <dtEditQt/dynamiclabelcontrol.h>
#include <dtEditQt/dynamiclongcontrol.h>
#include <dtEditQt/dynamicnamecontrol.h>
#include <dtEditQt/dynamicresourcecontrol.h>
#include <dtEditQt/dynamicstringcontrol.h>
#include <dtEditQt/dynamicvec3control.h>
#include <dtEditQt/dynamicvec2control.h>
#include <dtEditQt/dynamicvec4control.h>
#include <dtEditQt/dynamicgameeventcontrol.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/propertyeditormodel.h>
#include <dtEditQt/propertyeditortreeview.h>
#include <dtEditQt/viewportmanager.h>

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

namespace dtEditQt
{

    ///////////////////////////////////////////////////////////////////////////////
    PropertyEditor::PropertyEditor(QMainWindow * parent) : QDockWidget(parent)
    {
        LOG_INFO("Initializing PropertyEditor");
        propertyModel = NULL;
        propertyTree = NULL;
        rootProperty = NULL;

        lastScrollBarLocation = 0; // top
        lastHeaderPosition = 100; // rough guess on a good size.

        baseGroupBoxName = tr("Actor Properties");

        setWindowTitle(tr("Property Editor"));

        setupUI();

        // listen for selection changed event
        connect(&EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorProxyRefPtrVector &)),
            this, SLOT(handleActorsSelected(ActorProxyRefPtrVector&)));

        // listen for property change events and update the tree.  These can be generated
        // by the viewports, or the tree itself.
        connect(&EditorEvents::GetInstance(), SIGNAL(actorPropertyChanged(ActorProxyRefPtr,
            ActorPropertyRefPtr)),
            this, SLOT(actorPropertyChanged(ActorProxyRefPtr,
            ActorPropertyRefPtr)));

        // listen for name changes so we can update our group box label or handle undo changes
        connect(&EditorEvents::GetInstance(), SIGNAL(proxyNameChanged(ActorProxyRefPtr, std::string)),
            this, SLOT(proxyNameChanged(ActorProxyRefPtr, std::string)));
        
        // listen for closing
        connect(&EditorEvents::GetInstance(), SIGNAL(editorCloseEvent()),
            this, SLOT(OnEditorClosing()));

        controlFactory = new dtUtil::ObjectFactory<dtDAL::DataType *, DynamicAbstractControl>;

        // register all the data types with the dynamic control factory
        controlFactory->RegisterType<DynamicStringControl>(&(dtDAL::DataType::STRING));
        controlFactory->RegisterType<DynamicFloatControl>(&(dtDAL::DataType::FLOAT));
        controlFactory->RegisterType<DynamicDoubleControl>(&(dtDAL::DataType::DOUBLE));
        controlFactory->RegisterType<DynamicVec3Control>(&(dtDAL::DataType::VEC3));
        controlFactory->RegisterType<DynamicVec3Control>(&(dtDAL::DataType::VEC3F));
        controlFactory->RegisterType<DynamicVec3Control>(&(dtDAL::DataType::VEC3D));
        controlFactory->RegisterType<DynamicIntControl>(&(dtDAL::DataType::INT));
        controlFactory->RegisterType<DynamicLongControl>(&(dtDAL::DataType::LONGINT));
        controlFactory->RegisterType<DynamicBoolControl>(&(dtDAL::DataType::BOOLEAN));
        controlFactory->RegisterType<DynamicVec2Control>(&(dtDAL::DataType::VEC2));
        controlFactory->RegisterType<DynamicVec2Control>(&(dtDAL::DataType::VEC2F));
        controlFactory->RegisterType<DynamicVec2Control>(&(dtDAL::DataType::VEC2D));
        controlFactory->RegisterType<DynamicVec4Control>(&(dtDAL::DataType::VEC4));
        controlFactory->RegisterType<DynamicVec4Control>(&(dtDAL::DataType::VEC4F));
        controlFactory->RegisterType<DynamicVec4Control>(&(dtDAL::DataType::VEC4D));
        controlFactory->RegisterType<DynamicEnumControl>(&(dtDAL::DataType::ENUMERATION));
        controlFactory->RegisterType<DynamicColorRGBAControl>(&(dtDAL::DataType::RGBACOLOR));
        controlFactory->RegisterType<DynamicResourceControl>(&(dtDAL::DataType::SOUND));
        controlFactory->RegisterType<DynamicResourceControl>(&(dtDAL::DataType::STATIC_MESH));
        controlFactory->RegisterType<DynamicResourceControl>(&(dtDAL::DataType::TEXTURE));
        //controlFactory->RegisterType<DynamicResourceControl>(&(dtDAL::DataType::CHARACTER));
        controlFactory->RegisterType<DynamicResourceControl>(&(dtDAL::DataType::TERRAIN));
        controlFactory->RegisterType<DynamicResourceControl>(&(dtDAL::DataType::PARTICLE_SYSTEM));
        controlFactory->RegisterType<DynamicActorControl>(&(dtDAL::DataType::ACTOR));
        controlFactory->RegisterType<DynamicGameEventControl>(&(dtDAL::DataType::GAME_EVENT));
        controlFactory->RegisterType<DynamicGroupPropertyControl>(&(dtDAL::DataType::GROUP));
        controlFactory->RegisterType<DynamicResourceControl>(&(dtDAL::DataType::SKELETAL_MESH));
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::OnEditorClosing()
    {
        // listen for selection changed event
        disconnect(&EditorEvents::GetInstance(), SIGNAL(selectedActors(ActorProxyRefPtrVector &)),
            this, SLOT(handleActorsSelected(ActorProxyRefPtrVector&)));

        // listen for property change events and update the tree.  These can be generated
        // by the viewports, or the tree itself.
        disconnect(&EditorEvents::GetInstance(), SIGNAL(actorPropertyChanged(ActorProxyRefPtr,
            ActorPropertyRefPtr)),
            this, SLOT(actorPropertyChanged(ActorProxyRefPtr,
            ActorPropertyRefPtr)));

        // listen for name changes so we can update our group box label or handle undo changes
        disconnect(&EditorEvents::GetInstance(), SIGNAL(proxyNameChanged(ActorProxyRefPtr, std::string)),
            this, SLOT(proxyNameChanged(ActorProxyRefPtr, std::string)));
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::setupUI()
    {
        // create the main widget and the scroll and setup our editor docking window.
        mainAreaWidget = new QWidget(this);
        QGridLayout *mainAreaLayout = new QGridLayout(mainAreaWidget);
        setWidget(mainAreaWidget);

        // build the dynamic property area
        actorPropBox = new QGroupBox(baseGroupBoxName, mainAreaWidget);
        actorPropBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
        mainAreaLayout->addWidget(actorPropBox, 0, 0);
        dynamicControlLayout = new QGridLayout(actorPropBox);

        // create the base model and root properties for our tree
        propertyModel = new PropertyEditorModel(this);
        rootProperty = new DynamicGroupControl("root");

        // Left here.  This is the code you need here if you want to NOT recreate the
        // tree everytime.  See the comment a few methods down.
        //propertyTree = new PropertyEditorTreeView(propertyModel, actorPropBox);
        //propertyTree->setMinimumSize(100, 100);
        //propertyTree->setRoot(rootProperty);
        //dynamicControlLayout->addWidget(propertyTree);

        refreshSelectedActors();

    }

    ///////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::handleActorsSelected(std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > &actors)
    {
        std::vector<dtCore::RefPtr<dtDAL::ActorProxy > >::const_iterator iter;

        // get the currently open tree branches and current caret position so we
        // can scroll back to it as best as we can.  Only do this if we have
        // exactly 1.  That way, it will remember the last valid display.
        if (selectedActors.size() == 1)
            markCurrentExpansion();

        // clear our selected list.
        selectedActors.clear();

        // copy passed in actors to our internal list.
        for(iter = actors.begin(); iter != actors.end(); ++iter)
        {
            dtCore::RefPtr<dtDAL::ActorProxy> myProxy = (*iter);
            selectedActors.push_back(myProxy);
            LOG_INFO(std::string("Selected Actors found a proxy") + myProxy->GetName());
        }

        // turn off screen updates so that we don't watch it draw
        this->setUpdatesEnabled(false);
        mainAreaWidget->setUpdatesEnabled(false);
        actorPropBox->setUpdatesEnabled(false);

        refreshSelectedActors();

        // turn them back on, so it looks right
        this->setUpdatesEnabled(true);
        mainAreaWidget->setUpdatesEnabled(true);
        actorPropBox->setUpdatesEnabled(true);

    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::refreshSelectedActors()
    {
        CommitCurrentEdits();
            
        std::vector<dtCore::RefPtr<dtDAL::ActorProxy > >::const_iterator iter;
        bool isMultiSelect = selectedActors.size() > 1;

        // The ordering of the following code block has been known to cause
        // problems in various versions of qt.  This code works in 4.2.1.
        // if it breaks in a later version, past experience has shown that
        // changing the order in some way will make it work
        delete propertyTree;
        rootProperty->removeAllChildren(propertyModel);
        propertyTree = new PropertyEditorTreeView(propertyModel, actorPropBox);
        dynamicControlLayout->addWidget(propertyTree);
        propertyTree->setRoot(rootProperty);

        resetGroupBoxLabel();
        // Walk our selection items.
        for(iter = selectedActors.begin(); iter != selectedActors.end(); ++iter)
        {
            dtCore::RefPtr<dtDAL::ActorProxy> myProxy = (*iter);

            // build the dynamic controls
            if (!isMultiSelect)
            {
                buildDynamicControls(myProxy);
            }
            else
            {
                // create a single label entry for each multi selected proxy
                DynamicLabelControl *labelControl = new DynamicLabelControl();
                labelControl->initializeData(rootProperty, propertyModel, myProxy.get(), NULL);
                labelControl->setDisplayValues(tr(myProxy->GetActorType().GetName().c_str()), "",
                    QString(tr(myProxy->GetName().c_str())));
                rootProperty->addChildControl(labelControl, propertyModel);
            }
        }

        propertyTree->reset();

        // we deleted the tree, so we have  to reset some sizes
        //actorPropBox->setMinimumSize(actorPropBox->sizeHint());
        //mainAreaWidget->setMinimumSize(mainAreaWidget->sizeHint());
        propertyTree->setMinimumSize(80, 80);
        propertyTree->update();

        //propertyTree->resizeColumnToContents(0);
        //propertyTree->resizeColumnToContents(1);

        // Now, go back and try to re-expand items and restore our scroll position
        restorePreviousExpansion();

        propertyTree->show();
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::resetGroupBoxLabel()
    {
        if (selectedActors.size() == 0)
        {
            // just put our title
            actorPropBox->setTitle(baseGroupBoxName);
        }
        else if (selectedActors.size() == 1)
        {
            // set the name in the group box.
            dtCore::RefPtr<dtDAL::ActorProxy> myProxy = selectedActors[0];
            QString label;
            if(myProxy == EditorData::GetInstance().getCurrentMap()->GetEnvironmentActor())
               label = baseGroupBoxName + " ('" + tr(myProxy->GetName().c_str()) + " *Environment Actor*' selected)";
            else
               label = baseGroupBoxName + " ('" + tr(myProxy->GetName().c_str()) + "' selected)";
            actorPropBox->setTitle(label);
        }
        else
        {
            //  put the count of selections
            QString label = baseGroupBoxName + " (" + QString::number(selectedActors.size()) + " selected)";
            actorPropBox->setTitle(label);
        }
    }


    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::buildDynamicControls(dtCore::RefPtr<dtDAL::ActorProxy> proxy)
    {
        dtDAL::ActorProperty *curProp;
        std::vector<dtDAL::ActorProperty *> propList;
        DynamicAbstractControl *newControl;
        std::vector<dtDAL::ActorProperty *>::const_iterator propIter;
        int row = 0;

        proxy->GetPropertyList(propList);

        // create controls for the basic properties - name, type, etc...

        // create the basic actor group
        DynamicGroupControl *baseGroupControl = new DynamicGroupControl("Actor Information");
        baseGroupControl->initializeData(rootProperty, propertyModel, NULL, NULL);
        rootProperty->addChildControl(baseGroupControl, propertyModel);

        // name of actor
        DynamicNameControl *nameControl = new DynamicNameControl();
        nameControl->initializeData(baseGroupControl, propertyModel, proxy.get(), NULL);
        baseGroupControl->addChildControl(nameControl, propertyModel);

        // Category of actor
        DynamicLabelControl *labelControl = new DynamicLabelControl();
        labelControl->initializeData(baseGroupControl, propertyModel, proxy.get(), NULL);
        labelControl->setDisplayValues("Actor Category", "The category of the Actor - visible in the Actor Browser",
            QString(tr(proxy->GetActorType().GetCategory().c_str())));
        baseGroupControl->addChildControl(labelControl, propertyModel);

        // Type of actor
        labelControl = new DynamicLabelControl();
        labelControl->initializeData(baseGroupControl, propertyModel, proxy.get(), NULL);
        labelControl->setDisplayValues("Actor Type", "The actual type of the actor as defined in the by the imported library",
            QString(tr(proxy->GetActorType().GetName().c_str())));
        baseGroupControl->addChildControl(labelControl, propertyModel);

        // Class of actor
        labelControl = new DynamicLabelControl();
        labelControl->initializeData(baseGroupControl, propertyModel, proxy.get(), NULL);
        labelControl->setDisplayValues("Actor Class", "The Delta3D C++ class name for this actor - useful if you are trying to reference this actor in code",
            QString(tr(proxy->GetClassName().c_str())));
        baseGroupControl->addChildControl(labelControl, propertyModel);


        // for each property, create a new dynamic control and add it to a group, if appropriate.
        for (propIter = propList.begin(); propIter != propList.end(); ++propIter)
        {
            curProp = (*propIter);
            try
            {
                // first create the control.  Sometimes the controls aren't creatable, so
                // check that first before we do other work.  Excepts if it fails
                newControl = controlFactory->CreateObject(&curProp->GetPropertyType());
                if (newControl == NULL)
                {
                    LOG_ERROR("Object Factory failed to create a control for property: " + curProp->GetDataType().GetName());
                }
                else
                {
                    newControl->setTreeView(propertyTree);

                    // Work with the group.  Requires finding an existing group or creating one,
                    // and eventually adding our new control to that group control
                    const std::string &groupName = curProp->GetGroupName();
                    if (!groupName.empty())
                    {
                        // find our group
                        DynamicGroupControl *groupControl = rootProperty->getChildGroupControl(QString(groupName.c_str()));

                        // if no group, then create one.
                        if (groupControl == NULL)
                        {
                            groupControl = new DynamicGroupControl(groupName);
                            groupControl->initializeData(rootProperty, propertyModel, proxy.get(), NULL);
                            rootProperty->addChildControl(groupControl, propertyModel);
                        }

                        // add our new control to the group.
                        newControl->initializeData(groupControl, propertyModel, proxy.get(), curProp);
                        groupControl->addChildControl(newControl, propertyModel);

                    }
                    else
                    {
                        // there's no group, so use the root.
                        newControl->initializeData(rootProperty, propertyModel, proxy.get(), curProp);
                        rootProperty->addChildControl(newControl, propertyModel);
                    }

                    // the following code doesn't work.  I'm leaving it here for reference.
                    // basically, it's supposed to check and create the control in such a way
                    // that it's always visible regardless if the user had just clicked in the
                    // control or not.  QT creates and destroys the edit controls on the fly.
                    // make the new controls editor persistent if necessary.
                    //if (newControl->isNeedsPersistentEditor()) {
                    //    QModelIndex index = propertyModel->indexOf(newControl, 1);
                    //    propertyTree->openPersistentEditor(index);
                    //}
                }

            }
            catch (dtUtil::Exception &ex)
            {
                LOG_ERROR("Failed to create a control for property: " + curProp->GetDataType().GetName() +
                    " with error: " + ex.What());
            }

            row++;
        }

    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::markCurrentExpansion()
    {
        // clear out previous marks
        expandedTreeNames.clear();

        if (propertyTree != NULL && rootProperty != NULL)
        {
            // start recursion
            recurseMarkCurrentExpansion(rootProperty, expandedTreeNames);

            // also store the last location of the scroll bar... so that they go back
            // to where they were next time.
            lastScrollBarLocation = propertyTree->verticalScrollBar()->value();//sliderPosition();
            lastHeaderPosition = propertyTree->header()->sectionSize(0);
        }

    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::recurseMarkCurrentExpansion(DynamicAbstractControl *parent,
        dtUtil::tree<QString> &currentTree)
    {
        for (int i = 0; i < parent->getChildCount(); i++)
        {
            DynamicAbstractControl *child = parent->getChild(i);

            // if we have children, then we could potentially be expanded...
            if (child->getChildCount() > 0)
            {
                QModelIndex index = propertyModel->indexOf(child);
                if (propertyTree->isExpanded(index))
                {
                    // add it to our list
                    dtUtil::tree<QString> &insertedItem = currentTree.
                        insert(child->getDisplayName()).tree_ref();

                    // recurse on the child with the new tree
                    recurseMarkCurrentExpansion(child, insertedItem);
                }
            }

        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::restorePreviousExpansion()
    {
        recurseRestorePreviousExpansion(rootProperty, expandedTreeNames);

        // Put the scroll bar back where it was last time
        propertyTree->verticalScrollBar()->setValue(lastScrollBarLocation);//setSliderPosition(lastScrollBarLocation);
        // reset the little header scroll bar
        propertyTree->header()->resizeSection(0, lastHeaderPosition);
        lastScrollBarLocation = propertyTree->verticalScrollBar()->value();//sliderPosition();
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::recurseRestorePreviousExpansion(DynamicAbstractControl *parent,
        dtUtil::tree<QString> &currentTree)
    {
        // walk through the children...
        for (dtUtil::tree<QString>::const_iterator iter = currentTree.in(); iter != currentTree.end(); ++iter)
        {
            QString name = (*iter);

            // Try to find a control with this name in our propertyModel
            for (int i = 0; i < parent->getChildCount(); i ++)
            {
                DynamicAbstractControl *child = parent->getChild(i);
                // found a match!  expand it
                if (child->getDisplayName() == name)
                {
                    QModelIndex childIndex = propertyModel->indexOf(child);
                    propertyTree->setExpanded(childIndex, true);

                    // recurse over the children of this object
                    recurseRestorePreviousExpansion(child, iter.tree_ref());
                }
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::actorPropertyChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy,
        dtCore::RefPtr<dtDAL::ActorProperty> property)
    {
        propertyTree->viewport()->update();

        if(property->GetDataType() == dtDAL::DataType::TERRAIN)
        {
           if(ViewportManager::GetInstance().IsPagingEnabled())
              ViewportManager::GetInstance().EnablePaging(false);

           ViewportManager::GetInstance().EnablePaging(true);
        }
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::closeEvent(QCloseEvent *e)
    {
        EditorActions::GetInstance().actionWindowsPropertyEditor->setChecked(false);
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::proxyNameChanged(dtCore::RefPtr<dtDAL::ActorProxy> proxy, std::string oldName)
    {
        resetGroupBoxLabel();
        propertyTree->viewport()->update();
    }
}
