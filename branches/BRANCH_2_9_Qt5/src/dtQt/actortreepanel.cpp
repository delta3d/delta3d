
////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_actortreepanel.h"
#include <QtCore/QString>
#include <dtQt/actortreepanel.h>



namespace dtQt
{
   /////////////////////////////////////////////////////////////////////////////
   // CLASS CODE
   /////////////////////////////////////////////////////////////////////////////
   ActorTreePanel::ActorTreePanel(QWidget* parent)
      : BaseClass(parent)
      , mUI(new Ui::ActorTreePanel)
   {
      mUI->setupUi(this);

      CreateConnections();

      UpdateUI();
   }

   ActorTreePanel::~ActorTreePanel()
   {
      delete mUI;
      mUI = NULL;
   }

   void ActorTreePanel::CreateConnections()
   {
      // BUTTONS
      connect(mUI->mButtonExpandAll, SIGNAL(clicked()),
         mUI->mTree, SLOT(expandAll()));
      connect(mUI->mButtonCollapseAll, SIGNAL(clicked()),
         mUI->mTree, SLOT(collapseAll()));

      // TREE
      connect(mUI->mTree, SIGNAL(SignalUpdatedUI()),
         this, SLOT(OnTreeWidgetUpdated()));
   }
   
   ActorTreeWidget& ActorTreePanel::GetTreeWidget()
   {
      return *mUI->mTree;
   }

   void ActorTreePanel::OnActorsSelected(ActorRefPtrVector& actors)
   {
      ActorWeakPtrVector actorArray;

      if ( ! actors.empty())
      {
         actorArray.reserve(actors.size());

         ActorRefPtrVector::iterator curIter = actors.begin();
         ActorRefPtrVector::iterator endIter = actors.end();
         for (; curIter != endIter; ++curIter)
         {
            actorArray.push_back(curIter->get());
         }
      }

      mUI->mTree->SetActors(actorArray);
   }

   void ActorTreePanel::UpdateUI()
   {
      mUI->mTree->UpdateUI();

      // NOTE: Other UI updates will be triggered when the tree widget is updated.
   }

   void ActorTreePanel::OnTreeWidgetUpdated()
   {
      QString qstr = QString::number(mUI->mTree->GetActorCount());
      mUI->mActorCount->setText(qstr);
   }

}
