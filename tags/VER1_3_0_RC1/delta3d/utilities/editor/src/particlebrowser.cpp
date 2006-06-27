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

//had to add this header to make it compile in linux
//not sure why.
#include <QtGui/QHeaderView>

#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtCore/QString>

#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>

#include <QtGui/QGroupBox>

#include <QtGui/QPushButton>

#include <dtCore/scene.h>
#include <dtCore/object.h>

#include <dtDAL/enginepropertytypes.h>
#include <dtDAL/datatype.h>
#include <dtDAL/librarymanager.h>

#include "dtEditQt/editordata.h"
#include "dtEditQt/mainwindow.h"
#include "dtEditQt/particlebrowser.h"
#include "dtEditQt/perspectiveviewport.h"
#include "dtEditQt/resourcetreewidget.h"
#include "dtEditQt/viewportcontainer.h"
#include "dtEditQt/viewportmanager.h"
#include "dtEditQt/camera.h"
#include "dtEditQt/uiresources.h"

namespace dtEditQt
{

    ///////////////////////////////////////////////////////////////////////////////
    ParticleBrowser::ParticleBrowser(dtDAL::DataType &type,QWidget *parent)
        : ResourceAbstractBrowser(&type,parent)
    {

        // This sets our resource icon that is visible on leaf nodes
        resourceIcon = new QIcon();
        resourceIcon->addPixmap(QPixmap(UIResources::ICON_PARTICLE_RESOURCE.c_str()));
        ResourceAbstractBrowser::resourceIcon = *resourceIcon;

        // create a new scene for the particle viewport
        particleScene = new dtCore::Scene();
        previewObject = new dtCore::Object();
        particleScene->AddDrawable(previewObject.get());
        camera = new Camera();

        // setup right mouse click context menu
        createActions();
        createContextMenu();

        connect(&EditorEvents::getInstance(),SIGNAL(currentMapChanged()),
            this,SLOT(selectionChanged()));

        QGridLayout *grid = new QGridLayout(this);
        grid->addWidget(listGroup(), 0, 0);
        grid->addWidget(standardButtons(QString("Resource Tools")), 1, 0, Qt::AlignCenter);
    }
    ///////////////////////////////////////////////////////////////////////////////
    ParticleBrowser::~ParticleBrowser(){}
    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *ParticleBrowser::listGroup()
    {
        QGroupBox *group = new QGroupBox(this);

        QGridLayout *grid = new QGridLayout(group);

        grid->addWidget(tree,0,0);

        return group;
    }
    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *ParticleBrowser::previewGroup()
    {
        QGroupBox *groupBox = new QGroupBox(tr("Preview"));

        QGridLayout *grid = new QGridLayout(groupBox);

        // New reference of the viewport manager singleton
        ViewportManager &vpMgr = ViewportManager::getInstance();

        // Create the perspective viewport for the static mesh preview window
        perspView = (PerspectiveViewport *)vpMgr.createViewport("Preview",
            ViewportManager::ViewportType::PERSPECTIVE);

        // Assign the viewport a new scene
        perspView->setScene(particleScene.get());

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
    void ParticleBrowser::createActions()
    {
        ResourceAbstractBrowser::createActions();

        setCreateAction = new QAction(tr("&Create Actor"),getCurrentParent());
        setCreateAction->setCheckable(false);
        connect(setCreateAction, SIGNAL(triggered()),this,SLOT(createActor()));
        setCreateAction->setEnabled(false);
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ParticleBrowser::createContextMenu()
    {
        ResourceAbstractBrowser::createContextMenu();
        contextMenu->addAction(setCreateAction);
    }
    ///////////////////////////////////////////////////////////////////////////////
    // Slots
    ///////////////////////////////////////////////////////////////////////////////
    void ParticleBrowser::displaySelection()
    {
        QString file;
        QString context;

        dtDAL::Project &project = dtDAL::Project::GetInstance();

        // Find the currently selected tree item
        dtDAL::ResourceDescriptor resource = EditorData::getInstance().getCurrentParticleResource();
        context = QString(project.GetContext().c_str());

        file = QString(project.GetResourcePath(resource).c_str());

        if(file != NULL && !resource.GetResourceName().empty())
        {

            // The following is performed to comply with linux and windows file systems
            file = context+"\\"+file;
            file.replace("\\","/");

            dtCore::Object *c = new dtCore::Object();

            c->LoadFile(file.toStdString());
            particleScene->AddDrawable(c);

        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ParticleBrowser::selectionChanged()
    {
        // This is the abstract base classes original functionality
        ResourceAbstractBrowser::selectionChanged();

        // Let's assume that the map could be closed
        setCreateAction->setEnabled(false);

        if(selection != NULL)
        {
            if(selection->isResource())
            {
                if(EditorData::getInstance().getCurrentMap() != NULL)
                {
                    setCreateAction->setEnabled(true);
                }
            }
            else{
                setCreateAction->setEnabled(false);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void ParticleBrowser::createActor()
    {

        EditorData::getInstance().getMainWindow()->startWaitCursor();

        if(selection->isResource())
        {

            LOG_INFO("User Created an Actor - Slot");

            // if we have an actor type, then create the proxy and emit the signal
            /*
            * The following code finds the actor type by a hard-coded string for the
            * category and name. This means that if the actor for Particle System
            * is changed in any way that the following code will break. This was
            * implemented as a quick and dirty solution to assigning particles to an
            * actor of this type.
            */
            osg::ref_ptr<dtDAL::ActorType> particleActor =
                dtDAL::LibraryManager::GetInstance().FindActorType("dtcore","Particle System").get();

            // create our new actor proxy from the mesh actor type that was
            // found by the results of our hard coded search above.
            if(particleActor!=NULL)
            {
                osg::ref_ptr<dtDAL::ActorProxy> proxy =
                        dtDAL::LibraryManager::GetInstance().CreateActorProxy(*particleActor).get();

                // check to make sure both the mesh actor and the proxy are valid.
                // If the user has somehow modified the above hard coded static mesh object
                // the application could potentially be in a dangerous state.
                if (proxy.valid())
                {
                    // grab the actor property type
                    dtDAL::ResourceActorProperty *resourceProp = dynamic_cast<dtDAL::ResourceActorProperty *>
                        (proxy->GetProperty("particle file"));

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
