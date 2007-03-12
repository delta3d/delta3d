/*  -*- c++ -*-
Copyright (C) 2003 <ryu@gpul.org>

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

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
*/

#ifndef __DTCHAR__MODEL_H__
#define __DTCHAR__MODEL_H__

#include <osg/Geode>
#include <cal3d/cal3d.h>
#include <dtChar/coremodel.h>
#include <dtChar/export.h>

#include <osg/Timer>

/**
* Class that updates the models to generate the new deformed vertices
*/
class CalUpdateCallback: public osg::NodeCallback {
public:
   CalUpdateCallback(CalModel *_model): model(_model) {
      previous = timer.tick();
      _timeScale = 1.0f;
   }
   virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
   {
      osg::Timer_t current = timer.tick();
      double elapsed = timer.delta_s(previous, current);
      model->update(elapsed*getTimeScale());
      previous=current;
      traverse(node,nv);
   }
   void setTimeScale(float timeScale) {
      _timeScale = timeScale;
   }

   float getTimeScale() {
      return (_timeScale);
   }
private:
   CalModel *model;
   osg::Timer timer;
   osg::Timer_t previous;
   float _timeScale;
};


namespace dtChar {

   ///@brief Adapter that allows having a model in the scene graph.

   /** 
   * This class is an osg::Node so you can add it to your scenegraph. Moreover
   * it handles automatically updating the cal3d model associated, and the
   * generation of drawables binded to submeshes of the model.
   *
   * Note that this node installs an UpdateCallback so don't overwrite it...
   */
   class DT_CHAR_EXPORT Model: public osg::Geode {
   public:
      Model();
      Model(CoreModel *core);

      virtual osg::Object* cloneType() const { return new Model(); }

      /** Clone the an object, with Object* return type.
      Must be defined by derived classes.*/
      virtual osg::Object* clone(const osg::CopyOp&) const;

      virtual bool isSameKindAs(const osg::Object *obj) const { return dynamic_cast<const Model*>(obj) != 0; }

      /** return the name of the object's library. Must be defined
      by derived classes. The OpenSceneGraph convention the is
      that the namspace of a library is the same as the library name.*/
      virtual const char* libraryName() const { return "dtChar"; }

      /** return the name of the object's class type. Must be defined
      by derived classes.*/
      virtual const char* className() const { return "Model"; }

      inline ~Model() { /*model.destroy();*/ }

      /**
      * Set the time scale factor
      */
      void SetTimeScale(float timeScale);

      /**
      * Get the time scale factor
      */
      float GetTimeScale();

      /**
      * Create an "instance" of the reference (core) model given.
      * This method creates lots of drawables each one with a single
      * material, to take advantage of OSG state sorting.
      */
      void Create(CoreModel *core);

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

      /**
      * Starts an animation in loop mode
      * @param id The animation id
      * @param weigth The strength of the loop (there can be several simultaneous)
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
      * Number of animations in this model
      */
      inline unsigned AnimationCount() {
         return mCalModel->getCoreModel()->getCoreAnimationCount();
      }

      /**
      * Get the core model
      */
      const CoreModel *GetCoreModel() const { return mCoreModel.get(); }

      /**
      * Expose the underlying Cal3D model
      */
      inline CalModel *GetCalModel(void) { return mCalModel; }

      /**
      * Expose the underlying Cal3D model
      */
      inline const CalModel *GetCalModel(void) const { return mCalModel; }

   private:
      CalModel *mCalModel;
      osg::ref_ptr<CoreModel> mCoreModel;
      //osg::ref_ptr<CalUpdateCallback> calUpdateCallback;
      CalUpdateCallback *mCalUpdateCallback;
   };

}; // namespace dtChar

#endif
