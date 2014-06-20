/* -*-c++-*-
 * Delta3D Simulation Training And Game Editor (STAGE)
 * STAGE - This source file (.h & .cpp) - Using 'The MIT License'
 * Copyright (C) 2007-2008, MOVES Institute
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
 * Michael Guerrero
 */
#include <prefix/stageprefix.h>

#include <QtGui/QHBoxLayout>
#include <QtGui/QGridLayout>
#include <QtGui/QSplitter>
#include <QtCore/QString>
#include <QtGui/QGroupBox>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QAction>
#include <QtGui/QMenu>
#include <QtGui/QContextMenuEvent>
#include <QtGui/QIcon>
#include <QtGui/QTextEdit>
#include <QtGui/QScrollBar>

#include <dtEditQt/skeletalmeshbrowser.h>
#include <dtEditQt/resourcetreewidget.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/viewportcontainer.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/stagecamera.h>
#include <dtEditQt/uiresources.h>

#include <dtCore/project.h>
#include <dtCore/librarymanager.h>
#include <dtCore/map.h>
#include <dtCore/resourceactorproperty.h>

#include <dtCore/scene.h>
#include <dtAnim/chardrawable.h>
#include <dtAnim/cal3ddatabase.h>

#include <dtUtil/log.h>
#include <dtUtil/nodeprintout.h>

#include <osg/BoundingSphere>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   SkeletalMeshBrowser::SkeletalMeshBrowser(dtCore::DataType& type, QWidget* parent)
      : ResourceAbstractBrowser(&type, parent)
   {
      // This sets our resource icon that is visible on leaf nodes
      QIcon resourceIcon;
      resourceIcon.addPixmap(QPixmap(UIResources::ICON_STATICMESH_RESOURCE.c_str()));
      ResourceAbstractBrowser::mResourceIcon = resourceIcon;

      // create a new scene for the skeletal mesh viewport
      meshScene = new dtCore::Scene();

      camera = new StageCamera();
      camera->makePerspective(60.0f,1.333f,0.01f,100000.0f);

      QSplitter* splitter = new QSplitter(Qt::Vertical,this);

      splitter->addWidget(previewGroup());
      splitter->addWidget(listGroup());

      splitter->setStretchFactor(0,1);
      splitter->setStretchFactor(1,1);

      // setup the grid layouts
      grid = new QGridLayout(this);
      grid->addWidget(splitter, 0, 0);
      grid->addWidget(standardButtons(QString("Resource Tools")), 1, 0, Qt::AlignCenter);

      createActions();
      createContextMenu();

      connect(&EditorEvents::GetInstance(),SIGNAL(currentMapChanged()),
         this,SLOT(selectionChanged()));

      // This corrects the stretch for the last row
      grid->setRowStretch(1,0);

      camera->makePerspective(60.0f,1.333f,0.01f,100000.0f);
   }

   ///////////////////////////////////////////////////////////////////////////////
   SkeletalMeshBrowser::~SkeletalMeshBrowser() {}

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* SkeletalMeshBrowser::previewGroup()
   {
      QGroupBox* groupBox = new QGroupBox(tr("Preview"));

      QGridLayout* grid = new QGridLayout(groupBox);

      // New reference of the viewport manager singleton
      ViewportManager& vpMgr = ViewportManager::GetInstance();

      // Create the perspective viewport for the static mesh preview window
      perspView = (PerspectiveViewport*)vpMgr.createViewport("Preview",
         ViewportManager::ViewportType::PERSPECTIVE);

      // Assign the viewport a new scene
      perspView->setScene(meshScene.get());

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
   QGroupBox* SkeletalMeshBrowser::listGroup()
   {
      QGroupBox*   groupBox = new QGroupBox(tr("Skeletal Meshes"));
      QGridLayout* grid     = new QGridLayout(groupBox);
      QHBoxLayout* hbox     = new QHBoxLayout();

      // Checkbox for auto preview
      previewChk = new QCheckBox(tr("Auto Preview"), groupBox);
      connect(previewChk,SIGNAL(stateChanged(int)), this, SLOT(checkBoxSelected()));
      previewChk->setChecked(false);

      // Preview button for a selected mesh
      previewBtn = new QPushButton("Preview", groupBox);
      connect(previewBtn, SIGNAL(clicked()), this, SLOT(displaySelection()));
      previewBtn->setDisabled(true);

      hbox->addWidget(previewChk, 0, Qt::AlignLeft);
      hbox->addWidget(previewBtn, 0, Qt::AlignRight);
      grid->addLayout(hbox, 0, 0);
      grid->addWidget(mTree, 1, 0);
      mTree->setResourceName("SkeletalMesh");

      return groupBox;
   }
   ///////////////////////////////////////////////////////////////////////////////
   // Keyboard Event filter
   ///////////////////////////////////////////////////////////////////////////////
   bool SkeletalMeshBrowser::eventFilter(QObject* obj, QEvent* e)
   {
      if (obj == mTree)
      {
         //For some reason, KeyPress is getting defined by something...
         //Without this undef, it will not compile under Linux..
         //It would be great if someone could figure out exactly what's
         //going on.
#undef KeyPress
         if (e->type() == QEvent::KeyPress)
         {
            QKeyEvent* keyEvent = (QKeyEvent *)e;
            switch (keyEvent->key())
            {
            case Qt::Key_Return:
               if (mSelection->isResource())
               {
                  selectionChanged();
                  displaySelection();
               }
               break;
            case Qt::Key_Enter:
               if (mSelection->isResource())
               {
                  selectionChanged();
                  displaySelection();
               }
               break;
            default:
               return mTree->eventFilter(obj, e);
            }
         }
         else
         {
            // pass the event on to the parent class
            return mTree->eventFilter(obj, e);
         }
      }
      return false;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SkeletalMeshBrowser::createActions()
   {
      ResourceAbstractBrowser::createActions();

      setCreateAction = new QAction(tr("&Create Actor"), getCurrentParent());
      setCreateAction->setCheckable(false);
      connect(setCreateAction, SIGNAL(triggered()), this, SLOT(createActor()));
      setCreateAction->setEnabled(false);
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SkeletalMeshBrowser::createContextMenu()
   {
      ResourceAbstractBrowser::createContextMenu();
      mContextMenu->addAction(setCreateAction);
   }

   ///////////////////////////////////////////////////////////////////////////////
   // Slots
   ///////////////////////////////////////////////////////////////////////////////
   void SkeletalMeshBrowser::displaySelection()
   {
      ResourceTreeWidget* selection = currentSelection();
      bool validFile = false;

      if (selection != NULL)
      {
         QString file;
         QString context;

         dtCore::Project& project = dtCore::Project::GetInstance();

         // Find the currently selected tree item
         dtCore::ResourceDescriptor resource = EditorData::GetInstance().getCurrentResource(dtCore::DataType::SKELETAL_MESH);

         try
         {
            file = QString(project.GetResourcePath(resource).c_str());
            validFile = true;
         }
         catch (dtUtil::Exception&)
         {
            validFile = false;
         }

         if (file != NULL && validFile == true)
         {
            file.replace("\\", "/");

            if (previewObject.valid() && meshScene->GetChildIndex(previewObject.get()) == (unsigned)meshScene->GetNumberOfAddedDrawable())
            {
               meshScene->AddChild(previewObject.get());
            }

            dtCore::RefPtr<dtAnim::Cal3DModelWrapper> animWrap = dtAnim::Cal3DDatabase::GetInstance().Load(file.toStdString());

            if (animWrap.valid())
            {
               if (!previewObject.valid())
               {
                  previewObject = new dtAnim::CharDrawable(animWrap);
                  meshScene->AddChild(previewObject.get());
               }
               else
               {
                  previewObject->SetCal3DWrapper(animWrap);
               }
            }
            else
            {
               if (previewObject.valid())
               {
                  previewObject->SetCal3DWrapper(NULL);
               }
            }
            // Load the new file.
            perspView->refresh();

            SetCameraLookAt(*camera, *previewObject);

            perspView->refresh();
            perspView->GetQGLWidget()->setFocus();
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SkeletalMeshBrowser::selectionChanged()
   {
      // This is the abstract base classes original functionality
      ResourceAbstractBrowser::selectionChanged();

      // Let's assume that the map could be closed
      setCreateAction->setEnabled(false);

      // When any item is selected, clear the scene
      if (previewObject.valid())
      {
         meshScene->RemoveChild(previewObject.get());
      }
      perspView->refresh();

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
            perspView->refresh();
            previewBtn->setDisabled(true);
            setCreateAction->setEnabled(false);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SkeletalMeshBrowser::checkBoxSelected()
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
   void SkeletalMeshBrowser::doubleClickEvent()
   {
      if (mSelection->isResource())
      {
         displaySelection();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SkeletalMeshBrowser::deleteItemEvent()
   {
      if (mSelection->isResource())
      {
         // When any item is selected, clear the scene
         meshScene->RemoveChild(previewObject.get());
         perspView->refresh();
         previewBtn->setDisabled(true);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void SkeletalMeshBrowser::createActor()
   {
      EditorData::GetInstance().getMainWindow()->startWaitCursor();

      if (mSelection->isResource())
      {
         LOG_INFO("User Created an Actor - Slot");

         // if we have an actor type, then create the proxy and emit the signal
         /*
         * The following code finds the actor type by a hard-coded string for the
         * category and name. This means that if the actor for Skeletal Mesh Object
         * is changed in any way that the following code will break. This was
         * implemented as a quick and dirty solution to assigning meshes to an
         * actor of this type.
         */
         dtCore::RefPtr<const dtCore::ActorType> meshActor =
            dtCore::LibraryManager::GetInstance().FindActorType("Animation", "Animation");

         // create our new actor proxy from the mesh actor type that was
         // found by the results of our hard coded search above.
         if (meshActor != NULL)
         {
            dtCore::RefPtr<dtCore::BaseActorObject> proxy =
               dtCore::LibraryManager::GetInstance().CreateActor(*meshActor).get();

            // check to make sure both the mesh actor and the proxy are valid.
            // If the user has somehow modified the above hard coded static mesh object
            // the application could potentially be in a dangerous state.
            if (proxy.valid())
            {
               // grab the actor property type
               dtCore::ResourceActorProperty* resourceProp = dynamic_cast<dtCore::ResourceActorProperty*>
                  (proxy->GetProperty("Skeletal Mesh"));

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
