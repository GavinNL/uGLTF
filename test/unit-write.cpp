#include "catch.hpp"
#include <ugltf/ugltf.h>
#include <iostream>
#include <fstream>
#include <set>


SCENARIO("Writing a GLB file")
{
    GIVEN("A GLTF model with a texture")
    {
        uGLTF::GLTFModel M;

        std::ifstream in("BoxTextured.gltf");

        REQUIRE( M.load(in) );

        WHEN("We save the model as a GLB file")
        {
            std::ofstream out("BoxTextured.gltf.glb");
            M.writeGLB( out );
            out.close();

            WHEN("We can load model again")
            {
                std::ifstream in2("BoxTextured.gltf.glb");
                uGLTF::GLTFModel M2;
                REQUIRE( M2.load(in2) );
                THEN("The embedded buffers are loaded into the m_data variable")
                {
                    REQUIRE( M2.buffers.size() == 1);
                    REQUIRE( M2.buffers[0].byteLength == 840);
                    REQUIRE( M2.buffers[0].m_data.size() == 840);
                    REQUIRE( M2.buffers[0].uri == "");
                }


                THEN("The images are loaded into the m_imageData variable")
                {
                    REQUIRE( M2.images.size() == 1);
                    REQUIRE( M2.images[0].bufferView == std::numeric_limits<uint32_t>::max() );
                    REQUIRE( M2.images[0].m_imageData.size() == 23516 );
                }
            }

        }
    }
}



SCENARIO("Writing a GLTF embedded")
{
    GIVEN("A GLTF model with a texture")
    {
        uGLTF::GLTFModel M;

        std::ifstream in("BoxTextured.gltf");

        REQUIRE( M.load(in) );

        WHEN("We save the model as a GLTF embedded file")
        {
            std::ofstream out("BoxTextured.gltf.gltf");
            M.writeEmbedded( out );
            out.close();

            WHEN("We can load model again")
            {
                std::ifstream in2("BoxTextured.gltf.gltf");
                uGLTF::GLTFModel M2;
                REQUIRE( M2.load(in2) );
                THEN("The embedded buffers are loaded into the m_data variable")
                {
                    REQUIRE( M2.buffers.size() == 1);
                    REQUIRE( M2.buffers[0].byteLength == 840);
                    REQUIRE( M2.buffers[0].m_data.size() == 840);
                    REQUIRE( M2.buffers[0].uri == "");
                }


                THEN("The images are loaded into the m_imageData variable")
                {
                    REQUIRE( M2.images.size() == 1);
                    REQUIRE( M2.images[0].bufferView == std::numeric_limits<uint32_t>::max() );
                    REQUIRE( M2.images[0].m_imageData.size() == 23516 );
                }
            }

        }
    }
}

