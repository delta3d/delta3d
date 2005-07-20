/*
* Delta3D Open Source Game and Simulation Engine Level Editor
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
* @author Curtiss Murphy
*/
#include "dtEditQt/actortypetreewidget.h"

#include <QLabel>
#include <QGridLayout>
//#include <QVBoxWidget>
#include <QScrollArea>
#include <QScrollBar>
#include <QTreeWidget>
//#include <QGridWidget>
#include <QStringList>
#include <QMainWindow>
#include <QHeaderView>
#include <QGroupBox>
#include <QTreeView>
#include <QAction>
#include <QHeaderView>
#include <dtCore/deltadrawable.h>
#include "dtEditQt/global.h"
#include "dtEditQt/dynamicabstractcontrol.h"
#include "dtEditQt/dynamicabstractparentcontrol.h"
#include "dtEditQt/dynamicboolcontrol.h"
#include "dtEditQt/dynamiccolorrgbacontrol.h"
#include "dtEditQt/dynamicenumcontrol.h"
#include "dtEditQt/dynamicfloatcontrol.h"
#include "dtEditQt/dynamicdoublecontrol.h"
#include "dtEditQt/dynamicgroupcontrol.h"
#include "dtEditQt/dynamicintcontrol.h"
#include "dtEditQt/dynamiclabelcontrol.h"
#include "dtEditQt/dynamiclongcontrol.h"
#include "dtEditQt/dynamicnamecontrol.h"
#include "dtEditQt/dynamicresourcecontrol.h"
#include "dtEditQt/dynamicstringcontrol.h"
#include "dtEditQt/dynamicvec3control.h"
#include "dtEditQt/dynamicvec2control.h"
#include "dtEditQt/dynamicvec4control.h"
#include "dtEditQt/editoractions.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/propertyeditor.h"
#include "dtEditQt/propertyeditormodel.h"
#include "dtEditQt/propertyeditortreeview.h"
#include "dtDAL/actorproxy.h"
#include "dtDAL/actorproperty.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtDAL/exception.h"
#include "dtDAL/datatype.h"
#include "dtDAL/librarymanager.h"
#include "dtDAL/log.h"
#include "dtDAL/tree.h"

#include <osg/Referenced>
#include <osg/ref_ptr>
#include <vector>
#include <math.h>


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
        connect(&EditorEvents::getInstance(), SIGNAL(selectedActors(std::vector<osg::ref_ptr<dtDAL::ActorProxy> > &)),
            this, SLOT(handleActorsSelected(std::vector<osg::ref_ptr<dtDAL::ActorProxy> >&)));

        // listen for property change events and update the tree.  These can be generated
        // by the viewports, or the tree itself.
        connect(&EditorEvents::getInstance(), SIGNAL(actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy>,
            osg::ref_ptr<dtDAL::ActorProperty>)),
            this, SLOT(actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy>,
            osg::ref_ptr<dtDAL::ActorProperty>)));

        // listen for name changes so we can update our group box label or handle undo changes
        connect(&EditorEvents::getInstance(), SIGNAL(proxyNameChanged(osg::ref_ptr<dtDAL::ActorProxy>, std::string)),
            this, SLOT(proxyNameChanged(osg::ref_ptr<dtDAL::ActorProxy>, std::string)));

        controlFactory = new dtUtil::ObjectFactory<dtDAL::DataType *, DynamicAbstractControl>;

        // register all the data types with the dynamic control factory
        controlFactory->RegisterType<DynamicStringControl>(&(dtDAL::DataType::STRING));
        controlFactory->RegisterType<DynamicFloatControl>(&(dtDAL::DataType::FLOAT));
        controlFactory->RegisterType<DynamicDoubleControl>(&(dtDAL::DataType::DOUBLE));
        controlFactory->RegisterType<DynamicVec3Control>(&(dtDAL::DataType::VEC3));
        controlFactory->RegisterType<DynamicIntControl>(&(dtDAL::DataType::INT));
        controlFactory->RegisterType<DynamicLongControl>(&(dtDAL::DataType::LONGINT));
        controlFactory->RegisterType<DynamicBoolControl>(&(dtDAL::DataType::BOOLEAN));
        controlFactory->RegisterType<DynamicVec2Control>(&(dtDAL::DataType::VEC2));
        controlFactory->RegisterType<DynamicVec4Control>(&(dtDAL::DataType::VEC4));
        controlFactory->RegisterType<DynamicEnumControl>(&(dtDAL::DataType::ENUMERATION));
        controlFactory->RegisterType<DynamicColorRGBAControl>(&(dtDAL::DataType::RGBACOLOR));
        controlFactory->RegisterType<DynamicResourceControl>(&(dtDAL::DataType::SOUND));
        controlFactory->RegisterType<DynamicResourceControl>(&(dtDAL::DataType::STATIC_MESH));
        controlFactory->RegisterType<DynamicResourceControl>(&(dtDAL::DataType::TEXTURE));
        controlFactory->RegisterType<DynamicResourceControl>(&(dtDAL::DataType::CHARACTER));
        controlFactory->RegisterType<DynamicResourceControl>(&(dtDAL::DataType::TERRAIN));
        controlFactory->RegisterType<DynamicResourceControl>(&(dtDAL::DataType::PARTICLE_SYSTEM));

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
    void PropertyEditor::handleActorsSelected(std::vector<osg::ref_ptr<dtDAL::ActorProxy> > &actors)
    {
        std::vector<osg::ref_ptr<dtDAL::ActorProxy > >::const_iterator iter;

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
            osg::ref_ptr<dtDAL::ActorProxy> myProxy = (*iter);
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
        std::vector<osg::ref_ptr<dtDAL::ActorProxy > >::const_iterator iter;
        bool isMultiSelect = selectedActors.size() > 1;

        // QT has an issue when you remove children.  If you have created an
        // editor at some point for a control in a tree, it holds onto that control.
        // Then, at some point, it tries to access the pointer, that has since been
        // deleted when we change selections.  So, delete the tree and start over :(
        // This was originally found in Beta2.
        //
        // Note 2 - 6/23/05 - This was attempted with RC1 but it still seems to have problems 
        // when you try to add and remove elements from the tree.  If you attempt to not recreate 
        // it, you need to do some work in dynamicGroupControl and DynamicAbstractParentControl with their 
        // adding and removing children.  See there for more info.
        if (propertyTree != NULL)
            delete propertyTree;
        propertyTree = new PropertyEditorTreeView(propertyModel, actorPropBox);
        rootProperty->removeAllChildren(propertyModel);
        propertyTree->setRoot(rootProperty);
        dynamicControlLayout->addWidget(propertyTree);

        resetGroupBoxLabel();

        // Walk our selection items.
        for(iter = selectedActors.begin(); iter != selectedActors.end(); ++iter)
        {
            osg::ref_ptr<dtDAL::ActorProxy> myProxy = (*iter);

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

        //propertyTree->reset();

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
            osg::ref_ptr<dtDAL::ActorProxy> myProxy = selectedActors[0];
            QString label = baseGroupBoxName + " ('" + tr(myProxy->GetName().c_str()) + "' selected)";
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
    void PropertyEditor::buildDynamicControls(osg::ref_ptr<dtDAL::ActorProxy> proxy)
    {
        dtDAL::ActorProperty *curProp;
        std::vector<dtDAL::ActorProperty *> propList;
        DynamicAbstractControl *newControl;
        std::vector<dtDAL::ActorProperty *>::const_iterator propIter;
        int row = 0;
        int tempCount = 0;

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
                    LOG_ERROR("Object Factory failed to create a control for property: " + curProp->GetPropertyType().GetName());
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
            catch (dtDAL::Exception &ex)
            {
                LOG_ERROR("Failed to create a control for property: " + curProp->GetPropertyType().GetName() +
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
        core::tree<QString> &currentTree)
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
                    core::tree<QString> &insertedItem = currentTree.
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
        core::tree<QString> &currentTree)
    {
        // walk through the children...
        for (core::tree<QString>::const_iterator iter = currentTree.in(); iter != currentTree.end(); ++iter)
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
    void PropertyEditor::actorPropertyChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy,
        osg::ref_ptr<dtDAL::ActorProperty> property)
    {
        propertyTree->viewport()->update();
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::closeEvent(QCloseEvent *e)
    {
        EditorActions::getInstance().actionWindowsPropertyEditor->setChecked(false);
    }

    /////////////////////////////////////////////////////////////////////////////////
    void PropertyEditor::proxyNameChanged(osg::ref_ptr<dtDAL::ActorProxy> proxy, std::string oldName)
    {
        resetGroupBoxLabel();
        propertyTree->viewport()->update();
    }
}
