#include <dtEditQt/externaltool.h>
#include <dtEditQt/externaltoolargparser.h>
#include <QtGui/QAction>
#include <QtGui/QMessageBox>
#include <QtCore/QProcess>
#include <iostream>

//////////////////////////////////////////////////////////////////////////
dtEditQt::ExternalTool::ExternalTool()
   : mAction(new QAction(NULL))
   , mCommand(QString())
   , mArgs(QString())
   , mIconFilename(QString())
   , mProcess(new QProcess())
{
   mAction->setText("defaultTitle");
   connect(mAction, SIGNAL(triggered()), this, SLOT(OnStartTool()));
}

//////////////////////////////////////////////////////////////////////////
dtEditQt::ExternalTool::~ExternalTool()
{
   if (mProcess->state() == QProcess::Running)
   {
      mProcess->terminate();

      if (mProcess->waitForFinished() == false)
      {
         mProcess->kill(); //really?
      }
   }

   mArgParserContainer.clear();
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

   // Automatically surround the command with quotes, for those OS's that have
   // spaces in the path.
   const QString withQuotes = QString("\"%1\"").arg(mCommand);

   mProcess->setProcessChannelMode(QProcess::ForwardedChannels);

   mProcess->start(withQuotes + " " + ExpandArguments(mArgs)); //tack on any arguments

   if (mProcess->waitForStarted() == false)
   {
      QMessageBox::warning(NULL, tr("External Tool"),
                         tr("Failed to start tool.  Does file exist?\n") + mCommand);
   }
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


//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalTool::SetArgs(const QString& args)
{
   mArgs = args;
}

//////////////////////////////////////////////////////////////////////////
const QString& dtEditQt::ExternalTool::GetArgs() const
{
   return mArgs;
}

//////////////////////////////////////////////////////////////////////////
QString dtEditQt::ExternalTool::GetWorkingDir() const
{
   return mProcess->workingDirectory();
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalTool::SetWorkingDir(const QString& dir)
{
   mProcess->setWorkingDirectory(dir);
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalTool::SetIcon(const QString& iconFilename)
{
   mIconFilename = iconFilename;
   GetAction()->setIcon(QIcon(mIconFilename));
}

//////////////////////////////////////////////////////////////////////////
const QString& dtEditQt::ExternalTool::GetIcon() const
{
   return mIconFilename;
}

//////////////////////////////////////////////////////////////////////////
QString dtEditQt::ExternalTool::ExpandArguments(const QString& args) const
{
   QString expandedArgs(args);

   QListIterator<const ExternalToolArgParser*> i(mArgParserContainer);
   while (i.hasNext())
   {
      expandedArgs = i.next()->ExpandArguments(expandedArgs);
   }

   return expandedArgs;
}

//////////////////////////////////////////////////////////////////////////
void dtEditQt::ExternalTool::SetArgParsers(const QList<const ExternalToolArgParser*>& parsers)
{
   mArgParserContainer = parsers;
}

//////////////////////////////////////////////////////////////////////////
const QList<const dtEditQt::ExternalToolArgParser*>& dtEditQt::ExternalTool::GetArgParsers() const
{
   return mArgParserContainer;
}
