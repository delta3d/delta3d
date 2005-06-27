/*
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2005, BMH Associates, Inc.
*
* This library is free software; you can redistribute it and/or modify it under
* the terms of the GNU Lesser General Public License as published by the Free
* Software Foundation; either version 2.1 of the License, or (at your option)
* any later version.
*
* This library is distributed in the hope that it will be useful, but WITHOUT
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
* FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
* details.
*
* You should have received a copy of the GNU Lesser General Public License
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
#include <QContextMenuEvent>

#include "dtEditQt/resourcetreewidget.h"
#include "dtDAL/project.h"
#include "dtEditQt/editordata.h"
#include "dtEditQt/editorevents.h"
#include "dtEditQt/mainwindow.h"

#include <dtCore/scene.h>
#include <dtCore/object.h>

#include <osg/BoundingSphere>

#include "dtEditQt/perspectiveviewport.h"
#include "dtEditQt/viewportcontainer.h"
#include "dtEditQt/viewportmanager.h"
#include "dtEditQt/camera.h"

#include "dtEditQt/characterbrowser.h"

namespace dtEditQt
{

    CharacterBrowser::CharacterBrowser(dtDAL::DataType &type,QWidget *parent)
        : ResourceAbstractBrowser(&type,parent)
    {
        // create a new scene for the character viewport
        characterScene = new dtCore::Scene();
        previewObject = new dtCore::Object();
        characterScene->AddDrawable(previewObject.get());
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

        // This corrects the stretch for the last row
        grid->setRowStretch(1,0);
    }
    ///////////////////////////////////////////////////////////////////////////////
    CharacterBrowser::~CharacterBrowser(){}
    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *CharacterBrowser::previewGroup()
    {
        QGroupBox *groupBox = new QGroupBox(tr("Preview"));

        QGridLayout *grid = new QGridLayout(groupBox);

        // New reference of the viewport manager singleton
        ViewportManager &vpMgr = ViewportManager::getInstance();

        // Create the perspective viewport for the character preview window
        perspView = (PerspectiveViewport *)vpMgr.createViewport("Preview",
            ViewportManager::ViewportType::PERSPECTIVE);

        // Assign the viewport a new scene
        perspView->setScene(characterScene.get());

        //By default, perspective viewports have their camera set to the world view
        //camera.  The world view camera is what is used in the main perspective view.
        perspView->setCamera(camera.get());

        //No need for an overlay for this viewport since we cannot select characters
        //in the preview window.
        perspView->setOverlay(NULL);

        // Disable the interaction modes
        perspView->setAutoInteractionMode(false);
        perspView->setAutoSceneUpdate(false);
        perspView->setEnableKeyBindings(false);

        // Enable camera mode for this view
        perspView->setCameraMode();

        // Create a viewport container for our character window
        container = new ViewportContainer(perspView,groupBox);
        container->setViewport(perspView);

        grid->addWidget(container,0,0);
        return groupBox;
    }
    ///////////////////////////////////////////////////////////////////////////////
    QGroupBox *CharacterBrowser::listGroup()
    {
        QGroupBox *groupBox = new QGroupBox(tr("Characters"));
        QGridLayout *grid = new QGridLayout(groupBox);
        QHBoxLayout *hbox = new QHBoxLayout();

        // Checkbox for auto preview
        previewChk = new QCheckBox(tr("Auto Preview"),groupBox);
        connect(previewChk,SIGNAL(stateChanged(int)),this,SLOT(checkBoxSelected()));
        previewChk->setChecked(false);

        // Preview button for a selected character
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
    bool CharacterBrowser::eventFilter(QObject *obj, QEvent *e)
    {
        if (obj == tree)
        {
            //For some reason, KeyPress is getting defined by something...
            //Without this undef, it will not compile under Linux..
            //It would be great if someone could figure out exactly what's
            //going on.
#undef KeyPress
            if (e->type() == QEvent::KeyPress)
            {
                QKeyEvent *keyEvent = (QKeyEvent *)e;
                switch(keyEvent->key())
                {
                case Qt::Key_Return :
                    if(selection->isResource())
                    {
                        selectionChanged();
                        displaySelection();
                    }
                    break;
                case Qt::Key_Enter:
                    if(selection->isResource())
                    {
                        selectionChanged();
                        displaySelection();
                    }
                    break;
                default:
                    return tree->eventFilter(obj,e);
                }
            }
            else
            {
                // pass the event on to the parent class
                return tree->eventFilter(obj, e);
            }
        }
        return false;
    }
    ///////////////////////////////////////////////////////////////////////////////
    // Slots
    ///////////////////////////////////////////////////////////////////////////////
    void CharacterBrowser::displaySelection()
    {
        {
            QString file;
            QString context;
            bool validFile = false;

            EditorData::getInstance().getMainWindow()->startWaitCursor();

            dtDAL::Project &project = dtDAL::Project::GetInstance();

            // Find the currently selected tree item
            dtDAL::ResourceDescriptor resource = EditorData::getInstance().getCurrentCharacterResource();

            try
            {
                file = QString(project.GetResourcePath(resource).c_str());
                validFile = true;
            }
            catch (dtDAL::Exception &e)
            {
                validFile = false;
            }

            if(validFile == true)
            {
                context = QString(project.GetContext().c_str());

                file = context+"\\"+file;
                file.replace("\\",QString(dtDAL::FileUtils::PATH_SEPARATOR));

                if(characterScene->GetDrawableIndex(previewObject.get())==(unsigned)characterScene->GetNumberOfAddedDrawable())
                {
                    characterScene->AddDrawable(previewObject.get());
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
                {
                    camera->setPosition(viewDir*offset*-2.0f);
                }
                else
                {
                    camera->setPosition(bs.center());
                }

                perspView->refresh();
            }

            EditorData::getInstance().getMainWindow()->endWaitCursor();
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void CharacterBrowser::selectionChanged()
    {
        // This is the abstract base classes original functionality
        ResourceAbstractBrowser::selectionChanged();

        // When any item is selected, clear the scene
        characterScene->RemoveDrawable(previewObject.get());
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
                //context sensitive menu items
                previewBtn->setDisabled(false);
            }
            else
            {
                perspView->refresh();
                previewBtn->setDisabled(true);
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////
    void CharacterBrowser::checkBoxSelected()
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
    void CharacterBrowser::doubleClickEvent()
    {
        if(selection->isResource())
        {
            displaySelection();
        }
    }
}
