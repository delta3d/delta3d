/* 
 * Delta3D Open Source Game and Simulation Engine 
 * Copyright (C) 2004 MOVES Institute 
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

#ifndef DELTA_CLOUDDOME
#define DELTA_CLOUDDOME

/* clouddome.h
* Date: 28/04/04
*/

#include "dtCore/enveffect.h"
#include "sg.h"
#include "dtCore/pnoise.h"
#include "dtCore/refptr.h"

#include <osg/Group>
#include <osg/Geode>
#include <osg/Geometry>
#include <osg/Transform>
#include <osg/Texture3D>
#include <osgUtil/CullVisitor>
#include <osg/Fog>

#include <osgGL2/ProgramObject>

namespace dtCore
{
	class DT_EXPORT CloudDome : public dtCore::EnvEffect
	{

	public:
		DECLARE_MANAGEMENT_LAYER(CloudDome)

		CloudDome(int   octaves,
			      int   frequency,
                  float amp,
                  float persistence,
				  float cutoff,
                  float exponent,
                  float radius,
				  int   segments);


		CloudDome(float radius,
				  int   segments,
				  std::string filename);

		~CloudDome();

		osg::Group *GetNode(void) {return mNode.get();}

		// getters
		float getScale()            { return mScale; }
		float getExponent()         { return mExponent; }
		float getCutoff()           { return mCutoff; }
		float getSpeedX()           { return mSpeedX; }
		float getSpeedY()           { return mSpeedY; }
		float getBias()             { return mBias; }
		osg::Vec3 *getCloudColor()  { return mCloudColor; }
		bool getEnable()            { return mEnable; }

		//setters
		void setScale(float scale)          { mScale        = scale; }
		void setExponent(float exponent)    { mExponent     = exponent; }
		void setCutoff(float cutoff)        { mCutoff       = cutoff; }
		void setSpeedX(float speedX)        { mSpeedX       = speedX; }
		void setSpeedY(float speedY)        { mSpeedY       = speedY; }
		void setBias(float bias)            { mBias         = bias; }
		void setCloudColor(osg::Vec3 *mCC)  { mCloudColor   = mCC; }
		void setShaderEnable(bool enable)   { mEnable       = enable; }

		virtual void Repaint(sgVec4 sky_color, sgVec4 fog_color, 
			double sun_angle, double sunAzimuth,
			double vis);

	private:
		class MoveEarthySkyWithEyePointTransform : public osg::Transform
		{
		public:

			/** Get the transformation matrix which moves from local coords to world coords.*/
			virtual bool computeLocalToWorldMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const 
			{
				osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
				if (cv)
				{
					osg::Vec3 eyePointLocal = cv->getEyeLocal();
					matrix.preMult(osg::Matrix::translate(eyePointLocal.x(),eyePointLocal.y(),eyePointLocal.z()));
				}
				return true;
			}

			/** Get the transformation matrix which moves from world coords to local coords.*/
			virtual bool computeWorldToLocalMatrix(osg::Matrix& matrix,osg::NodeVisitor* nv) const
			{    
				osgUtil::CullVisitor* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
				if (cv)
				{
					osg::Vec3 eyePointLocal = cv->getEyeLocal();
					matrix.postMult(osg::Matrix::translate(-eyePointLocal.x(),-eyePointLocal.y(),-eyePointLocal.z()));
				}
				return true;
			}
		};


		void Create( void );
		osg::Geode *createDome(float, int);
		osg::Image *loadImageFile(std::string);
		void loadShaderSource( osgGL2::ShaderObject* , std::string );
		virtual void OnMessage(MessageData *data);
		void Update(const double deltaFrameTime);

      dtCore::RefPtr<osg::Group> mNode;
		dtCore::RefPtr<osg::Geode> mDome;
		dtCore::RefPtr<osg::Image> mImage_3D;
		dtCore::RefPtr<osg::Texture3D> mTex3D;
		std::string mFileName;
		osg::Vec3 *mFogColor;

		int mOctaves;
		int mFrequency;
		float mPersistence;
		float mAmplitude;

		dtCore::RefPtr<MoveEarthySkyWithEyePointTransform> mXform;
		double ctime;
		bool mEnable;
		bool shaders_enabled;
		float mRadius;
		int mSegments;
		int mWidth;
		int mHeight;
		int mSlices;

		std::vector<osgGL2::ProgramObjectPtr> _progObjList;
		osgGL2::ProgramObject* Cloud_ProgObj;
		osgGL2::ShaderObject*  Cloud_VertObj;
		osgGL2::ShaderObject*  Cloud_FragObj;

		// Uniform variables for shaders
		float mScale;
		float mExponent;
		float mCutoff;
		float mSpeedX;
		float mSpeedY;
		float mBias;
		osg::Vec3 *mCloudColor;
		osg::Vec3 *mOffset;

	};
}

#endif // DELTA_CLOUDDOME
