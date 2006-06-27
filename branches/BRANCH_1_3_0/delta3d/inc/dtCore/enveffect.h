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

#ifndef DELTA_ENVEFFECT
#define DELTA_ENVEFFECT

#include <dtCore/refptr.h>
#include <dtCore/deltadrawable.h>
#include <osg/Node>
#include <osg/Vec3>

namespace dtCore
{
   ///A base class for all Environmental Effects
   class DT_CORE_EXPORT EnvEffect : public dtCore::DeltaDrawable
   {
         DECLARE_MANAGEMENT_LAYER(EnvEffect)

      public:

         EnvEffect( const std::string& name = "EnvEffect" );

      protected:

         virtual ~EnvEffect();
   
      public:

         /// Must override this to supply the repainting routine
         virtual void Repaint(   const osg::Vec3& skyColor, 
                                 const osg::Vec3& fogColor,
                                 double sunAngle, 
                                 double sunAzimuth,
                                 double visibility ) = 0;

         ///required by DeltaDrawable
         osg::Node* GetOSGNode(){return mNode.get();}
         const osg::Node* GetOSGNode() const{return mNode.get();}

         void SetOSGNode(osg::Node* pNode){mNode = pNode;}

      private:

         // Disallowed to prevent compile errors on VS2003. It apparently
         // creates this functions even if they are not used, and if
         // this class is forward declared, these implicit functions will
         // cause compiler errors for missing calls to "ref".
         EnvEffect& operator=( const EnvEffect& ); 
         EnvEffect( const EnvEffect& );

         dtCore::RefPtr<osg::Node> mNode;
   };
}

#endif // DELTA_ENVEFFECT
