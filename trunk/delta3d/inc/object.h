// object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OBJECT_H__010FD38C_9DA6_4889_BD79_33CC9C620EC9__INCLUDED_)
#define AFX_OBJECT_H__010FD38C_9DA6_4889_BD79_33CC9C620EC9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "base.h"
#include <osg/ref_ptr>
#include <osg/MatrixTransform>
#include <osg/Geode>
#include "notify.h"
#include "transformable.h"
#include "drawable.h"
#include "physical.h"

namespace dtCore
{
   
   ///A visual Object

 /** The Object represents a visual object in the Scene.  Once instantiated, 
   * a pre-built geometry file can be loaded using LoadFile().  The file specified
   * is searched for using the paths supplied to SetDataFilePathList().
   * Since Object is
   * derived from Transform, it may be positioned and queried using any of 
   * Transform's methods.
   *
   * The Object must be added to a Scene to be viewed using Scene::AddObject().
   */
   class DT_EXPORT Object : public Transformable, public Drawable, public Physical
   {
      DECLARE_MANAGEMENT_LAYER(Object)
         
      public:
         Object(std::string name = "Object");
         virtual ~Object();
         
         ///Get a handle to the OSG Node
         virtual osg::Node * GetOSGNode(void) {return mNode.get();}
         
         ///Load a file from disk
         virtual  bool LoadFile( std::string filename, bool useCache = true);

         ///Get the filename of the last loaded file
         virtual  std::string GetFilename(void) const {return mFilename;}
         
      protected:
         osg::ref_ptr<osg::MatrixTransform> mNode; ///<Contains the actual model
         std::string mFilename; ///<The filename of the last file loaded
   };
   
};

#endif // !defined(AFX_OBJECT_H__010FD38C_9DA6_4889_BD79_33CC9C620EC9__INCLUDED_)
