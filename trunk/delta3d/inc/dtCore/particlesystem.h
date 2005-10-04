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

#ifndef DELTA_PARTICLESYSTEM
#define DELTA_PARTICLESYSTEM

// particlesystem.h: Declaration of the ParticleSystem class.
//
//////////////////////////////////////////////////////////////////////


#include "dtCore/transformable.h"
#include "dtCore/loadable.h"

namespace dtCore
{
   /**
    * A particle system.
    */
   class DT_EXPORT ParticleSystem : public Transformable, public Loadable
   {
      DECLARE_MANAGEMENT_LAYER(ParticleSystem)

      public:

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         ParticleSystem(std::string name = "ParticleSystem");

         /**
          * Destructor.
          */
         virtual ~ParticleSystem();         

         ///Load a file from disk
         virtual osg::Node* LoadFile( const std::string& filename, bool useCache = false);
         
         /**
          * Enables or disables this particle system.  Particle systems
          * are enabled by default.
          *
          * @param enable true to enable the particle system, false
          * to disable it
          */
         void SetEnabled(bool enable);
         
         /**
          * Checks whether this particle system is enabled.
          *
          * @return true if the particle system is enabled, false
          * otherwise
          */
         bool IsEnabled();
         
         /**
          * Sets the parent-relative state of this particle system.  If
          * parent-relative mode is enabled, the entire particle system
          * will be positioned relative to the parent.  If disabled, only
          * the emitter will be positioned relative to the parent.  By
          * default, particle systems are not parent-relative.
          *
          * @param parentRelative true to enable parent-relative mode,
          * false to disable it
          */
         void SetParentRelative(bool parentRelative);
         
         /**
          * Returns the parent-relative state of this particle system.
          *
          * @return true if the particle system is in parent-relative mode,
          * false if not
          */
         bool IsParentRelative();
         

   private:
         
         /**
          * Whether or not the particle system is enabled.
          */
         bool mEnabled;
         
         /**
          * Whether or not the particle system is in parent-relative
          * mode.
          */
         bool mParentRelative;

         RefPtr<osg::Node> mLoadedFile; ///<handle to the whole system
         
   };
};


#endif // DELTA_PARTICLESYSTEM
