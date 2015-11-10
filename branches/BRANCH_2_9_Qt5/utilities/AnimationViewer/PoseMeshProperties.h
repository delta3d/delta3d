#ifndef __POSEMESH_PROPERTIES_H__
#define __POSEMESH_PROPERTIES_H__

#include <QtWidgets/QTreeWidget>

///////////////////////////////////////////////////////////////////////////////

namespace dtAnim 
{
   class BaseModelWrapper;
   class PoseMesh; 
}

class PoseMeshItem;

///////////////////////////////////////////////////////////////////////////////

class PoseMeshProperties: public QTreeWidget
{   
   Q_OBJECT
public:  
 
   PoseMeshProperties();
   virtual ~PoseMeshProperties(){}

   void AddMesh(const dtAnim::PoseMesh &newMesh, const dtAnim::BaseModelWrapper &model);

public slots:
   void OnItemDoubleClicked(QTreeWidgetItem* item, int column);
   void OnItemToggled(QTreeWidgetItem* item, int column);
   void OnItemExpanded(QTreeWidgetItem* item);
   void OnBlendUpdate(const std::vector<std::pair<float, float> >& weightTimeList);
   void OnItemAdded(const PoseMeshItem* meshItem);
   void OnNewItemBlend(const dtAnim::PoseMesh* posemesh, float itemAzimuth, float itemElevation);

signals:
   void ViewPoseMesh(const std::string& meshName);
   void PoseMeshStatusChanged(const std::string& meshName, bool isEnabled);

private:  

   typedef std::map<const dtAnim::PoseMesh*, QTreeWidgetItem*> StatePropertyMap;

   std::vector<QTreeWidgetItem*> mVertProperties;   
   StatePropertyMap mStatePropertyMap;
};

///////////////////////////////////////////////////////////////////////////////

#endif // _POSEMESH_PROPERTIES_H_
