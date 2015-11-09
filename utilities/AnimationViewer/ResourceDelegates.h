
#ifndef __RESOURCE_DELEGATES_H__
#define __RESOURCE_DELEGATES_H__

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <QtGui/QItemDelegate>
#include <QtGui/QWidget>
#include <dtCore/observerptr.h>
#include <dtAnim/basemodeldata.h>
#include <dtAnim/basemodelwrapper.h>
#include <osg/Texture2D>



////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
class QPushButton;

namespace osg
{
   class Texture2D;
}



/////////////////////////////////////////////////////////////////////////////////
// FILE BUTTONS EDITOR CODE
/////////////////////////////////////////////////////////////////////////////////
class FileButtonsEditor : public QWidget
{
   Q_OBJECT
public:
   typedef QWidget BaseClass;

   FileButtonsEditor(QWidget* parent = 0);

signals:
   void SignalClickedFile();
   void SignalClickedRemove();

public slots:
   void OnClickedFile();
   void OnClickedRemove();

public:
   QPushButton* mButtonFile;
   QPushButton* mButtonRemove;
};



////////////////////////////////////////////////////////////////////////////////
// FILE ITEM DELEGATE CODE
////////////////////////////////////////////////////////////////////////////////
class FileItemDelegate : public QItemDelegate
{
   Q_OBJECT
public:
   typedef QItemDelegate BaseClass;

   FileItemDelegate(QObject* parent = 0);

   void Reset();

   void SetCharModelData(dtAnim::BaseModelData* data);
   dtAnim::BaseModelData* GetCharModelData();
   const dtAnim::BaseModelData* GetCharModelData() const;

   void SetCharModelWrapper(dtAnim::BaseModelWrapper* wrapper);
   dtAnim::BaseModelWrapper* GetCharModelWrapper();
   const dtAnim::BaseModelWrapper* GetCharModelWrapper() const;

   bool IsDataValid(const QString& data) const;

   bool IsFileRemovalAllowed(const std::string& file) const;

   QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
      const QModelIndex& index) const;

   void setEditorData(QWidget* editor, const QModelIndex& index) const;

   void setModelData(QWidget* editor, QAbstractItemModel* model,
      const QModelIndex& index) const;

   void updateEditorGeometry(QWidget* editor,
      const QStyleOptionViewItem& option, const QModelIndex& index) const;

   bool ApplyData(const QString& data) const;

   bool AskUser(const std::string& promptTitle, const std::string& question) const;

signals:
   void SignalResourceEditStart(int fileType, const std::string& objectName) const;
   void SignalResourceEditEnd(int fileType, const std::string& objectName) const;
   void SignalResourceRemoved(int fileType, const std::string& objectName) const;

public slots:
   void OnOpenFile() const;
   void OnRemoveFile() const;

private:
   bool ReplaceFile(dtAnim::BaseModelData& modelData, dtAnim::BaseModelWrapper& modelWrapper,
      const std::string& objectName, const std::string& oldFile, const std::string& newFile) const;

   void LoadMaterial(dtAnim::BaseModelWrapper& model, const std::string& materialName) const;

   mutable int mFileType;
   mutable QPushButton* mButton;
   mutable std::string mObjectName;
   mutable QString mNewValue;
   mutable QString mPrevValue;
   mutable dtCore::ObserverPtr<dtAnim::BaseModelData> mCharData;
   mutable dtCore::ObserverPtr<dtAnim::BaseModelWrapper> mCharWrapper;

   typedef std::map<std::string, dtCore::RefPtr<osg::Texture2D> > TextureMap;
   mutable TextureMap mTextures;
};



////////////////////////////////////////////////////////////////////////////////
// OBJECT NAME ITEM DELEGATE CODE
////////////////////////////////////////////////////////////////////////////////
class ObjectNameItemDelegate : public QItemDelegate
{
   Q_OBJECT
public:
   typedef QItemDelegate BaseClass;

   ObjectNameItemDelegate(QObject* parent = 0);

   void Reset();

   void SetCharModelData(dtAnim::BaseModelData* data);
   dtAnim::BaseModelData* GetCharModelData();
   const dtAnim::BaseModelData* GetCharModelData() const;

   void SetCharModelWrapper(dtAnim::BaseModelWrapper* wrapper);
   dtAnim::BaseModelWrapper* GetCharModelWrapper();
   const dtAnim::BaseModelWrapper* GetCharModelWrapper() const;

   bool IsDataValid(const QString& data) const;

   QWidget* createEditor(QWidget* parent, const QStyleOptionViewItem& option,
      const QModelIndex& index) const;

   void setEditorData(QWidget* editor, const QModelIndex& index) const;

   void setModelData(QWidget* editor, QAbstractItemModel* model,
      const QModelIndex& index) const;

   void updateEditorGeometry(QWidget* editor,
      const QStyleOptionViewItem& option, const QModelIndex& index) const;

   bool ApplyData(const QString& data) const;

signals:
   void SignalResourceNameChanged(int fileType, const std::string& oldName, const std::string& newName) const;

private:
   void RenameSkeleton(const std::string& oldName, const std::string& newName) const;
   void RenameAnimations(const std::string& oldName, const std::string& newName) const;
   void RenameMeshes(const std::string& oldName, const std::string& newName) const;
   void RenameMaterials(const std::string& oldName, const std::string& newName) const;
   void RenameMorphs(const std::string& oldName, const std::string& newName) const;

   mutable int mFileType;
   mutable QString mPrevValue;
   mutable dtCore::ObserverPtr<dtAnim::BaseModelData> mCharData;
   mutable dtCore::ObserverPtr<dtAnim::BaseModelWrapper> mCharWrapper;
};

#endif
