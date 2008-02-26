#include "PoseMeshProperties.h"
#include <dtAnim/PoseMesh.h>
#include <QtGui/QTreeWidgetItem>
#include <QtCore/QString>

PoseMeshProperties::PoseMeshProperties()
: QTreeWidget()
{
   QStringList headerLables;
   headerLables << "Property" << "value";

   setHeaderLabels(headerLables);

   connect(this, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)),
           this, SLOT(OnItemDoubleClicked(QTreeWidgetItem*, int)));
}

void PoseMeshProperties::AddMesh(const dtAnim::PoseMesh &newMesh)
{  
   QTreeWidgetItem *meshItem = new QTreeWidgetItem;
   meshItem->setText(0, newMesh.GetName().c_str());

   QTreeWidgetItem *boneNameItem = new QTreeWidgetItem(meshItem);
   QTreeWidgetItem *rootVertItem = new QTreeWidgetItem(meshItem); 

   rootVertItem->setText(0, "Vertices");

   QString boneValue = QString("{ id = %1 }").arg(newMesh.GetBoneID());

   boneNameItem->setText(0, QString::fromStdString(newMesh.GetBoneName()));
   boneNameItem->setText(1, boneValue);

   const dtAnim::PoseMesh::VertexVector &rawVerts = newMesh.GetVertices();

   // Add data values for the vertex items
   for (size_t vertIndex = 0; vertIndex < rawVerts.size(); ++vertIndex)
   {
      int animID = rawVerts[vertIndex]->mAnimID;
      osg::Vec3 rawData = rawVerts[vertIndex]->mData;

      QString dataString = QString("{ anim id = %1 }").arg(animID, 3);
      dataString.append( QString(", { data = [%1, %2] }").arg(rawData.x()).arg(rawData.y()));

      QTreeWidgetItem *vertItem = new QTreeWidgetItem(rootVertItem);
      vertItem->setText(0, QString("%1").arg(vertIndex));
      vertItem->setText(1, dataString);      
   }  
  
   addTopLevelItem(meshItem);     
}

void PoseMeshProperties::OnItemDoubleClicked(QTreeWidgetItem *item, int column)
{
   emit ViewPoseMesh(item->text(0).toStdString());
}