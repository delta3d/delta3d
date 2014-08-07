#include <dtActors/watergridbuilder.h>

#include <dtUtil/mathdefines.h>

namespace dtActors
{
   ////////////////////////////////////////////////////////////////////////////////
   osg::Node* WaterGridBuilder::CreateQuad(osg::Texture2D* texture, int renderBin)
   {
      float offset = 10.0f;

      osg::Geometry* geo = new osg::Geometry;
      geo->setUseDisplayList(false);
      osg::Vec4Array* colors = new osg::Vec4Array;
      colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
      geo->setColorArray(colors);
      geo->setColorBinding(osg::Geometry::BIND_OVERALL);
      osg::Vec3Array *vx = new osg::Vec3Array;
      vx->push_back(osg::Vec3(-offset, -offset, 0));
      vx->push_back(osg::Vec3(offset, -offset, 0));
      vx->push_back(osg::Vec3(offset, offset, 0 ));
      vx->push_back(osg::Vec3(-offset, offset, 0));
      geo->setVertexArray(vx);
      osg::Vec3Array *nx = new osg::Vec3Array;
      nx->push_back(osg::Vec3(0, 0, 1));
      geo->setNormalArray(nx);

      if(texture != NULL)
      {
         osg::Vec2Array *tx = new osg::Vec2Array;
         tx->push_back(osg::Vec2(0, 0));
         tx->push_back(osg::Vec2(1, 0));
         tx->push_back(osg::Vec2(1, 1));
         tx->push_back(osg::Vec2(0, 1));
         geo->setTexCoordArray(0, tx);
         geo->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
      }

      geo->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));
      osg::Geode *geode = new osg::Geode;
      geode->addDrawable(geo);
      geode->setCullingActive(false);
      osg::StateSet* ss = geode->getOrCreateStateSet();
      ss->setMode(GL_LIGHTING, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
      ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OVERRIDE | osg::StateAttribute::OFF);
      ss->setRenderBinDetails(renderBin, "RenderBin");
      return geode;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Texture2D* WaterGridBuilder::CreateTexture(int width, int height)
   {
      osg::Texture2D* tex = new osg::Texture2D();
      tex->setTextureSize(width, height);
      tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
      tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
      tex->setInternalFormat(GL_RGBA);
      tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR/*_MIPMAP_LINEAR*/);
      tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR/*_MIPMAP_LINEAR*/);
      return tex;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Geometry* WaterGridBuilder::BuildRadialGrid(float &outComputedRadialDistance)
   {
      osg::Geometry* geometry = new osg::Geometry();

      int N = 360; // rows from center outward
      int K = 360;// columns around the circle

      //calculate num verts and num indices
      int numVerts = N * K;
      int numIndices = (N - 1) * (K - 1) * 6;

      //lets make the geometry
      dtCore::RefPtr<osg::Vec4Array> pVerts = new osg::Vec4Array(numVerts);
      dtCore::RefPtr<osg::DrawElementsUInt> pIndices = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
      pIndices->reserve(numIndices);

      float a0 = 0.01f;
      float a1 = 2.0f; // 5.0f;
      float outerMostRingDistance = 150.0; // the furthest rings get an extra reach.
      float middleRingDistance = 4.0; // Middle rings get a minor boost too.
      int numOuterRings = 16;
      int numMiddleRings = 16;
      float innerExpBase = 1.03f;
      float middleExpBase = 1.2f;
      float outerExpBase = 1.19f;
      //float exponent = 3;

      float r = a0;
      for(int i = 0; i < N; ++i)
      {
         float radiusIncrement = a0;
         // Radius extends with each row/ring.
         if (i != 0)
         {
            radiusIncrement += a1 * powf(innerExpBase, i); //= a0 + a1 * powf(base, i);
         }

         // Final rows/rings get an extra boost - solves some shrinkage & horizon problems.
         if ((i + numOuterRings) > N)
         {
            radiusIncrement += outerMostRingDistance * powf(outerExpBase, (i + numOuterRings - N));
         }
         // Middle rows/rings get a little boost - solves some shrinkage & horizon problems
         else if ((i + numOuterRings + numMiddleRings) > N)
         {
            radiusIncrement += middleRingDistance * powf(middleExpBase, (i + numOuterRings + numMiddleRings - N));
         }

         r += radiusIncrement;

         for(int j = 0; j < K; ++j)
         {
            //float PI2j = osg::PI * 2.0f * j;
            float x = 0.0;
            if(j == K - 1)
            {
               //setting this to one makes the water mesh fully wrap around
               x = 1;
            }
            else
            {
               //we are setting our vertex position as a -1,1 quantized angle and a depth
               x = float((2.0 * (double(j) / double(K))) - 1.0);
            }
            float y = r;
            float z = radiusIncrement; // We put the radius increment into the Z so we can use it in the shader
            
            float groupNum = 56.0f * (float(i) / float(N));
            (*pVerts)[(i * K) + j ].set(x, y, z, groupNum);
         }
      }
      outComputedRadialDistance = r;

      //std::cout << "WaterGridActor - Max Radial Distance = " << mComputedRadialDistance << std::endl;

      for(int i = 0; i < N - 1; ++i)
      {
         for(int j = 0; j < K - 1; ++j)
         {
            int JPlusOne = (j + 1);// % K;

            pIndices->addElement( (i * K) + j );
            pIndices->addElement( ((i + 1) * K) + j );
            pIndices->addElement( ((i + 1) * K) + (JPlusOne) );

            pIndices->addElement( ((i + 1) * K) + (JPlusOne) );
            pIndices->addElement( (i * K) + (JPlusOne) );
            pIndices->addElement( (i * K) + j );
            
         }
      }

      geometry->setVertexArray(pVerts.get());      
      geometry->addPrimitiveSet(pIndices.get());
      
      return geometry;
   }


   ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
   void WaterGridBuilder::BuildWavesFromSeaState(WaterGridActor::SeaState* seaState, std::vector<WaterGridActor::Wave>& waveList)
   {
      int numWaves = 16;

      float waveLenMod = 2.0f;
      float ampMod = 0.25f;

      if(seaState == &WaterGridActor::SeaState::SeaState_0)
      {
         //AddRandomWaves(waveList, waveLenMod * 3.167f, ampMod * 00.16f, 1.0f, 2.5f, numWaves);
         AddDefaultWaves(waveList);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_1)
      {
         AddRandomWaves(waveList, waveLenMod * 8.667f, ampMod * 00.667f, 1.5f, 5.0f, numWaves);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_2)
      {         
         AddRandomWaves(waveList, waveLenMod * 12.667f, ampMod * 0.9667f, 1.75f, 6.0f, numWaves);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_3)
      {
         AddRandomWaves(waveList, waveLenMod * 16.667f, ampMod * 1.1667f, 2.0f, 6.5f, numWaves);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_4)
      {
         AddRandomWaves(waveList, waveLenMod * 3.167f, ampMod * 00.16f, 1.0f, 2.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 12.667f, ampMod * 0.9667f, 1.75f, 6.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 16.667f, ampMod * 1.1667f, 2.0f, 6.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 20.66f, ampMod * 1.5f, 4.5f, 8.5f, 4);

      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_5)
      {
         AddRandomWaves(waveList, waveLenMod * 12.667f, ampMod * 0.9667f, 1.75f, 6.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 16.667f, ampMod * 1.1667f, 2.0f, 6.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 20.66f, ampMod * 1.5f, 4.5f, 8.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 26.66f, ampMod * 2.0f, 4.5f, 8.5f, 4);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_6)
      {
         AddRandomWaves(waveList, waveLenMod * 11.667f, ampMod * 1.1471f, 2.0f, 6.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 26.66f, ampMod * 2.14f, 4.5f, 8.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 43.33, ampMod * 3.341f, 5.0f, 9.0f, 2);
         AddRandomWaves(waveList, waveLenMod * 65.33, ampMod * 5.4667, 6.0f, 8.0f, 2);
         AddRandomWaves(waveList, waveLenMod * 95.667, ampMod * 6.133f, 7.5f, 12.0f, 4);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_7)
      {
         AddRandomWaves(waveList, waveLenMod * 9.667f, ampMod * 1.167f, 3.0f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 26.66f, ampMod * 3.14f, 4.5f, 8.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 63.33, ampMod * 4.341f, 7.0f, 13.0f, 2);
         AddRandomWaves(waveList, waveLenMod * 85.33, ampMod * 6.667, 6.0f, 10.0f, 2);
         AddRandomWaves(waveList, waveLenMod * 109.667, ampMod * 8.33f, 10.5f, 20.0f, 4);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_8)
      {
         AddRandomWaves(waveList, waveLenMod * 9.667f, ampMod * 1.167f, 3.0f, 4.5f, 2);
         AddRandomWaves(waveList, waveLenMod * 26.66f, ampMod * 3.14f, 4.5f, 8.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 63.33, ampMod * 5.341f, 7.0f, 13.0f, 2);
         AddRandomWaves(waveList, waveLenMod * 85.33, ampMod * 7.667, 6.0f, 10.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 109.667, ampMod * 10.33f, 10.5f, 20.0f, 4);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_9)
      {
         AddRandomWaves(waveList, waveLenMod * 12.667f, ampMod * 1.1667f, 3.0f, 6.5f, 2);
         AddRandomWaves(waveList, waveLenMod * 26.66f, ampMod * 3.14f, 4.5f, 8.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 63.33, ampMod * 5.341f, 7.0f, 13.0f, 2);
         AddRandomWaves(waveList, waveLenMod * 83.33, ampMod * 9.667, 6.0f, 10.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 129.667, ampMod * 12.33f, 10.5f, 20.0f, 4);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_10)
      {
         AddRandomWaves(waveList, waveLenMod * 12.667f, ampMod * 2.1667f, 4.0f, 6.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 26.66f, ampMod * 4.14f, 3.5f, 5.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 73.33, ampMod * 8.341f, 5.0f, 11.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 133.33, ampMod * 16.667, 8.0f, 12.0f, 2);
         AddRandomWaves(waveList, waveLenMod * 329.667, ampMod * 28.33f, 10.5f, 20.0f, 2);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_11)
      {
         AddRandomWaves(waveList, waveLenMod * 12.667f, ampMod * 1.1667f, 2.0f, 6.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 22.667f, ampMod * 4.1667f, 2.0f, 6.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 53.33, ampMod * 8.0f, 7.0f, 13.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 143.33, ampMod * 16.667, 6.0f, 10.0f, 2);
         AddRandomWaves(waveList, waveLenMod * 399.667, ampMod * 64.33f, 10.5f, 20.0f, 2);

      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_12)
      {        
         AddRandomWaves(waveList, waveLenMod * 38.667f, ampMod * 2.1667f, 2.0f, 6.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 46.667f, ampMod * 4.1667f, 2.0f, 6.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 93.33, ampMod * 6.667, 6.0f, 10.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 123.33, ampMod * 12.67f, 8.5f, 14.0f, 2);
         AddRandomWaves(waveList, waveLenMod * 545.667, ampMod * 125.33f, 33.5f, 33.0f, 3);
      }

   }


   /////////////////////////////////////////////////////////////////////////////
   void WaterGridBuilder::AddRandomWaves(std::vector<WaterGridActor::Wave>& waveList, float meanWaveLength, float meanAmplitude, float minPeriod, float maxPeriod, unsigned numWaves)
   {
      float waveLenStart = meanWaveLength / 2.0f;
      float waveLenIncrement = meanWaveLength / numWaves;

      float ampStart = meanAmplitude / 2.0f;
      float ampIncrement = meanAmplitude / numWaves;

      for(unsigned i = 0; i < numWaves; ++i, waveLenStart += waveLenIncrement, ampStart += ampIncrement)
      {
         WaterGridActor::Wave w;
         w.mWaveLength = waveLenStart;
         w.mAmplitude = ampStart;
         w.mSpeed = dtUtil::RandFloat(minPeriod, maxPeriod);
         w.mSteepness = 1.0f;
         w.mDirectionInDegrees = dtUtil::RandFloat(-10.3333f, 10.3333f);
         SetWaveDirection(w);
         waveList.push_back(w); // -3
      }

   }


   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridBuilder::BuildTextureWaves(std::vector<WaterGridActor::TextureWave>& waveList)
   {
      const float kArray[] = {1.33f, 1.76f, 3.0f, 2.246f,
         1.0f, 3.71f, 1.0f, 1.75f,
         1.5f, 1.0f, 1.0f, 2.0f,
         2.2f, 2.0f, 1.113f, 1.0f,
         1.33f, 1.76f, 3.0f, 2.246f,
         1.0f, 3.71f, 1.0f, 1.75f,
         1.5f, 1.0f, 1.0f, 2.0f,
         2.2f, 2.0f, 1.113f, 1.0f};

      const float waveLengthArray[] = {0.1788f, 0.0535f, 0.12186f, 0.24f,
         0.14f, 0.116844f, 0.97437f, 0.0805f,
         0.067f, 0.3565f, 0.67135f , 0.191f,
         0.155f, 0.13917f, 0.275f, .448f,
         0.1788f, 0.0535f, 0.12186f, 0.24f,
         0.14f, 0.116844f, 0.97437f, 0.0805f,
         0.067f, 0.3565f, 0.67135f , 0.191f,
         0.155f, 0.13917f, 0.275f, .448f};

      const float waveSpeedArray[] = {0.0953f, 0.03839f, 0.0311f, 0.04221f,
         0.11497f, 0.143213f, 0.14571f, 0.051181f,

         0.01473f, 0.1531f, 0.2131f, 0.0221f,
         0.121497f, 0.1213f, 0.14571f, 0.1181f,
         0.0953f, 0.03839f, 0.0311f, 0.04221f,
         0.11497f, 0.143213f, 0.14571f, 0.051181f,

         0.01473f, 0.1531f, 0.2131f, 0.0221f,
         0.121497f, 0.1213f, 0.14571f, 0.1181f};

      for(int i = 0; i < WaterGridActor::MAX_TEXTURE_WAVES; ++i)
      {
         WaterGridActor::TextureWave tw;
         tw.mSteepness = kArray[i];
         tw.mSpeed = waveSpeedArray[i];
         tw.mWaveLength = waveLengthArray[i];

         waveList.push_back(tw);
      }
   }

   ////////////////////////////////////////////////////////////////////////////////
   void WaterGridBuilder::SetWaveDirection(WaterGridActor::Wave& wave)
   {
      wave.mDirection.set(sin(osg::DegreesToRadians(wave.mDirectionInDegrees)),
         cos(osg::DegreesToRadians(wave.mDirectionInDegrees)));

      wave.mDirection.normalize();
   }

   void WaterGridBuilder::AddDefaultWaves(std::vector<WaterGridActor::Wave>& waveList)
   {
      WaterGridActor::Wave w;

      //w.mWaveLength = 4.8f;
      //w.mAmplitude = 0.08f;
      //w.mSpeed = 0.5f;
      //w.mSteepness = 0.5;
      //w.mDirectionInDegrees = -5.7f;
      //SetWaveDirection(w);
      //waveList.push_back(w); // -3

      w.mWaveLength = 5.33f;
      w.mAmplitude = 0.09f;
      w.mSpeed = 0.3f;
      w.mSteepness = 0.1f;
      w.mDirectionInDegrees = 181.0f;
      SetWaveDirection(w);
      waveList.push_back(w); // 1

      //w.mWaveLength = 6.37f;
      //w.mAmplitude = 0.11f;
      //w.mSpeed = 0.7f;
      //w.mSteepness = 0.7f;
      //w.mDirectionInDegrees = 15.3f;
      //SetWaveDirection(w);
      //waveList.push_back(w);  // -1

      w.mWaveLength = 6.89f;
      w.mAmplitude = 0.08f;
      w.mSpeed = 0.65f;
      w.mSteepness = 0.8f;
      w.mDirectionInDegrees = 4.2f;
      SetWaveDirection(w);
      waveList.push_back(w); // 2

      w.mWaveLength = 16.51f;
      w.mAmplitude = 0.11f;
      w.mSpeed = 0.8f;
      w.mSteepness = 0.1f;
      w.mDirectionInDegrees = -15.0f;
      SetWaveDirection(w);
      waveList.push_back(w); // 3

      w.mWaveLength = 19.89f;
      w.mAmplitude = 0.12f;
      w.mSpeed = 1.2f;
      w.mSteepness = 0.1f;
      w.mDirectionInDegrees = 184.0f;
      SetWaveDirection(w);
      waveList.push_back(w); // 4

      w.mWaveLength = 35.51f;
      w.mAmplitude = 0.22f;
      w.mSpeed = 1.3f;
      w.mSteepness = 0.3f;
      w.mDirectionInDegrees = 10.7f;
      SetWaveDirection(w);
      waveList.push_back(w);  // 5

      w.mWaveLength = 42.18f;
      w.mAmplitude = 0.35f;
      w.mSpeed = 1.1f;
      w.mSteepness = 0.3f;
      w.mDirectionInDegrees = 0.0f;
      SetWaveDirection(w);
      waveList.push_back(w); // 6

      //w.mWaveLength = 54.055f;
      //w.mAmplitude = 0.28f;
      //w.mSpeed = 1.2f;
      //w.mSteepness = 0.2f;
      //w.mDirectionInDegrees = -10.4f;
      //SetWaveDirection(w);
      //waveList.push_back(w); // 5

      //w.mWaveLength = 92.15f;
      //w.mAmplitude = 0.25f;
      //w.mSpeed = 1.5f;
      //w.mSteepness = 0.1f;
      //w.mDirectionInDegrees = 178.1f;
      //SetWaveDirection(w);
      //waveList.push_back(w);  // 6

      w.mWaveLength = 106.98f;
      w.mAmplitude = 0.34f;
      w.mSpeed = 1.8f;
      w.mSteepness = 0.3f;
      w.mDirectionInDegrees = 5.0f;
      SetWaveDirection(w);
      waveList.push_back(w); // 7

      w.mWaveLength = 148.7f;
      w.mAmplitude = 0.25f;
      w.mSpeed = 2.3f;
      w.mSteepness = 0.1f;
      w.mDirectionInDegrees = 5.0f;
      SetWaveDirection(w);
      waveList.push_back(w); // 8

      w.mWaveLength = 181.35f;
      w.mAmplitude = 0.88f;
      w.mSpeed = 2.8f;
      w.mSteepness = 0.6f;
      w.mDirectionInDegrees = -1.8f;
      SetWaveDirection(w);
      waveList.push_back(w); // 9

      //w.mWaveLength = 188.9f;
      //w.mAmplitude = 0.92f;
      //w.mSpeed = 3.5f;
      //w.mSteepness = 0.1f;
      //w.mDirectionInDegrees = 1.3f;
      //SetWaveDirection(w);
      //waveList.push_back(w);  // 10

      //w.mWaveLength = 200.25f;
      //w.mAmplitude = 0.62f;
      //w.mSpeed = 3.5f;
      //w.mSteepness = 0.1f;
      //w.mDirectionInDegrees = 8.1f;
      //SetWaveDirection(w);
      //waveList.push_back(w);  // 11

      //w.mWaveLength = 213.275f;
      //w.mAmplitude = 0.3f;
      //w.mSpeed = 3.8f;
      //w.mSteepness = 0.5f;
      //w.mDirectionInDegrees = 5.3f;
      //SetWaveDirection(w);
      //waveList.push_back(w); // 12

      w.mWaveLength = 507.4f;
      w.mAmplitude = 0.12f;
      w.mSpeed = 2.5f;
      w.mSteepness = 0.0f;
      w.mDirectionInDegrees = 174.0f;
      SetWaveDirection(w);
      waveList.push_back(w); // 10

      w.mWaveLength = 526.5f;
      w.mAmplitude = 1.8f;
      w.mSpeed = 6.9f;
      w.mSteepness = 0.5f;
      w.mDirectionInDegrees = -3.0f;
      SetWaveDirection(w);
      waveList.push_back(w); // 11

      //w.mWaveLength = 540.4f;
      //w.mAmplitude = 1.91f;
      //w.mSpeed = 6.8f;
      //w.mSteepness = 0.1f;
      //w.mDirectionInDegrees = 2.5f;
      //SetWaveDirection(w);
      //waveList.push_back(w); // 15

      //w.mWaveLength = 557.2f;
      //w.mAmplitude = 0.63f;
      //w.mSpeed = 1.8f;
      //w.mSteepness = 0.0f;
      //w.mDirectionInDegrees = -179.3f;
      //SetWaveDirection(w);
      //waveList.push_back(w);  // 16

      w.mWaveLength = 582.4f;
      w.mAmplitude = 1.51f;
      w.mSpeed = 7.2f;
      w.mSteepness = 0.3f;
      w.mDirectionInDegrees = 12.0f;
      SetWaveDirection(w);
      waveList.push_back(w);  // 12

      w.mWaveLength = 1250.3f;
      w.mAmplitude = 2.5f;
      w.mSpeed = 9.2f;
      w.mSteepness = 0.1f;
      w.mDirectionInDegrees = 3.0f;
      SetWaveDirection(w);
      waveList.push_back(w); // 13

      w.mWaveLength = 1268.0f;
      w.mAmplitude = 0.9f;
      w.mSpeed = 3.5f;
      w.mSteepness = 0.0f;
      w.mDirectionInDegrees = 183.0f;
      SetWaveDirection(w);
      waveList.push_back(w); // 14

      //w.mWaveLength = 1280.7f;
      //w.mAmplitude = 2.61f;
      //w.mSpeed = 9.3f;
      //w.mSteepness = 0.5f;
      //w.mDirectionInDegrees = -3.5f;
      //SetWaveDirection(w);
      //waveList.push_back(w); // 20

      //w.mWaveLength = 1310.6f;
      //w.mAmplitude = 1.5f;
      //w.mSpeed = 2.8f;
      //w.mSteepness = 0.3f;
      //w.mDirectionInDegrees = -179.3f;
      //SetWaveDirection(w);
      //waveList.push_back(w);  // 21

      //w.mWaveLength = 1325.1f;
      //w.mAmplitude = 2.9f;
      //w.mSpeed = 10.1f;
      //w.mSteepness = 0.2f;
      //w.mDirectionInDegrees = -6.0f;
      //SetWaveDirection(w);
      //waveList.push_back(w);  // 15

      w.mWaveLength = 3600.3f;
      w.mAmplitude = 5.2f;
      w.mSpeed = 15.2f;
      w.mSteepness = 0.1f;
      w.mDirectionInDegrees = 2.5f;
      SetWaveDirection(w);
      waveList.push_back(w); // 15

      //w.mWaveLength = 3727.7f;
      //w.mAmplitude = 4.1f;
      //w.mSpeed = 14.3f;
      //w.mSteepness = 0.5f;
      //w.mDirectionInDegrees = -3.9f;
      //SetWaveDirection(w);
      //waveList.push_back(w); // 24

      //w.mWaveLength = 3929.6f;
      //w.mAmplitude = 1.8f;
      //w.mSpeed = 7.4f;
      //w.mSteepness = 0.1f;
      //w.mDirectionInDegrees = -179.3f;
      //SetWaveDirection(w);
      //waveList.push_back(w);  // 25

      w.mWaveLength = 4017.1f;
      w.mAmplitude = 6.1f;
      w.mSpeed = 16.1f;
      w.mSteepness = 0.2f;
      w.mDirectionInDegrees = -5.7f;
      SetWaveDirection(w);
      waveList.push_back(w);  // 16
   }
}
////////////////////////////////////////////////////////////////////////////////
