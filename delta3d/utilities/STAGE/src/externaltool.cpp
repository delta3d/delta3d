#include <dtEditQt/externaltool.h>
#include <QtGui/QAction>
#include <QtCore/QProcess>
#include <iostream>

//////////////////////////////////////////////////////////////////////////
dtEditQt::ExternalTool::ExternalTool()
:
 mAction(new QAction(NULL))
 , mCommand(QString())
 , mProcess(new QProcess())
{
   mAction->setText("defaultTitle");
   connect(mAction, SIGNAL(triggered()), this, SLOT(OnStartTool()));
}

//////////////////////////////////////////////////////////////////////////
dtEditQt::ExternalTool::~ExternalTool()
{
   std::cout << "externalTool destructed:" << GetTitle().toStdString() << std::endl;

   if (mProcess->state() == QProcess::Running)
   {
      mProcess->terminate();
      
      if (mProcess->waitForFinished() == false)
      {
         mProcess->kill(); //really?
      }
   }
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalTool::SetTitle(const QString& title)
{
   mAction->setText(title);
}

//////////////////////////////////////////////////////////////////////////
QString dtEditQt::ExternalTool::GetTitle() const
{
   return mAction->text();
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalTool::OnStartTool()
{
   if (mCommand.isEmpty())
   {
      std::cout << "No command for: " << GetTitle().toStdString() << std::endl;
      return;
   }

   if (mProcess->state() == QProcess::Running)
   {
      std::cout << GetTitle().toStdString() << " already running." << std::endl;
      return;
   }

   mProcess->start(mCommand);

   if (mProcess->waitForStarted() == false)
   {
      std::cout << "failed starting: "<< GetTitle().toStdString() << " " << mCommand.toStdString() << std::endl;
   }
   else
      std::cout << GetTitle().toStdString() << " started" << std::endl;

}

//////////////////////////////////////////////////////////////////////////
QAction* dtEditQt::ExternalTool::GetAction() const
{
   return mAction;
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalTool::SetCmd(const QString& command)
{
   mCommand = command;
}

//////////////////////////////////////////////////////////////////////////
const QString& dtEditQt::ExternalTool::GetCmd() const
{
   return mCommand;
}
