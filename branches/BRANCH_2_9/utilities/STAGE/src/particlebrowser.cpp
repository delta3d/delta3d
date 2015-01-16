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
 * Teague Coonan
 */

// had to add this header to make it compile in linux
// not sure why.
#include <prefix/stageprefix.h>
#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtCore/QString>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QGroupBox>
#include <QtGui/QCheckBox>
#include <QtGui/QSplitter>
#include <QtGui/QPushButton>

#include <dtCore/scene.h>
#include <dtCore/object.h>

#include <dtCore/resourceactorproperty.h>
#include <dtCore/librarymanager.h>
#include <dtCore/map.h>

#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/particlebrowser.h>
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/resourcetreewidget.h>
#include <dtEditQt/viewportcontainer.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/stagecamera.h>
#include <dtEditQt/uiresources.h>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   ParticleBrowser::ParticleBrowser(dtCore::DataType& type, QWidget* parent)
      : ResourceAbstractBrowser(&type, parent)
   {
      // This sets our resource icon that is visible on leaf nodes
      QIcon resourceIcon;
      resourceIcon.addPixmap(QPixmap(UIResources::ICON_PARTICLE_RESOURCE.c_str()));
      ResourceAbstractBrowser::mResourceIcon = resourceIcon;

      // create a new scene for the particle viewport
      particleScene = new dtCore::Scene();
      previewObject = new dtCore::Object();
      particleScene->AddChild(previewObject.get());
      camera = new StageCamera();
      camera->makePerspective(60.0f,1.333f,0.01f,100000.0f);

      // setup right mouse click context menu
      createActions();
      createContextMenu();

      connect(&EditorEvents::GetInstance(), SIGNAL(currentMapChanged()),
         this, SLOT(selectionChanged()));

      QSplitter* splitter = new QSplitter(Qt::Vertical, this);

      splitter->addWidget(previewGroup());
      splitter->addWidget(listGroup());

      splitter->setStretchFactor(0, 1);
      splitter->setStretchFactor(1, 1);

      QGridLayout* grid = new QGridLayout(this);
      grid->addWidget(splitter, 0, 0);
      grid->addWidget(standardButtons(QString("Resource Tools")), 1, 0, Qt::AlignCenter);

      //camera->setViewport(0, 0, perspView->width(), perspView->height()); //TODO E!
   }

   ///////////////////////////////////////////////////////////////////////////////
   ParticleBrowser::~ParticleBrowser() {}

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* ParticleBrowser::listGroup()
   {
      QGroupBox* groupBox = new QGroupBox(this);
      // Checkbox for auto preview
      previewChk = new QCheckBox(tr("Auto Preview"), groupBox);
      connect(previewChk, SIGNAL(stateChanged(int)), this, SLOT(checkBoxSelected()));
      previewChk->setChecked(false);

      // Preview button for a selected mesh
      previewBtn = new QPushButton("Preview", groupBox);
      connect(previewBtn, SIGNAL(clicked()), this, SLOT(displaySelection()));
      previewBtn->setDisabled(true);

      QGridLayout* grid = new QGridLayout(groupBox);
      QHBoxLayout* hbox = new QHBoxLayout();

      hbox->addWidget(previewChk, 0, Qt::AlignLeft);
      hbox->addWidget(previewBtn, 0, Qt::AlignRight);

      grid->addLayout(hbox, 0, 0);
      grid->addWidget(mTree, 1, 0);

      mTree->setResourceName("Particle");

      return groupBox;
   }

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* ParticleBrowser::previewGroup()
   {
      QGroupBox* groupBox = new QGroupBox(tr("Preview"));

      QGridLayout* grid = new QGridLayout(groupBox);

      // New reference of the viewport manager singleton
      ViewportManager& vpMgr = ViewportManager::GetInstance();

      // Create the perspective viewport for the static mesh preview window
      perspView = (PerspectiveViewport*)vpMgr.createViewport("Preview",
         ViewportManager::ViewportType::PERSPECTIVE);

      // Assign the viewport a new scene
      perspView->setScene(particleScene.get());

      // By default, perspective viewports have their camera set to the world view
      // camera.  The world view camera is what is used in the main perspective view.
      //perspView->setCamera(camera.get()); //E!

      // Disable the interaction modes
      perspView->setAutoInteractionMode(false);
      perspView->setAutoSceneUpdate(false);
      perspView->setEnableKeyBindings(false);

      // Create a viewport container for our static mesh window
      container = new ViewportContainer(perspView, groupBox);
      container->setViewport(perspView);

      grid->addWidget(container, 0, 0);
      return groupBox;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ParticleBrowser::createActions()
   {
      ResourceAbstractBrowser::createActions();

      setCreateAction = new QAction(tr("&Create Actor"), getCurrentParent());
      setCreateAction->setCheckable(false);
      connect(setCreateAction, SIGNAL(triggered()), this, SLOT(createActor()));
      setCreateAction->setEnabled(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ParticleBrowser::createContextMenu()
   {
      ResourceAbstractBrowser::createContextMenu();
      mContextMenu->addAction(setCreateAction);
   }

   ///////////////////////////////////////////////////////////////////////////////
   // Slots
   ///////////////////////////////////////////////////////////////////////////////
   void ParticleBrowser::displaySelection()
   {
      QString file;
      QString context;

      dtCore::Project& project = dtCore::Project::GetInstance();

      // Find the currently selected tree item
      dtCore::ResourceDescriptor resource = EditorData::GetInstance().getCurrentResource(dtCore::DataType::PARTICLE_SYSTEM);

      file = QString(project.GetResourcePath(resource).c_str());

      if (file != NULL && !resource.GetResourceName().empty())
      {
         // The following is performed to comply with linux and windows file systems
         file.replace("\\", "/");

         dtCore::RefPtr<dtCore::Object> c = new dtCore::Object();

         c->LoadFile(file.toStdString());
         particleScene->RemoveAllDrawables();
         particleScene->AddChild(c.get());
         perspView->GetQGLWidget()->setFocus();
         SetCameraLookAt(*camera, *c);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ParticleBrowser::checkBoxSelected()
   {
      if (previewChk->isChecked())
      {
         if (mSelection->isResource())
         {
            // preview current item
            selectionChanged();
            displaySelection();
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ParticleBrowser::selectionChanged()
   {
      // This is the abstract base classes original functionality
      ResourceAbstractBrowser::selectionChanged();

      // Let's assume that the map could be closed
      setCreateAction->setEnabled(false);

      if (mSelection != NULL)
      {
         if (mSelection->isResource())
         {
            // auto preview
            if (previewChk->isChecked())
            {
               displaySelection();
            }
            previewBtn->setDisabled(false);
            if (EditorData::GetInstance().getCurrentMap() != NULL)
            {
               setCreateAction->setEnabled(true);
            }
         }
         else
         {
            setCreateAction->setEnabled(false);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void ParticleBrowser::createActor()
   {
      EditorData::GetInstance().getMainWindow()->startWaitCursor();

      if (mSelection->isResource())
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
         dtCore::RefPtr<const dtCore::ActorType> particleActor =
            dtCore::LibraryManager::GetInstance().FindActorType("dtcore", "Particle System");

         // create our new actor proxy from the mesh actor type that was
         // found by the results of our hard coded search above.
         if (particleActor != NULL)
         {
            dtCore::RefPtr<dtCore::BaseActorObject> proxy =
               dtCore::LibraryManager::GetInstance().CreateActor(*particleActor).get();

            // check to make sure both the mesh actor and the proxy are valid.
            // If the user has somehow modified the above hard coded static mesh object
            // the application could potentially be in a dangerous state.
            if (proxy.valid())
            {
               // grab the actor property type
               dtCore::ResourceActorProperty* resourceProp = dynamic_cast<dtCore::ResourceActorProperty*>
                  (proxy->GetProperty("particle file"));

               if (resourceProp != NULL)
               {
                  resourceProp->SetValue(mSelection->getResourceDescriptor());
               }

               // add the new proxy to the map
               dtCore::RefPtr<dtCore::Map> mapPtr = EditorData::GetInstance().getCurrentMap();
               if (mapPtr.valid())
               {
                  mapPtr->AddProxy(*proxy, true);
               }

               // Let the world know that a new proxy exists
               EditorEvents::GetInstance().emitBeginChangeTransaction();
               EditorEvents::GetInstance().emitActorProxyCreated(proxy, false);
               ViewportManager::GetInstance().placeProxyInFrontOfCamera(proxy.get());
               EditorEvents::GetInstance().emitEndChangeTransaction();

               // Now, let the world that it should select the new actor proxy.
               std::vector< dtCore::RefPtr<dtCore::BaseActorObject> > actors;

               actors.push_back(proxy);
               EditorEvents::GetInstance().emitActorsSelected(actors);
            }
         }
         EditorData::GetInstance().getMainWindow()->endWaitCursor();
      }
   }

} // namespace dtEditQt
