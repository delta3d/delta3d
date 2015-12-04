/* -*-c++-*-
 * dtRender
 * Copyright 2014, Caper Holdings LLC
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
 * This software was developed by Alion Science and Technology Corporation under
 * circumstances in which the U. S. Government may have rights in the software.
 * 
 * Bradley Anderegg
 */


#include <dtRender/optimizeractcomp.h>
#include <dtGame/gameactor.h>
#include <dtGame/message.h>

#include <dtCore/propertymacros.h>
#include <dtUtil/cullmask.h>

#include <osg/NodeVisitor>
#include <osgUtil/Optimizer>
#include <osg/Geode>
#include <osg/OcclusionQueryNode>
#include <osg/Texture2D>
#include <osg/Texture3D>

namespace dtRender
{

   //class for managing using a traversal mask
   class TraversalMaskComparison: public osgUtil::Optimizer::IsOperationPermissibleForObjectCallback
   {
   public:
      TraversalMaskComparison(unsigned int mask)
         : mTraversalMask(mask)
      {

      }

      virtual bool isOperationPermissibleForObjectImplementation(const osgUtil::Optimizer* optimizer, const osg::Node* node,unsigned int option) const
      {
         return (mTraversalMask & node->getNodeMask()) != 0;
      }

      unsigned int mTraversalMask;
   };


   

   /////////////////////////////////////////////////////////////////////
   class CompressTexturesVisitor : public osg::NodeVisitor
   {
   public:

      CompressTexturesVisitor(osg::Texture::InternalFormatMode internalFormatMode):
         osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN),
         _internalFormatMode(internalFormatMode) {}

      virtual void apply(osg::Node& node)
      {
         if (node.getStateSet()) apply(*node.getStateSet());
         traverse(node);
      }

      virtual void apply(osg::Geode& node)
      {
         if (node.getStateSet()) apply(*node.getStateSet());

         for(unsigned int i=0;i<node.getNumDrawables();++i)
         {
            osg::Drawable* drawable = node.getDrawable(i);
            if (drawable && drawable->getStateSet()) apply(*drawable->getStateSet());
         }

         traverse(node);
      }

      virtual void apply(osg::StateSet& stateset)
      {
         // search for the existence of any texture object attributes
         for(unsigned int i=0;i<stateset.getTextureAttributeList().size();++i)
         {
            osg::Texture* texture = dynamic_cast<osg::Texture*>(stateset.getTextureAttribute(i,osg::StateAttribute::TEXTURE));
            if (texture)
            {
               _textureSet.insert(texture);
            }
         }
      }

      void compress()
      {
         osg::ref_ptr<osg::State> state = new osg::State;

         for(TextureSet::iterator itr=_textureSet.begin();
            itr!=_textureSet.end();
            ++itr)
         {
            osg::Texture* texture = const_cast<osg::Texture*>(itr->get());

            osg::Texture2D* texture2D = dynamic_cast<osg::Texture2D*>(texture);
            osg::Texture3D* texture3D = dynamic_cast<osg::Texture3D*>(texture);

            osg::ref_ptr<osg::Image> image = texture2D ? texture2D->getImage() : (texture3D ? texture3D->getImage() : 0);
            if (image.valid() &&
               (image->getPixelFormat()==GL_RGB || image->getPixelFormat()==GL_RGBA) &&
               (image->s()>=32 && image->t()>=32))
            {
               texture->setInternalFormatMode(_internalFormatMode);

               // need to disable the unref after apply, other the image could go out of scope.
               bool unrefImageDataAfterApply = texture->getUnRefImageDataAfterApply();
               texture->setUnRefImageDataAfterApply(false);

               // get OpenGL driver to create texture from image.
               texture->apply(*state);

               // restore the original setting
               texture->setUnRefImageDataAfterApply(unrefImageDataAfterApply);

               image->readImageFromCurrentTexture(0,true);

               texture->setInternalFormatMode(osg::Texture::USE_IMAGE_DATA_FORMAT);
            }
         }
      }

      typedef std::set< osg::ref_ptr<osg::Texture> > TextureSet;
      TextureSet                          _textureSet;
      osg::Texture::InternalFormatMode    _internalFormatMode;

   };


   // Use this visitor to insert OcclusionQueryNodes (OQNs) in the
   //   visited subgraph. Only one OQN will test any particular node
   //   (no nesting). See also OcclusionQueryNonFlatVisitor.
   class OcclusionQueryVisitor : public osg::NodeVisitor
   {
   public:
      OcclusionQueryVisitor();
      virtual ~OcclusionQueryVisitor();

      // Specify the vertex count threshold for performing occlusion
      //   query tests. Nodes in the scene graph whose total child geometry
      //   contains fewer vertices than the specified threshold will
      //   never be tested, just drawn. (In fact, they will br treated as
      //   potential occluders and rendered first in front-to-back order.)
      void setOccluderThreshold( int vertices );
      int getOccluderThreshold() const;

      virtual void apply( osg::OcclusionQueryNode& oqn );
      virtual void apply( osg::Group& group );
      virtual void apply( osg::Geode& geode );

   protected:
      void addOQN( osg::Node& node );

      // When an OQR creates all OQNs and each OQN shares the same OQC,
      //   these methods are used to uniquely name all OQNs. Handy
      //   for debugging.
      std::string getNextOQNName();
      int getNameIdx() const { return _nameIdx; }

      osg::ref_ptr<osg::StateSet> _state;
      osg::ref_ptr<osg::StateSet> _debugState;

      unsigned int _nameIdx;

      int _occluderThreshold;
   };

   unsigned int countGeometryVertices( osg::Geometry* geom )
   {
      if (!geom->getVertexArray())
         return 0;

      // TBD This will eventually iterate over the PrimitiveSets and total the
      //   number of vertices actually used. But for now, it just returns the
      //   size of the vertex array.

      return geom->getVertexArray()->getNumElements();
   }

   class VertexCounter : public osg::NodeVisitor
   {
   public:
      VertexCounter( int limit )
         : osg::NodeVisitor( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ),
         _limit( limit ),
         _total( 0 ) {}
      ~VertexCounter() {}

      int getTotal() { return _total; }
      bool exceeded() const { return _total > _limit; }
      void reset() { _total = 0; }

      virtual void apply( osg::Node& node )
      {
         // Check for early abort. If out total already exceeds the
         //   max number of vertices, no need to traverse further.
         if (exceeded())
            return;
         traverse( node );
      }

      virtual void apply( osg::Geode& geode )
      {
         // Possible early abort.
         if (exceeded())
            return;

         unsigned int i;
         for( i = 0; i < geode.getNumDrawables(); i++ )
         {
            osg::Geometry* geom = dynamic_cast<osg::Geometry *>(geode.getDrawable(i));
            if( !geom )
               continue;

            _total += countGeometryVertices( geom );

            if (_total > _limit)
               break;
         }
      }

   protected:
      int _limit;
      int _total;
   };



   OcclusionQueryVisitor::OcclusionQueryVisitor()
      : osg::NodeVisitor( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ),
      _nameIdx( 0 ),
      _occluderThreshold( 5000 )
   {
      // Create a dummy OcclusionQueryNode just so we can get its state.
      // We'll then share that state between all OQNs we add to the visited scene graph.
      osg::ref_ptr<osg::OcclusionQueryNode> oqn = new osg::OcclusionQueryNode;

      _state = oqn->getQueryStateSet();
      _debugState = oqn->getDebugStateSet();
   }

   OcclusionQueryVisitor::~OcclusionQueryVisitor()
   {
      osg::notify( osg::INFO ) <<
         "osgOQ: OcclusionQueryVisitor: Added " << getNameIdx() <<
         " OQNodes." << std::endl;
   }

   void
      OcclusionQueryVisitor::setOccluderThreshold( int vertices )
   {
      _occluderThreshold = vertices;
   }
   int
      OcclusionQueryVisitor::getOccluderThreshold() const
   {
      return _occluderThreshold;
   }

   void
      OcclusionQueryVisitor::apply( osg::OcclusionQueryNode& oqn )
   {
      // A subgraph is already under osgOQ control.
      // Don't traverse further.
      return;
   }

   void
      OcclusionQueryVisitor::apply( osg::Group& group )
   {
      if (group.getNumParents() == 0)
      {
         // Can't add an OQN above a root node.
         traverse( group );
         return;
      }

      int preTraverseOQNCount = getNameIdx();
      traverse( group );

      if (getNameIdx() > preTraverseOQNCount)
         // A least one OQN was added below the current node.
            //   Don't add one here to avoid hierarchical nesting.
               return;

      // There are no OQNs below this group. If the vertex
      //   count exceeds the threshold, add an OQN here.
      addOQN( group );
   }

   void
      OcclusionQueryVisitor::apply( osg::Geode& geode )
   {
      if (geode.getNumParents() == 0)
      {
         // Can't add an OQN above a root node.
         traverse( geode );
         return;
      }

      addOQN( geode );
   }

   void
      OcclusionQueryVisitor::addOQN( osg::Node& node )
   {
      VertexCounter vc( _occluderThreshold );
      node.accept( vc );
      if (vc.exceeded())
      {
         // Insert OQN(s) above this node.
         unsigned int np = node.getNumParents();
         while (np--)
         {
            osg::Group* parent = dynamic_cast<osg::Group*>( node.getParent( np ) );
            if (parent != NULL)
            {
               osg::ref_ptr<osg::OcclusionQueryNode> oqn = new osg::OcclusionQueryNode();
               oqn->addChild( &node );
               parent->replaceChild( &node, oqn.get() );

               oqn->setName( getNextOQNName() );
               // Set all OQNs to use the same query StateSets (instead of multiple copies
               //   of the same StateSet) for efficiency.
               oqn->setQueryStateSet( _state.get() );
               oqn->setDebugStateSet( _debugState.get() );
            }
         }
      }
   }

   std::string
      OcclusionQueryVisitor::getNextOQNName()
   {
      std::ostringstream ostr;
      ostr << "OQNode_" << _nameIdx++;
      return ostr.str();
   }




   const dtGame::ActorComponent::ACType OptimzerActComp::TYPE(new dtCore::ActorType("OptimzerActComp", "ActorComponents",
          "An actor component which runs the optimizer on the actors node.",
          dtGame::ActorComponent::BaseActorComponentType));

   const dtUtil::RefString OptimzerActComp::PROPERTY_OPTIMIZER_COMP_NAME("Optimzer Actor Component Name");

   const dtUtil::RefString OptimzerActComp::INVOKABLE_MAP_LOADED("Map Loaded");

   
   /////////////////////////////////////////////////////////////////////////////
   OptimzerActComp::OptimzerActComp()
   : dtGame::ActorComponent(OptimzerActComp::TYPE)
   , mSpatializeGroups(false)
   , mFlattenStaticTransforms(false)
   , mShareDuplicateState(false)
   , mMergeGeometry(false)
   , mCheckGeometry(false)
   , mDefaultOSGOptimizations(false)
   , mOptimizerOptionsOverride(0)
   , mCompressTextures(false)
   , mMinVertsForOccluders(1000)
   , mCreateOccluders(false)
   , mTraversalMask(dtUtil::CullMask::SCENE_INTERSECT_MASK)
   , mName(PROPERTY_OPTIMIZER_COMP_NAME)
   {
   }

   OptimzerActComp::~OptimzerActComp()
   {
      // Cleanup now happens in OnRemovedFromWorld.  It can't happen here because it's virtual.
   }

   void OptimzerActComp::SetName(const dtUtil::RefString& n)
   {
      mName = n;
   }

   const dtUtil::RefString& OptimzerActComp::GetName() const
   {
      return mName;
   }

   
   void OptimzerActComp::OnEnteredWorld()
   {
      BaseClass::OnEnteredWorld();

      RegisterForMapLoaded();
   }

   void OptimzerActComp::OnRemovedFromWorld()
   {
      CleanUp();
   }

   void OptimzerActComp::CleanUp()
   {

   }

   void OptimzerActComp::OnAddedToActor(dtCore::BaseActorObject& actor)
   {
      
   }

   void OptimzerActComp::OnRemovedFromActor(dtCore::BaseActorObject& actor)
   {
   }

   void OptimzerActComp::Optimize()
   {
      dtGame::GameActorProxy* act = NULL;
      GetOwner(act);

      if(act->GetDrawable() != NULL )
      {
         osg::Node* rootNode = act->GetDrawable()->GetOSGNode();
         osgUtil::Optimizer opt;
         
         LOG_DEBUG("Optimizer Actor Component running optimizations.");

         if(mTraversalMask != 0)
         {
            opt.setIsOperationPermissibleForObjectCallback(new TraversalMaskComparison(mTraversalMask));
         }

         if(mOptimizerOptionsOverride != 0)
         {
            opt.optimize(rootNode, osgUtil::Optimizer::OptimizationOptions(mOptimizerOptionsOverride));
         }
         else if (mDefaultOSGOptimizations)
         {
            opt.optimize(rootNode, osgUtil::Optimizer::DEFAULT_OPTIMIZATIONS);
         }
         else
         {
            if(mFlattenStaticTransforms)
            {
               opt.optimize(rootNode, osgUtil::Optimizer::STATIC_OBJECT_DETECTION | osgUtil::Optimizer::FLATTEN_STATIC_TRANSFORMS_DUPLICATING_SHARED_SUBGRAPHS);
            }

            if(mMergeGeometry)
            {
               opt.optimize(rootNode, osgUtil::Optimizer::MERGE_GEODES);
            }
         
            if(mCheckGeometry)
            {
               opt.optimize(rootNode, osgUtil::Optimizer::CHECK_GEOMETRY);
            }

            if(mSpatializeGroups)
            {
               opt.optimize(rootNode, osgUtil::Optimizer::SPATIALIZE_GROUPS);
            }

            if(mShareDuplicateState)
            {
               opt.optimize(rootNode, osgUtil::Optimizer::SHARE_DUPLICATE_STATE);
            }
         }

         if(mCompressTextures)
         {
            LOG_ALWAYS("Optimizer is compressing textures.");
            CompressTexturesVisitor ctv(osg::Texture::USE_ARB_COMPRESSION);
            ctv.setTraversalMask(mTraversalMask);
            rootNode->accept(ctv);
            ctv.compress();  
            LOG_ALWAYS("Done compressing textures.");
         }

         if(mCreateOccluders)
         {
            LOG_ALWAYS("Optimizer is creating occluders.");

            OcclusionQueryVisitor oqv;
            oqv.setOccluderThreshold(mMinVertsForOccluders);
            oqv.setTraversalMask(mTraversalMask);
            rootNode->accept( oqv );

            LOG_ALWAYS("Done creating occluders.");
         }

         LOG_DEBUG("Optimizer Actor Component finished optimizing actor subgraph");
      }
      else
      {
         LOG_ERROR("Unable to run optimizer on base actor, drawable is NULL.");
      }
   }
     
   void OptimzerActComp::BuildPropertyMap()
   {
      static const dtUtil::RefString GROUP("OptimzerActComp");

      dtGame::GameActorProxy* actor = NULL;
      GetOwner(actor);

      typedef dtCore::PropertyRegHelper<dtCore::PropertyContainer&, OptimzerActComp> RegHelperType;
      RegHelperType propReg(*actor, this, GROUP);

      DT_REGISTER_PROPERTY(SpatializeGroups, "Reorganizes the scene spatially for optimizing culling and traversal time.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(FlattenStaticTransforms, "Pushes static transforms through to geometry, removes high cost traversal of deep scenes with many transforms.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(ShareDuplicateState, "Combines objects with the same state set to actually share a state instance optimizes drawing time.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(MergeGeometry, "Merges geometry with shared state sets to optimize number of draw calls, cpu draw time.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(CheckGeometry, "Checks for legitimate geometry, good for debugging problems with a mesh.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(DefaultOSGOptimizations, "Runs far more options for optimization, for more specific optimizations use the override.", RegHelperType, propReg);
      DT_REGISTER_PROPERTY(OptimizerOptionsOverride, "Set this with a custom mask of optimizer options to override.", RegHelperType, propReg);

      //DT_REGISTER_PROPERTY(CompressTextures, "Set this option to run a visitor to compress the textures.", RegHelperType, propReg);
      
      //DT_REGISTER_PROPERTY(CreateOccluders, "Set this option to run a visitor which will create occluders.", RegHelperType, propReg);
      //DT_REGISTER_PROPERTY(MinVertsForOccluders, "This option controls the minimum number of vertices required to be an occluders.", RegHelperType, propReg);

      
      DT_REGISTER_PROPERTY(TraversalMask, "Sets the mask used for traversal, by default it will optimize all scene geometry below it.", RegHelperType, propReg);

   }


   //////////////////////////////////////////////////////////////////////////
   void OptimzerActComp::RegisterForMapLoaded()
   {
      dtGame::GameActorProxy* owner = NULL;
      GetOwner(owner);
      if (!owner->IsRemote())
      {
         std::string tickInvokable = "Map Loaded " + GetType()->GetFullName();
         if(!owner->GetInvokable(tickInvokable))
         {
            owner->AddInvokable(*new dtGame::Invokable(tickInvokable, dtUtil::MakeFunctor(&OptimzerActComp::OnAddedMap, this)));
         }
         owner->RegisterForMessages(dtGame::MessageType::INFO_MAP_CHANGE_LOAD_END, tickInvokable);
      }
   }

   void OptimzerActComp::SetNameByString(const std::string& name)
   {
      mName = name;
   }

   const std::string& OptimzerActComp::GetNameAsString() const
   {
      return mName;
   }

   void OptimzerActComp::OnAddedMap(const dtGame::Message&)
   {
      Optimize();
   }


} // namespace dtRender
