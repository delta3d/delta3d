#include "nodebutton.h"

#include "maphierarchyeditor.h"
#include "topbottomconnectornogap.h"
#include "buttonconnector.h"

#include <dtCore/baseactorobject.h>
#include <dtEditQt/editorevents.h>
#include <dtUtil/log.h>

#include <QtGui/QDragEnterEvent>

NodeButton::NodeButton(dtCore::BaseActorObject* bao,
                       QWidget* parent, Qt::WindowFlags f /*= 0*/)
   : QWidget(parent, f)
   , mUI()
   , mTreeParent(NULL)
   , mChildren(0)
   , mRow(-1)
   , mCol(-1)
   , mGridLayout(dynamic_cast<MapHierarchyEditorPlugin*>(parent)->GetGridLayout())
   , mBaseActor(bao)
{
   //apply layout made with QtDesigner
   mUI.setupUi(this);

   if (bao != NULL)
   {
      if (bao->GetName() != "")
      {
         mUI.theButton->setText(QString(bao->GetName().c_str()));
      }
      else
      {
         mUI.theButton->setText(QString(bao->GetClassName().c_str()));
      }
   }
   else
   {
      mUI.theButton->setText("Scene");
   }

   SetLineWidths(0, 0);

   connect(mUI.theButton, SIGNAL(clickedLeft()),
           this, SLOT(OnLeftClickButton()));

   setAcceptDrops(true);
}

NodeButton::~NodeButton()
{
}

void NodeButton::Place(int row, int col)
{   
   mGridLayout->addWidget(this, row, col);

   mRow = row;
   mCol = col;   
}

void NodeButton::AddChild(NodeButton* n)
{   
   if (n == NULL)
   {
      LOG_ERROR("Attempted to add a null child to a NodeButton\n");
      return;
   }

   n->SetTreeParent(this);

   mChildren.push_back(n);

   //now for the GUI-ness
   n->SetTopLineWidth(2);
   if (mChildren.size() == 1)
   {
      //use the topBottomConnectorNoGap widget
      TopBottomConnectorNoGap* tbnc = new TopBottomConnectorNoGap();
      mGridLayout->addWidget(tbnc, mRow + 1, mCol);

      mGridLayout->addWidget(n, mRow + 2, mCol);
      n->SetRow(mRow + 2); n->SetCol(mCol);

      SetBottomLineWidth(2);      
   }
   else if (mChildren.size() == 2)
   {
      //Get rid of the topBottom connector and use a topBottomRight
      DeleteLayoutItemAt(mRow + 1, mCol);
      ButtonConnector* bc = new ButtonConnector(ButtonConnector::TOP_BOTTOM_RIGHT);
      mGridLayout->addWidget(bc, mRow + 1, mCol);

      mGridLayout->addWidget(n, mRow + 2, mCol + 1);
      n->SetRow(mRow + 2); n->SetCol(mCol + 1);
   }
   else //mChildren.size > 2
   {
      //Get rid of the bottomLeft connector and use a bottomLeftRight
      DeleteLayoutItemAt(mRow + 1, mChildren.size() - 2);
      ButtonConnector* bc = new ButtonConnector(ButtonConnector::BOTTOM_LEFT_RIGHT);
      mGridLayout->addWidget(bc, mRow + 1, mChildren.size() - 2);

      mGridLayout->addWidget(n, mRow + 2, mChildren.size() - 1);
      n->SetRow(mRow + 2); n->SetCol(mChildren.size() - 1);
   }

   if (mChildren.size() > 1)
   {
      //rightmost connector is always a BottomLeft
      ButtonConnector* bc = new ButtonConnector(ButtonConnector::BOTTOM_LEFT);
      mGridLayout->addWidget(bc, mRow + 1, mCol + mChildren.size() - 1);
   }
}

void NodeButton::SetTreeParent(NodeButton* n)
{
   if (n == NULL)
   {
      LOG_ERROR("Only root node should have NULL parent, and it wouldn't call this method.");      
   }

   mTreeParent = n;
}

std::string NodeButton::GetActorID() const
{
   return mBaseActor->GetId().ToString();
}

void NodeButton::SetLineWidths(int top, int bottom)
{
   SetTopLineWidth(top);
   SetBottomLineWidth(bottom);
}

void NodeButton::SetTopLineWidth(int w)
{
   SetLineWidth(mUI.topConnector, w);
   SetLineWidth(mUI.topMidConnector, w);
}

void NodeButton::SetBottomLineWidth(int w)
{
   SetLineWidth(mUI.bottomConnector, w);
   SetLineWidth(mUI.bottomMidConnector, w);   
}

void NodeButton::OnLeftClickButton()
{
   if (mBaseActor == NULL)
      return;

   ActorRefPtrVector thisActor;
   thisActor.push_back(mBaseActor);
   dtEditQt::EditorEvents::GetInstance().emitActorsSelected(thisActor);
}

void NodeButton::dragEnterEvent(QDragEnterEvent* myEvent)
{
   if(myEvent->mimeData()->hasFormat("Node"))
   {
      myEvent->acceptProposedAction();
   }
}

void NodeButton::dropEvent(QDropEvent* myEvent)
{   
   if(myEvent->mimeData()->hasFormat("Node") &&
      myEvent->possibleActions() & Qt::MoveAction)
   {
      myEvent->acceptProposedAction();

      QByteArray ba = myEvent->mimeData()->data("Node");
   }
}

void NodeButton::SetLineWidth(QFrame* line, int width)
{
   if (line == NULL)
      return;

   line->setLineWidth(width);
   if (width <= 0)
   {
      line->setFrameShadow(QFrame::Sunken);
   }
   else
   {
      line->setFrameShadow(QFrame::Plain);
   }
}

void NodeButton::DeleteLayoutItemAt(int row, int col)
{
   QLayoutItem* item = mGridLayout->itemAtPosition(row, col);
   if (item != NULL)
   {
      if (item->widget())
      {
         QWidget* w = item->widget();
         mGridLayout->removeWidget(w);
         delete w;         
      }  
   }
}
