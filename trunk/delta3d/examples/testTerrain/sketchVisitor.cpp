#include "sketchVisitor.h"
sketchVisitor::sketchVisitor() : osg::NodeVisitor(TRAVERSE_ALL_CHILDREN)
{
   numNodesTraversed =0; 
}

void sketchVisitor::apply(osg::Node &searchNode) 
{
   osg::NodePath& np = getNodePath();
   if (searchNode.getName() != "")
   {
      for (int i = 1; i < np.size() ; i++)
      {
         std::cout << "|  ";
      }

      std::cout << "-" << searchNode.getName() << std::endl;
   }
   numNodesTraversed++;
   traverse(searchNode); 
} 