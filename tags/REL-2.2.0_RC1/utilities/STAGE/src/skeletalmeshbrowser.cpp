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
#include <prefix/dtstageprefix-src.h>
#include <QtCore/QDir>
#include <QtGui/QHeaderView>

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
#include <dtEditQt/camera.h>
#include <dtEditQt/uiresources.h>

#include <dtDAL/project.h>
#include <dtDAL/exceptionenum.h>
#include <dtDAL/librarymanager.h>
#include <dtDAL/map.h>
#include <dtDAL/enginepropertytypes.h>

#include <dtCore/scene.h>
#include <dtCore/object.h>

#include <dtUtil/log.h>
#include <dtUtil/nodeprintout.h>

#include <osg/BoundingSphere>

namespace dtEditQt
{

   ///////////////////////////////////////////////////////////////////////////////
   SkeletalMeshBrowser::SkeletalMeshBrowser(dtDAL::DataType &type,QWidget *parent)
      : ResourceAbstractBrowser(&type,parent)
   {

      // This sets our resource icon that is visible on leaf nodes
      QIcon resourceIcon;
      resourceIcon.addPixmap(QPixmap(UIResources::ICON_STATICMESH_RESOURCE.c_str()));
      ResourceAbstractBrowser::resourceIcon = resourceIcon;

      // create a new scene for the skeletal mesh viewport
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

      connect(&EditorEvents::GetInstance(),SIGNAL(currentMapChanged()),
         this,SLOT(selectionChanged()));

      // This corrects the stretch for the last row
      grid->setRowStretch(1,0);
   }
   ///////////////////////////////////////////////////////////////////////////////
   SkeletalMeshBrowser::~SkeletalMeshBrowser(){}
   ///////////////////////////////////////////////////////////////////////////////
   QGroupBox *SkeletalMeshBrowser::previewGroup()
   {
      QGroupBox *groupBox = new QGroupBox(tr("Preview"));

      QGridLayout *grid = new QGridLayout(groupBox);

      // New reference of the viewport manager singleton
      ViewportManager &vpMgr = ViewportManager::GetInstance();

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
   QGroupBox *SkeletalMeshBrowser::listGroup()
   {
      QGroupBox *groupBox = new QGroupBox(tr("Skeletal Meshes"));
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
   bool SkeletalMeshBrowser::eventFilter(QObject *obj, QEvent *e)
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
   void SkeletalMeshBrowser::createActions()
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

      setOSGDump = new QAction(tr("Preview File"), getCurrentParent());
      connect(setOSGDump, SIGNAL(triggered()), this, SLOT(viewOSGContents()));
   }
   ///////////////////////////////////////////////////////////////////////////////
   void SkeletalMeshBrowser::createContextMenu()
   {
      ResourceAbstractBrowser::createContextMenu();
      contextMenu->addAction(setCreateAction);
      contextMenu->addAction(setSGPreviewAction);
      contextMenu->addAction(setOSGDump);
   }
   ///////////////////////////////////////////////////////////////////////////////
   // Slots
   ///////////////////////////////////////////////////////////////////////////////
   void SkeletalMeshBrowser::displaySelection()
   {
      ResourceTreeWidget *selection = currentSelection();
      bool validFile = false;

      if(selection != NULL)
      {
         QString file;
         QString context;

         dtDAL::Project &project = dtDAL::Project::GetInstance();

         // Find the currently selected tree item
         dtDAL::ResourceDescriptor resource = EditorData::GetInstance().getCurrentMeshResource();

         try
         {
            file = QString(project.GetResourcePath(resource).c_str());
            validFile = true;
         }
         catch (dtUtil::Exception &)
         {
            validFile = false;
         }

         if(file != NULL && validFile==true)
         {
            context = QString(project.GetContext().c_str());
            // The following is performed to comply with linux and windows file systems
            file = context+"\\"+file;
            file.replace("\\","/");

            if(meshScene->GetDrawableIndex(previewObject.get())==(unsigned)meshScene->GetNumberOfAddedDrawable())
            {
               meshScene->AddDrawable(previewObject.get());
            }

            //Load the new file.
            previewObject->LoadFile(file.toStdString());
            previewObject->RecenterGeometryUponLoad();
            perspView->refresh();

            SetCameraLookAt(*camera, *previewObject);

            perspView->refresh();
            perspView->setFocus();
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

            if(EditorData::GetInstance().getCurrentMap() != NULL)
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
   void SkeletalMeshBrowser::doubleClickEvent()
   {
      if(selection->isResource())
      {
         displaySelection();
      }
   }
   ///////////////////////////////////////////////////////////////////////////////
   void SkeletalMeshBrowser::deleteItemEvent()
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
   void SkeletalMeshBrowser::createActor()
   {
      EditorData::GetInstance().getMainWindow()->startWaitCursor();

      if(selection->isResource())
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
         dtCore::RefPtr<const dtDAL::ActorType> meshActor =
            dtDAL::LibraryManager::GetInstance().FindActorType("Animation","Animation");

         // create our new actor proxy from the mesh actor type that was
         // found by the results of our hard coded search above.
         if (meshActor != NULL)
         {
            dtCore::RefPtr<dtDAL::ActorProxy> proxy =
               dtDAL::LibraryManager::GetInstance().CreateActorProxy(*meshActor).get();

            // check to make sure both the mesh actor and the proxy are valid.
            // If the user has somehow modified the above hard coded static mesh object
            // the application could potentially be in a dangerous state.
            if (proxy.valid())
            {
               // grab the actor property type
               dtDAL::ResourceActorProperty *resourceProp = dynamic_cast<dtDAL::ResourceActorProperty *>
                  (proxy->GetProperty("Skeletal Mesh"));

               if (resourceProp != NULL)
               {
                  resourceProp->SetValue(&selection->getResourceDescriptor());
               }

               // add the new proxy to the map
               dtCore::RefPtr<dtDAL::Map> mapPtr = EditorData::GetInstance().getCurrentMap();
               if (mapPtr.valid())
               {
                  mapPtr->AddProxy(*proxy);
               }

               // Let the world know that a new proxy exists
               EditorEvents::GetInstance().emitBeginChangeTransaction();
               EditorEvents::GetInstance().emitActorProxyCreated(proxy, false);
               ViewportManager::GetInstance().placeProxyInFrontOfCamera(proxy.get());
               EditorEvents::GetInstance().emitEndChangeTransaction();

               // Now, let the world that it should select the new actor proxy.
               std::vector<dtCore::RefPtr<dtDAL::ActorProxy> > actors;

               actors.push_back(proxy);
               EditorEvents::GetInstance().emitActorsSelected(actors);
            }
         }
         EditorData::GetInstance().getMainWindow()->endWaitCursor();
      }
   }
   ///////////////////////////////////////////////////////////////////////////////
   void SkeletalMeshBrowser::viewSceneGraph()
   {
      QString resourceName;
      // Make sure we have a valid resource
      if(selection->isResource())
      {
         QDialog dlg(this);
         dlg.setModal(true);
         dlg.setWindowTitle(tr("Node Hierarchy"));
         dlg.setMinimumSize(400, 400);
         dlg.setSizeGripEnabled(true);

         QVBoxLayout *vLayout = new QVBoxLayout(&dlg);
         QTextEdit *text = new QTextEdit(&dlg);
         QPushButton *close = new QPushButton(tr("Close"), &dlg);

         dtDAL::ResourceDescriptor &rd = selection->getResourceDescriptor();
         const std::string fileName = dtDAL::Project::GetInstance().GetResourcePath(rd);

         dtCore::RefPtr<dtCore::Object> obj = new dtCore::Object;
         osg::Node *node = obj->LoadFile(fileName);

         // If the file was successfully loaded, continue
         if (node)
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
   void SkeletalMeshBrowser::viewOSGContents()
   {
      QString resourceName;
      // Make sure we have a valid resource
      if(selection->isResource())
      {
         QDialog dlg(this);
         dlg.setModal(true);
         dlg.setWindowTitle(tr("OSG Hierarchy"));
         dlg.setMinimumSize(400, 400);
         dlg.setSizeGripEnabled(true);

         QVBoxLayout *vLayout = new QVBoxLayout(&dlg);
         QTextEdit *text = new QTextEdit(&dlg);
         QPushButton *close = new QPushButton(tr("Close"), &dlg);

         text->addScrollBarWidget(new QScrollBar(this), Qt::AlignRight);

         dtDAL::ResourceDescriptor &rd = selection->getResourceDescriptor();
         const std::string fileName = dtDAL::Project::GetInstance().GetResourcePath(rd);

         dtCore::RefPtr<dtCore::Object> obj = new dtCore::Object;
         osg::Node *node = obj->LoadFile(fileName);

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
}
