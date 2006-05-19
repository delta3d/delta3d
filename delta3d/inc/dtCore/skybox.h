/* -*-c++-*- 
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
 *@author Bradley Anderegg
*/

#ifndef DELTA_SKYBOX
#define DELTA_SKYBOX

#include <dtCore/enveffect.h>
//#include <dtCore/skydome.h>
#include <dtCore/refptr.h>

#include <osg/CameraNode>
#include <osg/Drawable>
#include <osg/NodeCallback>

/// @cond DOXYGEN_SHOULD_SKIP_THIS
namespace osg
{
   class Group;
   class NodeCallback;
   class Texture2D;
   class TextureCubeMap;
}
/// @endcond

namespace dtCore
{
   class MoveEarthySkyWithEyePointTransform;

   ///A six-sided textured cube to represent distant scenery

   /** The SkyBox is an Environmental Effect that is very useful in certain
   * applications.  Essentially, its a large 6-sided cube with the Camera
   * centered in the middle.  Each side of the cube has a texture applied to
   * it representing far away scenery (sky, clouds, buildings, hills, etc.).
   * To use this class, create an instance of it, pass in the texture filenames
   * using SetTextureFilename(),
   * then add it to a Environment using Environment::AddEffect().  The terms
   * "front", "right", etc. map to compass headings.  For example, front is 
   * north, right is east, top is up.
   *
   * The resolution of the textures should be based on the screen resolution
   * and the field of view ( texRes = screenRes / tan(fov/2) ) for a
   * texel per pixel correlation.
   * \code
   SkyBox *box = new SkyBox("skyBox");
   box->SetTexture( SkyBox::SKYBOX_FRONT, "front.bmp");
   box->SetTexture( SkyBox::SKYBOX_BACK, "back.bmp");
   ...
   Environment *env = new Environment("myEnv");
   env->AddEffect( box );
   * \endcode
   *
   * Typically, this would be the only environmental effect added to the
   * Environment.  Adding fog, sun, and clouds typically make things look a 
   * little weird.
   */
class DT_CORE_EXPORT SkyBox : public EnvEffect
{
protected:
   class RenderProfile;
private:
   DECLARE_MANAGEMENT_LAYER(SkyBox)

public:
	/**This selects the render method for the SkyBox
	//RP_DEFAULT will use a cubemap if you can support it
	//otherwise it will use the old technique with a textured cube
	//the angular map will only work with angular textures
	//as those used with hdr, they are called light probes
	//http://www.debevec.org/Probes/
	*/
	enum RenderProfileEnum
	{
		RP_FIXED_FUNCTION = 0,
		RP_CUBE_MAP,
		RP_ANGULAR_MAP,
		RP_DEFAULT,
		RP_COUNT
	};

	/**
	//SkyBoxSideEnum selects the side of the cube to texture
	//if RenderProfileEnum is set to RP_ANGULAR_MAP, this is irrelevant
	//and ignores the side
	*/
       enum SkyBoxSideEnum
       {
		SKYBOX_FRONT = 0,
		SKYBOX_RIGHT,
		SKYBOX_BACK,      
		SKYBOX_LEFT,
		SKYBOX_TOP,
		SKYBOX_BOTTOM
	};

   class ConfigCallback: public osg::NodeCallback
   {
   public:
      ConfigCallback(SkyBox* mp):mSkyBox(mp){}

      void operator()(osg::Node*, osg::NodeVisitor* nv)
      {				
         mSkyBox->Config();
      }
   private:
      SkyBox* mSkyBox; ///< Weak pointer to SkyBox to prevent circular reference
   };

   friend class ConfigCallback;

public:
   SkyBox(  const std::string& name="SkyBox", 
            RenderProfileEnum pRenderProfile = RP_DEFAULT );
protected:
   virtual ~SkyBox();

public:
   /// Must override this to supply the repainting routine
   virtual void Repaint(   const osg::Vec3& skyColor, 
                           const osg::Vec3& fogColor,
                           double sunAngle, 
                           double sunAzimuth,
                           double visibility );

   /// Set the texture for this side of the skybox
   void SetTexture(SkyBoxSideEnum side, const std::string& filename);

protected:
	
	virtual void Config();
	virtual void CheckHardware();
	virtual void SetRenderProfile(RenderProfileEnum pRenderProfile);

	RenderProfileEnum mRenderProfilePreference;
	bool mSupportedProfiles[RP_COUNT];
   dtCore::RefPtr<RenderProfile> mRenderProfile;
	
	bool mInitializedTextures;
	std::string mTexList[6];
	bool mTexPreSetList[6];

   dtCore::RefPtr<osg::Geode> mTempGeode;

	///this is a custom drawable for the AngularMapProfile
	///and the CubeMapProfile
	class DT_CORE_EXPORT SkyBoxDrawable: public osg::Drawable
	{
	public:

		META_Object(osg::Drawable, SkyBoxDrawable);
		SkyBoxDrawable(const SkyBoxDrawable& bd, const osg::CopyOp& copyop=osg::CopyOp::SHALLOW_COPY)
		{
		}

		SkyBoxDrawable(){setUseDisplayList(false);}

		/*virtual*/ void drawImplementation(osg::State& state) const;
	};

	///a base class to allow the user to choose different ways
	///to render the same scene
	class RenderProfile: public osg::Referenced
	{
	public:
		virtual void Config(osg::Group*) = 0;
		virtual void SetTexture(SkyBoxSideEnum side, const std::string& filename) = 0;	
	};

	///this class will use an angular map or light probe
	///to act as a skybox
	class DT_CORE_EXPORT AngularMapProfile: public SkyBox::RenderProfile
	{
	protected:
		class UpdateViewCallback: public osg::NodeCallback
		{
		public:
			UpdateViewCallback(AngularMapProfile* mp):mProfile(mp){}

			void operator()(osg::Node*, osg::NodeVisitor* nv)
			{
				if(osg::CameraNode* cn = dynamic_cast<osg::CameraNode*>(nv->getNodePath()[0]))
				{
					mProfile->UpdateViewMatrix(cn->getViewMatrix(), cn->getProjectionMatrix());
				}
			}
		private:
			dtCore::RefPtr<AngularMapProfile> mProfile;
		};

		friend class UpdateViewCallback;

	public:
		AngularMapProfile();

		void Config(osg::Group*);
		void SetTexture(SkyBoxSideEnum side, const std::string& filename);	

	protected:

		void UpdateViewMatrix(const osg::Matrix& viewMat, const osg::Matrix& projMat);

		dtCore::RefPtr<osg::Geode>			   mGeode;
		dtCore::RefPtr<osg::Texture2D>		mAngularMap;
		dtCore::RefPtr<osg::Program>			mProgram;
		dtCore::RefPtr<osg::Uniform>			mInverseModelViewProjMatrix;

	};

	///this class will use a 2D ortho quad and lookup
	///into a cubemap to find the texture value
	class DT_CORE_EXPORT CubeMapProfile: public SkyBox::RenderProfile
	{
	protected:
		class UpdateViewCallback: public osg::NodeCallback
		{
		public:
			UpdateViewCallback(CubeMapProfile* mp):mProfile(mp){}

			void operator()(osg::Node*, osg::NodeVisitor* nv)
			{				
				if(osg::CameraNode* cn = dynamic_cast<osg::CameraNode*>(nv->getNodePath()[0]))
				{
					mProfile->UpdateViewMatrix(cn->getViewMatrix(), cn->getProjectionMatrix());
				}
			}
		private:
         dtCore::RefPtr<CubeMapProfile> mProfile;
		};

		friend class UpdateViewCallback;

	public:
		CubeMapProfile();
		void Config(osg::Group*);
		void SetTexture(SkyBoxSideEnum side, const std::string& filename);	

	protected:
		void UpdateViewMatrix(const osg::Matrix& viewMat, const osg::Matrix& projMat);

		dtCore::RefPtr<osg::Geode>			   mGeode;
		dtCore::RefPtr<osg::TextureCubeMap>	mCubeMap;
		dtCore::RefPtr<osg::Program>			mProgram;
		dtCore::RefPtr<osg::Uniform>			mInverseModelViewProjMatrix;
	};

	///this render profile will render the skybox as usual with the
	///fixed function pipeline
	class DT_CORE_EXPORT FixedFunctionProfile: public SkyBox::RenderProfile
	{

   public:
		FixedFunctionProfile();
		void Config(osg::Group* pNode);
		void SetTexture(SkyBox::SkyBoxSideEnum side, const std::string& filename);

	protected:

		osg::Node* MakeBox();

		dtCore::RefPtr<osg::Geode> mGeode;
      dtCore::RefPtr<dtCore::MoveEarthySkyWithEyePointTransform> mXform;
		dtCore::RefPtr<osg::Texture2D> mTextureList[6];
	};
   
};

}

#endif // DELTA_SKYBOX
