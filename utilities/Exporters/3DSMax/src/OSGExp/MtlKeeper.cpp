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
 *	FILE:			MtlKeeper.cpp
 *
 *	DESCRIPTION:	Source file for the Material Keeper class.
 *					Every added MAX material to this keeper class
 *					will be converted into a corresponding osg::stateset
 *					with an osg::material and an osg::texture(s).
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 19.11.2002
 *
 *					22.12.2002 added support for MAX multi/sub material.
 *
 *					09.01.2003 optimized image amount on disk by
 *					asserting that no equal images would be written twice
 *					on disk.
 * 
 *					16.01.2003 added support for composite MAX
 *					materials into different texture units in OSG
 *					(Multitexturing). Added support for mixing textures 
 *					and	environment maps with material using 
 *					osg::TexEnvCombiner. Added support for MAX's reflection
 *					maps, especially MAX cubemapping from file into 
 *					osg::TextureCubeMap.
 *
 *					24.01.2003 added support for whitening the Diffuse
 *					color if MAX material is 100% textured. Added alpha blend 
 *					function to osg::statesets having textures with alpha channels.
 *					
 *					27.01.2003 added support for setting texture compression format.
 *
 *					17.05.2003 rewritten the class to make it more cleaner.
 *
 *					22.09.2005 Joran: Fixed the shell material handling. It doesn't
 *					crash anymore on multi-materials and the Self-Illumination
 *					is handeled right.
 *
 *					23.09.2005 Joran: Only set the emissive color when it is turned
 *					on. Otherwize set it to black.
 *
 *					23.09.2005 Joran: When using the setDiffuseToWhite color not only
 *					the diffuse component is set to white, but also the ambient component
 *					is converted to gray.
 *
 *                  27.09.2005 Boto: Added Self Illumination map export.
 *
 *                  27.09.2005 Joran: Set diffuse col to white for 100% textured now moved
 *                  to the spot where the diffuse texture is applied. It now will only be done
 *                  for the diffuse texture.
 *
 *                  28.9.2005 Joran: In stead of setting self illumination to white, the 
 *                  diffuse color is copied to the emissive color. This way also none
 *                  diffuse baked textures will work.
 *
 *                  29.09.2005 Joran: Automaticly turn two sided lighting on for two sided
 *                  materials.
 *
 *                  08.02.2006 Joran: Added simple support for achitectural materials. They
 *                  are now detected and handled as standard materials.
 *
 *					27.09.2007 Farshid Lashkari: Fixed that too much memory was allocated
 *					for textures, which could cause out-of-memory errors.
 *
 *					27.09.2007 Farshid Lashkari: Added two sided support for composite
 *					materials.
 *
 *					01.10.2007 Farshid Lashkari: Disabled submaterials are not exported.
 *
 *					02.10.2007 Farshid Lashkari: Texture memory usage improvements. When compressed
 *					textures is turned on, textures will immediately be compressed and 
 *					uncompressed data will be released during export.
 *
 *                  05.11.2007 Daniel Sjolie: Added useOriginalTextures Option
 *
 *					25.01.2008 Farshid Lashkari: Fix for the processing of materials where the 
 *					exporter would not properly check for composite materials before checking
 *					if the sub-material is enabled.
 */

#include "MtlKeeper.h"

#include <osg/LightModel>

#include <direct.h>
#include <stdlib.h>

#include <sstream>

TextureCompressor::TextureCompressor()
{
    WNDCLASS wndClass;

    memset(&wndClass, 0, sizeof(WNDCLASS));
    wndClass.style         = CS_OWNDC;
	wndClass.lpfnWndProc   = (WNDPROC)StaticWindowProc;
	wndClass.cbClsExtra    = 0;
	wndClass.cbWndExtra    = 0;
	wndClass.hInstance     = GetModuleHandle(NULL);
	wndClass.hIcon         = NULL;
	wndClass.hCursor       = NULL;
	wndClass.hbrBackground = NULL;
	wndClass.lpszMenuName  = NULL;
	wndClass.lpszClassName = "GraphicsContext";
	RegisterClass(&wndClass);

    hwnd=CreateWindow(
        "GraphicsContext",
        "GraphicsContext",
        WS_POPUP,
        0,0,1,1,
        0,0,
        GetModuleHandle(NULL),
        NULL);

    hdc=GetDC(hwnd);

	// Define pixel format descriptor.
	PIXELFORMATDESCRIPTOR pfd = { 
        sizeof(PIXELFORMATDESCRIPTOR),				// size
	    1,											// version
	    PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW,	// support double-buffering
	    PFD_TYPE_RGBA,								// color type
	    32,											// prefered color depth
	    0, 0, 0, 0, 0, 0,							// color bits (ignored)
	    0,											// no alpha buffer
	    0,											// alpha bits (ignored)
	    0,											// no accumulation buffer
	    0, 0, 0, 0,									// accum bits (ignored)
	    24,											// depth buffer
	    0,											// no stencil buffer
	    0,											// no auxiliary buffers
	    PFD_MAIN_PLANE,								// main layer
	    0,											// reserved
	    0, 0, 0,									// no layer, visible, damage masks
    };

  	int pixelFormat = ChoosePixelFormat(hdc,&pfd);
	SetPixelFormat(hdc,pixelFormat,&pfd);

    hglrc=wglCreateContext(hdc);
    hglrcOld=wglGetCurrentContext();
    hdcOld=wglGetCurrentDC();
    wglMakeCurrent(hdc,hglrc);

	state = new osg::State;
}

TextureCompressor::~TextureCompressor()
{
    wglMakeCurrent(hdcOld,hglrcOld);
    wglDeleteContext(hglrc);
    ReleaseDC(hwnd,hdc);
    DestroyWindow(hwnd);
    UnregisterClass("GraphicsContext",GetModuleHandle(NULL));
}

void TextureCompressor::compress(osg::Texture2D *texture)
{
	osg::Image *image = texture->getImage();
	
	if(image && 
		(image->getPixelFormat()==GL_RGB || image->getPixelFormat()==GL_RGBA) &&
		(image->s()>=32 && image->t()>=32) &&
		!osg::Texture::isCompressedInternalFormat(image->getInternalTextureFormat()))
	{
		// get OpenGL driver to create texture from image.
		texture->apply(*state);

		image->readImageFromCurrentTexture(0,true);

		texture->setInternalFormatMode(osg::Texture::USE_IMAGE_DATA_FORMAT);
	}
}

LRESULT CALLBACK TextureCompressor::StaticWindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    return DefWindowProc( hWnd, message, wParam, lParam );
}

/**
 * Constructor.
 */
MtlKeeper::MtlKeeper(){
	_hasShownMemErr = FALSE;
	_compressor = NULL;
}

/**
 * Destructor.
 */
MtlKeeper::~MtlKeeper(){
	if(_compressor) delete _compressor;
	// Find image data array and delete it.
	//for(MtlKeeper::ImgList::iterator itr=_imgList.begin(); itr!=_imgList.end(); ++itr){
	//	if(itr->second.second)
	//		delete [] itr->second.second;
	//}
}

/**
 * This method will convert a MAX material into a corresponding
 * osg::StateSet with material and textures. The stateset will be added
 * to a list for later use.
 *
 * If the MAX material is a multi material its sub materials will also be
 * converted and added to the list.
 */
BOOL MtlKeeper::addMtl(Mtl* maxMtl, Options* options, TimeValue t,bool isShellMaterial) { 
	if (!maxMtl) return FALSE;

	// Handle the shell material
	if(maxMtl->ClassID()==Class_ID(BAKE_SHELL_CLASS_ID, 0)) {
		maxMtl=maxMtl->GetSubMtl(1);
		if (!maxMtl) return FALSE;
		isShellMaterial=true;
	}

	// See if material is allready in list.
	for(MtlKeeper::MtlList::iterator itr=_mtlList.begin(); itr!=_mtlList.end(); ++itr){
		if(itr->first == maxMtl)
			return FALSE;
	}

	// Do the material conversion.
	osg::ref_ptr<osg::StateSet> stateset = convertMaterial(maxMtl, options, t);

	if(stateset.valid()){
		// Set Cull Face to OFF if the material is two sided, otherwise set it ON.
        if(isTwoSided(maxMtl)) {
			stateset->setMode(GL_CULL_FACE,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);

            if(options->getAutoTwoSidedLighting()) {
                osg::LightModel *lightModel=new osg::LightModel;
                lightModel->setTwoSided(true);
                stateset->setAttribute(lightModel,osg::StateAttribute::ON);
            }
        }
        else {
			stateset->setMode(GL_CULL_FACE,osg::StateAttribute::ON);
        }

		// Set normalize normals to ON if set by user.
		if(options->getNormalize())
			stateset->setMode(GL_NORMALIZE,osg::StateAttribute::ON);
		else
			stateset->setMode(GL_NORMALIZE,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);

		// Turn of lighting if set by the user.
		if(options->getTurnOffLighting())
			stateset->setMode(GL_LIGHTING,osg::StateAttribute::OVERRIDE|osg::StateAttribute::OFF);
		else
			stateset->setMode(GL_LIGHTING,osg::StateAttribute::ON);

		// The options to set self illumination for baked materials
		if(isShellMaterial && options->getWhitenSelfIllum())
			setSelfIllum(stateset.get());

		// Add stateset to list.
		_mtlList[maxMtl] = stateset;
	}

	// If the MAX material has sub materials then add them to the list as well.
	if (maxMtl->NumSubMtls() > 0)  {

		// The composite materials properties are saved in a parameter block.
		IParamBlock2* pblock2 = NULL;
		if (maxMtl->ClassID() == Class_ID(COMPMTL_CLASS_ID_A, COMPMTL_CLASS_ID_B)) {
			pblock2 = maxMtl->GetParamBlock(0);
		}

		for (int i=0; i<maxMtl->NumSubMtls(); i++) {
			Mtl* subMtl = maxMtl->GetSubMtl(i);
			if (subMtl) {
				//If composite sub-material is disabled, then don't process it
				if(i > 0 && pblock2) {
					BOOL isEnabled;
					Interval iv;
					pblock2->GetValue(compmat_map_on,t,isEnabled,iv,i-1);
					if(!isEnabled) continue;
				}
				addMtl(subMtl, options, t, isShellMaterial);
			}
		}
	}

	return TRUE;
}

/** 
 * This method will seek the mtlList for a MAX material and if found
 * it will return the corresponding OSG stateset. If the searched MAX material is
 * a multi material, its sub materials will be searched for in the list
 * and the first sub material found will be returned.
 */ 
osg::ref_ptr<osg::StateSet>  MtlKeeper::getStateSet(Mtl* maxMtl){
	if(maxMtl == NULL)
		return NULL;

	if(maxMtl->ClassID()==Class_ID(BAKE_SHELL_CLASS_ID, 0)) {
		maxMtl=maxMtl->GetSubMtl(1);
		if (!maxMtl) return FALSE;
	}

	for(MtlKeeper::MtlList::iterator itr=_mtlList.begin(); itr!=_mtlList.end(); ++itr){
		if(itr->first == maxMtl)
			return itr->second;
	}
	// If the MAX material is a multi material it will not 
	// be found in the list - but its sub materials will!
	// Therefore, return the first valid stateset found
	// within the list of sub materials.
	if (maxMtl->NumSubMtls() > 0)  {
		for (int i=0; i<maxMtl->NumSubMtls(); i++) {
			Mtl* subMtl = maxMtl->GetSubMtl(i);
			osg::ref_ptr<osg::StateSet> stateset = getStateSet(subMtl);
			if(stateset.valid())
				return stateset;
		}
	}

	return NULL;
}

/**
 * General method to convert a MAX material.
 */
osg::ref_ptr<osg::StateSet> MtlKeeper::convertMaterial(Mtl* maxMtl, Options* options, TimeValue t){

	// Is this a standard material.
	if (maxMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0))
		return convertStdMaterial(maxMtl, options, t);

	// Is this an architectural material, just pretend it is a normal material.
	if (maxMtl->ClassID() == Class_ID(ARCHMAT_CLASS_ID_A, ARCHMAT_CLASS_ID_B))
		return convertStdMaterial(maxMtl, options, t);

    // Is this a composite material.
	if (maxMtl->ClassID() == Class_ID(COMPMTL_CLASS_ID_A, COMPMTL_CLASS_ID_B))
		return convertCompMaterial(maxMtl, options, t);

	return NULL;
}

/**
 * This method will convert a MAX standard material, and return a
 * corresponding osg::StateSet.
 */
osg::ref_ptr<osg::StateSet> MtlKeeper::convertStdMaterial(Mtl* maxMtl, Options* options, TimeValue t){
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();

	// Create and set osg::Material on stateset
	createAndSetMaterial(maxMtl, stateset.get(), options, t);

	// Convert the material to a standard material
	StdMat* stdMtl = (StdMat*) maxMtl;
	// The blend amount for the map.
	float blendAmount;

	// Convert any maps in the diffuse slot.
	if(maxMtl->GetSubTexmap(ID_DI) && maxMtl->SubTexmapOn(ID_DI)){
		blendAmount = stdMtl->GetTexmapAmt(ID_DI,t);
		convertMap(maxMtl, maxMtl->GetSubTexmap(ID_DI), blendAmount, stateset.get(), options, t);
   		// If no mix then set the diffuse color to white.
		if(options->getWhitenTexMat() && std::abs(blendAmount-1.0f)<=0.001f)
			setDiffuseColorToWhite(stateset.get());
	}
    // modified by boto ( 03/22/2005 ): added support for self-illumination map
	// Convert any maps in the self-illumination slot.
    if(options->getExportSelfIllumMaps() && maxMtl->GetSubTexmap(ID_SI) && maxMtl->SubTexmapOn(ID_SI)){
		blendAmount = stdMtl->GetTexmapAmt(ID_SI,t);
		convertMap(maxMtl, maxMtl->GetSubTexmap(ID_SI), blendAmount, stateset.get(), options, t);
	}
	// Convert any maps in the opacity slot.
    if(options->getExportOpacityMaps() && maxMtl->GetSubTexmap(ID_OP) && maxMtl->SubTexmapOn(ID_OP)){
		blendAmount = stdMtl->GetTexmapAmt(ID_OP,t);
		convertOpacityMap(maxMtl, maxMtl->GetSubTexmap(ID_OP), blendAmount, stateset.get(), options, t);
	}
	// Convert any maps in the reflection slot.
	if(maxMtl->GetSubTexmap(ID_RL) && maxMtl->SubTexmapOn(ID_RL)){
		blendAmount = stdMtl->GetTexmapAmt(ID_RL,t);
		convertMap(maxMtl, maxMtl->GetSubTexmap(ID_RL), blendAmount, stateset.get(), options, t);
	}
   // Convert any maps in the bump slot.
	if(maxMtl->GetSubTexmap(ID_BU) && maxMtl->SubTexmapOn(ID_BU)){
		blendAmount = stdMtl->GetTexmapAmt(ID_BU,t);
		convertMap(maxMtl, maxMtl->GetSubTexmap(ID_BU), blendAmount, stateset.get(), options, t);
	}
	return stateset;	
}

/**
 * This method will convert a MAX composite material, and return a
 * corresponding osg::StateSet.
 */
osg::ref_ptr<osg::StateSet> MtlKeeper::convertCompMaterial(Mtl* maxMtl, Options* options, TimeValue t){
	osg::ref_ptr<osg::StateSet> stateset = new osg::StateSet();

	// The first sub material is the base material, set this to the stateset.
	Mtl* baseMtl = maxMtl->GetSubMtl(0);
	osg::ref_ptr<osg::Material> osgMtl = createAndSetMaterial(baseMtl, stateset.get(), options, t);

	// The composite materials properties are saved in a parameter block.
	IParamBlock2* pblock2 = maxMtl->GetParamBlock(0);

	// Get all the textures from the i'th composite material.
	if (pblock2 && maxMtl->NumSubMtls() > 1)  {
		for (int i=1; i<maxMtl->NumSubMtls(); i++) {
			// Get the i'th sub material.
			Mtl* subMtl = maxMtl->GetSubMtl(i);
			if(subMtl){
				// Get the i'th composite material's parameters. 
				int type;
				float amount;
				BOOL isEnabled;
				Interval iv;
				pblock2->GetValue(compmat_type,t,type,iv,i-1);
				pblock2->GetValue(compmat_map_on,t,isEnabled,iv,i-1);
				pblock2->GetValue(compmat_amount,t,amount,iv,i-1);

				if(isEnabled && options->getExportTextures()){

					int newTexUnit = -1;
					// Convert the submaterial and add it to the material list.
					addMtl(subMtl, options, t);
					// Get the material from the list
					osg::ref_ptr<osg::StateSet> substateset = getStateSet(subMtl);
					// If the substateset is not valid then just continue to the next.
					if (!substateset.valid())
						continue;
					// Copy anything from the first valid texture unit in the substateset to the final stateset.
					osg::StateSet::TextureAttributeList tal = substateset->getTextureAttributeList();
					for(unsigned int unit=0; unit<tal.size(); unit++){
						// Is there a valid texture in this unit.
						if(substateset->getTextureAttribute(unit, osg::StateAttribute::TEXTURE)){
							// Get the map channel for the texture unit on the substateset.
							int mapChan = getMapChannel(substateset.get(),unit);
							// Get the next available texture unit on the final stateset.
							newTexUnit = getNextTexUnit(stateset.get(), mapChan);
							// Copy all stateattributes to the new unit.
							for(osg::StateSet::AttributeList::iterator aitr=tal[unit].begin(); aitr!=tal[unit].end(); ++aitr){
								stateset->setTextureAttribute(newTexUnit, aitr->second.first.get(),aitr->second.second);
							}
							// Copy all modes to new unit.
							osg::StateSet::ModeList ml = substateset->getTextureModeList()[unit];
							for(osg::StateSet::ModeList::const_iterator mitr=ml.begin(); mitr!=ml.end(); ++mitr){
								stateset->setTextureMode(newTexUnit, mitr->first, mitr->second);
							}
							// Exit for loop - we have found our texture.
							break;
						}
					}

					if(newTexUnit!=-1){

						// Get the blend amount for this map.
						float blendAmount = (amount/100.0f);

						// Add interpolate combiner if blend amount is set.
						if(amount != 100.0f)
							addInterpolateCombiner(stateset.get(), newTexUnit, blendAmount);
						else{
							// If no blend amount then set material color to white.
							if(options->getWhitenTexMat())
								setDiffuseColorToWhite(stateset.get());
							// Use TexEnvironments to mix the diffuse maps.
							switch(type){
							// The type is A. We will Add.
							case 0:
								addTexEnv(stateset.get(), newTexUnit, osg::TexEnv::ADD);
								break;
							// The type is S. We will Substract.
							case 1:
								addSubtractCombiner(stateset.get(), newTexUnit);
								break;
							// The type is M. We will Modulate.
							case 2:
								addTexEnv(stateset.get(), newTexUnit, osg::TexEnv::MODULATE);								
								break;
							default:
								break;
							}
						}
					}
				}
			}
		}
	}
	return stateset;
}

/**
 * This method will create and set a MAX standard material to a given stateset.
 */
osg::ref_ptr<osg::Material> MtlKeeper::createAndSetMaterial(Mtl* maxMtl, osg::StateSet* stateset, Options* options, TimeValue t){
	if(!maxMtl)
		return NULL;

	// If this is a standard material then make a corresponding OSG material.
	if (maxMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)){
		// Cast the material to the Standard material class.
		StdMat* stdMtl = (StdMat *)maxMtl;
		// Make the OSG material.
		osg::ref_ptr<osg::Material> osgMtl = createMaterial(stdMtl, options, t);
		// Set the material to the stateset.
		stateset->setAttribute(osgMtl.get());
		// Set blend function if material is transparent.
		if (stdMtl->GetOpacity(t)!= 1.0f) 
			addAlphaBlendFunc(stateset);
		return osgMtl;
	}

	return NULL;
}

/**
 * Returns an osg::Material corresponding to the information found in a
 * standard MAX material.
 */
osg::ref_ptr<osg::Material> MtlKeeper::createMaterial(StdMat* stdMtl, Options* options, TimeValue t){
	// Make the OSG material.
	osg::ref_ptr<osg::Material> osgMtl = new osg::Material();
	Color amb  = stdMtl->GetAmbient(t);
	Color diff = stdMtl->GetDiffuse(t);
	Color spec = stdMtl->GetSpecular(t);
	Color self = stdMtl->GetSelfIllumColor(t);
	// The definition on shininess and opacity values in MAX:
	// 0 = 100 % opacity and 1 = 0% opacity
	// 0 = 100 % specular contrib and 1 = 0% specular contrib.
	// Shininess in OSG ranges for 0-128
	float alpha     = stdMtl->GetOpacity(t);
	float shininess = stdMtl->GetShininess(t);
	float shinstr =  stdMtl->GetShinStr(t);
	osgMtl->setColorMode(osg::Material::OFF);
	osgMtl->setAmbient( osg::Material::FRONT_AND_BACK, 
						osg::Vec4(amb.r,  amb.g,  amb.b, alpha));
	osgMtl->setDiffuse( osg::Material::FRONT_AND_BACK, 
						osg::Vec4(diff.r, diff.g, diff.b, alpha));
	if(stdMtl->GetSelfIllumColorOn()) 
		osgMtl->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(self.r, self.g, self.b, alpha));
	else 
		osgMtl->setEmission(osg::Material::FRONT_AND_BACK,osg::Vec4(0,0,0,alpha));

	if( shinstr > 0.0f )
	{
		// We multiply specular color to shinstr to get closer
		// to the Blinn specular shading model. The only problem is
		// that shinstr varies from 0.0 to 9.99. Thats way we clamp it.
		osgMtl->setSpecular(osg::Material::FRONT_AND_BACK, 
			osg::Vec4( 0.0f, 0.0f, 0.0f, alpha ) + 
			( osg::Vec4(spec.r, spec.g, spec.b, 0.0f ) * ( shinstr / 9.99f ) ) );
		osgMtl->setShininess(osg::Material::FRONT_AND_BACK, shininess * 128.0f ); 
	}

   //sets the max material name on the osg material
   osgMtl->setName(stdMtl->GetName());

	return osgMtl;
}

/**
 * Generel method to convert a MAX texture map to an osg::Texture.
 */
osg::ref_ptr<osg::Texture> MtlKeeper::convertMap(Mtl* maxMtl, Texmap* tmap, float blendAmount, osg::StateSet* stateset, Options* options, TimeValue t){
	// Is this a bitmap map.
	if(tmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0))
		return convertBitmapMap(maxMtl, tmap, blendAmount, stateset, options, t);

	// Is this a reflection map.
	else if(tmap->ClassID() == Class_ID(ACUBIC_CLASS_ID,0))
		return convertReflectionMap(maxMtl, tmap, blendAmount, stateset, options, t);

	// Convert unknown map
    else if(options->getExportUnknownMaps())
		return convertUnknownMap(maxMtl, tmap, blendAmount, stateset, options, t);

	return NULL;
}

/**
 * Converts a MAX bitmap texture map and adds it to the given stateset
 * in the right texture unit.
 */
osg::ref_ptr<osg::Texture> MtlKeeper::convertBitmapMap(Mtl* maxMtl, Texmap* tmap, float blendAmount, osg::StateSet* stateset, Options* options, TimeValue t){
	// Create texture from bitmap.
	osg::ref_ptr<osg::Texture> osgTex = createTexture2D(maxMtl, tmap, stateset, options, t).get();
	if(osgTex.valid()){
		// Is this texture map used as an environment map.
		BOOL envMap = isEnvMap(tmap);

		// Figure out which texture unit to put texture in.
		int texUnit = getNextTexUnit(stateset, envMap?0:tmap->GetMapChannel());

		// Set the texture to the stateset.
		stateset->setTextureAttributeAndModes(texUnit, osgTex.get(), osg::StateAttribute::ON);

		// If the bitmap in is used as a spherical environment then set the stateset.
		if(envMap)
			addTexGen(stateset, texUnit, osg::TexGen::SPHERE_MAP);

		// Use TexEnvCombiner to blend the bitmap with the color.
		if(std::abs(blendAmount-1.0f)>0.001f)
			addInterpolateCombiner(stateset, texUnit, blendAmount);
	}	
	return osgTex;
}

/**
 * Converts a MAX reflection texture map and adds it to the given stateset
 * in the right texture unit.
 */
osg::ref_ptr<osg::Texture> MtlKeeper::convertReflectionMap(Mtl* maxMtl, Texmap* tmap, float blendAmount, osg::StateSet* stateset, Options* options, TimeValue t){
	// Create a texture cube map from the information in the texmap.
	osg::ref_ptr<osg::Texture> osgTex = createTextureCubeMap(maxMtl, tmap, options, t).get();
	if(osgTex.valid()){
		// Figure out which map channel / texture unit to put texture in.
		int texUnit = getNextTexUnit(stateset, 0);

		// Set the texture cube map to the stateset.
		stateset->setTextureAttributeAndModes(texUnit, osgTex.get(), osg::StateAttribute::ON);
	
		// Add texture coordinate generation reflect map.
		addTexGen(stateset, texUnit, osg::TexGen::REFLECTION_MAP);

		// Use TexEnvCombiner to blend the bitmap with the color.
		if(std::abs(blendAmount-1.0f)>0.001f)
			addInterpolateCombiner(stateset, texUnit, blendAmount);
	}
	return osgTex;
}

/**
 * Converts a MAX opacity texture map and adds it to the given stateset
 * in the right texture unit.
 */
osg::ref_ptr<osg::Texture> MtlKeeper::convertOpacityMap(Mtl* maxMtl, Texmap* tmap, float blendAmount, osg::StateSet* stateset, Options* options, TimeValue t){

	if ((tmap == NULL) || (tmap->ClassID() != Class_ID(BMTEX_CLASS_ID, 0)))
		return NULL;

	// Cast the texmap to a bitmap texture. 
	BitmapTex *bmt = (BitmapTex*) tmap;
	// Get the bitmap.
	Bitmap *bmap = bmt->GetBitmap(t);

	if(!bmap)
		return NULL;

	// Create a name for this bitmap.
	std::string oldname = std::string(bmt->GetMapName());
	std::string newname = createMapName(oldname, options);

	// if the the bitmap does not have any alpha source we will create one from the RGB source.
	if(!bmap->HasAlpha())
		bmap = createAlphaBitmapFromBitmap(bmap);

	// Create a texture2d from the information in the texmap.
	osg::ref_ptr<osg::Texture> osgTex = createTexture2D(maxMtl, bmap, newname, stateset, options, t).get();
	if(osgTex.valid()){
		// Figure out which texture unit to put texture in.
		int texUnit = getNextTexUnit(stateset, tmap->GetMapChannel());

		// Set the texture  map to the stateset.
		stateset->setTextureAttributeAndModes(texUnit, osgTex.get(), osg::StateAttribute::ON);

		// Use TexEnvCombiner to blend the alpha channels.
		addOpacityInterpolateCombiner(stateset, texUnit, blendAmount);
	}
	return osgTex;
}

/**
 * Converts an unknown texture map and adds it to the given stateset
 * in the right texture unit.
 */
osg::ref_ptr<osg::Texture> MtlKeeper::convertUnknownMap(Mtl* maxMtl, Texmap* tmap, float blendAmount, osg::StateSet* stateset, Options* options, TimeValue t){

	BitmapInfo bi;
	bi.SetType(BMM_TRUE_32);
	bi.SetWidth(256);
	bi.SetHeight(256);
	//bi.SetFlags(MAP_HAS_ALPHA);
	Bitmap* bmap = TheManager->Create(&bi);
	tmap->RenderBitmap(t, bmap);

	// Create a name for this bitmap.
	std::ostringstream buffer;
	buffer << "Map" << _imgList.size();
	std::string oldname = buffer.str();
	std::string newname = createMapName(oldname, options);

	// Create a texture2d from the information in the bitmap.
	osg::ref_ptr<osg::Texture> osgTex = createTexture2D(maxMtl, bmap, newname, stateset, options, t).get();
	if(osgTex.valid()){
		// Is this texture map used as an environment map.
		BOOL envMap = isEnvMap(tmap);

		// Figure out which texture unit to put texture in.
		int texUnit = getNextTexUnit(stateset, envMap?0:tmap->GetMapChannel());

		// Set the texture map to the stateset.
		stateset->setTextureAttributeAndModes(texUnit, osgTex.get(), osg::StateAttribute::ON);

		// If the bitmap in is used as a spherical environment then set the stateset.
		if(envMap)
			addTexGen(stateset, texUnit, osg::TexGen::SPHERE_MAP);

		// Use TexEnvCombiner to blend the bitmap.
		if(std::abs(blendAmount-1.0f)>0.001f)
			addInterpolateCombiner(stateset, texUnit, blendAmount);
	}
	return osgTex;
}

/**
 * Returns an osg::Texture2D from the information found in the given MAX texture map.
 */
osg::ref_ptr<osg::Texture2D> MtlKeeper::createTexture2D(Mtl* maxMtl, Texmap* tmap, osg::StateSet* stateset, Options* options, TimeValue t){

	// Is this a bitmap texture.
	if (tmap && tmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {
		// Cast the texmap to a bitmap texture. 
		BitmapTex *bmt = (BitmapTex*) tmap;
		// Get the bitmap.
		Bitmap *bmap = bmt->GetBitmap(t);

		// Create a name for this bitmap.
		std::string oldname = std::string(bmt->GetMapName());
		std::string newname = createMapName(oldname, options);

		return createTexture2D(maxMtl, bmap, newname, stateset, options, t);
	}
	return NULL;

}

/**
 * This method will create and return an osg::Texture2D from the information
 * found in the given MAX bitmap map.
 */
osg::ref_ptr<osg::Texture2D> MtlKeeper::createTexture2D(Mtl* maxMtl, Bitmap* bmap, std::string name, osg::StateSet* stateset, Options* options, TimeValue t){

	if(!bmap)
		return NULL;

	//Get compression mode
	osg::Texture::InternalFormatMode internalFormat = getTexComp(options);

	// Create texture and set compression.
	osg::ref_ptr<osg::Texture2D> osgTex = new osg::Texture2D();
    osgTex->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
	osgTex->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
	osgTex->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
    osgTex->setInternalFormatMode(internalFormat);

	// Create image and set it to the texture.
	ImageData* imageData = createImage(maxMtl, bmap, name, options, t);
	if(imageData != NULL) {
      osgTex->setImage(imageData->mImage.get());

      // If this bitmap has an alpha channel then add an alpha blend function to stateset.
      if(options->getAutoAlphaTextures() && bmap->HasAlpha() && imageData->mTranslucent){
         addAlphaBlendFunc(stateset);
      }

		// Compress texture immediately to free up memory usage
		if(internalFormat != osg::Texture::USE_IMAGE_DATA_FORMAT) {
			if (getEmbedImageData(options))
			{
				if(!_compressor)
					_compressor = new TextureCompressor;
				_compressor->compress(osgTex.get());
			}
      }
	}

	return osgTex;
}

/**
 * This method will create and return an osg::TextureCubeMap from the information
 * found in the given MAX texture map.
 */
osg::ref_ptr<osg::TextureCubeMap> MtlKeeper::createTextureCubeMap(Mtl* maxMtl, Texmap* tmap, Options* options, TimeValue t){

	// Is this an acubic map.
	if (tmap && tmap->ClassID() == Class_ID(ACUBIC_CLASS_ID,0)) {
		// The acubic map's properties are saved in a parameter block.
		IParamBlock2* pblock = tmap->GetParamBlock(0);
			
		BOOL on;
		Interval iv;
		std::string filenames[6];	// source files
		osg::ref_ptr<osg::Image> images[6];	// the images for cubemap.
		std::string path(options->getExportPath());
		path.append("\\");
		pblock->GetValue(acubic_source,t,on,iv);

	    // create and setup the texture object
		osg::ref_ptr<osg::TextureCubeMap> tcm = new osg::TextureCubeMap();
	    tcm->setWrap(osg::Texture::WRAP_S, osg::Texture::CLAMP_TO_EDGE);
		tcm->setWrap(osg::Texture::WRAP_T, osg::Texture::CLAMP_TO_EDGE);
		tcm->setWrap(osg::Texture::WRAP_R, osg::Texture::CLAMP_TO_EDGE);
		tcm->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
		tcm->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);     
		tcm->setInternalFormatMode(getTexComp(options));

				
		// If the acubic_source is TRUE (=use file source) and there are assigned 
		// 6 maps in the Up, Down, Right, Left, Front and Back slots, we will use these maps.
		if(on){
			for (int i = 0 ; i < 6; i++){

#if(MAX_VERSION_MAJOR >= 12)
				const MCHAR* name = NULL;
#else
				MCHAR* name = NULL;
#endif
				
				pblock->GetValue(acubic_bitmap_names,t,name,iv,i);
				filenames[i] = std::string(name);
			}	

			// Read bitmaps from disk, rotate them 180 degree and create
			// their corresponding osg::Images.
			// We need to rotate the maps because MAX saves them differently
			// than OpenGL uses them.
			for(int j=0;j<6;j++){
				// Read the bitmap from disk.
				Bitmap* bmap = readBitmap(filenames[j], options);
				// Rotate bitmap 180 degree.
				bmap = rotateBitmap(bmap);
				// Create a name for the bitmap.
				std::string name = createMapName(filenames[j], options);
				// Create an osg::Image.
				images[j] = createImage(maxMtl, bmap, name, options, t)->mImage;
			}

			// Assign the six images to the texture object.
			tcm->setImage(osg::TextureCubeMap::POSITIVE_Y, images[0].get());
			tcm->setImage(osg::TextureCubeMap::NEGATIVE_Y, images[1].get());
			tcm->setImage(osg::TextureCubeMap::POSITIVE_Z, images[2].get());
			tcm->setImage(osg::TextureCubeMap::POSITIVE_X, images[3].get());
			tcm->setImage(osg::TextureCubeMap::NEGATIVE_Z, images[4].get());
			tcm->setImage(osg::TextureCubeMap::NEGATIVE_X, images[5].get());
			return tcm;
		} 
		// If acubic_source is FALSE (=automatic) we could add an OSG cubemapgenerator, but it
		// is not supported in the OSG file format.
		else{
			return tcm;
		}

	}// End acubic map.

	return NULL;
}

/**
 * Create an image from a bitmap texture. Extract the original 
 * name from the bitmap texture and create a new name.
 */
ImageData* MtlKeeper::createImage(Mtl* maxMtl, BitmapTex* bmt, Options* options, TimeValue t){

	// Create a name for this bitmap.
	std::string oldname = std::string(bmt->GetMapName());
	std::string newname = createMapName(oldname, options);

	// Get the bitmap.
	Bitmap *bmap = bmt->GetBitmap(t);
	
	// Create the image.
	return createImage(maxMtl, bmap, newname, options, t);
}

/**
 * Returns an osg::Image corresponding to the given bitmap.
 * The method will write the image to disk if the user has choosen this
 * options. An image list is used to assert that we are not creating 
 * an image with the same name several times.
 */
ImageData* MtlKeeper::createImage(Mtl* maxMtl, Bitmap* bmap, std::string name, Options* options, TimeValue t){

	// See if image is allready in list.
	MtlKeeper::ImgList::iterator itr = _imgList.find(name);
	if(itr != _imgList.end()) {
		return itr->second.get();
	}

	// Create image and set the name.
   osg::ref_ptr<ImageData> imgData = new ImageData;
   imgData->mImage = new osg::Image();
	osg::Image* image = imgData->mImage.get();
	image->setFileName(name);

   // The file being output may be either an OSG or IVE.
   // If the option to include textures is not set for
   // the case of exporting to IVE...
   if( ! options->getIncludeImageDataInIveFile())
   {
      // ...force the texture to be externally referenced.
      // This will be the case either way for OSG format,
      // since it is merely text and will not embed image data.
      image->setWriteHint(osg::Image::EXTERNAL_FILE);
   }

	// Write the image to disk.
	if(options->getWriteTexture()){
		int status = writeBitmap(bmap, name, options);
		if(status != BMMRES_SUCCESS && options->getShowErrMsg())
			printError(status);
	}

   bool dataToOSGImage = options->getQuickView() || getEmbedImageData(options);
   BOOL hasAlpha = bmap->HasAlpha();
   unsigned char* data = NULL;
   osg::ref_ptr<osg::Image> tmpImg;
	if(dataToOSGImage || hasAlpha)
   {
      // Create a temporary image if it is only needed for determining transparency.
      if( ! dataToOSGImage && hasAlpha)
      {
         tmpImg = new osg::Image();
      }

      // Determine if the actual image instance should be used, or the temporary image.
      osg::Image* theImage = dataToOSGImage ? image : tmpImg.get();

		// Retrieve image properties.
		int width = bmap->Width();
	    int height = bmap->Height();
		int r = 1; // we're not a 3d texture...
	    unsigned int dataType = GL_UNSIGNED_BYTE;
		unsigned int internalFormat = hasAlpha ? 4 : 3;
		unsigned int pixelFormat = hasAlpha ? GL_RGBA : GL_RGB;

      data = new (std::nothrow) unsigned char[width*height*internalFormat];

		if(data){
			BMM_Color_64 *in = new BMM_Color_64[width];
			int pos = 0;
			for(int h=height-1; h>=0; h--){
				bmap->GetLinearPixels(0, h, width, in);
				BMM_Color_64 *i = in;
				for(int w=0; w<width; w++){
					data[pos++] = i->r;
					data[pos++] = i->g;
					data[pos++] = i->b;
					if(hasAlpha)
						data[pos++] = i->a;
					i++;
				}
			}
			delete [] in;

         // Set the data on the image.
			theImage->setImage(width, height, r, internalFormat, pixelFormat, dataType, data, osg::Image::USE_NEW_DELETE);
		}
		else if(options->getShowErrMsg() && !_hasShownMemErr){
			char buf[500];
			sprintf(buf,"Error converting image %s, could not allocate data array.", name.c_str());
			MessageBox (GetActiveWindow(), buf, _T("Warning"),
					MB_OK | MB_ICONWARNING);
			_hasShownMemErr = TRUE;
		}
	}

   // Determine if the image is truly translucent even though it has an alpha channel.
   if(hasAlpha)
   {
      imgData->mTranslucent = tmpImg.valid() ? tmpImg->isImageTranslucent() : image->isImageTranslucent();
   }

	// Add image to list.
	_imgList[name] = imgData.get();

	return imgData.get();
}

/**
 * This method will print any error possible to occur when opening, closing 
 * and writing the image file.
 */
void MtlKeeper::printError(int error){
	std::string errorString;
	if(error==BMMRES_ERRORTAKENCARE)
		errorString = std::string("Could not find a device to handle the image.");
	else if(error==BMMRES_INTERNALERROR)
		errorString = std::string("Image could not be opened for writing.");

	char buf[500];
	sprintf(buf,"Error writing image:\n%s", errorString.c_str());
	MessageBox (GetActiveWindow(), buf, TEXT("Warning"),
			        MB_OK | MB_ICONWARNING) ;
}

/**
 * This method will return a texture name made from the texture's
 * current name and the format we would like save the texture in.
 */
std::string MtlKeeper::createMapName(std::string oldname, Options* options){

	if ( options->getUseOriginalTextureFiles() && !options->getWriteTexture() ) { // Use original file(name)s
		return oldname;
	}
	char* texFormat = options->getTexFormat(options->getTexFormatIndex());

	// Get texture name.
	TCHAR p[300];
	TCHAR f[100];
	TCHAR e[10];
	BMMSplitFilename(oldname.c_str(), p, f, e);

	// Make a filename for the texture.
	char buf[500];
	sprintf(buf,"images\\%s.%s", f, texFormat);
	std::string newname(buf);
	return newname;
}

/**
 * This method will read a bitmap from the disk.
 */
Bitmap* MtlKeeper::readBitmap(std::string name, Options* options){
	Bitmap* bmap;
	BitmapInfo bi;
	BMMRES status;
	
	// Set name on bitmap info.
	bi.SetName(_T(name.c_str()));
 
	// Load the image
	bmap = TheManager->Load(&bi, &status);
	if (status != BMMRES_SUCCESS && options->getShowErrMsg()) {
		char buf[500];
		sprintf(buf,"Error opening image %s", _T(name.c_str()));
		MessageBox (GetActiveWindow(), buf, _T("Warning"),
					MB_OK | MB_ICONWARNING) ;
	}
	return bmap;
}

/**
 * This method will save a bitmap on the disk.
 * MAX will itself find the right image plugin to save the 
 * bitmap from the extension given in the image name.
 */
int MtlKeeper::writeBitmap(Bitmap* bmap, std::string name, Options* options){

	BitmapInfo bi;
	BMMRES status;
	std::string path(options->getExportPath());
	path.append("\\");
	// Set name on bitmap info.
	bi.SetName((path + name).c_str());
	
	// Make image directory.
	_mkdir((path.append("images")).c_str());

	// Write the bitmap to disk.
	status = bmap->OpenOutput(&bi);
	if(status!=BMMRES_SUCCESS)
		return status;
	status = bmap->Write(&bi);
	if(status!=BMMRES_SUCCESS)
		return status;
	bmap->Close(&bi);
	return status;
}

/**
 * This method will rotate a bitmap image 180 degree 
 * by substituting pixels. This is needed when creating
 * cube maps because MAX uses them the other than OpenGL.
 */
Bitmap* MtlKeeper::rotateBitmap(Bitmap* bmap){

	if(bmap == NULL)
		return NULL;

	BitmapInfo bi;
	bi.SetType(BMM_TRUE_64);
	bi.SetWidth(bmap->Width());
	bi.SetHeight(bmap->Height());
	bi.SetFlags(bmap->Flags());
	Bitmap* res = TheManager->Create(&bi);

	BMM_Color_fl fillcolor;
	fillcolor.a=0.0;
	res->CopyImage(bmap, COPY_IMAGE_USE_CUSTOM, fillcolor ,&bi);

	int w = bmap->Width();
	int h = bmap->Height();
	BMM_Color_fl p1;
	BMM_Color_fl p2;

	for(int i=0;i<w;i++){
		for(int j=0;j<h/2;j++){
			res->GetPixels(i, j, 1, &p1);
			res->GetPixels((w-1)-i, (h-1)-j, 1, &p2);
			res->PutPixels((w-1)-i, (h-1)-j, 1, &p1);	
			res->PutPixels(i, j, 1, &p2);			
		}
	}
	return res;
}

/*
 * This method will create an new bitmap using the given bitmap 
 * as alpha source.
 */
Bitmap* MtlKeeper::createAlphaBitmapFromBitmap(Bitmap* bmap){

	if(bmap == NULL || bmap->HasAlpha())
		return NULL;

	BitmapInfo bi;
	bi.SetFlags(bmap->Flags()|MAP_HAS_ALPHA);
	bi.SetType(BMM_TRUE_32);
	bi.SetWidth(bmap->Width());
	bi.SetHeight(bmap->Height());

	Bitmap* res = TheManager->Create(&bi);

	int w = bmap->Width();
	int h = bmap->Height();

	BMM_Color_fl p1;

	// Use an average rgb as alpha source.
	for(int i=0;i<w;i++){
		for(int j=0;j<h;j++){
			bmap->GetPixels(i, j, 1, &p1);
			p1.a = (p1.r + p1.g + p1.b)/3.0f;
			res->PutPixels(i, j, 1, &p1);
		}
	}
	return res;
}

/**
 * This method will set the Diffuse color to white in the
 * material of the given stateset. This is often used when
 * an object is 100% textured.
 */
void MtlKeeper::setDiffuseColorToWhite(osg::StateSet* stateset){

	osg::Material* osgMtl = (osg::Material*) stateset->getAttribute(osg::StateAttribute::MATERIAL);

	if(osgMtl){
		osg::Vec4 color = osgMtl->getDiffuse(osg::Material::FRONT_AND_BACK);
		osgMtl->setDiffuse( osg::Material::FRONT_AND_BACK, osg::Vec4(1, 1, 1, color.a()));

		// Also the ambient should be converted to an intencity without color
		color = osgMtl->getAmbient(osg::Material::FRONT_AND_BACK);
		float bw=.3f*color.r()+.59f*color.g()+.11f*color.b();
		osgMtl->setAmbient( osg::Material::FRONT_AND_BACK, osg::Vec4(bw, bw, bw, color.a()));
	}
}

/**
 * This method will copy the diffuse color to the emmision color
 * and set ambient and diffuse to black.
 * This is often used when converting a baked shell material.
 */
void MtlKeeper::setSelfIllum(osg::StateSet* stateset){

	osg::Material* osgMtl = (osg::Material*) stateset->getAttribute(osg::StateAttribute::MATERIAL);
	
	if(osgMtl){
        osg::Vec4 color=osgMtl->getDiffuse(osg::Material::FRONT_AND_BACK);
		osgMtl->setEmission(osg::Material::FRONT_AND_BACK, color);
        osgMtl->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0,0,0,color.a()));
   		osgMtl->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0,0,0,color.a()));
	}
}

/**
 * This method will set the MtlKeeper class's texture compression format.
 */
osg::Texture::InternalFormatMode MtlKeeper::getTexComp(Options* options){

	std::string comp(options->getTexComp(options->getTexCompIndex()));

	if(comp.compare(std::string(options->getTexComp(0)))==0)
		return osg::Texture::USE_IMAGE_DATA_FORMAT;
	else if (comp.compare(std::string(options->getTexComp(1)))==0)
		return osg::Texture::USE_ARB_COMPRESSION;
	else if (comp.compare(std::string(options->getTexComp(2)))==0)
		return osg::Texture::USE_S3TC_DXT1_COMPRESSION;
	else if (comp.compare(std::string(options->getTexComp(3)))==0)
		return osg::Texture::USE_S3TC_DXT3_COMPRESSION;
	else if (comp.compare(std::string(options->getTexComp(4)))==0)
		return osg::Texture::USE_S3TC_DXT5_COMPRESSION;
    else
        return osg::Texture::USE_IMAGE_DATA_FORMAT;
}

/**
 * This method will return whether image data is being embedded in an IVE file
 */
bool MtlKeeper::getEmbedImageData(Options* options)
{
	return (options->getExportExtension().compare(".ive")==0 
				|| options->getExportExtension().compare(".IVE")==0) && options->getIncludeImageDataInIveFile();
}

/**
 * This method will add a texture environment INTERPOLATE combiner
 * to the given stateset in the given texture unit.
 */
osg::ref_ptr<osg::TexEnvCombine> MtlKeeper::addInterpolateCombiner(osg::StateSet* stateset, int texUnit, float amount){
	osg::ref_ptr<osg::TexEnvCombine> texenv = new osg::TexEnvCombine();    
	texenv->setCombine_RGB(osg::TexEnvCombine::INTERPOLATE);
	texenv->setSource0_RGB(osg::TexEnvCombine::TEXTURE);
	texenv->setSource1_RGB(osg::TexEnvCombine::PREVIOUS);
	texenv->setSource2_RGB(osg::TexEnvCombine::CONSTANT);
	texenv->setConstantColor(osg::Vec4(amount, amount, amount, amount));
	stateset->setTextureAttributeAndModes(texUnit, texenv.get(), osg::StateAttribute::ON);
	return texenv;
}

/**
 * This method will add a texture environment SUBTRACT combiner
 * to the given stateset in the given texture unit.
 */
osg::ref_ptr<osg::TexEnvCombine> MtlKeeper::addSubtractCombiner(osg::StateSet* stateset, int texUnit){
	osg::ref_ptr<osg::TexEnvCombine> texenv = new osg::TexEnvCombine();    
	texenv->setCombine_RGB(osg::TexEnvCombine::SUBTRACT );
	texenv->setSource0_RGB(osg::TexEnvCombine::TEXTURE);
	texenv->setSource1_RGB(osg::TexEnvCombine::PREVIOUS);
	stateset->setTextureAttributeAndModes(texUnit, texenv.get(), osg::StateAttribute::ON);
	return texenv;
}

/**
 * This method will add a texture environment MODULATE combiner
 * to the given stateset in the given texture unit.
 */
osg::ref_ptr<osg::TexEnvCombine> MtlKeeper::addOpacityInterpolateCombiner(osg::StateSet* stateset, int texUnit, float amount){
	osg::ref_ptr<osg::TexEnvCombine> texenv = new osg::TexEnvCombine();
	texenv->setCombine_RGB(osg::TexEnvCombine::REPLACE);
	texenv->setSource0_RGB(osg::TexEnvCombine::PREVIOUS);
	texenv->setCombine_Alpha(osg::TexEnvCombine::INTERPOLATE);
	texenv->setSource0_Alpha(osg::TexEnvCombine::TEXTURE);
	texenv->setSource1_Alpha(osg::TexEnvCombine::PREVIOUS);
	texenv->setSource2_Alpha(osg::TexEnvCombine::CONSTANT);
	texenv->setConstantColor(osg::Vec4(amount, amount, amount, amount));
	stateset->setTextureAttributeAndModes(texUnit, texenv.get(), osg::StateAttribute::ON);
	return texenv;
} 

/**
 * This method will add an alpha blend function to the given stateset.
 */
osg::ref_ptr<osg::BlendFunc> MtlKeeper::addAlphaBlendFunc(osg::StateSet* stateset){
	osg::ref_ptr<osg::BlendFunc> osgBlendFunc = new osg::BlendFunc();
	osgBlendFunc->setFunction(osg::BlendFunc::SRC_ALPHA, osg::BlendFunc::ONE_MINUS_SRC_ALPHA);
	stateset->setAttribute(osgBlendFunc.get());
	stateset->setMode(GL_BLEND, osg::StateAttribute::ON);
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	return osgBlendFunc;
}

/** 
 * This method will add a texture generator to the given stateset.
 */
osg::ref_ptr<osg::TexGen> MtlKeeper::addTexGen(osg::StateSet* stateset, int texUnit, osg::TexGen::Mode mode){
	osg::ref_ptr<osg::TexGen> texgen = new osg::TexGen();
	texgen->setMode(mode);
	stateset->setTextureAttributeAndModes(texUnit,texgen.get(),osg::StateAttribute::ON);
	return texgen;	
}

/**
 * This method will add a texture environment the given 
 * stateset in the given texture unit.
 */
osg::ref_ptr<osg::TexEnv> MtlKeeper::addTexEnv(osg::StateSet* stateset, int texUnit, osg::TexEnv::Mode mode){
	osg::ref_ptr<osg::TexEnv> texenv = new osg::TexEnv();    
	texenv->setMode(mode);
	stateset->setTextureAttributeAndModes(texUnit, texenv.get(), osg::StateAttribute::ON);
	return texenv;
}

/** 
 * This method will return true if the MAX material is two sided.
 */
BOOL MtlKeeper::isTwoSided(Mtl* maxMtl){

	if(maxMtl == NULL)
		return FALSE;
	
	// Is this a standard material.
	if (maxMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)){
		// It is! See if it is two sided.
		StdMat* stdMtl = (StdMat *)maxMtl;
		return stdMtl->GetTwoSided();
	}
	// Is this a baked material
	else if(maxMtl->ClassID() == Class_ID(BAKE_SHELL_CLASS_ID, 0)){
		// We are only interrested in the baked material which is
		// sub material number 1 (original material is 0).
		Mtl* bakedMtl = maxMtl->GetSubMtl(1);
		if(bakedMtl && bakedMtl->ClassID() == Class_ID(DMTL_CLASS_ID, 0)){
			// See if this is two sided.
			StdMat* stdMtl = (StdMat *)bakedMtl;
			return stdMtl->GetTwoSided();
		}
	} else if(maxMtl->ClassID() == Class_ID(COMPMTL_CLASS_ID_A, COMPMTL_CLASS_ID_B)){
		//Check if any sub materials are two-sided
		if(maxMtl->NumSubMtls() > 0) {
			for(int i = 0; i < maxMtl->NumSubMtls(); i++) {
				StdMat* subMtl = (StdMat *)maxMtl->GetSubMtl(i);
				if(subMtl && subMtl->GetTwoSided()) return TRUE;
			}
		}
	}

	return FALSE;
}

/**
 * This method will return true if the texture is used as
 * an environment map.
 */
BOOL MtlKeeper::isEnvMap(Texmap* tmap){

	if (tmap && tmap->ClassID() == Class_ID(BMTEX_CLASS_ID, 0)) {

		// Cast the texmap to a bitmap texture. 
		BitmapTex *bmt = (BitmapTex*) tmap;
	    StdUVGen *uv = bmt->GetUVGen();

	    // Try Requirements, as StdUV does override this...
	    ULONG reqs = 0l;
	    reqs = uv->Requirements(0);
	    if (reqs & MTLREQ_VIEW_DEP || reqs == 0) {
			if(uv->GetCoordMapping(0)==UVMAP_SPHERE_ENV)
				return TRUE;
	    } 
	}
	return FALSE;
}

/** 
 * This method will return the next available texture unit in the
 * stateset and save the mapping channel used in MAX. We save the
 * mapping channel, such that we can use it later on when assigning
 * texture coordinates to the geometry. If a mapping channel with
 * the value of 0 is assigned, it simply means that the texture 
 * corrdinate is generate by a osg::TexGen.
 */
int MtlKeeper::getNextTexUnit(osg::StateSet* stateset, int mapChannel){
	int i=0;
	for(i=0; i<10; i++){
		osg::Texture* tex = (osg::Texture*)stateset->getTextureAttribute(i, osg::StateAttribute::TEXTURE);
		if(!tex) break;
	}
	// See if stateset is allready in mapping list.
	for(MtlKeeper::MappingList::iterator itr=_mappingList.begin(); itr!=_mappingList.end(); ++itr){
		if(itr->first == stateset){
			itr->second.push_back(mapChannel);
			return i;
		}
	}
	_mappingList[stateset].push_back(mapChannel);
	return i;
}

/**
 * Returns the number of texture units used in the given stateset.
 */
int MtlKeeper::getNumTexUnits(osg::StateSet* stateset){
	// See if stateset is in mapping list.
	for(MtlKeeper::MappingList::iterator itr=_mappingList.begin(); itr!=_mappingList.end(); ++itr){
		if(itr->first == stateset){
			return itr->second.size();;
		}
	}
	return 0;
}

/**
 * This method will return the mapping channel used in MAX for the
 * corresponding texture unit. It method maps texture units in a given
 * stateset to the mapping channel used in MAX.
 */
int MtlKeeper::getMapChannel(osg::StateSet* stateset, unsigned int unit){
	// See if stateset is in mapping list.
	for(MtlKeeper::MappingList::iterator itr=_mappingList.begin(); itr!=_mappingList.end(); ++itr){
		if(itr->first == stateset){
			if(unit< itr->second.size())
				return itr->second[unit];
		}
	}
	return -1;									
}

/**
 * Returns the mapping list associated with the given stateset.
 */
std::vector<int> MtlKeeper::getMappingList(osg::StateSet* stateset){
	// See if stateset is in mapping list.
	for(MtlKeeper::MappingList::iterator itr=_mappingList.begin(); itr!=_mappingList.end(); ++itr)
		if(itr->first == stateset)
			return itr->second;
	std::vector<int> ret;
	return ret;									
}

/**
 * This method finds the first texture and texture unit
 * in a stateset.
 */
osg::ref_ptr<osg::Texture> MtlKeeper::getTexture(osg::StateSet* stateset, int &texUnit){
	if(!stateset)
		return NULL;

	for(int i=0; i<10;i++){
		osg::ref_ptr<osg::Texture> tex = (osg::Texture*)stateset->getTextureAttribute(i, osg::StateAttribute::TEXTURE);
		texUnit = i;
		if(tex.valid())
			return tex;
	}
	return NULL;
}

/**
 * This method will set the wrap parameter for the texture in the given 
 * unit on the given stateset.
 */
void MtlKeeper::setTextureWrap(osg::StateSet* stateset, int texUnit, 
							   osg::Texture::WrapParameter param, osg::Texture::WrapMode mode){
	osg::Texture* tex = (osg::Texture*)stateset->getTextureAttribute(texUnit, osg::StateAttribute::TEXTURE);
	if(tex)
		tex->setWrap(param,mode);
}
