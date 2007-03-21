/* 
* Delta3D Open Source Game and Simulation Engine 
* Copyright (C) 2007 MOVES Institute 
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
* Erik Johnson
*/

#ifndef chardrawable_h__
#define chardrawable_h__

#include "dtCore/transformable.h"
#include "dtChar/export.h"

class CalModel;



namespace dtChar
{
   /** Simple class that wraps up a dtChar::Model so it can be added to the
     * Scene.  
     *
     * Usage:
     * @code
     *   dtCore::RefPtr<dtChar::CharDrawable> char = new dtChar::CharDrawable();
     *   dtCore::RefPtr<dtChar::CoreModel> core = new dtChar::CoreModel();
     *   core->LoadSkeleton(...);
     *   core->LoadAnimation(...);
     *   char->Create( *core );
     *   myScene->AddDrawable( *char );
     * @endcode
     */

   class CoreModel;
   class Cal3DWrapper;

   class DT_CHAR_EXPORT CharDrawable : public dtCore::Transformable
   {
   public:
   	CharDrawable();
   	~CharDrawable();

    void OnMessage(Base::MessageData *data);

    /**
    * Create an "instance" of the reference (core) model given.
    * This method creates lots of drawables each one with a single
    * material, to take advantage of OSG state sorting.
    */
    void Create(const std::string &filename);

    /**
    * Starts an animation in loop mode
    * @param id The animation id
    * @param weight The strength of the loop (there can be several simultaneous)
    * @param delay Time to reach the indicated weigth
    */
    void StartLoop(unsigned id, float weight, float delay);

    /**
    * Stops an animation
    * @param id The animation id
    * @param delay Time to fade out
    */
    void StopLoop(unsigned id, float delay);

    /**
    * Starts an animation with max strength over other animations
    * @param id The animation id
    * @param delay_in Time to reach max strength at begining of animation
    * @param delay_out Time to leave max strength at end of animation
    */
    void StartAction(unsigned id, float delay_in, float delay_out);

    /**
    * Stop an action animation
    * @param id The animation id
    */
    void StopAction(unsigned id);

    dtChar::Cal3DWrapper* GetCal3DWrapper() {return mCal3DWrapper.get(); }

   private:
      
      osg::ref_ptr<osg::Geode>    mGeode; 
      dtCore::RefPtr<dtChar::Cal3DWrapper> mCal3DWrapper;


   };
}
#endif // chardrawable_h__