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
* @author Teague Coonan
*/

#include <QDir>
#include <QHeaderView>

#include <QHBoxLayout>
#include <QGridLayout>
#include <QSplitter>
#include <QString>

#include <QGroupBox>

#include <QPushButton>
#include <QCheckBox>

#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <QIcon>

#include "dtEditQt/staticmeshbrowser.h"
#include "dtEditQt/resourcetreewidget.h"
#include "dtDAL/project.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"
#include "dtDAL/exception.h"
#include "dtDAL/librarymanager.h"
#include <dtUtil/log.h>
#include "dtDAL/map.h"
#include "dtDAL/enginepropertytypes.h"
#include "dtEditQt/mainwindow.h"

#include <dtCore/scene.h>
#include <dtCore/object.h>

#include <osg/BoundingSphere>

#include "dtEditQt/perspectiveviewport.h"
#include "dtEditQt/viewportcontainer.h"
#include "dtEditQt/viewportmanager.h"
#include "dtEditQt/camera.h"
#include "dtEditQt/uiresources.h"

namespace dtEditQt
{

    ///////////////////////////////////////////////////////////////////////////////
    StaticMeshBrowser::StaticMeshBrowser(dtDAL::DataType &type,QWidget *parent)
        : ResourceAbstractBrowser(&type,parent)
    {

        // This sets our resource icon that is visible on leaf nodes
        resourceIcon = new QIcon();
        resourceIcon->addPixmap(QPixmap(UIResources::ICON_STATICMESH_RESOURCE.c_str()));
        ResourceAbstractBrowser::resourceIcon = *resourceIcon;

        // create a new scene for the static mesh viewport
        meshScene = new dtCore::Scene();
        previewObject = new dtCore::Object();
        meshScene->AddDrawable(previewObject.get());
        camera = new Camera();
        camera->makePerspective(60.0f,1.333f,0.1f,100000.0f);

        QSplitter *splitter = new QSplitter(Qt::Vertical,this);

        splitter->addWidget(previewGroup());
        splitter->addWidget(listGroup());

        splitter->setStretchFactor(0,1);
        splitter->setStretchFactor(1,1);

        // setup the grid layouts
        grid = new QGridLayout(this);
        grid->addWidget(splitter,0,0);
        grid->addWidget(standardButtons(QString("Resource Tools")),1,0,Qt::AlignCenter);

        createActions();
        createContextMenu();

        connect(&EditorEvents::getInstance(),SIGNAL(currentMapChanged()),
            this,SLOT(selectionChanged()));

        // This corrects the stretch for the last row
        grid->setRowStretch(1,0);
    }
    ///////////////////////////////////////////////////////////////////////////////
    StaticMeshBrowser::~StaticMeshBrowser(){}
    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *StaticMeshBrowser::previewGroup()
    {
        QGroupBox *groupBox = new QGroupBox(tr("Preview"));

        QGridLayout *grid = new QGridLayout(groupBox);

        // New reference of the viewport manager singleton
        ViewportManager &vpMgr = ViewportManager::getInstance();

        // Create the perspective viewport for the static mesh preview window
        perspView = (PerspectiveViewport *)vpMgr.createViewport("Preview",
            ViewportManager::ViewportType::PERSPECTIVE);

        // Assign the viewport a new scene
        perspView->setScene(meshScene.get());

        //By default, perspective viewports have their camera set to the world view
        //camera.  The world view camera is what is used in the main perspective view.
        perspView->setCamera(camera.get());

        //No need for an overlay for this viewport since we cannot select meshes
        //in the preview window.
        perspView->setOverlay(NULL);

        // Disable the interaction modes
        perspView->setAutoInteractionMode(false);
        perspView->setAutoSceneUpdate(false);
        perspView->setEnableKeyBindings(false);

        // Enable camera mode for this view
        perspView->setCameraMode();

        // Create a viewport container for our static mesh window
        container = new ViewportContainer(perspView,groupBox);
        container->setViewport(perspView);

        grid->addWidget(container,0,0);
        return groupBox;
    }
    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *StaticMeshBrowser::listGroup()
    {
        QGroupBox *groupBox = new QGroupBox(tr("Static Meshes"));
        QGridLayout *grid = new QGridLayout(groupBox);
        QHBoxLayout *hbox = new QHBoxLayout();

        // Checkbox for auto preview
        previewChk = new QCheckBox(tr("Auto Preview"),groupBox);
        connect(previewChk,SIGNAL(stateChanged(int)),this,SLOT(checkBoxSelected()));
        previewChk->setChecked(false);

        // Preview button for a selected mesh
        previewBtn = new QPushButton("Preview",groupBox);
        connect(previewBtn, SIGNAL(clicked()), this, SLOT(displaySelection()));
        previewBtn->setDisabled(true);

        hbox->addWidget(previewChk,0,Qt::AlignLeft);
        hbox->addWidget(previewBtn,0,Qt::AlignRight);
        grid->addLayout(hbox,0,0);
        grid->addWidget(tree,1,0);

        return groupBox;
    }
    ///////////////////////////////////////////////////////////////////////////////
    // Keyboard Event filter
    ///////////////////////////////////////////////////////////////////////////////
    bool StaticMeshBrowser::eventFilter(QObject *obj, QEvent *e)
    {
        if (obj == tree) {
            //For some reason, KeyPress is getting defined by something...
            //Without this undef, it will not compile under Linux..
            //It would be great if someone could figure out exactly what's
            //going on.
#undef KeyPress
            if (e->type() == QEvent::KeyPress) {
                QKeyEvent *keyEvent = (QKeyEvent *)e;
                switch(keyEvent->key())
                {
                case Qt::Key_Return :
                    if(selection->isResource()){
                        selectionChanged();
                        displaySelection();
                    }
                    break;
                case Qt::Key_Enter:
                    if(selection->isResource()){
                        selectionChanged();
                        displaySelection();
                    }
                    break;
                default:
                    return tree->eventFilter(obj,e);
                }
            }
            else{
                // pass the event on to the parent class
                return tree->eventFilter(obj, e);
            }
        }
        return false;
    }
    ///////////////////////////////////////////////////////////////////////////////
    void StaticMeshBrowser::createActions()
    {
        ResourceAbstractBrowser::createActions();

        setCreateAction = new QAction(tr("&Create Actor"),getCurrentParent());
        setCreateAction->setCheckable(false);
        connect(setCreateAction, SIGNAL(triggered()),this,SLOT(createActor()));
        setCreateAction->setEnabled(false);
    }
    ///////////////////////////////////////////////////////////////////////////////
    void StaticMeshBrowser::createContextMenu()
    {
        ResourceAbstractBrowser::createContextMenu();
        contextMenu->addAction(setCreateAction);
    }
    ///////////////////////////////////////////////////////////////////////////////
    // Slots
    ///////////////////////////////////////////////////////////////////////////////
    void StaticMeshBrowser::displaySelection()
    {
        ResourceTreeWidget *selection = currentSelection();
        bool validFile = false;

        if(selection != NULL)
        {
            QString file;
            QString context;

            dtDAL::Project &project = dtDAL::Project::GetInstance();

            // Find the currently selected tree item
            dtDAL::ResourceDescriptor resource = EditorData::getInstance().getCurrentMeshResource();

            try
            {
                file = QString(project.GetResourcePath(resource).c_str());
                validFile = true;
            }
            catch (dtDAL::Exception &e)
            {
                validFile = false;
            }

            if(file != NULL && validFile==true)
            {
                context = QString(project.GetContext().c_str());
                // The following is performed to comply with linux and windows file systems
                file = context+"\\"+file;
                file.replace("\\","/");

                if(meshScene->GetDrawableIndex(previewObject.get())==(unsigned)meshScene->GetNumberOfAddedDrawable()){
                    meshScene->AddDrawable(previewObject.get());
                }

                //Load the new file.
                previewObject->LoadFile(file.toStdString());
                previewObject->RecenterGeometryUponLoad();
                perspView->refresh();

                //Now we need to get the bounding volume to determine the extents
                //of the new static mesh.  If the extents are within a reasonable
                //size, the camera will be placed such that the static mesh is
                //slightly in front of the camera.  If the mesh is too large,
                //the camera is placed in the center of the mesh.
                const osg::BoundingSphere &bs = previewObject->GetOSGNode()->getBound();
                float offset = (bs.radius() < 1000.0f) ? bs.radius() : 0.0f;

                camera->resetRotation();
                osg::Vec3 viewDir = camera->getViewDir();

                if (offset > 0.0f)
                    camera->setPosition(viewDir*offset*-2.0f);
                else
                    camera->setPosition(bs.center());

                perspView->refresh();
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void StaticMeshBrowser::selectionChanged()
    {
        // This is the abstract base classes original functionality
        ResourceAbstractBrowser::selectionChanged();

        // Let's assume that the map could be closed
        setCreateAction->setEnabled(false);

        // When any item is selected, clear the scene
        meshScene->RemoveDrawable(previewObject.get());
        perspView->refresh();

        if(selection != NULL)
        {
            if(selection->isResource())
            {
                // auto preview
                if(previewChk->isChecked())
                {
                    displaySelection();
                }
                previewBtn->setDisabled(false);

                if(EditorData::getInstance().getCurrentMap() != NULL)
                {
                    setCreateAction->setEnabled(true);
                }
            }
            else
            {
                perspView->refresh();
                previewBtn->setDisabled(true);
                setCreateAction->setEnabled(false);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void StaticMeshBrowser::checkBoxSelected()
    {
        if(previewChk->isChecked())
        {
            if(selection->isResource())
            {
                // preview current item
                selectionChanged();
                displaySelection();
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void StaticMeshBrowser::doubleClickEvent()
    {
        if(selection->isResource())
        {
            displaySelection();
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void StaticMeshBrowser::deleteItemEvent()
    {
        if(selection->isResource())
        {
            // When any item is selected, clear the scene
            meshScene->RemoveDrawable(previewObject.get());
            perspView->refresh();
            previewBtn->setDisabled(true);
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void StaticMeshBrowser::createActor()
    {

        EditorData::getInstance().getMainWindow()->startWaitCursor();

        if(selection->isResource())
        {

            LOG_INFO("User Created an Actor - Slot");

            // if we have an actor type, then create the proxy and emit the signal
            /*
            * The following code finds the actor type by a hard-coded string for the
            * category and name. This means that if the actor for Static Mesh Object
            * is changed in any way that the following code will break. This was
            * implemented as a quick and dirty solution to assigning meshes to an
            * actor of this type.
            */
            osg::ref_ptr<dtDAL::ActorType> meshActor =
                dtDAL::LibraryManager::GetInstance().FindActorType("dtcore","Static Mesh");

            // create our new actor proxy from the mesh actor type that was
            // found by the results of our hard coded search above.
            if(meshActor!=NULL)
            {
                osg::ref_ptr<dtDAL::ActorProxy> proxy =
                        dtDAL::LibraryManager::GetInstance().CreateActorProxy(*meshActor.get());

                // check to make sure both the mesh actor and the proxy are valid.
                // If the user has somehow modified the above hard coded static mesh object
                // the application could potentially be in a dangerous state.
                if (proxy.valid())
                {
                    // grab the actor property type
                    dtDAL::ResourceActorProperty *resourceProp = dynamic_cast<dtDAL::ResourceActorProperty *>
                        (proxy->GetProperty("static mesh"));

                    if (resourceProp != NULL)
                    {
                        resourceProp->SetValue(&selection->getResourceDescriptor());
                    }

                    // add the new proxy to the map
                    osg::ref_ptr<dtDAL::Map> mapPtr = EditorData::getInstance().getCurrentMap();
                    if (mapPtr.valid())
                    {
                        mapPtr->AddProxy(*proxy);
                    }

                    // Let the world know that a new proxy exists
                    EditorEvents::getInstance().emitBeginChangeTransaction();
                    EditorEvents::getInstance().emitActorProxyCreated(proxy, false);
                    ViewportManager::getInstance().placeProxyInFrontOfCamera(proxy.get());
                    EditorEvents::getInstance().emitEndChangeTransaction();

                    // Now, let the world that it should select the new actor proxy.
                    std::vector<osg::ref_ptr<dtDAL::ActorProxy> > actors;

                    actors.push_back(proxy);
                    EditorEvents::getInstance().emitActorsSelected(actors);
                }
            }
            EditorData::getInstance().getMainWindow()->endWaitCursor();
        }
    }
}
