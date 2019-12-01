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
   osg::Texture2D* WaterGridBuilder::CreateTexture(int width, int height, bool mipMap)
   {
      osg::Texture2D* tex = new osg::Texture2D();
      tex->setTextureSize(width, height);
      tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
      tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
      tex->setInternalFormat(GL_RGBA);
      if(mipMap)
      {
         tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR_MIPMAP_LINEAR);
         tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
         tex->setUseHardwareMipMapGeneration(true);
      }
      else
      {
         tex->setFilter(osg::Texture2D::MIN_FILTER,osg::Texture2D::LINEAR);
         tex->setFilter(osg::Texture2D::MAG_FILTER,osg::Texture2D::LINEAR);
      }
      return tex;
   }

   ////////////////////////////////////////////////////////////////////////////////
   osg::Geometry* WaterGridBuilder::BuildRadialGrid(float numRows, float numColumns, float& outComputedRadialDistance, float& outNearDistBetweenVerts, float& outFarDistBetweenVerts)
   {
      osg::Geometry* geometry = new osg::Geometry();

      int N = numRows; // rows from center outward
      int K = numColumns;// columns around the circle

      //calculate num verts and num indices
      int numVerts = N * K;
      int numIndices = (N - 1) * (K - 1) * 6;

      //lets make the geometry
      dtCore::RefPtr<osg::Vec4Array> pVerts = new osg::Vec4Array(numVerts);
      dtCore::RefPtr<osg::DrawElementsUInt> pIndices = new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES);
      pIndices->reserve(numIndices);

      float a0 = 0.5f;
      float a1 = 1.205f; // 5.0f;
      float outerMostRingDistance = 1250.0; // the furthest rings get an extra reach.
      float middleRingDistance = 12.5; // Middle rings get a minor boost too.
      int numOuterRings = 10;
      int numMiddleRings = 50;
      float innerExpBase = 1.0015f;
      float middleExpBase = 1.05f;
      float outerExpBase = 1.5f;
      //float exponent = 3;

      float r = a0;
      for(int i = 0; i < N; ++i)
      {
         float radiusIncrement = a0;
         // Radius extends with each row/ring.
         if (i != 0)
            radiusIncrement += a1 * powf(innerExpBase, i); //= a0 + a1 * powf(base, i);

         // Final rows/rings get an extra boost - solves some shrinkage & horizon problems.
         if ((i + numOuterRings) > N)
            radiusIncrement += outerMostRingDistance * powf(outerExpBase, (i + numOuterRings - N));
         // Middle rows/rings get a little boost - solves some shrinkage & horizon problems
         else if ((i + numOuterRings + numMiddleRings) > N)
            radiusIncrement += middleRingDistance * powf(middleExpBase, (i + numOuterRings + numMiddleRings - N));

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

            float groupNum = float(i);
            (*pVerts)[(i * K) + j ].set(x, y, z, groupNum);
         }


         //save the radius increment for cpu calculations
         if(i == 0)
         {
            outNearDistBetweenVerts = radiusIncrement;
         }
         else if(i == (N - numOuterRings ) )
         {
            outFarDistBetweenVerts = radiusIncrement;
            outComputedRadialDistance = r;
         }

      }

      //LOG_ALWAYS("WATER GRID REACH IN METERS = " + dtUtil::ToString(r));            

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
      //int numWaves = 16;

      float waveLenMod = 2.0;
      float ampMod = 0.25f;

      if(seaState == &WaterGridActor::SeaState::SeaState_0)
      {
         AddRandomWaves(waveList, waveLenMod * 13.1667f, ampMod * 0.64217f, 1.0f, 2.5f, 1);
         AddRandomWaves(waveList, waveLenMod * 12.3667f, ampMod * 0.7533f, 1.15f, 3.0f, 1);
         AddRandomWaves(waveList, waveLenMod * 17.5667f, ampMod * 0.931f, 2.1f, 4.5f, 1);
         AddRandomWaves(waveList, waveLenMod * 19.766f, ampMod * 1.357f, 1.0f, 5.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_1)
      {
         AddRandomWaves(waveList, waveLenMod * 13.1667f, ampMod * 0.64217f, 1.0f, 2.5f, 1);
         AddRandomWaves(waveList, waveLenMod * 12.3667f, ampMod * 0.7533f, 1.15f, 3.0f, 1);
         AddRandomWaves(waveList, waveLenMod * 17.5667f, ampMod * 1.0831f, 2.1f, 4.5f, 1);
         AddRandomWaves(waveList, waveLenMod * 19.766f, ampMod * 1.757f, 1.0f, 5.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_2)
      {         
         AddRandomWaves(waveList, waveLenMod * 11.667f, ampMod * 00.9667f, 1.5f, 5.0f, 2);
         AddRandomWaves(waveList, waveLenMod * 13.1667f, ampMod * 1.14217f, 1.0f, 2.5f, 2);
         AddRandomWaves(waveList, waveLenMod * 19.3667f, ampMod * 1.231f, 1.15f, 3.0f, 2);
         AddRandomWaves(waveList, waveLenMod * 21.5667f, ampMod * 1.36831f, 2.1f, 4.5f, 2);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_3)
      {
         AddRandomWaves(waveList, waveLenMod * 15.1667f, ampMod * 0.9331f, 1.0f, 2.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 17.3667f, ampMod * 1.046f, 1.15f, 3.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 19.5667f, ampMod * 1.187f, 2.1f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 23.766f, ampMod * 1.2951f, 1.0f, 5.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 28.766f, ampMod * 1.391f, 1.0f, 5.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_4)
      {
         AddRandomWaves(waveList, waveLenMod * 15.1667f, ampMod * 0.9331f, 1.0f, 2.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 19.3667f, ampMod * 1.046f, 1.15f, 3.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 25.5667f, ampMod * 1.187f, 2.1f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 28.766f, ampMod * 1.2951f, 1.0f, 5.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 31.766f, ampMod * 1.391f, 1.0f, 5.5f, 1);

      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_5)
      {
         AddRandomWaves(waveList, waveLenMod * 11.1667f, ampMod * 1.1431f, 1.0f, 2.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 15.3667f, ampMod * 1.2761f, 1.15f, 3.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 18.5667f, ampMod * 1.63f, 2.1f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 24.766f, ampMod * 2.1791f, 1.0f, 5.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 31.766f, ampMod * 2.821f, 1.0f, 5.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_6)
      {
         AddRandomWaves(waveList, waveLenMod * 11.1667f, ampMod * 1.431f, 1.0f, 2.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 15.3667f, ampMod * 2.2761f, 1.15f, 3.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 18.5667f, ampMod * 2.63f, 2.1f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 24.766f, ampMod * 3.1791f, 1.0f, 5.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 31.766f, ampMod * 3.821f, 1.0f, 5.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_7)
      {
         AddRandomWaves(waveList, waveLenMod * 11.1667f, ampMod * 1.7431f, 1.0f, 2.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 15.3667f, ampMod * 2.2761f, 1.15f, 3.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 28.5667f, ampMod * 3.63f, 2.1f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 34.766f, ampMod * 3.791f, 1.0f, 5.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 41.766f, ampMod * 4.821f, 1.0f, 5.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_8)
      {
         AddRandomWaves(waveList, waveLenMod * 15.3167f, ampMod * 2.167f, 1.0f, 2.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 21.667f, ampMod * 3.231f, 1.15f, 3.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 34.1667f, ampMod * 4.361f, 2.1f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 45.66f, ampMod * 4.0517f, 1.0f, 5.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 52.66f, ampMod * 5.2517f, 1.0f, 5.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_9)
      {
         AddRandomWaves(waveList, waveLenMod * 19.667f, ampMod * 2.1967f, 1.75f, 4.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 21.667f, ampMod * 3.667f, 1.0f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 37.66f, ampMod * 4.13f, 1.5f, 5.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 41.66f, ampMod * 5.391f, 1.5f, 5.5f, 3);
         AddRandomWaves(waveList, waveLenMod * 51.66f, ampMod * 6.91f, 1.5f, 5.5f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_10)
      {
         AddRandomWaves(waveList, waveLenMod * 19.667f, ampMod * 3.9147f, 1.0f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 29.66f, ampMod * 4.514f, 1.5f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 34.33, ampMod * 5.7541f, 1.5f, 5.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 51.33, ampMod * 6.4667, 2.0f, 5.0f, 3);
         AddRandomWaves(waveList, waveLenMod * 63.667, ampMod * 7.133f, 2.5f, 5.0f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_11)
      {
         AddRandomWaves(waveList, waveLenMod * 29.667f, ampMod * 6.9147f, 1.0f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 39.66f, ampMod * 7.514f, 1.5f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 54.33, ampMod * 8.7541f, 1.5f, 5.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 73.33, ampMod * 11.4667, 2.0f, 5.0f, 3);
         AddRandomWaves(waveList, waveLenMod * 101.667, ampMod * 15.133f, 2.5f, 5.0f, 1);
      }
      else if(seaState == &WaterGridActor::SeaState::SeaState_12)
      {
         AddRandomWaves(waveList, waveLenMod * 39.667f, ampMod * 7.9147f, 1.0f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 49.66f, ampMod * 8.514f, 1.5f, 4.5f, 4);
         AddRandomWaves(waveList, waveLenMod * 64.33, ampMod * 9.7541f, 1.5f, 5.0f, 4);
         AddRandomWaves(waveList, waveLenMod * 91.33, ampMod * 15.4667, 2.0f, 5.0f, 3);
         AddRandomWaves(waveList, waveLenMod * 114.667, ampMod * 19.133f, 2.5f, 5.0f, 1);
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
         w.mDirectionInDegrees = 90.0f + dtUtil::RandFloat(-7.3333f, 7.3333f);
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

}
////////////////////////////////////////////////////////////////////////////////
