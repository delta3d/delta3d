#ifndef externaltooldialog_h__
#define externaltooldialog_h__

#include <QtGui/QDialog>
#include "ui_externaltooldialog.h"
#include <dtEditQt/externaltool.h>
#include <QtCore/QList>

namespace dtEditQt
{

   class ExternalToolDialog : public QDialog
   {
      Q_OBJECT

   public:
      ExternalToolDialog(QList<ExternalTool*> &tools, QWidget* parent=NULL);
      ~ExternalToolDialog();

   public slots:
      virtual void accept();
      
   protected slots:
      void OnNewTool();   
      void OnRemoveTool();   
      void OnToolSelectionChanged();
      void OnApplyChanges();
      void OnToolModified();
      void OnStringChanged(const QString& text);
      void OnFindCommandFile();
      void OnFindWorkingDir();
      void OnFindIconFile();
      void OnMoveToolDown();
      void OnMoveToolUp();

   private:
      void SetupConnections();
      void PopulateToolsUI();
      ExternalTool* GetSelectedTool() const;
      void SetOkButtonEnabled(bool enabled);
      void SetModifyButtonsEnabled(bool enabled);
      void ResetTool(ExternalTool& tool) const;

      Ui::ExternalToolDiag ui;

      QList<ExternalTool*> *mTools;      
   };
}

#endif // externaltooldialog_h__

