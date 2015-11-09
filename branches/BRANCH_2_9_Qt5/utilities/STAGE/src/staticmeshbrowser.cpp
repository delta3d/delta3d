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
#include <QtGui/QMessageBox>

#include <dtEditQt/staticmeshbrowser.h>
#include <dtEditQt/resourcetreewidget.h>
#include <dtEditQt/editordata.h>
#include <dtEditQt/editoractions.h>
#include <dtEditQt/editorevents.h>
#include <dtEditQt/mainwindow.h>
#include <dtEditQt/perspectiveviewport.h>
#include <dtEditQt/viewportcontainer.h>
#include <dtEditQt/viewportmanager.h>
#include <dtEditQt/stagecamera.h>
#include <dtEditQt/uiresources.h>

#include <dtCore/project.h>
#include <dtCore/actorfactory.h>
#include <dtCore/map.h>
#include <dtCore/resourceactorproperty.h>
#include <dtGame/actorcomponent.h>
#include <dtGame/actorcomponentcontainer.h>

#include <dtCore/scene.h>
#include <dtCore/object.h>

#include <dtUtil/log.h>
#include <dtUtil/nodeprintout.h>

#include <osg/BoundingSphere>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   StaticMeshBrowser::StaticMeshBrowser(dtCore::DataType& type, QWidget* parent, bool allowPreview, bool allowActorCreate)
      : ResourceAbstractBrowser(&type, parent)
      , container(nullptr)
      , perspView(nullptr)
      , meshScene(nullptr)
      , camera(nullptr)
      , previewObject(nullptr)
      , previewChk(nullptr)
      , previewBtn(nullptr)
      , setCreateAction(nullptr)
      , setSGPreviewAction(nullptr)
      , setOSGDump(nullptr)
      , mAllowPreview(allowPreview)
      , mAllowCreateActor(allowActorCreate)
   {
      // This sets our resource icon that is visible on leaf nodes
      QIcon resourceIcon;
      resourceIcon.addPixmap(QPixmap(UIResources::ICON_STATICMESH_RESOURCE.c_str()));
      ResourceAbstractBrowser::mResourceIcon = resourceIcon;

      QWidget* listWidget = listGroup(allowPreview);
      // setup the grid layouts
      grid = new QGridLayout(this);
      if (allowPreview)
      {
         // create a new scene for the static mesh viewport
         meshScene = new dtCore::Scene();

         QSplitter* splitter = new QSplitter(Qt::Vertical, this);

         splitter->addWidget(previewGroup());
         splitter->addWidget(listWidget);

         splitter->setStretchFactor(0,1);
         splitter->setStretchFactor(1,1);

         grid->addWidget(splitter, 0, 0);
      }
      else
      {
         grid->addWidget(listWidget);
      }
      grid->addWidget(standardButtons(QString("Resource Tools")), 1, 0, Qt::AlignCenter);

      createActions();
      createContextMenu();

      connect(&EditorEvents::GetInstance(), SIGNAL(currentMapChanged()),
         this, SLOT(selectionChanged()));

      // This corrects the stretch for the last row
      grid->setRowStretch(1, 0);

      //camera->setViewport(0, 0, perspView->width(), perspView->height()); //TODO E!
   }

   ///////////////////////////////////////////////////////////////////////////////
   StaticMeshBrowser::~StaticMeshBrowser() {}

   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox* StaticMeshBrowser::previewGroup()
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
      camera = perspView->getCamera();
      camera->makePerspective(60.0f,1.333f,0.01f,100000.0f);

      //By default, perspective viewports have their camera set to the world view
      //camera.  The world view camera is what is used in the main perspective view.
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
   QGroupBox* StaticMeshBrowser::listGroup(bool allowPreview)
   {
      QGroupBox*   groupBox = new QGroupBox(tr(mResourceType->GetDisplayName().c_str()));
      QGridLayout* grid     = new QGridLayout(groupBox);
      QHBoxLayout* hbox     = new QHBoxLayout();

      if (allowPreview)
      {
         // Checkbox for auto preview
         previewChk = new QCheckBox(tr("Auto Preview"), groupBox);
         connect(previewChk, SIGNAL(stateChanged(int)), this, SLOT(checkBoxSelected()));
         previewChk->setChecked(false);

         // Preview button for a selected mesh
         previewBtn = new QPushButton("Preview", groupBox);
         connect(previewBtn, SIGNAL(clicked()), this, SLOT(displaySelection()));
         previewBtn->setDisabled(true);
         hbox->addWidget(previewChk, 0, Qt::AlignLeft);
         hbox->addWidget(previewBtn, 0, Qt::AlignRight);
         grid->addLayout(hbox, 0, 0);
      }

      grid->addWidget(mTree, 1, 0);
      mTree->setResourceName(mResourceType->GetDisplayName());


      return groupBox;
   }

   ///////////////////////////////////////////////////////////////////////////////
   // Keyboard Event filter
   ///////////////////////////////////////////////////////////////////////////////
   bool StaticMeshBrowser::eventFilter(QObject* obj, QEvent* e)
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
            QKeyEvent* keyEvent = (QKeyEvent*)e;
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
   void StaticMeshBrowser::createActions()
   {
      ResourceAbstractBrowser::createActions();

      if (mAllowCreateActor)
      {
         setCreateAction = new QAction(tr("&Create Actor"),getCurrentParent());
         setCreateAction->setCheckable(false);
         connect(setCreateAction, SIGNAL(triggered()),this,SLOT(createActor()));
         setCreateAction->setEnabled(false);
      }

      if (mAllowPreview)
      {
         // Allow the preview of the scene graph for an ive file
         setSGPreviewAction = new QAction(tr("Preview Scene Graph"), getCurrentParent());
         //setSGPreviewAction->setCheckable(false);
         connect(setSGPreviewAction, SIGNAL(triggered()),this,SLOT(viewSceneGraph()));
         //setSGPreviewAction->setEnabled(false);

         setOSGDump = new QAction(tr("Preview OSG File"), getCurrentParent());
         connect(setOSGDump, SIGNAL(triggered()), this, SLOT(viewOSGContents()));
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StaticMeshBrowser::createContextMenu()
   {
      ResourceAbstractBrowser::createContextMenu();
      if (setCreateAction != nullptr) mContextMenu->addAction(setCreateAction);
      if (setSGPreviewAction != nullptr) mContextMenu->addAction(setSGPreviewAction);
      if (setOSGDump != nullptr) mContextMenu->addAction(setOSGDump);
   }

   ///////////////////////////////////////////////////////////////////////////////
   // Slots
   ///////////////////////////////////////////////////////////////////////////////
   void StaticMeshBrowser::displaySelection()
   {
      ResourceTreeWidget* selection = currentSelection();

      if (selection != NULL)
      {
         // Find the currently selected tree item
         dtCore::ResourceDescriptor resource = EditorData::GetInstance().getCurrentResource(*mResourceType);

         if (meshScene != nullptr && !resource.IsEmpty())
         {
            if (previewObject != nullptr && previewObject->GetDrawable() != nullptr)
            {
               meshScene->RemoveChild(previewObject->GetDrawable());
            }

            previewObject = CreateActor(resource);

            if (previewObject != nullptr && previewObject->GetDrawable<dtCore::Transformable>() != NULL)
            {
               meshScene->AddChild(previewObject->GetDrawable());

               perspView->refresh();

               SetCameraLookAt(*camera, *previewObject->GetDrawable<dtCore::Transformable>());

               perspView->refresh();
               perspView->GetQGLWidget()->setFocus();
            }
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StaticMeshBrowser::selectionChanged()
   {
      // This is the abstract base classes original functionality
      ResourceAbstractBrowser::selectionChanged();

      // Let's assume that the map could be closed
      if (setCreateAction != nullptr) setCreateAction->setEnabled(false);

      if (meshScene != nullptr)
      {
         // When any item is selected, clear the scene
         if (previewObject != nullptr && previewObject->GetDrawable() != nullptr)
            meshScene->RemoveChild(previewObject->GetDrawable());
         perspView->refresh();
      }

      if (mSelection != NULL)
      {
         if (mSelection->isResource())
         {
            if (meshScene != nullptr)
            {
               // auto preview
               if (previewChk->isChecked())
               {
                  displaySelection();
               }
               previewBtn->setDisabled(false);
            }

            if (EditorData::GetInstance().getCurrentMap() != NULL)
            {
               if (setCreateAction != nullptr)
                  setCreateAction->setEnabled(true);
            }
         }
         else
         {
            if (perspView != nullptr)
               perspView->refresh();
            if (previewBtn != nullptr)
               previewBtn->setDisabled(true);
            if (setCreateAction != nullptr)
               setCreateAction->setEnabled(false);
         }
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StaticMeshBrowser::checkBoxSelected()
   {
      if (mAllowPreview && previewChk->isChecked())
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
   void StaticMeshBrowser::doubleClickEvent()
   {
      if (mAllowPreview && mSelection->isResource())
      {
         displaySelection();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StaticMeshBrowser::deleteItemEvent()
   {
      if (mSelection->isResource() && meshScene != nullptr)
      {
         // When any item is selected, clear the scene
         if (previewObject != nullptr && previewObject->GetDrawable() != nullptr)
            meshScene->RemoveChild(previewObject->GetDrawable());
         perspView->refresh();
         previewBtn->setDisabled(true);
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   dtCore::RefPtr<dtCore::BaseActorObject> StaticMeshBrowser::CreateActor(const dtCore::ResourceDescriptor& rd)
   {
      dtCore::RefPtr<dtCore::BaseActorObject> result;

      /**
       * This nonsense needs to be replaced with simply a Game Actor that has a specific resource added.
       */
      dtCore::RefPtr<const dtCore::ActorType> meshActor;
      std::string propertyName;
      dtCore::ResourceActorProperty* resourceProp = NULL;
      if (*mResourceType == dtCore::DataType::PARTICLE_SYSTEM)
      {
         meshActor = dtCore::ActorFactory::GetInstance().FindActorType("dtcore", "Particle System");
         propertyName = "Particle(s) File";
      }
      else if (*mResourceType == dtCore::DataType::SKELETAL_MESH)
      {
         dtCore::ActorFactory::GetInstance().LoadActorRegistry("dtAnim");
         meshActor = dtCore::ActorFactory::GetInstance().FindActorType("dtanim", "AnimationGameActor");
         propertyName = "Skeletal Mesh";
      }
      else
      {
         meshActor = dtCore::ActorFactory::GetInstance().FindActorType("dtcore","Static Mesh");
         propertyName = "static mesh";
      }


      if (meshActor.valid())
      {
         result = dtCore::ActorFactory::GetInstance().CreateActor(*meshActor).get();
         if (result.valid())
         {

            result->GetProperty(propertyName, resourceProp);
            if (resourceProp == nullptr)
            {
               dtGame::ActorComponentVector comps;
               dtGame::ActorComponentContainer* acc = dynamic_cast<dtGame::ActorComponentContainer*>(result.get());
               if (acc)
               {
                  acc->GetAllComponents(comps);
               }

               for (unsigned i = 0; i < comps.size() && resourceProp == nullptr; ++i)
               {
                  comps[i]->GetProperty(propertyName, resourceProp);
               }
            }

            // check to make sure both the mesh actor and the proxy are valid.
            // If the user has somehow modified the above hard coded static mesh object
            // the application could potentially be in a dangerous state.
            if (resourceProp != nullptr)
            {
               resourceProp->SetValue(rd);
            }
         }
      }
      return result;
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StaticMeshBrowser::createActor()
   {
      EditorData::GetInstance().getMainWindow()->startWaitCursor();

      if (mSelection->isResource())
      {
         LOG_INFO("User Created an Actor - Slot");

         dtCore::RefPtr<dtCore::BaseActorObject> newActor = CreateActor(mSelection->getResourceDescriptor());

         if (newActor.valid())
         {
            // add the new proxy to the map
            dtCore::RefPtr<dtCore::Map> mapPtr = EditorData::GetInstance().getCurrentMap();
            if (mapPtr.valid())
            {
               EditorActions::GetInstance().AddActorToMap(*newActor, *mapPtr, true);
            }

            // Let the world know that a new newActor exists
            EditorEvents::GetInstance().emitBeginChangeTransaction();
            EditorEvents::GetInstance().emitActorProxyCreated(newActor, false);
            ViewportManager::GetInstance().placeProxyInFrontOfCamera(newActor.get());
            EditorEvents::GetInstance().emitEndChangeTransaction();

            // Now, let the world that it should select the new actor newActor.
            std::vector< dtCore::RefPtr<dtCore::BaseActorObject> > actors;

            actors.push_back(newActor);
            EditorEvents::GetInstance().emitActorsSelected(actors);
         }
         EditorData::GetInstance().getMainWindow()->endWaitCursor();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StaticMeshBrowser::viewSceneGraph()
   {
      QString resourceName;
      // Make sure we have a valid resource
      if (mSelection->isResource())
      {
         QDialog dlg(this);
         dlg.setModal(true);
         dlg.setWindowTitle(tr("Node Hierarchy"));
         dlg.setMinimumSize(400, 400);
         dlg.setSizeGripEnabled(true);

         QVBoxLayout* vLayout = new QVBoxLayout(&dlg);
         QTextEdit*   text    = new QTextEdit(&dlg);
         QPushButton* close   = new QPushButton(tr("Close"), &dlg);

         dtCore::ResourceDescriptor& rd = mSelection->getResourceDescriptor();
         const std::string fileName = dtCore::Project::GetInstance().GetResourcePath(rd);

         dtCore::RefPtr<dtCore::Object> obj = new dtCore::Object;
         osg::Node* node = obj->LoadFile(fileName);

         // If the file was successfully loaded, continue
         if (node != NULL)
         {
            dtCore::RefPtr<dtUtil::NodePrintOut> nodepo = new dtUtil::NodePrintOut;

            text->addScrollBarWidget(new QScrollBar(this), Qt::AlignRight);
            text->setText( tr(nodepo->CollectNodeData(*node).c_str()) );

            obj = NULL;
            nodepo = NULL;

            vLayout->addWidget(text);
            vLayout->addWidget(close);

            connect(close, SIGNAL(clicked()), &dlg, SLOT(close()));
            dlg.exec();
         }
      }
   }

   /////////////////////////////////////////////////////////////////////////////////
   void StaticMeshBrowser::viewOSGContents()
   {
      try
      {
         QString resourceName;
         // Make sure we have a valid resource
         if (mSelection->isResource())
         {
            QDialog dlg(this);
            dlg.setModal(true);
            dlg.setWindowTitle(tr("OSG Hierarchy"));
            dlg.setMinimumSize(400, 400);
            dlg.setSizeGripEnabled(true);

            QVBoxLayout* vLayout = new QVBoxLayout(&dlg);
            QTextEdit*   text    = new QTextEdit(&dlg);
            QPushButton* close   = new QPushButton(tr("Close"), &dlg);

            text->addScrollBarWidget(new QScrollBar(this), Qt::AlignRight);

            dtCore::ResourceDescriptor& rd = mSelection->getResourceDescriptor();
            const std::string fileName = dtCore::Project::GetInstance().GetResourcePath(rd);

            dtCore::RefPtr<dtCore::Object> obj = new dtCore::Object;
            osg::Node* node = obj->LoadFile(fileName);

            // If the file was successfully loaded, continue
            if (node)
            {
               std::ostringstream oss;
               dtCore::RefPtr<dtUtil::NodePrintOut> nodepo = new dtUtil::NodePrintOut;
               nodepo->PrintNodeToOSGFile(*node, oss);

               std::string osgOutput = oss.str();
               text->setText(tr(oss.str().c_str()));

               obj = NULL;
               nodepo = NULL;

               vLayout->addWidget(text);
               vLayout->addWidget(close);

               connect(close, SIGNAL(clicked()), &dlg, SLOT(close()));
               dlg.exec();
            }
         }
      }
      catch (const dtUtil::Exception& ex)
      {
         ex.LogException(dtUtil::Log::LOG_ERROR);
         QMessageBox::critical(this, tr("Error"), tr("Error creating osg text file: ") + ex.ToString().c_str());
      }
      catch(...)
      {
         LOG_ERROR("Unknown exception caught while trying to create osg text buffer.");
         QMessageBox::critical(this, tr("Error"), tr("Unknown exception thrown creating osg text file."));
      }
   }

} // namespace dtEditQt
