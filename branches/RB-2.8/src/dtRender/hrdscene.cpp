/* -*-c++-*-
* Delta3D Open Source Game and Simulation Engine
* Copyright (C) 2014, Caper Holdings, LLC
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
* Bradley Anderegg
*/

#include <dtRender/HDRScene.h>

#include <dtCore/observerptr.h>

#include <osg/Camera>

#include <osgPPU/Processor.h>
#include <osgPPU/Unit.h>

namespace dtRender
{

   
//---------------------------------------------------------------
// PPU setup for HDR Rendering
//
// The pipeline is build based on the following:
//     http://msdn2.microsoft.com/en-us/library/bb173484(VS.85).aspx
//
//---------------------------------------------------------------
class HDRRendering
{
    public:
        float mMidGrey;
        float mHDRBlurSigma;
        float mHDRBlurRadius;
        float mGlareFactor;
        float mAdaptFactor;
        float mMinLuminance;
        float mMaxLuminance;

        // Setup default hdr values
        HDRRendering()
        {
            mMidGrey = 0.45;
            mHDRBlurSigma = 4.0;
            mHDRBlurRadius = 7.0;
            mGlareFactor = 2.5;
            mMinLuminance = 0.2;
            mMaxLuminance = 5.0;
            mAdaptFactor = 0.01;
        }

        //------------------------------------------------------------------------
        void createHDRPipeline(osgPPU::Processor* parent, osgPPU::Unit*& firstUnit, osgPPU::Unit*& lastUnit)
        {
            osg::ref_ptr<osgDB::ReaderWriter::Options> fragmentOptions = new osgDB::ReaderWriter::Options("fragment");
            osg::ref_ptr<osgDB::ReaderWriter::Options> vertexOptions = new osgDB::ReaderWriter::Options("vertex");

            // first a simple bypass to get the data from somewhere
            // there must be a camera bypass already specified
            // You need this ppu to relay on it with following ppus
            osgPPU::UnitBypass* bypass = new osgPPU::UnitBypass();
            bypass->setName("HDRBypass");
            firstUnit = bypass;
            lastUnit = bypass;
//return;

            // Now we have got a texture with only to bright pixels.
            // To simulate hdr glare we have to blur this texture.
            // We do this by first downsampling the texture and
            // applying separated gauss filter afterwards.
            osgPPU::UnitInResampleOut* resample = new osgPPU::UnitInResampleOut();
            {
                resample->setName("Resample");
                resample->setFactorX(0.25);
                resample->setFactorY(0.25);
            }
            bypass->addChild(resample);

            // Now we need a ppu which do compute the luminance of the scene.
            // We need to compute luminance per pixel and current luminance
            // of all pixels. For the first case we simply bypass the incoming
            // data through a luminance shader, which do compute the luminance.
            // For the second case we use the concept of mipmaps and store the
            // resulting luminance in the last mipmap level. For more info about
            // this step take a look into the according shaders.
            osgPPU::UnitInOut* pixelLuminance = new osgPPU::UnitInOut();
            pixelLuminance->setName("ComputePixelLuminance");
            {
                // create shader which do compute luminance per pixel
                osgPPU::ShaderAttribute* lumShader = new osgPPU::ShaderAttribute();
                lumShader->addShader(osgDB::readShaderFile("Data/glsl/luminance_fp.glsl", fragmentOptions.get()));
                lumShader->setName("LuminanceShader");
                lumShader->add("texUnit0", osg::Uniform::SAMPLER_2D);
                lumShader->set("texUnit0", 0);

                // set both shaders
                //pixelLuminance->setShader(lumShader);
                pixelLuminance->getOrCreateStateSet()->setAttributeAndModes(lumShader);
            }
            resample->addChild(pixelLuminance);

            // now we do the second case computing the average scene luminance based on mipmaps
            osgPPU::UnitInMipmapOut* sceneLuminance = new osgPPU::UnitInMipmapOut();
            sceneLuminance->setName("ComputeSceneLuminance");
            {
                // create shader which do compute the scene's luminance in mipmap levels
                osgPPU::ShaderAttribute* lumShaderMipmap = new osgPPU::ShaderAttribute();
                lumShaderMipmap->addShader(osgDB::readShaderFile("Data/glsl/luminance_mipmap_fp.glsl", fragmentOptions.get()));
                lumShaderMipmap->setName("LuminanceShaderMipmap");

                // setup input texture
                lumShaderMipmap->add("texUnit0", osg::Uniform::SAMPLER_2D);
                lumShaderMipmap->set("texUnit0", 0);

                // set shader
                //sceneLuminance->setShader(lumShaderMipmap);
                sceneLuminance->getOrCreateStateSet()->setAttributeAndModes(lumShaderMipmap);

                // we want that the mipmaps are generated for the input texture 0,
                // which is the pixelLuminance
                // Here no new textures are generated, but hte input texture is get
                // additional mipmap levels, where we store our results
                sceneLuminance->setGenerateMipmapForInputTexture(0);
            }
            pixelLuminance->addChild(sceneLuminance);

            // Now we need to setup a ppu which do pass only bright values
            // This ppu has two inputs, one is the original hdr scene data
            // and the other is the compute luminance. The according shader
            // do perform simple tonemapping operation and decides then
            // which pixels are too bright, so that they can not be represented
            // correctly. This pixels are passed through and will be blurred
            // later to simulate hdr glare.
            osgPPU::Unit* brightpass = new osgPPU::UnitInOut();
            brightpass->setName("Brightpass");
            {
                // setup brightpass shader
                osgPPU::ShaderAttribute* brightpassSh = new osgPPU::ShaderAttribute();
                brightpassSh->addShader(osgDB::readShaderFile("Data/glsl/brightpass_fp.glsl", fragmentOptions.get()));
                brightpassSh->setName("BrightpassShader");

                brightpassSh->add("g_fMiddleGray", osg::Uniform::FLOAT);
                brightpassSh->set("g_fMiddleGray", mMidGrey);
                //brightpass->setShader(brightpassSh);
                brightpass->getOrCreateStateSet()->setAttributeAndModes(brightpassSh);

                // brightpass ppu does get two input textures, hence add them
                brightpass->setInputToUniform(resample, "hdrInput", true);
                brightpass->setInputToUniform(sceneLuminance, "lumInput", true);
            }

            // now we perform a gauss blur on the downsampled data
            osgPPU::UnitInOut* blurx = new osgPPU::UnitInOut();
            osgPPU::UnitInOut* blury = new osgPPU::UnitInOut();
            {
                // set name and indicies
                blurx->setName("BlurHorizontal");
                blury->setName("BlurVertical");

                // read shaders from file
                osg::Shader* vshader = osgDB::readShaderFile("Data/glsl/gauss_convolution_vp.glsl", vertexOptions.get());
                osg::Shader* fhshader = osgDB::readShaderFile("Data/glsl/gauss_convolution_1Dx_fp.glsl", fragmentOptions.get());
                osg::Shader* fvshader = osgDB::readShaderFile("Data/glsl/gauss_convolution_1Dy_fp.glsl", fragmentOptions.get());

                // setup horizontal blur shaders
                osgPPU::ShaderAttribute* gaussx = new osgPPU::ShaderAttribute();
                gaussx->addShader(vshader);
                gaussx->addShader(fhshader);
                gaussx->setName("BlurHorizontalShader");

                gaussx->add("sigma", osg::Uniform::FLOAT);
                gaussx->add("radius", osg::Uniform::FLOAT);
                gaussx->add("texUnit0", osg::Uniform::SAMPLER_2D);

                gaussx->set("sigma", mHDRBlurSigma);
                gaussx->set("radius", mHDRBlurRadius);
                gaussx->set("texUnit0", 0);

                //blurx->setShader(gaussx);
                blurx->getOrCreateStateSet()->setAttributeAndModes(gaussx);

                // setup vertical blur shaders
                osgPPU::ShaderAttribute* gaussy = new osgPPU::ShaderAttribute();
                gaussy->addShader(vshader);
                gaussy->addShader(fvshader);
                gaussy->setName("BlurVerticalShader");

                gaussy->add("sigma", osg::Uniform::FLOAT);
                gaussy->add("radius", osg::Uniform::FLOAT);
                gaussy->add("texUnit0", osg::Uniform::SAMPLER_2D);

                gaussy->set("sigma", mHDRBlurSigma);
                gaussy->set("radius", mHDRBlurRadius);
                gaussy->set("texUnit0", 0);

                //blury->setShader(gaussy);
                blury->getOrCreateStateSet()->setAttributeAndModes(gaussy);
            }

            brightpass->addChild(blurx);
            blurx->addChild(blury);

            // And finally we add a ppu which do use all the computed results:
            //  hdr scene data, luminance and blurred bright pixels
            // to combine them together. This is done by applying tonemapping
            // operation on the hdr values and adding the blurred brightpassed
            // pixels with some glare factor on it.
            osgPPU::Unit* hdr = new osgPPU::UnitInOut();
            {
                // setup inputs, name and index
                hdr->setName("HDR-Result");

                // setup shader
                osgPPU::ShaderAttribute* sh = new osgPPU::ShaderAttribute();
                sh->addShader(osgDB::readShaderFile("Data/glsl/tonemap_hdr_fp.glsl", fragmentOptions.get()));
                sh->setName("HDRResultShader");

                sh->add("fBlurFactor", osg::Uniform::FLOAT);
                sh->add("g_fMiddleGray", osg::Uniform::FLOAT);

                sh->set("fBlurFactor", mGlareFactor);
                sh->set("g_fMiddleGray", mMidGrey);

                //hdr->setShader(sh);
                hdr->getOrCreateStateSet()->setAttributeAndModes(sh);
                hdr->setInputTextureIndexForViewportReference(0); // we want to setup viewport based on this input

                // add inputs as uniform parameters
                hdr->setInputToUniform(bypass, "hdrInput", true);
                hdr->setInputToUniform(blury, "blurInput", true);
                hdr->setInputToUniform(sceneLuminance, "lumInput", true);
            }

            // this is the last unit which is responsible for rendering, the rest is like offline units
            lastUnit = hdr;

            // Create a simple ppu which do
            // compute the adapted luminance value.
            // The ppu does use the input of the previous frame and do recompute it.
            osgPPU::UnitInOut* adaptedlum = new osgPPU::UnitInOut();
            {
                adaptedlum->setName("AdaptedLuminance");

                // create shader which do compute the adapted luminance value
                osgPPU::ShaderAttribute* adaptedShader = new osgPPU::ShaderAttribute();
                adaptedShader->addShader(osgDB::readShaderFile("Data/glsl/luminance_adapted_fp.glsl", fragmentOptions.get()));
                adaptedShader->setName("AdaptLuminanceShader");

                // setup computed current luminance  input texture
                adaptedShader->add("texLuminance", osg::Uniform::SAMPLER_2D);
                adaptedShader->set("texLuminance", 0);

                // shader do also need the adapted luminance as input
                adaptedShader->add("texAdaptedLuminance", osg::Uniform::SAMPLER_2D);
                adaptedShader->set("texAdaptedLuminance", 1);

                // setup shader parameters
                adaptedShader->add("maxLuminance", osg::Uniform::FLOAT);
                adaptedShader->add("minLuminance", osg::Uniform::FLOAT);
                adaptedShader->add("adaptScaleFactor", osg::Uniform::FLOAT);

                // invFrameTime - parameter is updated during the runtime, hence add the
                // uniform to the unit and not to the shader, so that it can be accessed easily.
                //adaptedShader->add("invFrameTime", osg::Uniform::FLOAT);
                adaptedlum->getOrCreateStateSet()->getOrCreateUniform("invFrameTime", osg::Uniform::FLOAT);

                // Set maximum and minimum representable luminance values
                adaptedShader->set("maxLuminance", mMaxLuminance);
                adaptedShader->set("minLuminance", mMinLuminance);

                // Set scaling factor which decides how fast eye adapts to new luminance
                adaptedShader->set("adaptScaleFactor", mAdaptFactor);

                // set both shaders
                //adaptedlum->setShader(adaptedShader);
                adaptedlum->getOrCreateStateSet()->setAttributeAndModes(adaptedShader);

                // we just want to have this size of the viewport
                // we do not want to have any referenced viewport, therefor -1
                adaptedlum->setViewport(new osg::Viewport(0,0,1,1));
                adaptedlum->setInputTextureIndexForViewportReference(-1);
            }
            // second input is the input from the scene luminance
            sceneLuminance->addChild(adaptedlum);


            // The adapted luminance ppu do compute it. However if you
            // can follow me for now, you maybe encounter, that this ppu do
            // have to write into the same texture as it also read from.
            // To prevent this, we just generate an inout ppu which do
            // nothing than render the copy of input to the output.
            // We will use the output of this ppu as input for the
            // adapted luminance ppu. In this way we do not write to the
            // same texture as we have readed from.
            osgPPU::UnitInOut* adaptedlumCopy = new osgPPU::UnitInOut();
            adaptedlumCopy->setName("AdaptedLuminanceCopy");
            adaptedlumCopy->addChild(adaptedlum);

            // now connect the output of the adaptedlum with the rest where it is needed
            adaptedlum->addChild(adaptedlumCopy);

            adaptedlum->addChild(brightpass);
            brightpass->setInputToUniform(adaptedlum, "texAdaptedLuminance");

            adaptedlum->addChild(hdr);
            hdr->setInputToUniform(adaptedlum, "texAdaptedLuminance");
        }
};




   const dtCore::RefPtr<SceneType> HDRScene::HDR_SCENE(new SceneType("HDR Scene", "Scene", "Creates an osgPPU HDR Scene."));

   HDRScene::HDRScene(osg::Camera* cam)
   : BaseClass(*PPU_SCENE, SceneEnum::PRE_RENDER)
   , mImpl(new HDRSceneImpl())
   {
      SetName("HDRScene");
      
      mImpl->mPPUProcessor->setCamera(cam);
   }


   HDRScene::~HDRScene()
   {
      delete mImpl;
   }


   void HDRScene::CreateScene( SceneManager& sm, const GraphicsQuality& g)
   {
      osgPPU::Processor* proc = PPUScene::GetPPUProcessor();

      osgPPU::Unit* firstUnit;
      osgPPU::Unit* lastUnit;

      proc->dirtyUnitSubgraph();

      mHDRSetup.createHDRPipeline(mProcessor.get(), firstUnit, lastUnit);
      proc->addChild(firstUnit);


      // add a text ppu after the pipeline is setted up
      osgPPU::UnitText* fpstext = new osgPPU::UnitText();
      {
            fpstext->setName("FPSTextPPU");
            fpstext->setSize(44);
            fpstext->setText("Example HDR-pipeline from a .ppu file (note: no change in adaptive luminance)");
            fpstext->setPosition(0.01, 0.95);
            lastUnit->addChild(fpstext);
      }

      // now just as a gimmick do setup a text ppu, to render some info on the screen
      osgPPU::UnitText* pputext = new osgPPU::UnitText();
      pputext->setName("TextPPU");
      pputext->setSize(46);
      pputext->setText("osgPPU rocks!");
      pputext->setPosition(0.025, 0.425);
      fpstext->addChild(pputext);

      // The following setup does show how to include an offline ppu into the graph
      // This unit will just render the content of the input unit in a small window
      // over the screen.
      osgPPU::UnitInOut* bgppu = new osgPPU::UnitInOut();
      {
            // This is a simple texture unit, which do just provide a given texture
            // to the output, so that all children units can access this texture as input.
            osgPPU::UnitTexture* unittex = new osgPPU::UnitTexture();

            // it doesn't matter where to put this unit in the graph, because it
            // does not use any input. However so that this unit is updated every
            // frame we put it somewhere, in this case under the processor
            mProcessor->addChild(unittex);

            // just to find it later in the graph
            unittex->setName("TextureUnit");

            // input texture is output of the camera bypass, so that we see original scene view
            osg::Texture2D* img = new osg::Texture2D();
            osg::ref_ptr<osg::Image> imgfile = osgDB::readImageFile("textures/ShaderBase/reflect.rgb");
            if (imgfile == NULL)
            {
               printf("File not found: textures/ShaderBase/reflect.rgb !\n");
            }
            img->setImage(imgfile);
            unittex->setTexture(img);

            // create picture in picture ppu
            bgppu->setName("PictureInPicturePPU");

            // this ppu has to be rendered after the hdr pipeline output
            // however it shouldn't take the input of hdr output.
            pputext->addChild(bgppu);

            // at the beginning we setup the unittex as input to this unit
            unittex->addChild(bgppu);

            // we bypass the first input and also set them to ignore
            // this will make sure that bypassed input is used as soutput and ignored input 
            // is not used in computations
            bgppu->setInputBypass(0);
            bgppu->setIgnoreInput(0);

            // we do not want to use any ppu for viewport reference because we setted up our own viewport
            bgppu->setInputTextureIndexForViewportReference(-1);

            // setup new viewport, which will change the rendering position
            osg::Viewport* vp = new osg::Viewport(*(getCamera()->getViewport()));
            vp->x() = 10;
            vp->y() = 10;
            vp->width() *= 0.4;
            vp->height() *= 0.3;
            bgppu->setViewport(vp);
      }



      // As a last step we setup a ppu which do render the content of the result
      // on the screenbuffer. This ppu MUST be as one of the last, otherwise you
      // will not be able to get results from the ppu pipeline
      osgPPU::UnitOut* ppuout = new osgPPU::UnitOut();
      ppuout->setName("PipelineResult");
      ppuout->setInputTextureIndexForViewportReference(-1); // need this here to get viewport from camera
      bgppu->addChild(ppuout);
   }

   
   
}//namespace dtRender