#ifndef STAGE_MAP_HIERARCHY_NODE_BUTTON_H__
#define STAGE_MAP_HIERARCHY_NODE_BUTTON_H__

#include "export.h"
#include "ui_nodebutton.h"

#include <QtGui/QWidget>

namespace dtCore
{
   class BaseActorObject;
}

/**
 *  Nodes of the Map scene tree editor are edited via QPushButtons via this class
 */
class STAGE_MAP_HIERARCHY_EDITOR_EXPORT NodeButton
   : public QWidget
{
   Q_OBJECT
public:

   NodeButton(dtCore::BaseActorObject* bao,
              QWidget* parent, Qt::WindowFlags f = 0);
   ~NodeButton();

   ///Don't use this for anything except the root node
   //TODO: enforce that
   void Place(int row, int col);

   ///This is how node should get added to the QGridLayout (except root)
   void AddChild(NodeButton* n);

   void SetTreeParent(NodeButton* n);

   std::string GetActorID() const;

   void SetLineWidths(int top, int bottom);
   void SetTopLineWidth(int w);
   void SetBottomLineWidth(int w);

   void SetRow(int r) { mRow = r; }
   void SetCol(int c) { mCol = c; }

public slots:
   void OnLeftClickButton();
 
protected:
   void dragEnterEvent(QDragEnterEvent* myEvent);
   void dropEvent(QDropEvent* myEvent);

private:   
   void SetLineWidth(QFrame* line, int width);
   void DeleteLayoutItemAt(int row, int col);   

   Ui_NodeButton                                            mUI;

                 //"Tree" parent, as opposed to Qt's concept of "parent":
   NodeButton*                                              mTreeParent;
   std::vector<NodeButton*>                                 mChildren;

   //Keep track of location in QGridLayout
   int                                                      mRow;
   int                                                      mCol;

   QGridLayout*                                             mGridLayout;

   dtCore::BaseActorObject*                                  mBaseActor;
};

#endif //STAGE_MAP_HIERARCHY_NODE_BUTTON_H__
