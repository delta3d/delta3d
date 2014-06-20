/*
 * Delta3D Open Source Game and Simulation Engine
 * Copyright (C) 2005, BMH Associates, Inc.
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
 * David Guthrie
 */
#ifndef DELTA_STATIC_MESH_ACTOR_PROXY
#define DELTA_STATIC_MESH_ACTOR_PROXY

#include <osg/Texture2D>
#include <dtCore/plugin_export.h>
#include <dtActors/deltaobjectactorproxy.h>
#include <dtUtil/getsetmacros.h>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
    class Texture2D;
}
/// @endcond

namespace dtActors
{
    /**
     * @class StaticMeshActorProxy
     * This proxy represents a static mesh. A static mesh contains geometry and
     * textures representing an object in the scene such as a chair,a desk, a
     * tree, a boulder, etc. Note that this class will instance geometry. So
     * changes to one model at runtime will happen to all of them. Since this
     * class is normally used for static objects (trees, buildings, etc) if you
     * are using the same resource then you probably want to share geometry in
     * memory for efficiency.
     */
    class DT_PLUGIN_EXPORT StaticMeshActorProxy : public DeltaObjectActorProxy
    {
    public:

        // This class is not actually used in StaticMeshActorProxy, so it's #ifdef'd out
        // for now to avoid linker warnings. -osb
        #if 0

        /**
         * @class TextureEntry
         * This is a simple class which is used by the texture resource properties to
         * load textures for the static mesh.
         */
        class TextureEntry : public osg::Referenced
        {
        public:
            /**
             * Constructs the texture entry.
             * @param tex2D The texture object mapped to this entry.
             */
            TextureEntry(osg::Texture2D *tex2D) : mTexture(tex2D) { }

            /**
             * Loads a new texture image and uploads it to the texture object in this
             * entry.
             * @param fileName File name of the image to load.
             */
            void LoadFile(const std::string &fileName);

        protected:
            virtual ~TextureEntry() { }

        private:
            osg::ref_ptr<osg::Texture2D> mTexture;
        };

        #endif // 0

        StaticMeshActorProxy() { SetClassName("dtCore::Object"); }

        /**
         * Adds the properties for static mesh actors.
         */
        virtual void BuildPropertyMap();

        DT_DECLARE_ACCESSOR(dtCore::ResourceDescriptor, StaticMesh);

        /**
         * Gets the method by which this static mesh is rendered.
         * @return If there is no geometry currently assigned, this
         *  method will return RenderMode::DRAW_BILLBOARD_ICON.  If
         *  there is geometry assigned to this static mesh, RenderMode::DRAW_ACTOR
         *  is returned.
         */
        virtual const dtCore::BaseActorObject::RenderMode& GetRenderMode();

        /**
         * Gets the billboard used to represent static meshes if this proxy's
         * render mode is RenderMode::DRAW_BILLBOARD_ICON.
         * @return
         */
        virtual dtCore::ActorProxyIcon* GetBillBoardIcon();

    protected:

        /**
         * Creates a dtCore::Object and assigns a default name to it.
         */
        virtual void CreateDrawable();

        /**
         * Destructor
         */
        virtual ~StaticMeshActorProxy();

    private:
        #if 0
        ///List of textures that can be overridden on the static mesh.
        std::vector<osg::ref_ptr<TextureEntry> > mTextureSlots;
        #endif
    };
}

#endif
