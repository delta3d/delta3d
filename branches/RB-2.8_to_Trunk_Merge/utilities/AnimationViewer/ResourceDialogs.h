
#ifndef __RESOURCE_DIALOGS_H__
#define __RESOURCE_DIALOGS_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_ResourceAddDialog.h"
#include <QtGui/QDialog>
#include <dtCore/observerptr.h>
#include <dtAnim/basemodelwrapper.h>



/////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
/////////////////////////////////////////////////////////////////////////////////
class QIcon;

namespace Ui
{
   class ResourceAddDialog;
}



/////////////////////////////////////////////////////////////////////////////////
// MODEL RESOURCE FILTERS CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
class ModelResourceFilters
{
public:
   static const QString FILTER_OSG;
   static const QString FILTER_ANY;
   static const QString FILTER_ALL_NOT_SKEL;
   static const QString FILTER_SKEL;
   static const QString FILTER_ANIM;
   static const QString FILTER_MESH;
   static const QString FILTER_MAT;
   static const QString FILTER_MORPH;

   static const QString& GetFilterForFileType(int fileType);

private:
   ModelResourceFilters();
   ModelResourceFilters(const ModelResourceFilters&);
};



/////////////////////////////////////////////////////////////////////////////////
// MODEL RESOURCE ICONS CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
class ModelResourceIcons
{
public:
   static const QString ICON_NONE;
   static const QString ICON_MIXED;
   static const QString ICON_SKEL;
   static const QString ICON_ANIM;
   static const QString ICON_MESH;
   static const QString ICON_MAT;
   static const QString ICON_MORPH;

   static const QIcon GetIconForFileType(int fileType);

private:
   ModelResourceIcons();
   ModelResourceIcons(const ModelResourceIcons&);
};



/////////////////////////////////////////////////////////////////////////////////
// RESOURCE ADD DIALOG CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
class ResAddDialog : public QDialog
{
   Q_OBJECT
public:
   typedef QDialog BaseClass;

   ResAddDialog(QWidget* parent = 0);

   void SetModelWrapper(dtAnim::BaseModelWrapper* wrapper);

   bool IsDataChanged() const;

signals:
   void SignalChangedData();

public slots:
   void OnChangedObjectName(const QString& text);
   void OnClickedFile();

protected:
   virtual void accept();
   void UpdateUI();

   void SetupResourceTypeList();

private:
   bool mDataChanged;
   Ui::ResourceAddDialog mUI;
   dtCore::ObserverPtr<dtAnim::BaseModelWrapper> mWrapper;
};

#endif
