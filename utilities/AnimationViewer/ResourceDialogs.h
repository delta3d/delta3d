
#ifndef DELTA_RESOURCE_DIALOGS
#define DELTA_RESOURCE_DIALOGS

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include "ui_ResourceAddDialog.h"
#include <QtGui/QDialog>
#include <dtCore/observerptr.h>
#include <dtAnim/cal3dmodeldata.h>



/////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
/////////////////////////////////////////////////////////////////////////////////
class QIcon;

namespace Ui
{
   class ResourceAddDialog;
}



/////////////////////////////////////////////////////////////////////////////////
// CAL3D RESOURCE FILTERS CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
class Cal3DResourceFilters
{
public:
   static const QString FILTER_ANY;
   static const QString FILTER_ALL_NOT_SKEL;
   static const QString FILTER_SKEL;
   static const QString FILTER_ANIM;
   static const QString FILTER_MESH;
   static const QString FILTER_MAT;
   static const QString FILTER_MORPH;

   static const QString& GetFilterForFileType(int fileType);

private:
   Cal3DResourceFilters();
   Cal3DResourceFilters(const Cal3DResourceFilters&);
};



/////////////////////////////////////////////////////////////////////////////////
// CAL3D RESOURCE ICONS CLASS CODE
/////////////////////////////////////////////////////////////////////////////////
class Cal3DResourceIcons
{
public:
   static const QString ICON_NONE;
   static const QString ICON_SKEL;
   static const QString ICON_ANIM;
   static const QString ICON_MESH;
   static const QString ICON_MAT;
   static const QString ICON_MORPH;

   static const QIcon GetIconForFileType(int fileType);

private:
   Cal3DResourceIcons();
   Cal3DResourceIcons(const Cal3DResourceIcons&);
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

   void SetModelData(dtAnim::Cal3DModelData* modelData);

   bool IsDataChanged() const;

signals:
   void SignalChangedData();

public slots:
   void OnChangedObjectName(const QString& text);
   void OnClickedFile();

protected:
   virtual void accept();
   void UpdateUI();

private:
   bool mDataChanged;
   Ui::ResourceAddDialog mUI;
   dtCore::ObserverPtr<dtAnim::Cal3DModelData> mModelData;
};

#endif
