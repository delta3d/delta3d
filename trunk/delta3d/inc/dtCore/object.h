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
#include <dtCore/model.h>
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
         Object(TransformableNode& node, const std::string& name = "Object");

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
          * Sets the scale on this object
          * @param xyz The scale vector
          */
         void SetScale(const osg::Vec3& xyz);

         /**
          * Returns the scale vector
          * @return The scale
          */
         osg::Vec3 GetScale() const;

         /**
          * Returns the matrix transform of this object
          * @return The osg::MatrixTransform
          */
         osg::MatrixTransform& GetMatrixTransform();

         /**
          * Returns the matrix transform of this object
          * @return The osg::MatrixTransform
          */
         const osg::MatrixTransform& GetMatrixTransform() const;

         /**
         * Sets the models rotation
         * @param v3 the hpr
         */
         void SetModelRotation(const osg::Vec3& v3);

         /**
         * Returns the scale vector
         * @return The scale
         */
         osg::Vec3 GetModelRotation();

         /**
         * Sets the models translation
         * @param v3 the xyz
         */
         void SetModelTranslation(const osg::Vec3& v3);

         /**
         * Returns the scale vector
         * @return The scale
         */
         osg::Vec3 GetModelTranslation();

      protected:

         virtual ~Object();

      private:

         void Ctor();

         dtCore::RefPtr<Model> mModel;

         bool mRecenterGeometry;///<if we want to recenter the geometry of the object about the origin upon load
   };
}

#endif // DELTA_OBJECT
