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
#include <dtCore/model.h>
#include <dtCore/transformable.h>
#include <dtCore/resourcedescriptor.h>
#include <osg/MatrixTransform>
#include <osg/NodeCallback>
#include <osg/Vec3>
#include <dtUtil/macros.h>
#include <dtUtil/getsetmacros.h>

namespace dtCore
{
   /**
    * A visual Object with physical properties. The Object represents a
    * virtual object which is renderable, movable, and has physical properties.
    */
   class DT_CORE_EXPORT Object : public Loadable, public Transformable
   {
      DECLARE_MANAGEMENT_LAYER(Object)

      public:
         typedef Transformable BaseClass;

         Object(const std::string& name = "Object");

         /** Overloaded constructor will use the supplied node instead of
           * creating one internally.
           * @note: Calling LoadFile will overwrite the supplied node with whatever is loaded.
           * @param node : A node this class should use internally
           * @param name : The name of this instance
           */
         Object(TransformableNode& node, const std::string& name = "Object");

         /*!
          * Load Geometry from the following resource.  It won't load until the object is added to the scene.
          */
         DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, MeshResource);


         /// Loads the mesh when it's added to the scene.
         void AddedToScene(dtCore::Scene* scene) override;

         /**
          * This is the file version.  Set the MeshResource instead.
          */
         osg::Node* LoadFile(const std::string& filename, bool useCache = true) override;

         /**
          * Sets whether we will use the cache when we load files.  This must be set
          * before loading the mesh or it will have no effect.  To make this work, the
          * model is now loaded on entering the world the first time.
          * @param value new use cache value (default is true)
          */
         DT_DECLARE_ACCESSOR(bool, UseCache)

         /**
          * Recenter the geometry about the origin by setting the model transform properties
          * This gets the bounding box and finds the center of it.  This only works if set before the model is loaded.
          */
         DT_DECLARE_ACCESSOR(bool, RecenterGeometryUponLoad);

         /**
          * Generate Tangents for the mesh on load.
          */
         DT_DECLARE_ACCESSOR(bool, GenerateTangents);

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
   };
}

#endif // DELTA_OBJECT
