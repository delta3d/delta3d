#ifndef _POSEMESH_PROPERTIES_H_
#define _POSEMESH_PROPERTIES_H_

#include <QtGui/QTreeWidget>

namespace dtAnim 
{
   class PoseMesh; 
   class Cal3DModelWrapper;
}

class PoseMeshProperties: public QTreeWidget
{   
   Q_OBJECT
public:  
 
   PoseMeshProperties();
   virtual ~PoseMeshProperties(){}

   void AddMesh(const dtAnim::PoseMesh &newMesh, const dtAnim::Cal3DModelWrapper &model);

public slots:
   void OnItemDoubleClicked(QTreeWidgetItem *item, int column);
   void OnItemToggled(QTreeWidgetItem *item, int column);
   void OnItemExpanded(QTreeWidgetItem *item);
   void OnBlendUpdate(const std::vector<float> &weightList);

signals:
   void ViewPoseMesh(const std::string &meshName);
   void PoseMeshStatusChanged(const std::string &meshName, bool isEnabled);

private:

   std::vector<QTreeWidgetItem*> mVertProperties;   
};

#endif // _POSEMESH_PROPERTIES_H_
