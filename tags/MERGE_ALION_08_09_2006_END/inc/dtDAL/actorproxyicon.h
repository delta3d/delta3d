/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * @author Matthew W. Campbell
 */
#ifndef DELTA_ACTOR_PROXY_ICON
#define DELTA_ACTOR_PROXY_ICON

#include <map>
#include <dtUtil/enumeration.h>
#include <dtCore/deltadrawable.h>
#include <osg/Referenced>
#include <osg/ref_ptr>
#include <osg/Vec3>
#include <osg/Matrix>
#include <osg/Group>
#include "dtDAL/export.h"

namespace dtCore
{
   class Transformable;
}

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Geometry;
   class Image;
}

namespace dtEditQt
{
   class ViewportManager;
}
/// @endcond

namespace dtDAL
{
   // Forward declaration for a friend :)
   class Project;
   
   class DT_DAL_EXPORT ActorProxyIcon : public osg::Referenced
   {
      // Project & ViewportManager need to call LoadImages
      friend class Project;
      friend class dtEditQt::ViewportManager;

      public:

         //a basic means to encapsulate icon configure params
         struct ActorProxyIconConfig
         {
            ActorProxyIconConfig(): mUpVector(true), mForwardVector(true), /*mRightVector(true),*/ mScale(1.0) {}
            bool mUpVector;
            bool mForwardVector;
            //bool mRightVector;
            float mScale;
         };


         //Our custom Delta3D drawable.
         class BillBoardDrawable : public dtCore::DeltaDrawable
         {
            public:
               BillBoardDrawable()
               {
                  mNode = new osg::Group() ;
               }

               virtual bool AddChild(dtCore::DeltaDrawable *child)
               {
                  GetOSGNode()->asGroup()->addChild(child->GetOSGNode());
                  return dtCore::DeltaDrawable::AddChild(child);
               }

               virtual void RemoveChild(dtCore::DeltaDrawable *child)
               {
                  GetOSGNode()->asGroup()->removeChild(child->GetOSGNode());
                  dtCore::DeltaDrawable::RemoveChild(child);
               }

               ///required by DeltaDrawable
               osg::Node* GetOSGNode(){return mNode.get();}
               const osg::Node* GetOSGNode() const{return mNode.get();}

            protected:
               virtual ~BillBoardDrawable() { }

               dtCore::RefPtr<osg::Node> mNode;
         };

         //These are resources used by the editor to display billboards
         //for the actors that need them.
         static std::string IMAGE_BILLBOARD_GENERIC;
         static std::string IMAGE_BILLBOARD_CHARACTER;
         static std::string IMAGE_BILLBOARD_STATICMESH;
         static std::string IMAGE_BILLBOARD_LIGHT;
         static std::string IMAGE_BILLBOARD_SOUND;
         static std::string IMAGE_BILLBOARD_PARTICLESYSTEM;
         static std::string IMAGE_BILLBOARD_MESHTERRAIN;
         static std::string IMAGE_BILLBOARD_PLAYERSTART;
         static std::string IMAGE_BILLBOARD_TRIGGER;
         static std::string IMAGE_BILLBOARD_CAMERA;
         static std::string IMAGE_BILLBOARD_WAYPOINT;

         //Textures used to display the arrow orientation indicator on the billboard.
         static std::string IMAGE_ARROW_HEAD;
         static std::string IMAGE_ARROW_BODY;

         /**
          * This enumeration enumerates the different types of billboard icons that are
          * supported by default by the level editor.
          */
         class DT_DAL_EXPORT IconType : public dtUtil::Enumeration
         {
               DECLARE_ENUM(IconType);
            public:

               static const IconType GENERIC;
               static const IconType CHARACTER;
               static const IconType STATICMESH;
               static const IconType SOUND;
               static const IconType LIGHT;
               static const IconType PARTICLESYSTEM;
               static const IconType MESHTERRAIN;
               static const IconType PLAYERSTART;
               static const IconType TRIGGER;
               static const IconType CAMERA;
               static const IconType WAYPOINT;

            protected:
               IconType(const std::string &name) : Enumeration(name)
               {
                  AddInstance(this);
               }
         };

         /**
          * Initializes the paths to the billboard image files.
          * @note Called from LibraryManager.
          */
         static void staticInitialize();

         /**
          * Constructs a new actor proxy billboard icon.  This creates the
          * necessary scene geometry and attaches the appropriate image
          * to the icon.
          * @param type Type of icon to create.  By default, IconType::GENERIC is used.
          */
         ActorProxyIcon(const IconType &type = IconType::GENERIC);

         /**
         * Constructs a new actor proxy billboard icon using an optional config class
         * meant to be extendable to allow various options.
         */
         ActorProxyIcon(const IconType& type, const ActorProxyIconConfig& pConfig);

         /**
          * Sets the icon type used by this proxy icon.
          * @param type
          */
         void SetIconType(const IconType &type)
         {
            mIconType = &type;
            CreateBillBoard();
         }

         /**
          * Gets the Delta3D drawable for this proxy icon.
          * @return A Delta3D transformable which is the parent of the billboard geometry.
          */
         dtCore::DeltaDrawable* GetDrawable();

         /**
          * Gets the Delta3D drawable for this proxy icon.
          * @return A Delta3D transformable which is the parent of the billboard geometry.
          */
         const dtCore::DeltaDrawable* GetDrawable() const;

         bool OwnsDrawable(dtCore::DeltaDrawable *drawable) const;

         void SetPosition(const osg::Vec3 &newPos);
         void SetRotation(const osg::Matrix &rotMat);
         void SetActorRotation(const osg::Vec3 &hpr);
         void SetActorRotation(const osg::Matrix &mat);
         osg::Matrix GetActorRotation();
         void SetScale(const osg::Vec3 &newScale);        

      protected:
         virtual ~ActorProxyIcon();

         /**
          * Helper method assigns a texture and appropriate state to the billboard.
          * This should only be called be dtDAL::Project (which is a friend).
          */
         void LoadImages();
      private:
         ActorProxyIcon &operator=(const ActorProxyIcon &rhs);
         ActorProxyIcon(const ActorProxyIcon &rhs);

         /**
          * Helper method which builds the billboard geometry.
          */
         void CreateBillBoard();
    
         /**
          * Creates a geometry node for drawing an arrow.
          * @return The geometry node.
          */
         osg::Group *CreateOrientationArrow();

         /**
          * Checks the type of this billboard and loads the appropriate image.
          * @return An image mapping to the billboard type on this billboard icon.
          */
         osg::Image* GetBillBoardImage();

         /**
          * Creates a quad which is used for the billboard.
          * @param corner Upper left corner of the quad.
          * @param width Width of the quad.
          * @param height Height of the quad.
          * @return A geometry node holding the quad's geometry.
          */
         osg::Geometry* CreateGeom(const osg::Vec3 &corner, const osg::Vec3 &width,
                                 const osg::Vec3 &height);

         ///Type of the billboard icon.
         const IconType *mIconType;

         ///our custom config
         ActorProxyIconConfig mConfig;

         ///The actual billboard drawable which includes the billboard and an arrow
         ///depicting its actor's orientation.
         dtCore::RefPtr<dtCore::DeltaDrawable> mBillBoard;

         ///The underlying Delta3D drawable object.
         dtCore::RefPtr<dtCore::Transformable> mIconNode;

         ///A transformable used to represent an arrow depicting actor rotation.
         dtCore::RefPtr<dtCore::Transformable> mArrowNode;

         ///A transformable used to represent an arrow depicting actor up vector.
         dtCore::RefPtr<dtCore::Transformable> mArrowNodeUp;

         ///A state set containing the icon's texture.
         osg::ref_ptr<osg::StateSet> mIconStateSet;

         ///A state set containing the orientation arrow's cone texture.
         osg::ref_ptr<osg::StateSet> mConeStateSet;

         ///A state set containing the orientation arrow's cylinder texture.
         osg::ref_ptr<osg::StateSet> mCylinderStateSet;
   };
}

#endif
