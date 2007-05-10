/* -*-c++-*-
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2007, Alion Science and Technology
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
 * Bradley Anderegg 03/30/2007
 */

#ifndef __DELTA_ANIMATIONHELPER_H__
#define __DELTA_ANIMATIONHELPER_H__

#include <dtAnim/export.h>
#include <dtCore/refptr.h>

#include <osg/Referenced>

#include <string>
#include <vector>

namespace osg
{
   class Geode;
}

namespace dtDAL
{
   class ActorProperty;
   class ActorProxy;
}

namespace dtAnim
{
   class Cal3DModelWrapper;
   class Cal3DAnimator;
   class AnimNodeBuilder;
   class Cal3DLoader;
   class SequenceMixer;
   class SkeletalConfiguration;

/***
*  The AnimationHelper class is a utility class to simplify adding animation
*  to an articulated entity, it provides support for loading, rendering and
*  animating.  
*
* @usage The constructor takes an AnimNodeBuilder which will create an osg::Geode given
*        a Cal3DModelWrapper.  The builder will be used to create the geode on LoadModel(),
*        after that call GetGeode() and add this as a child of your actor's osg::Node
*/
class	DT_ANIM_EXPORT AnimationHelper: public osg::Referenced
{

public:

   /**
   * The constructor takes an AnimNodeBuilder which will create an osg::Geode given
   * a Cal3DModelWrapper.  The builder will be used to create the geode on LoadModel().
   */
   AnimationHelper(AnimNodeBuilder* pBuilder);

   /**
   * The user should call Update() on a per frame basis
   * this function updates the sequence mixer and the Cal3DAnimator
   */
   void Update(float dt);

   /**
   * This function loads a character XML file from string,
   * on loading it creates a Cal3DAnimator with the Cal3DModelWrapper
   * and then calls CreateGeode() on the AnimNodeBuilder
   *
   * @param the name of the file to load
   */
   void LoadModel(const std::string& pFilename);

   /**
   * This function plays the specified animation defined within the character XML
   *
   * @param The name of the animation to play
   */
   void PlayAnimation(const std::string& pAnim);

   /**
   *  This function stops playing an animation by name over the course
   *  of time specified by fade out.
   *
   * @param The name of the animation to clear
   * @param The amount of time to fade out over
   */
   void ClearAnimation(const std::string& pAnim, float fadeOutTime);

   /**
   * @return the osg::Geode() created by the builder on LoadModel
   */
   osg::Geode* GetGeode();

   /**
   * @return the osg::Geode() created by the builder on LoadModel
   */
   const osg::Geode* GetGeode() const;

   /**
   * @return The Cal3DAnimator created on LoadModel
   */
   Cal3DAnimator* GetAnimator();

   /**
   * @return The Cal3DAnimator created on LoadModel
   */
   const Cal3DAnimator* GetAnimator() const;

   /**
   * @return The Cal3DModelWrapper held by the animator
   */
   Cal3DModelWrapper* GetModelWrapper();

   /**
   * @return The Cal3DModelWrapper held by the animator
   */
   const Cal3DModelWrapper* GetModelWrapper() const;

   /**
   * @return The SequenceMixer used to play, clear, and register new animations
   */
   SequenceMixer* GetSequenceMixer();

   /**
   * @return The SequenceMixer used to play, clear, and register new animations
   */
   const SequenceMixer* GetSequenceMixer() const;
   
   /**
   * @return The SkeletalConfiguration used to define bone semantics
   */
   SkeletalConfiguration* GetSkeletalConfiguration();
   
   /**
   * @return The SkeletalConfiguration used to define bone semantics
   */
   const SkeletalConfiguration* GetSkeletalConfiguration() const;

   /**
   * This function is used to create the proper actor properties for an animated entity
   * after calling this function the user must iterate through the vector and add each property 
   * to its proxy.
   *
   * @param the actor proxy
   * @param an empty vector to fill of actor properties
   */
   virtual void GetActorProperties(dtDAL::ActorProxy& pProxy, std::vector<dtCore::RefPtr<dtDAL::ActorProperty> >& pFillVector);      


protected:
   virtual ~AnimationHelper();

private:

   dtCore::RefPtr<osg::Geode> mGeode;
   dtCore::RefPtr<Cal3DAnimator> mAnimator;
   dtCore::RefPtr<AnimNodeBuilder> mNodeBuilder;

   dtCore::RefPtr<SequenceMixer> mSequenceMixer;
   dtCore::RefPtr<SkeletalConfiguration> mSkeleton;

   static dtCore::RefPtr<Cal3DLoader> sModelLoader;
   
};

}//namespace dtAnim

#endif // __DELTA_ANIMATIONHELPER_H__

