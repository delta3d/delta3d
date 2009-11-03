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
 *	FILE:			Options.cpp
 *
 *	DESCRIPTION:	Source file for the Options class. This class 
 *					provides methods to get and set the options of the 
 *					exporter.
 *
 *	CREATED BY:		Rune Schmidt Jensen, rune@schmidt-jensen.com
 *
 *	HISTORY:		Created 27.01.2003
 *
 *					17.02.2003 added support for making the options
 *					"stiky" over sessions by writing and reading 
 *					properties to disk.
 *
 *                  08.02.2006 Joran: Added option to export textures
 *                  as jpegs.
 *
 *					27.09.2007 Farshid Lashkari: Fix for a small memory 
 *					leak in the Options class.
 *
 *                  05.11.2007 Daniel Sjolie: Added useOriginalTextures Option
 */		

#include "Options.h"

/**
 * Constructor for the Options class; sets the default values.
 */
Options::Options(){

	// Options for the plugin.
	_exportGeometry				= TRUE;
	_exportTextureCoords		= TRUE;
	_exportVertexNormals		= TRUE;
	_exportVertexColors			= TRUE;
	_exportMaterials			= TRUE;
	_exportTextures				= TRUE;
	_exportCameras				= TRUE;
	_exportLights				= TRUE;
	_exportHelpers				= TRUE;
	_exportShapes				= TRUE;
	_exportHiddenNodes			= FALSE;
	_exportAnimations			= TRUE;
	_exportParticles			= TRUE;
	_normalize					= TRUE;
	_whitenTexMat				= TRUE;
	_whitenSelfIllum			= TRUE;
	_saveFile					= TRUE;
	_quickView					= FALSE;
	_showErrMsg					= TRUE;
	_useIndices					= FALSE;
	_writeTexture				= TRUE;
	_includeImageDataInIveFile	= TRUE;

	_numTexFormat				= 4;
	_texFormatIndex				= 2;
	_texFormat					= new char*[_numTexFormat];
	// Must be sorted in alphabetic order.
    _texFormat[0]               = _T("jpg"); // No support for alpha channels.
	_texFormat[1]				= _T("png");
	_texFormat[2]				= _T("tga");
	_texFormat[3]				= _T("tif"); // No support for alpha channels.

	_numTexComp					= 5;
	_texCompIndex				= 0;
	_texComp					= new char*[_numTexComp];
	_texComp[0]					= _T("IMAGE_DATA_FORMAT");
	_texComp[1]					= _T("ARB_COMPRESSION");
	_texComp[2]					= _T("S3TC_DXT1_COMPRESSION");
	_texComp[3]					= _T("S3TC_DXT3_COMPRESSION");
	_texComp[4]					= _T("S3TC_DXT5_COMPRESSION");

	_useDefaultNodeMaskValue	= TRUE;
	_defaultNodeMaskValue		= 0x000000FF;

	_triStripGeometry			= FALSE;
	_mergeGeometry				= FALSE;
	_flattenStaticTransform		= FALSE;
	_shareDuplicateStates		= FALSE;
	_spatializeGroups			= FALSE;

	_turnOffLighting			= FALSE;

    _exportOpacityMaps          = TRUE;
    _exportUnknownMaps          = TRUE;
    _autoAlphaTextures          = TRUE;
    _autoTwoSidedLighting       = FALSE;
    _exportSelfIllumMaps        = FALSE;
    _exportPointHelpers         = FALSE;
}

Options::~Options()
{
	delete [] _texFormat;
	delete [] _texComp;
}

// NOTE: Update anytime the CFG file changes
#define CFG_VERSION 0x0B

/**
 * Write the properties of the options class to disk.
 */
BOOL Options::write( TSTR filename){
	FILE* cfgStream;

	cfgStream = fopen(filename, "wb");
	if (!cfgStream)
		return FALSE;

	// Write CFG version
	_putw(CFG_VERSION,				cfgStream);

	fputc(getExportGeometry(),		cfgStream);
	fputc(getExportTextureCoords(),	cfgStream);
	fputc(getExportVertexNormals(),	cfgStream);
	fputc(getExportVertexColors(),	cfgStream);
	fputc(getExportMaterials(),		cfgStream);
	fputc(getExportTextures(),		cfgStream);
	fputc(getExportCameras(),		cfgStream);
	fputc(getExportLights(),		cfgStream);
	fputc(getExportHelpers(),		cfgStream);
	fputc(getExportHiddenNodes(),	cfgStream);
	fputc(getExportAnimations(),	cfgStream);
	fputc(getNormalize(),			cfgStream);
	fputc(getWhitenTexMat(),		cfgStream);
	// Deprecated from version 0x05 and on.
//	fputc(getTriStrip(),			cfgStream);
	// Substituted with CONSTANT TRUE value:
	fputc(TRUE,						cfgStream);
	fputc(getWriteTexture(),		cfgStream);

	_putw(getTexFormatIndex(),		cfgStream);
	_putw(getTexCompIndex(),		cfgStream);

	// Added in new version 0x02
	fputc(getSaveFile(),			cfgStream);
	fputc(getQuickView(),			cfgStream);

	// Added in new version 0x03
	fputc(getExportShapes(),		cfgStream);

	// Added in new version 0x04
	fputc(getIncludeImageDataInIveFile(),	cfgStream);

	// Added in new version 0x05
	fputc(getWhitenSelfIllum(),		cfgStream);

	// Added in new version 0x06
	fputc(getShowErrMsg(),			cfgStream);
	fputc(getUseIndices(),			cfgStream);
	fputc(getExportParticles(),		cfgStream);

	// Added in new version 0x07
	fputc(getUseDefaultNodeMaskValue(),		cfgStream);
	_putw(getDefaultNodeMaskValue(),		cfgStream);

	// Added in new version 0x08
	fputc(getTriStripGeometry(),		cfgStream);
	fputc(getMergeGeometry(),			cfgStream);
	fputc(getFlattenStaticTransform(),	cfgStream);
	fputc(getShareDuplicateStates(),	cfgStream);
	fputc(getSpatializeGroups(),		cfgStream);

	// Added in new version 0x09
	fputc(getTurnOffLighting(),			cfgStream);

    // Added in new version 0x0A
    fputc(getExportOpacityMaps(),       cfgStream);
    fputc(getExportUnknownMaps(),       cfgStream);
    fputc(getAutoAlphaTextures(),       cfgStream);
    fputc(getAutoTwoSidedLighting(),    cfgStream);
    fputc(getExportSelfIllumMaps(),     cfgStream);
    fputc(getExportPointHelpers(),      cfgStream);

	// Added in new version 0x0B
	fputc(getUseOriginalTextureFiles(),		cfgStream);

    fputc(getReferencedGeometry(),      cfgStream);

	fclose(cfgStream);

	return TRUE;
}

/**
 * Load the properties of the options class from disk.
 */
BOOL Options::load(TSTR filename){

	FILE* cfgStream;

	cfgStream = fopen(filename, "rb");
	if (!cfgStream)
		return FALSE;

	// First item is a file version
	int fileVersion = _getw(cfgStream);

	if (fileVersion > CFG_VERSION) {
		// Unknown version
		fclose(cfgStream);
		return FALSE;
	}

	setExportGeometry(fgetc(cfgStream));
	setExportTextureCoords(fgetc(cfgStream));
	setExportVertexNormals(fgetc(cfgStream));
	setExportVertexColors(fgetc(cfgStream));
	setExportMaterials(fgetc(cfgStream));
	setExportTextures(fgetc(cfgStream));
	setExportCameras(fgetc(cfgStream));
	setExportLights(fgetc(cfgStream));
	setExportHelpers(fgetc(cfgStream));
	setExportHiddenNodes(fgetc(cfgStream));
	setExportAnimations(fgetc(cfgStream));
	setNormalize(fgetc(cfgStream));
	setWhitenTexMat(fgetc(cfgStream));
	// Deprecated
	//setTriStrip(fgetc(cfgStream));
	// Replaced with:
	fgetc(cfgStream);
	setWriteTexture(fgetc(cfgStream));

	setTexFormatIndex(_getw(cfgStream));
	setTexCompIndex(_getw(cfgStream));

	// Added for version 0x02
	if (fileVersion > 0x01) {
		setSaveFile(fgetc(cfgStream));
		setQuickView(fgetc(cfgStream));
	}

	// Added for version 0x03
	if (fileVersion > 0x02) {
		setExportShapes(fgetc(cfgStream));
	}
	// Added for version 0x04
	if (fileVersion > 0x03) {
		setIncludeImageDataInIveFile(fgetc(cfgStream));
	}
	// Added for version 0x05
	if (fileVersion > 0x04) {
		setWhitenSelfIllum(fgetc(cfgStream));
	}
	// Added for version 0x06
	if (fileVersion > 0x05) {
		setShowErrMsg(fgetc(cfgStream));
		setUseIndices(fgetc(cfgStream));
		setExportParticles(fgetc(cfgStream));
	}

	// Added for version 0x07
	if (fileVersion > 0x06) {
		setUseDefaultNodeMaskValue(fgetc(cfgStream));
		setDefaultNodeMaskValue(_getw(cfgStream));
	}

	// Added for version 0x08
	if (fileVersion > 0x07) {
		setTriStripGeometry(fgetc(cfgStream));
		setMergeGeometry(fgetc(cfgStream));
		setFlattenStaticTransform(fgetc(cfgStream));
		setShareDuplicateStates(fgetc(cfgStream));
		setSpatializeGroups(fgetc(cfgStream));
	}
	// Added for version 0x09
	if (fileVersion > 0x08) {
		setTurnOffLighting(fgetc(cfgStream));
	}
    // Added for version 0x0A
    if (fileVersion > 0x09) {
        setExportOpacityMaps(fgetc(cfgStream));
        setExportUnknownMaps(fgetc(cfgStream));
        setAutoAlphaTextures(fgetc(cfgStream));
        setAutoTwoSidedLighting(fgetc(cfgStream));
        setExportSelfIllumMaps(fgetc(cfgStream));
        setExportPointHelpers(fgetc(cfgStream));
    }

	// Added for version 0x0B
	if (fileVersion > 0x0A) {
		setUseOriginalTextureFiles(fgetc(cfgStream));
	}

    if (fileVersion > 0x0A) 
    {
       setReferencedGeometry(fgetc(cfgStream));
    }

	fclose(cfgStream);

	return TRUE;
}
