/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004-2005 MOVES Institute 
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free 
 * Software Foundation; either version 2.1 of the License, or (at your option) 
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS 
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more 
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License 
 * along with this library; if not, write to the Free Software Foundation, Inc., 
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA 
 *
*/

#ifndef DELTA_OBJECT
#define DELTA_OBJECT

// object.h: interface for the Object class.
//
//////////////////////////////////////////////////////////////////////

#include <dtCore/loadable.h>
#include <dtCore/physical.h>
#include <osg/MatrixTransform>
#include <osg/NodeCallback>
#include <osg/Vec3>

namespace dtCore
{
   /** 
    * A visual Object with physical properties. The Object represents a 
    * virtual object which is renderable, movable, and has physical properties.
    */
   class DT_CORE_EXPORT Object : public Loadable, public Physical
   {
      DECLARE_MANAGEMENT_LAYER(Object)
         
      public:
      
         Object(const std::string& name = "Object");

         /** Overloaded constructor will use the supplied node instead of
           * creating one internally.
           * @note: Calling LoadFile will overwrite the supplied node with whatever is loaded.
           * @param node : A node this class should use internally
           * @param name : The name of this instance
           */
         Object( TransformableNode &node, const std::string &name = "Object" );

         void SetModelTransform(const dtCore::Transform& xform);
         void GetModelTransform(dtCore::Transform& xform) const;

         //Changes the scale of the model without 
         void SetModelScale(const osg::Vec3& modelScale);
         void GetModelScale(osg::Vec3& modelScale) const;

         /// Get the model transform matrix as const.
         const osg::MatrixTransform& GetModelTransform() const { return *mModelTransform; }

         /** Get the model transform matrix by reference. If you call "setMatrix" on this, you must
          * call "SetDirty" on this class so the scale can be re-applied.
          */
         osg::MatrixTransform& GetModelTransform() { return *mModelTransform; }
         
         /*!
          * Load a geometry from a file using any supplied data file paths set in
          * dtCore::SetDataFilePathList().  Additional calls to this method will replace
          * the first geometry for the next.
          *
          * @param filename : The name of the file to be loaded
          * @param useCache : If true, use OSG's file cache
          */
         virtual osg::Node* LoadFile(const std::string& filename, bool useCache = true);

         ///recenters the object geometry on LoadFile
         void RecenterGeometryUponLoad( const bool enable = true ) { mRecenterGeometry = enable; }

         /**
          *  Tells this object that the scale vector has been changed, so that it
          *  can be reapplied (at great expense) during an update callback.
          * This is called automatcally by SetModelScale and SetModelTransform, but 
          * must be called explicitly if one modifies the model transform directly by calling
          * #GetModelTransform().setMatrix();
          */
         void SetDirty();

      protected:
         virtual ~Object();

      private:

         void Ctor();

         dtCore::RefPtr<osg::MatrixTransform> mModelTransform;
         dtCore::RefPtr<osg::NodeCallback> mUpdateCallback; 
         osg::Vec3 mScale;

         bool mRecenterGeometry;///<if we want to recenter the geometry of the object about the origin upon load
   };   
}

#endif // DELTA_OBJECT
