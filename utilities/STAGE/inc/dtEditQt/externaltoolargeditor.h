#ifndef externaltoolargeditor_h__
#define externaltoolargeditor_h__

#include <QtGui/QDialog>
#include "ui_externaltoolargeditor.h"

namespace dtEditQt
{
   class ExternalToolArgParser;

   /** Used to edit a list of arguments which can be passed to an ExternalTool.
    *  Uses the list of supplied ExternalToolArgParsers for predefined variable
    *  information.
    */
   class ExternalToolArgEditor : public QDialog
   {
      Q_OBJECT

   public:
      ExternalToolArgEditor(const QList<const ExternalToolArgParser*>& parsers,
         const QString& args = "",
         QWidget* parent = NULL);

      ~ExternalToolArgEditor();

      ///Get the currently edited list of arguments
      QString GetArgs() const;

   protected slots:
      void OnInsertArgument(QListWidgetItem* item);
      void OnDisplayParserDescription(QListWidgetItem* item);

   private:
      Ui::ExternalToolArgEditor mUI;
   };

} // namespace dtEditQt

#endif // externaltoolargeditor_h__
