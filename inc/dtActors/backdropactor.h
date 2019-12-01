#ifndef DELTA_BACKDROP_ACTOR_H__
#define DELTA_BACKDROP_ACTOR_H__

#include <dtCore/model.h>
#include <dtCore/transformable.h>
#include <dtCore/transformableactorproxy.h>
#include <dtCore/plugin_export.h> 

#include <osg/Geode>
#include <osg/Texture2D>

namespace dtActors
{

/**
 * A BackdropActor is a quad-shaped geometry that a texture can be placed on.
 * Its purpose is to provide a means for viewing something far off in the
 * distance without have to model it explicitly in the game.
 */

class DT_PLUGIN_EXPORT BackdropActor : public dtCore::Transformable   
{
public:
   BackdropActor(const std::string& name = "Backdrop Actor");
   
   /**
    *  Gets the Scale from the BackdropActor's scaling matrix.
    */
   osg::Vec3 GetScale() const;

   /**
    *  Sets the Scale on the BackdropActor's scaling matrix.
    */
   void SetScale(const osg::Vec3& xyz);

   /**
    *  Sets the the BackdropActor's front texture.
    *  @param  path  Path to the texture image file on disk.
    */
   void SetFrontTexture(const std::string& path);

   /**
    *  Sets the BackdropActors's back texture.
    *  @param  path  Path to the texture image file on disk.
    */
   void SetBackTexture(const std::string& path);

protected:
   virtual ~BackdropActor();
   //helper method to create quad
   void CreateBackdropGeometry();
   //helper method to load textures
   dtCore::RefPtr<osg::Texture2D> LoadTexture(const std::string& path);

private:
   dtCore::RefPtr<osg::Geode>           mFrontQuad;
   dtCore::RefPtr<osg::Geode>           mBackQuad;

   //used so I can scale (see GameMeshActor -- where I stole this idea from)
   dtCore::RefPtr<dtCore::Model>       mModel;
  
};


////////////////////////////////////////////////////////////////////////////////
// And now the proxy:
////////////////////////////////////////////////////////////////////////////////
class DT_PLUGIN_EXPORT BackdropActorProxy : public dtCore::TransformableActorProxy
{
public:
   BackdropActorProxy();

   /**
    *  Required to add properties.
    */
   void BuildPropertyMap();

   /**
    *  Wrapper that points to BackdropActor::SetFrontTexture
    */
   void SetFrontTexture(const std::string &path);

   /// Wrapper that points to BackdropActor::SetBackTexture
   void SetBackTexture(const std::string &path);   

protected:   
   /// Initializes the Actor
   void CreateDrawable();

};


} //namespace dtActors

#endif //DELTA_BACKDROP_ACTOR_H__
