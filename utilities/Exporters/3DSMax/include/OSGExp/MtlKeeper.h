/*
 * OSGExp - 3D Studio Max plugin for exporting OpenSceneGraph models.
 * Copyright (C) 2003  VR-C
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *	FILE:			MtlKeeper.h
 * 
 *	DESCRIPTION:	Header file for the Material Keeper Class
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 19.11.2002  
 *
 *					22.09.2005 Joran: Fixed the shell material handling. It doesn't
 *					crash anymore on multi-materials and the Self-Illumination
 *					is handeled right.
 *
 *                  22.09.2005 Joran: Renamed setSelfIllumToWhite to
 *                  setSelfIllum
 *
 *                  14.10.2005 Joran: Made addAlphaFunc static and public.
 *
 *                  08.02.2006 Joran: Added class id's for architectural materials.
 *
 *					02.10.2007 Farshid Lashkari: Texture memory usage improvements. When compressed
 *					textures is turned on, textures will immediately be compressed and 
 *					uncompressed data will be released during export.
 */

#ifndef __MTLKEEPER__H
#define __MTLKEEPER__H

#include <osg/Material>
#include <osg/Texture2D>
#include <osg/BlendFunc>
#include <osg/TextureCubeMap>
#include <osg/TexEnv>
#include <osg/TexEnvCombine>
#include <osg/TexGen>
#include <osgDB/ReadFile>

#include "Max.h"
#include "stdmat.h"
#include "bmmlib.h"			// Bitmap defines
#include "iparamm2.h"		// IParamBlock2

// Our own includes.
#include "Options.h"


// Composite materiale defines.
#define COMPMTL_CLASS_ID_A 0x61dc0cd7
#define COMPMTL_CLASS_ID_B 0x13640af6

// Architectural material defines
#define ARCHMAT_CLASS_ID_A 332471230
#define ARCHMAT_CLASS_ID_B 1763586103

// Type of parameters in a composite materiale.
enum { 
	compmat_mtls,
	compmat_type, 
	compmat_map_on, 
	compmat_amount
};

// Type of parameters in an acubic map
enum { 
	acubic_size, acubic_blur, acubic_bluroffset, acubic_near, acubic_far,
	acubic_source, acubic_useatmospheric, acubic_applyblur,
	acubic_frametype, acubic_nthframe,
	acubic_bitmap_names,
	acubic_outputname,
};

//Class that will create a graphics context that will be used for compressing textures
class TextureCompressor {
public:
    TextureCompressor();
    virtual ~TextureCompressor();

	void compress(osg::Texture2D *texture);
private:
    static LRESULT CALLBACK StaticWindowProc(HWND _hWnd, UINT message, WPARAM wParam, LPARAM lParam);
private:
    HWND hwnd;
    HGLRC hglrc;
    HGLRC hglrcOld;
    HDC hdc;
    HDC hdcOld;
	osg::ref_ptr<osg::State> state;
};

struct ImageData : public osg::Referenced
{
   ImageData()
      : mTranslucent(false)
   {}

   bool mTranslucent;
   osg::ref_ptr<osg::Image> mImage;

protected:
   virtual ~ImageData() {}
};


class MtlKeeper {
public:

	// Constructor, destructor.
	MtlKeeper();
	~MtlKeeper();

	// Add and get methods.
	BOOL								addMtl(Mtl* maxMtl, Options* options, TimeValue t,bool isShellMaterial=false);
	osg::ref_ptr<osg::StateSet>			getStateSet(Mtl* maxMtl);

	// Small utillity methods.
	int									getNumTexUnits(osg::StateSet* stateset);
	int									getMapChannel(osg::StateSet* stateset, unsigned int unit);
	std::vector<int>					getMappingList(osg::StateSet* stateset);

	osg::ref_ptr<osg::Texture>			getTexture(osg::StateSet* stateset, int &texUnit);
	void								setTextureWrap(osg::StateSet* stateset, int texUnit, 
										   osg::Texture::WrapParameter param, osg::Texture::WrapMode mode);
    static osg::Texture::InternalFormatMode getTexComp(Options* options);
	static bool getEmbedImageData(Options* options);
	static osg::ref_ptr<osg::BlendFunc> addAlphaBlendFunc(osg::StateSet* stateset);
private:

	// Converting materials.
	osg::ref_ptr<osg::StateSet>			convertMaterial(Mtl* maxMtl, Options* options, TimeValue t);
	osg::ref_ptr<osg::StateSet>			convertStdMaterial(Mtl* maxMtl, Options* options, TimeValue t);
	osg::ref_ptr<osg::StateSet>			convertCompMaterial(Mtl* maxMtl, Options* options, TimeValue t);
	osg::ref_ptr<osg::Material>			createAndSetMaterial(Mtl* maxMtl, osg::StateSet* stateset, Options* options, TimeValue t);
	osg::ref_ptr<osg::Material>			createMaterial(StdMat* stdMtl, Options* options, TimeValue t);

	// Converting textures.
	osg::ref_ptr<osg::Texture>			convertMap(Mtl* maxMtl, Texmap* tmap, float blendAmount, osg::StateSet* stateset, Options* options, TimeValue t);
	osg::ref_ptr<osg::Texture>			convertBitmapMap(Mtl* maxMtl, Texmap* tmap, float blendAmount, osg::StateSet* stateset, Options* options, TimeValue t);
	osg::ref_ptr<osg::Texture>			convertOpacityMap(Mtl* maxMtl, Texmap* tmap, float blendAmount, osg::StateSet* stateset, Options* options, TimeValue t);
	osg::ref_ptr<osg::Texture>			convertReflectionMap(Mtl* maxMtl, Texmap* tmap, float blendAmount, osg::StateSet* stateset, Options* options, TimeValue t);
	osg::ref_ptr<osg::Texture>			convertUnknownMap(Mtl* maxMtl, Texmap* tmap, float blendAmount, osg::StateSet* stateset, Options* options, TimeValue t);
	osg::ref_ptr<osg::Texture2D>		createTexture2D(Mtl* maxMtl, Texmap* tmap, osg::StateSet* stateset, Options* options, TimeValue t);
	osg::ref_ptr<osg::Texture2D>		createTexture2D(Mtl* maxMtl, Bitmap* bmap, std::string name, osg::StateSet* stateset, Options* options, TimeValue t);
	osg::ref_ptr<osg::TextureCubeMap>	createTextureCubeMap(Mtl* maxMtl, Texmap* tmap, Options* options, TimeValue t);
	ImageData* createImage(Mtl* maxMtl, BitmapTex* bmt, Options* options, TimeValue t);
	ImageData* createImage(Mtl* maxMtl, Bitmap* bmap, std::string name, Options* options, TimeValue t);

	// Reading and writing bitmaps.
	void								printError(int error);
	std::string							createMapName(std::string name, Options* options);
	Bitmap*								readBitmap(std::string name, Options* options);
	int									writeBitmap(Bitmap* bmap, std::string name, Options* options);
	Bitmap*								rotateBitmap(Bitmap* bmap);
	Bitmap*								createAlphaBitmapFromBitmap(Bitmap* bmap);

	// Setting properties on statesets..
	void								setDiffuseColorToWhite(osg::StateSet* stateset);
	void								setSelfIllum(osg::StateSet* stateset);
	osg::ref_ptr<osg::TexEnvCombine>	addInterpolateCombiner(osg::StateSet* stateset, int texUnit, float amount);
	osg::ref_ptr<osg::TexEnvCombine>	addSubtractCombiner(osg::StateSet* stateset, int texUnit);
	osg::ref_ptr<osg::TexEnvCombine>	addOpacityInterpolateCombiner(osg::StateSet* stateset, int texUnit, float amount);
	osg::ref_ptr<osg::TexGen>			addTexGen(osg::StateSet* stateset, int texUnit, osg::TexGen::Mode mode);
	osg::ref_ptr<osg::TexEnv>			addTexEnv(osg::StateSet* stateset, int texUnit, osg::TexEnv::Mode mode);

	// Small utility methods.
	BOOL								isTwoSided(Mtl* maxMtl);
	BOOL								isEnvMap(Texmap* tmap);
	int									getNextTexUnit(osg::StateSet* stateset, int mapChannel);

	// A container to map MAX materials to their corresponding osg::StateSets.
    typedef std::map<Mtl*, osg::ref_ptr< osg::StateSet > >  MtlList;
	MtlList								_mtlList;

	// A container to map image names to their respective osg::Images and data arrays.
    typedef std::map<std::string, osg::ref_ptr<ImageData> >  ImgList;
	ImgList								_imgList;

	// A container to map texture units to their repective mapping channels in MAX.
	typedef std::map< osg::ref_ptr< osg::StateSet >, std::vector<int> >	MappingList;
	MappingList							_mappingList;

	// Only show memory error one time.
	BOOL								_hasShownMemErr;

	// For compressing textures (Only created when used for first time)
	TextureCompressor*					_compressor;
};

#endif // __MTLKEEPER__H