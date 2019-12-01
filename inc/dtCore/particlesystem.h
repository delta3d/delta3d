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

#include <dtCore/transformable.h>
#include <dtCore/loadable.h>

#include <osg/MatrixTransform>
#include <osgParticle/ModularEmitter>
#include <osgParticle/Program>
#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>

namespace dtCore
{
   class DT_CORE_EXPORT ParticleLayer
   {
      DECLARE_MANAGEMENT_LAYER(ParticleLayer)

      public:
         /// Constructor
         ParticleLayer();

         /// Destructor
         virtual ~ParticleLayer();

         /// Copy Constructor
         ParticleLayer(const ParticleLayer &copyLayer);

         /// mutators for RefPtr Variables and string
         void SetGeode(osg::Geode& geode);
         void SetParticleSystem(osgParticle::ParticleSystem& particleSystem);
         void SetEmitterTransform(osg::MatrixTransform& matrixtransform);
         void SetModularEmitter(osgParticle::ModularEmitter& modularEmitter);
         void SetProgram(osgParticle::Program& program);
         void SetLayerName(const std::string& name);

         /// accessors for RefPtrVariables and string
         osg::Geode& GetGeode();
         const osg::Geode& GetGeode() const;

         osgParticle::ParticleSystem&  GetParticleSystem();
         const osgParticle::ParticleSystem&  GetParticleSystem() const;

         osg::MatrixTransform& GetEmitterTransform();
         const osg::MatrixTransform& GetEmitterTransform() const;

         osgParticle::ModularEmitter& GetModularEmitter();
         const osgParticle::ModularEmitter& GetModularEmitter() const;

         osgParticle::Program& GetProgram();
         const osgParticle::Program& GetProgram() const;

         const std::string& GetLayerName() const { return mLayerName; }

         /// Methods for telling if the Program is modular or fluid
         bool IsFluidProgram() const        {return !mProgTypeIsModular;}
         bool IsModularProgram() const      {return mProgTypeIsModular ;}

         /// Methods for Getting if the program is modular or fluid
         void SetIsModularProgram(bool Val) {mProgTypeIsModular  =  Val;}
         void SetIsFluidProgram(  bool Val) {mProgTypeIsModular  = !Val;}

         /// Operator overloadin' luvin
         bool operator==(const ParticleLayer& testLayer) const;

      private:
         /**
         * The geode that holds the drawable particle system, and whose name is
         * the name of the layer.
         */
         RefPtr<osg::Geode> mGeode;

         /**
         * The active particle system.
         */
         RefPtr<osgParticle::ParticleSystem> mParticleSystem;

         /**
         * The transform that controls the position of the emitter.
         */
         RefPtr<osg::MatrixTransform> mEmitterTransform;

         /**
         * The active emitter.
         */
         RefPtr<osgParticle::ModularEmitter> mModularEmitter;

         /**
         * The active program.
         * Can be modular or fluid
         */
         RefPtr<osgParticle::Program> mProgram;

         /**
         * Name of the layer
         */
         std::string mLayerName;

         /// For getting Program type, fluid or modular.
         /// Setuped on setup values functions
         bool mProgTypeIsModular;
   };

   /**
    * A particle system.
    */
   class DT_CORE_EXPORT ParticleSystem : public Transformable, public Loadable
   {
      DECLARE_MANAGEMENT_LAYER(ParticleSystem)

      public:
         typedef std::vector<ParticleLayer> LayerList;

         typedef Transformable BaseClass;

         /**
          * Constructor.
          *
          * @param name the instance name
          */
         ParticleSystem(std::string name = "ParticleSystem");

      protected:

         /**
          * Destructor.
          */
         virtual ~ParticleSystem();

      public:

         ///Performs collision detection and updates physics
         virtual void OnSystem(const dtUtil::RefString& str, double deltaSim, double deltaReal)
;

         ///Load a file from disk
         virtual osg::Node* LoadFile( const std::string& filename, bool useCache = true);

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
         bool IsEnabled() const;

         /**
          * Sets the parent-relative state of this particle system.  If
          * parent-relative mode is enabled, the entire particle system
          * will be positioned relative to the parent.  If disabled, only
          * the emitter will be positioned relative to the parent.  By
          * default, particle systems are not parent-relative.
          *
          * This must be set before loading the particle file.
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
         bool IsParentRelative() const;

         /**
          * GetAllLayers can be called to change all the effects
          * that were loaded in to do the same thing.
          * @return The link list of all the layers
          */
         LayerList& GetAllLayers() {return mLayers;}
         const LayerList& GetAllLayers() const {return mLayers;}

         /**
          * GetSingleLayer will return the Layer of said name,
          * Null will return if bad name sent in.
          *
          * @return Will return the link list you requested by name
          */
         ParticleLayer* GetSingleLayer(const std::string& layerName);
         const ParticleLayer* GetSingleLayer(const std::string& layerName) const;

         osgParticle::ParticleSystemUpdater* GetParticleSystemUpdater();
         const osgParticle::ParticleSystemUpdater* GetParticleSystemUpdater() const;

         /**
          * SetAllLayers Will take in the new list of layers
          * and set all the current layers to those of that
          * sent in
          */
         void SetAllLayers(const LayerList &layersToSet);

         /**
          * SetSingleLayer will take in the layerToSet
          * and set the layer in mlLayers to that sent in
          */
         void SetSingleLayer(ParticleLayer& layerToSet);

         /// Resets the particle system to the starting time.
         void ResetTime();

         const osg::Node* GetLoadedParticleSystemRoot() const;
         const osg::Node* GetCachedOriginalParticleSystemRoot() const;
         osg::Node* GetLoadedParticleSystemRoot();
         osg::Node* GetCachedOriginalParticleSystemRoot();

         static void ParseParticleLayers(osg::Node& ps, LayerList& layers,
                  dtCore::RefPtr<osgParticle::ParticleSystemUpdater>& particleSystemUpdater);

      protected:

         void CloneParticleSystemDrawables();

      private:
         void OnPause();
         void OnUnpause();

         /**
          * Whether or not the particle system is enabled.
          */
         bool mEnabled;

         /**
          * Whether or not the particle system is in parent-relative
          * mode.
          */
         bool mParentRelative;

         /**
          * Layer for all the attributes for a particle system
          * All particle systems contained within that we loaded
          */
         LayerList mLayers;

         /**
          * Handle to the whole system
          */
         RefPtr<osg::Node> mLoadedFile;
         //This hold onto the original, non copied particle system to help in stay in the cache
         RefPtr<osg::Node> mOriginalLoadedParticleSystem;
         RefPtr<osgParticle::ParticleSystemUpdater> mParticleSystemUpdater;

         // Map of particle systems and their frozen states before a pause
         typedef std::map<osgParticle::ParticleSystem*, bool> ParticleSystemBoolMap;
         ParticleSystemBoolMap mWasFrozenBeforePauseMap;
   };
}

#endif // DELTA_PARTICLESYSTEM
