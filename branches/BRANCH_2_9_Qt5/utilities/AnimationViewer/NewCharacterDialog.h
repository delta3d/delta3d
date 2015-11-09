#ifndef __NEW_CHARACTER_DIALOG_H__
#define __NEW_CHARACTER_DIALOG_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVE
////////////////////////////////////////////////////////////////////////////////
#include <QtGui/QDialog>
#include "ui_NewCharacterDialog.h"



////////////////////////////////////////////////////////////////////////////////
// CLASS CODE
////////////////////////////////////////////////////////////////////////////////
class NewCharacterDialog : public QDialog
{
   Q_OBJECT
public:
   typedef QDialog BaseClass;

   static const std::string DEFAULT_CHARACTER_NAME;

   NewCharacterDialog(QWidget* parent = 0);

   virtual ~NewCharacterDialog();

public slots:
   void OnNameChanged(const QString& text);

public:
   Ui::NewCharacterDialog mUI;
};

#endif
