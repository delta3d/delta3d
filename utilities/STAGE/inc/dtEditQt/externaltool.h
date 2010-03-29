#ifndef externaltool_h__
#define externaltool_h__

#include <QtCore/QObject>
class QAction;
class QProcess;

namespace dtEditQt
{
   class ExternalToolArgParser;

   ///Definition of an external application.
   class ExternalTool : public QObject
   {
      Q_OBJECT

   public:
      ExternalTool();
      ~ExternalTool();

      void SetTitle(const QString& title);
      QString GetTitle() const;

      void SetCmd(const QString& command);
      const QString& GetCmd() const;

      void SetArgs(const QString& args);
      const QString& GetArgs() const;

      void SetWorkingDir(const QString& dir);
      QString GetWorkingDir() const;

      void SetIcon(const QString& iconFilename);
      const QString& GetIcon() const;

      QAction* GetAction() const;

      void SetArgParsers(const QList<const ExternalToolArgParser*>& parsers);

      const QList<const dtEditQt::ExternalToolArgParser*>& GetArgParsers() const;

   protected slots:
      void OnStartTool();

   private:
      QString ExpandArguments(const QString& args) const;
      QAction*  mAction;
      QString   mCommand;
      QString   mArgs;
      QString   mIconFilename;
      QProcess* mProcess;
      QList<const ExternalToolArgParser*> mArgParserContainer;
   };

} // namespace dtEditQt

#endif // externaltool_h__
