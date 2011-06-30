
#ifndef DELTA_CHARACTER_RESOURCE_MODEL
#define DELTA_CHARACTER_RESOURCE_MODEL

////////////////////////////////////////////////////////////////////////////////
// INCLUDE DIRECTIVES
////////////////////////////////////////////////////////////////////////////////
#include <QtGui/QItemDelegate>
#include <QtGui/QWidget>
#include <dtCore/observerptr.h>
#include <dtAnim/cal3dmodeldata.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <osg/Texture2D>


////////////////////////////////////////////////////////////////////////////////
// FORWARD DECLARATIONS
////////////////////////////////////////////////////////////////////////////////
class QPushButton;

namespace osg
{
   class Texture2D;
}



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

   void SetCharModelData(dtAnim::Cal3DModelData* data);
   dtAnim::Cal3DModelData* GetCharModelData();
   const dtAnim::Cal3DModelData* GetCharModelData() const;

   void SetCharModelWrapper(dtAnim::Cal3DModelWrapper* wrapper);
   dtAnim::Cal3DModelWrapper* GetCharModelWrapper();
   const dtAnim::Cal3DModelWrapper* GetCharModelWrapper() const;

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
   void SignalResourceEditStart(int fileType, const std::string& objectName) const;
   void SignalResourceEditEnd(int fileType, const std::string& objectName) const;

public slots:
   void OnOpenFile() const;

private:
   bool ReplaceFile(dtAnim::Cal3DModelData& modelData, dtAnim::Cal3DModelWrapper& modelWrapper,
      const std::string& objectName, const std::string& oldFile, const std::string& newFile) const;

   void LoadMaterial(dtAnim::Cal3DModelData& modelData, int materialId) const;

   mutable int mFileType;
   mutable QPushButton* mButton;
   mutable std::string mObjectName;
   mutable QString mNewValue;
   mutable QString mPrevValue;
   mutable dtCore::ObserverPtr<dtAnim::Cal3DModelData> mCharData;
   mutable dtCore::ObserverPtr<dtAnim::Cal3DModelWrapper> mCharWrapper;

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

   void SetCharModelData(dtAnim::Cal3DModelData* data);
   dtAnim::Cal3DModelData* GetCharModelData();
   const dtAnim::Cal3DModelData* GetCharModelData() const;

   void SetCharModelWrapper(dtAnim::Cal3DModelWrapper* wrapper);
   dtAnim::Cal3DModelWrapper* GetCharModelWrapper();
   const dtAnim::Cal3DModelWrapper* GetCharModelWrapper() const;

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
   mutable dtCore::ObserverPtr<dtAnim::Cal3DModelData> mCharData;
   mutable dtCore::ObserverPtr<dtAnim::Cal3DModelWrapper> mCharWrapper;
};

#endif
