#include "PoseMeshProperties.h"
#include <dtAnim/posemesh.h>
#include <dtAnim/cal3dmodelwrapper.h>
#include <dtUtil/macros.h>
#include <QtGui/QTreeWidgetItem>
#include <QtCore/QString>

const QString STRING_ENABLED("Enabled");
const QString STRING_DISABLED("Disabled");

/////////////////////////////////////////////////////////////////////////////////////////
PoseMeshProperties::PoseMeshProperties()
: QTreeWidget()
{
   QStringList headerLables;
   headerLables << "Property" << "value" << "Blend";

   setHeaderLabels(headerLables);

   connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
           this, SLOT(OnItemDoubleClicked(QTreeWidgetItem*, int)));

   // Catch when a pose mesh checkbox is toggled
   connect(this, SIGNAL(itemChanged(QTreeWidgetItem*, int)),
           this, SLOT(OnItemToggled(QTreeWidgetItem*, int)));

   connect(this, SIGNAL(itemExpanded(QTreeWidgetItem*)),
           this, SLOT(OnItemExpanded(QTreeWidgetItem*)));
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshProperties::AddMesh(const dtAnim::PoseMesh &newMesh, const dtAnim::Cal3DModelWrapper &model)
{  
   // Tree depth 0
   // Create the tree item and set its defaults
   QTreeWidgetItem *meshItem = new QTreeWidgetItem;
   meshItem->setText(0, newMesh.GetName().c_str());
   meshItem->setCheckState(0, Qt::Checked);
   meshItem->setText(1, STRING_ENABLED);

   // Items at tree depth 1
   QTreeWidgetItem *boneNameItem = new QTreeWidgetItem(meshItem);
   QTreeWidgetItem *rootVertItem = new QTreeWidgetItem(meshItem); 

   rootVertItem->setText(0, "Vertices");

   // Get the bone name
   QString boneValue = QString("{ id = %1 }").arg(newMesh.GetBoneID());
   boneNameItem->setText(0, QString("Bone - ") + QString::fromStdString(newMesh.GetBoneName()));
   boneNameItem->setText(1, boneValue);

   const dtAnim::PoseMesh::VertexVector &rawVerts = newMesh.GetVertices();

   // Add data values for the vertex items
   for (size_t vertIndex = 0; vertIndex < rawVerts.size(); ++vertIndex)
   {
      int animID = rawVerts[vertIndex]->mAnimID;
      osg::Vec3 rawData = rawVerts[vertIndex]->mData;

      const std::string &animName = model.GetCoreAnimationName(animID);

      // TODO add animation names
      QString dataString = QString("{ anim id = %1 }").arg(animID, 3);
      dataString.append( QString(", { data = [%1, %2] }").arg(rawData.x()).arg(rawData.y()));

      QTreeWidgetItem *vertItem = new QTreeWidgetItem(rootVertItem);
      vertItem->setData(0, Qt::UserRole, QVariant(animID));
      vertItem->setText(0, QString("%1 - %2").arg(vertIndex).arg(animName.c_str()));
      vertItem->setText(1, dataString);    

      // Store verts for later updating
      mVertProperties.push_back(vertItem);
   }  

   // Make sure we can read everything in the first column
   resizeColumnToContents(0); 
  
   addTopLevelItem(meshItem);     
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshProperties::OnItemDoubleClicked(QTreeWidgetItem *item, int column)
{
   UNREFERENCED_PARAMETER(column);
   emit ViewPoseMesh(item->text(0).toStdString());
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshProperties::OnItemToggled(QTreeWidgetItem *item, int column)
{
   if (column == 0)
   {
      QString valueString = item->text(1);      

      // If this was a pose mesh checkbox, one of these should be true
      if (valueString == STRING_ENABLED)
      {         
         item->setText(1, STRING_DISABLED);
         emit PoseMeshStatusChanged(item->text(0).toStdString(), false);
      }
      else if (valueString == STRING_DISABLED)
      {
         item->setText(1, STRING_ENABLED);
         emit PoseMeshStatusChanged(item->text(0).toStdString(), true);
      }      
   }
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshProperties::OnItemExpanded(QTreeWidgetItem *item)
{
   UNREFERENCED_PARAMETER(item);

   // Make sure we can read everything in the columns
   resizeColumnToContents(0);
   resizeColumnToContents(1);
}

/////////////////////////////////////////////////////////////////////////////////////////
void PoseMeshProperties::OnBlendUpdate(const std::vector<float> &weightList)
{
   for (size_t vertIndex = 0; vertIndex < mVertProperties.size(); ++vertIndex)
   {
      unsigned int animID = mVertProperties[vertIndex]->data(0, Qt::UserRole).toUInt();
      assert(animID < weightList.size());

      mVertProperties[vertIndex]->setText(2, QString("%%1").arg(weightList[animID]));
   }
}
