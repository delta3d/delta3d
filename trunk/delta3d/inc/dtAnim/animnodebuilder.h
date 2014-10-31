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
 * Bradley Anderegg 03/28/2007
 */

#ifndef __DELTA_ANIMNODEBUILDER_H__
#define __DELTA_ANIMNODEBUILDER_H__

#include <dtAnim/export.h>
#include <dtAnim/basemodelwrapper.h>
#include <dtAnim/nodebuilderinterface.h>
#include <dtAnim/geometrybuilder.h>
#include <dtCore/refptr.h>
#include <dtUtil/functor.h>

#include <osg/Referenced>
#include <osg/Node> // needed for the bounding sphere callback

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Geode;
   class Program;
}
/// @endcond

namespace dtCore
{
   class ShaderProgram;
}


namespace dtAnim
{
   class BaseModelData;
   class Array;

   /**
    * Class used to generate the renderable geometry for an animated character.  This
    * class makes use of a pointer to a function which will do the actual building
    * of the geometry.  By default, AnimNodeBuilder will try to use the default
    * CreateSoftware(), CreateHardware, or CreateNULL() methods.  Call SetCreate()
    * to supply a custom create method.
    */
   class DT_ANIM_EXPORT AnimNodeBuilder: public osg::Referenced
   {
   public:
      /**
       * Prototype of the Create method.  Returns the Node containing the animated
       * character's geometry.
       * @code dtCore::RefPtr<osg::Node> MyCreateFunc(osg::Geode& geode, osg::RenderInfo* renderInfo, BaseModelWrapper* wrapper);
       * @endcode
       * @see CreateNode()
       */
      typedef dtUtil::Functor<dtCore::RefPtr<osg::Node>, TYPELIST_2(osg::RenderInfo*, dtAnim::BaseModelWrapper*)> CreateFunc;

      AnimNodeBuilder(bool useDeprecatedHardwareModel = false); //creates default builder
      AnimNodeBuilder(const CreateFunc& pCreate); //uses custom builder
      
      dtCore::RefPtr<dtAnim::NodeBuilderInterface> CreateNodeBuilder(const std::string& charSystem);
      dtCore::RefPtr<dtAnim::NodeBuilderInterface> CreateNodeBuilder(dtAnim::BaseModelWrapper& wrapper);

      /// @return the create function
      CreateFunc& GetCreate();

      /**
       * Set a custom CreateFunc for the AnimNodeBuilder.  Function
       * must remove the temporary geometry and drawcallback supplied
       * by CreateNode.
       * @param pCreate : the custom create function.
       */
      void SetCreate(const CreateFunc& pCreate);

      /**
       * Create the node that holds the animated character's geometry.  Will return
       * a valid Node which contains temporary geometry.  Actual character geometry
       * might not be present until the returned node is rendered in a Scene.
       * @param pWrapper : Pointer to the BaseModelWrapper to be used when building
       * the geometry.
       *
       * @param immediate : Optional parameter, when set to true, will immediately
       * create the character's geometry.  This requires a valid OpenGL Context.
       * When false, will defer the creation until a valid OpenGL Context is present.
       *
       * @return : RefPtr of a osg::Node which will contain the renderable geometry.  Temporary
       * geometry is a osg::Group with a child osg::Geode which contains one osg::Drawable
       * that has a Drawcallback assigned to it.
       */
      dtCore::RefPtr<osg::Node> CreateNode(dtAnim::BaseModelWrapper* wrapper, bool immediate = false);

      virtual dtCore::RefPtr<osg::Node> CreateSoftware(osg::RenderInfo* renderInfo, dtAnim::BaseModelWrapper* wrapper);
      virtual dtCore::RefPtr<osg::Node> CreateSoftwareNoVBO(osg::RenderInfo* renderInfo, dtAnim::BaseModelWrapper* wrapper);
      virtual dtCore::RefPtr<osg::Node> CreateHardware(osg::RenderInfo* renderInfo, dtAnim::BaseModelWrapper* wrapper);
      virtual dtCore::RefPtr<osg::Node> CreateNULL(osg::RenderInfo* renderInfo, dtAnim::BaseModelWrapper* wrapper);


      ///Does the hardware support hardware skinning?
      bool SupportsHardware() const;

      ///Does the hardware support software skinning?
      bool SupportsSoftware() const;

      static dtCore::ShaderProgram* LoadShaders(dtAnim::BaseModelData& modelData, osg::Geode& geode);

   protected:
      virtual ~AnimNodeBuilder();
      AnimNodeBuilder(const AnimNodeBuilder&);
      AnimNodeBuilder& operator=(const AnimNodeBuilder&);

   private:
      template <typename T>
      class Array
      {
      public:
        typedef T value_type;

        Array(size_t size = 0): mArray(NULL)
        {
          if (size > 0)
            mArray = new T[size];
        }

        ~Array()
        {
          delete[] mArray;
        }

        T& operator[](size_t index)
        {
          return mArray[index];
        }

        T* mArray;
      };

      CreateFunc mCreateFunc;

      ///Does the hardware support vertex buffers?
      bool SupportsVertexBuffers() const;

      bool mUseDeprecatedHardwareModel;

      static GeometryBuilder mGeometryBuilder;
   };

} // namespace dtAnim

#endif // __DELTA_ANIMNODEBUILDER_H__
