/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2014, Caper Holdings, LLC
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
* Bradley Anderegg
*/
#ifndef DELTA_SCENEGROUP_H
#define DELTA_SCENEGROUP_H

#include <dtRender/scenebase.h>
#include <osg/Group>

namespace dtRender
{
   /**
   *  The SceneGroup is meant to have multiple scene children
   *     it does not actually hold any other DeltaDrawable's.
   */
   class SceneGroupImpl;
   class DT_RENDER_EXPORT SceneGroup : public SceneBase
   {
   public:
      typedef SceneBase BaseClass;
      static const dtCore::RefPtr<SceneType> SCENE_GROUP;

      typedef std::vector<dtCore::RefPtr<SceneBase> > SceneArray;

   public:
      /**
      *  Default constructor used for non derived types, 
      *  passes SCENE_GROUP and DEFAULT_SCENE to base
      */
      SceneGroup();
      SceneGroup(const SceneType& sceneId, const SceneEnum& defaultScene);
      virtual ~SceneGroup();


      virtual void CreateScene(SceneManager&, const GraphicsQuality&);

      
      /**
      * Searches the scene for the first found scene of said type
      * @return NULL if the scene type cannot be found.
      */
      SceneBase* FindSceneByType(SceneType&);
      const SceneBase* FindSceneByType(SceneType&) const;
      
      /***
      * Fills a vector with all scenes of the specified type.
      */
      void GetAllScenesByType(SceneType&, std::vector<SceneBase*>&);


      /**
      *  The scene group must override add/remove child to pass
      *     on the drawable to its child scenes.
      */
      virtual bool AddChild(DeltaDrawable* child);

      /***
      *  RemoveChild() always assumes we are the parent
      *     because the drawable has a parent pointer.
      */
      virtual void RemoveChild(DeltaDrawable* child);

      /****
      *  These functions are slightly different the the DeltaDrawable ones-
      *     These are sorted in respect to their render order
      *     where as the DeltaDrawable children are not sorted.
      */
      SceneBase* GetSceneChild(unsigned index);
      const SceneBase* GetSceneChild(unsigned index) const;

      void GetSceneChildren(SceneArray& toFill);

      unsigned int GetNumSceneChildren() const;

      virtual bool ContainsActor(dtCore::DeltaDrawable& dd) const;

      virtual osg::Group* GetSceneNode();
      virtual const osg::Group* GetSceneNode() const;

      virtual SceneGroup* GetAsSceneGroup(){return this;}
      virtual const SceneGroup* GetAsSceneGroup() const{return this;}

   protected:
      /**
      *  A SceneGroup holds an array of scenes 
      *     sorted by their render order.
      *
      *  There is no RemoveScene(), use RemoveChild().
      */
      virtual bool AddScene(SceneBase&);
      SceneArray& GetChildArray();
      const SceneArray& GetChildArray() const;

   private:
      

      SceneGroupImpl* mImpl;

   };

}

#endif // DELTA_SCENEGROUP_H
