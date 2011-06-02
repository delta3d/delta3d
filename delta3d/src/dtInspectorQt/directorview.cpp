#include <dtInspectorQt/directorview.h>
#include "ui_dtinspectorqt.h"

#include <dtDirectorQt/directoreditor.h>
#include <dtDirectorQt/editornotifier.h>

#include <dtCore/uniqueid.h>

#include <dtDAL/project.h>
#include <dtDAL/map.h>
#include <dtDAL/baseactorobject.h>


//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DirectorView::DirectorView(Ui::InspectorWidget& ui)
:mUI(&ui)
{
   mFilterName = QString("dtDirector::Director");

   connect(mUI->directorScriptViewButton, SIGNAL(clicked()), this, SLOT(OnViewButtonClicked()));
}

//////////////////////////////////////////////////////////////////////////
dtInspectorQt::DirectorView::~DirectorView()
{
   mOperateOn = NULL;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DirectorView::OperateOn(dtCore::Base* b)
{
   dtDirector::DirectorInstance* director =
      dynamic_cast<dtDirector::DirectorInstance*>(b);

   mOperateOn = director;

   Update();
}

//////////////////////////////////////////////////////////////////////////
bool dtInspectorQt::DirectorView::IsOfType(QString name, dtCore::Base* object)
{
   dtDirector::DirectorInstance* director =
      dynamic_cast<dtDirector::DirectorInstance*>(object);
   if (name == mFilterName && director && !director->mDirector->GetParent())
   {
      return true;
   }

   return false;
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DirectorView::OnViewButtonClicked()
{
   dtDirector::DirectorEditor* editor =
      new dtDirector::DirectorEditor();

   if (editor)
   {
      dtCore::RefPtr<dtDirector::EditorNotifier> notifier =
         dynamic_cast<dtDirector::EditorNotifier*>(
         mOperateOn->mDirector->GetNotifier());

      if (!notifier)
      {
         notifier = new dtDirector::EditorNotifier();
      }

      notifier->AddEditor(editor);

      mOperateOn->mDirector->SetNotifier(notifier);

      editor->SetDirector(mOperateOn->mDirector);
      editor->show();
   }
}

//////////////////////////////////////////////////////////////////////////
void dtInspectorQt::DirectorView::Update()
{
   if (mOperateOn.valid())
   {
      mUI->directorScriptGroupBox->show();

      dtCore::UniqueId playerID = mOperateOn->mDirector->GetPlayer();
      dtDAL::Map* map = dtDAL::Project::GetInstance().GetMapForActorProxy(playerID);
      if (map)
      {
         dtDAL::BaseActorObject* player = map->GetProxyById(playerID);
         if (player)
         {
            mUI->directorScriptPlayerEdit->setText(player->GetName().c_str());
         }
      }
   }
   else
   {
      mUI->directorScriptGroupBox->hide();
   }
}

