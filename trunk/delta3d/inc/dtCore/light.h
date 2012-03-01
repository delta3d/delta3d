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

#ifndef DELTA_LIGHT
#define DELTA_LIGHT

#include <dtCore/export.h>
#include <dtUtil/macros.h>
#include <dtCore/transformable.h>

#include <osg/Vec4>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class LightSource;
}
/// @endcond

namespace dtCore
{
   const int MAX_LIGHTS = 8;

   class Scene;
   /**
   * An abstract class declaring the base functionality for a vertex light.
   * It's properties are thin wrappers of OpenGL light calls.
   */
   class DT_CORE_EXPORT Light : public Transformable
   {

      DECLARE_MANAGEMENT_LAYER(Light)

   public:

      ///GLOBAL lights everything in scene, LOCAL only lights children
      enum LightingMode
      {
         GLOBAL = 0,
         LOCAL  = 1
      };

      /**
       * Constructor
       *
       * @param number: the light number, 0-7, this will overright any other light with that number
       * @param name: a name for the light, defaulted to defaultLight
       * @param mode: specifys a lighting mode, GLOBAL effects whole scene, LOCAL only effects children
       */
      Light(int number, const std::string& name = "defaultLight", LightingMode mode = GLOBAL);

      ///Copy constructor from an osg::LightSource
      Light(const osg::LightSource& lightSource, const std::string& name = "defaultLight", LightingMode mode = GLOBAL);

   protected:

      virtual ~Light() = 0;

      ///Called from the constructor to setup any default Light attributes
      virtual void ApplyDefaults();

   public:

      ///Get the const internal osg::LightSource
      const osg::LightSource* GetLightSource() const;

      ///Get the non-const internal osg::LightSource
      osg::LightSource* GetLightSource();

      ///Change lighting mode to GLOBAL or LOCAL mode
      void SetLightingMode(LightingMode mode);

      LightingMode GetLightingMode() const { return mLightingMode; }

      ///Turn light on or off
      void SetEnabled(bool enabled);

      bool GetEnabled() { return mEnabled; }

      /**
       * Sets the OpenGL light number associated with this light. Lights numbers
       * must be in the range 0-7. There is a default scene light setup at light
       * number 0, so that will be overwritten if you set your own light to 0.
       *
       * @param number : The OpenGL light number
       */
      void SetNumber(int number);

      ///Returns the number of the light as specified in the constructor
      int GetNumber() const;

      ///sets the ambient light color
      void SetAmbient(float r, float g, float b, float a);
      void SetAmbient(const osg::Vec4& rgba);

      ///gets the ambient light color
      void GetAmbient(float& r, float& g, float& b, float& a) const;
      const osg::Vec4& GetAmbient() const;

      ///sets the diffuse light color
      void SetDiffuse(float r, float g, float b, float a);
      void SetDiffuse(const osg::Vec4& rgba);

      ///Gets the diffuse light color
      void GetDiffuse(float& r, float& g, float& b, float& a) const;
      const osg::Vec4& GetDiffuse() const;

      ///sets the specular light color
      void SetSpecular(float r, float g, float b, float a);
      void SetSpecular(const osg::Vec4& rgba);

      ///gets the specular light color
      void GetSpecular(float& r, float& g, float& b, float& a) const;
      const osg::Vec4& GetSpecular() const;

      ///adds the light to the root node of the scene
      virtual void AddedToScene(Scene* scene);

      ///adds a drawable as a child of this node
      virtual bool AddChild(DeltaDrawable* child);
      ///removes a drawable as a child of this node
      virtual void RemoveChild(DeltaDrawable* child);

   protected:

      LightingMode mLightingMode;
      RefPtr<osg::LightSource> mLightSource;
      bool mEnabled;
   };
}



#endif // DELTA_LIGHT
