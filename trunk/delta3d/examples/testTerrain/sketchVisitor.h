#ifndef SKETCH_VISITOR_H
#define SKETCH_VISITOR_H

#include <osg/NodeVisitor>
#include <osg/Node>

#include <osgSim/DOFTransform>
#include <osg/Group>
#include <osg/LOD>
#include <iostream>
#include <vector>

class sketchVisitor : public osg::NodeVisitor { 
public: 

   sketchVisitor();
   virtual void apply(osg::Node &searchNode);

private: 

   int numNodesTraversed;

}; 

#endif

