
#if !defined(AFX_TRANSFORMABLE_H__C6D53150_DAEC_4E42_BF12_C588CB62C637__INCLUDED_)
#define AFX_TRANSFORMABLE_H__C6D53150_DAEC_4E42_BF12_C588CB62C637__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "base.h"
#include "sg.h"
#include "transform.h"
#include <osg/ref_ptr>

namespace dtCore
{
   ///Anything that can be located and moved in 3D space
   
   /** The Transformable class is the base class of anything that can move in the 
     * virtual world.
     * 
     * The default coordinate system of dtCore is +X to the right, +Y forward into
     * the screen, and +Z is up.  Therefore, heading is around the Z axis, pitch
     * is around the X axis, and roll is around the Y axis.  The angles are all
     * right-hand-rule.
     */
   class Transformable : public Base  
   {
      DECLARE_MANAGEMENT_LAYER(Transformable)
   public:
      enum CoordSysEnum{
      REL_CS, ///< The Transform coordinate system is relative to the parent
      ABS_CS  ///< The Transform coordinate system is absolute
      } ;

      Transformable();
      virtual ~Transformable();

      //typedef std::vector<Transformable>  ChildList;
      typedef std::vector<osg::ref_ptr<Transformable> > ChildList;

      ///Test to see if child
      bool CanBeChild( Transformable *child );

      ///Add a Transformable child
	   void AddChild( Transformable *child );
         
      ///Remove a Transformable child
      void RemoveChild( Transformable *child );

      ///Return the number of Transformable children added
      inline unsigned int GetNumChildren() { return mChildList.size(); }

      ///Get the child specified by idx (0 to number of children-1)
      Transformable* GetChild( unsigned int idx ) {return mChildList[idx].get();}

      ///Get the immediate parent of this instance
      Transformable* GetParent(void) {return mParent.get();}

      ///Set the Transform to reposition this Transformable
      void SetTransform( Transform *xform, CoordSysEnum cs=ABS_CS );

      ///Get the current Transform of this Transformable
      void GetTransform( Transform *xform, CoordSysEnum cs=ABS_CS  );
      
   protected:

      Transform *mRelTransform;  ///<position relative to the parent
      ChildList mChildList;      ///<List of children Transformables added
      osg::ref_ptr<Transformable> mParent; ///<Any immediate parent of this instance

   private:

   /** Get the index number of child, return a value between
     * 0 and the number of children-1 if found, if not found then
     * return the number of children.
     */
      inline unsigned int GetChildIndex( const Transformable* child ) const
      {
         for (unsigned int childNum=0;childNum<mChildList.size();++childNum)
         {
            if (mChildList[childNum]==child) return childNum;
         } 
         return mChildList.size(); // node not found.
      }

      void Transformable::CalcAbsTransform( Transform *xform );

   };
};


#endif // !defined(AFX_TRANSFORMABLE_H__C6D53150_DAEC_4E42_BF12_C588CB62C637__INCLUDED_)
