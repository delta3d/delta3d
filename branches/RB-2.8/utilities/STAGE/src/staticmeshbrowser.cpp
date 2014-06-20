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
#include <dtCore/object.h>

#include <dtUtil/log.h>
#include <dtUtil/nodeprintout.h>

#include <osg/BoundingSphere>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   StaticMeshBrowser::StaticMeshBrowser(dtCore::DataType& type, QWidget* parent)
      : ResourceAbstractBrowser(&type, parent)
   {
      // This sets our resource icon that is visible on leaf nodes
      QIcon resourceIcon;
      resourceIcon.addPixmap(QPixmap(UIResources::ICON_STATICMESH_RESOURCE.c_str()));
      ResourceAbstractBrowser::mResourceIcon = resourceIcon;

      // create a new scene for the static mesh viewport
      meshScene = new dtCore::Scene();
      previewObject = new dtCore::Object();
      meshScene->AddChild(previewObject.get());
      camera = new StageCamera();
      camera->makePerspective(60.0f,1.333f,0.01f,100000.0f);

      QSplitter* splitter = new QSplitter(Qt::Vertical, this);

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
   QGroupBox* StaticMeshBrowser::listGroup()
   {
      QGroupBox*   groupBox = new QGroupBox(tr("Static Meshes"));
      QGridLayout* grid     = new QGridLayout(groupBox);
      QHBoxLayout* hbox     = new QHBoxLayout();

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
      grid->addWidget(mTree, 1, 0);
      mTree->setResourceName("StaticMesh");


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

      setCreateAction = new QAction(tr("&Create Actor"),getCurrentParent());
      setCreateAction->setCheckable(false);
      connect(setCreateAction, SIGNAL(triggered()),this,SLOT(createActor()));
      setCreateAction->setEnabled(false);

      // Allow the preview of the scene graph for an ive file
      setSGPreviewAction = new QAction(tr("Preview Scene Graph"), getCurrentParent());
      //setSGPreviewAction->setCheckable(false);
      connect(setSGPreviewAction, SIGNAL(triggered()),this,SLOT(viewSceneGraph()));
      //setSGPreviewAction->setEnabled(false);

      setOSGDump = new QAction(tr("Preview OSG File"), getCurrentParent());
      connect(setOSGDump, SIGNAL(triggered()), this, SLOT(viewOSGContents()));
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StaticMeshBrowser::createContextMenu()
   {
      ResourceAbstractBrowser::createContextMenu();
      mContextMenu->addAction(setCreateAction);
      mContextMenu->addAction(setSGPreviewAction);
      mContextMenu->addAction(setOSGDump);
   }

   ///////////////////////////////////////////////////////////////////////////////
   // Slots
   ///////////////////////////////////////////////////////////////////////////////
   void StaticMeshBrowser::displaySelection()
   {
      ResourceTreeWidget* selection = currentSelection();
      bool validFile = false;

      if (selection != NULL)
      {
         QString file;
         QString context;

         dtCore::Project& project = dtCore::Project::GetInstance();

         // Find the currently selected tree item
         dtCore::ResourceDescriptor resource = EditorData::GetInstance().getCurrentResource(dtCore::DataType::STATIC_MESH);

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
            // The following is performed to comply with linux and windows file systems
            file.replace("\\","/");

            if (meshScene->GetChildIndex(previewObject.get()) == (unsigned)meshScene->GetNumberOfAddedDrawable())
            {
               meshScene->AddChild(previewObject.get());
            }

            // Load the new file.
            previewObject->LoadFile(file.toStdString());
            previewObject->RecenterGeometryUponLoad();
            perspView->refresh();

            SetCameraLookAt(*camera, *previewObject);

            perspView->refresh();
            perspView->GetQGLWidget()->setFocus();
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
      meshScene->RemoveChild(previewObject.get());
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
   void StaticMeshBrowser::checkBoxSelected()
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
   void StaticMeshBrowser::doubleClickEvent()
   {
      if (mSelection->isResource())
      {
         displaySelection();
      }
   }

   ///////////////////////////////////////////////////////////////////////////////
   void StaticMeshBrowser::deleteItemEvent()
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
   void StaticMeshBrowser::createActor()
   {
      EditorData::GetInstance().getMainWindow()->startWaitCursor();

      if (mSelection->isResource())
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
         dtCore::RefPtr<const dtCore::ActorType> meshActor =
            dtCore::LibraryManager::GetInstance().FindActorType("dtcore","Static Mesh");

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
               dtCore::ResourceActorProperty* resourceProp = dynamic_cast<dtCore::ResourceActorProperty *>
                  (proxy->GetProperty("static mesh"));

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
