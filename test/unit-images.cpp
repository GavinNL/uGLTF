#include "catch.hpp"
#include <ugltf/ugltf.h>
#include <iostream>
#include <fstream>
#include <set>

#include <regex>


SCENARIO("Test Writing a GLB/NFTF files with images")
{
    GIVEN("A GLTF model with a 2 images")
    {
        uGLTF::GLTFModel M;

        {
            auto & I = M.newImage();
            I.m_imageData.insert( I.m_imageData.end(), 16, 0);
        }
        {
            auto & I = M.newImage();
            I.m_imageData.insert( I.m_imageData.end(), 16, 0xFF);
        }

        THEN("When we generate the JSON with embeddedbuffers turned off, the image URI is still written")
        {
            auto J = M.generateJSON(false);

          //  std::cout << J.dump(4);

            REQUIRE( J["images"].size() == 2);
            REQUIRE( J["images"][0]["uri"].get<std::string>() == "data:application/octet-stream;base64,AAAAAAAAAAAAAAAAAAAAAA==");
            REQUIRE( J["images"][1]["uri"].get<std::string>() == "data:application/octet-stream;base64,/////////////////////w==");
        }

        THEN("We can convert all the images data into Buffers")
        {
            M.convertImagesToBuffers();
            auto J = M.generateJSON(false);

            std::cout << J.dump(4) << std::endl;

            REQUIRE( J["images"].size() == 2);
            REQUIRE( J["images"][0].count("uri") == 0);
            REQUIRE( J["images"][1].count("uri") == 0);

            REQUIRE( J["images"][0]["bufferView"].get<uint32_t>() == 0);
            REQUIRE( J["images"][1]["bufferView"].get<uint32_t>() == 1);

            REQUIRE( J.count("bufferViews") == 1);
            REQUIRE( J.count("buffers") == 1);


            REQUIRE( J["bufferViews"].size() == 2);
            REQUIRE( J["buffers"].size() == 1);
        }

        THEN("We can convert all images into buffers")
        {
            M.convertImagesToBuffers();
            auto J = M.generateJSON(true);

            std::cout << J.dump(4) << std::endl;

            REQUIRE( J["images"].size() == 2);
            REQUIRE( J["images"][0].count("uri") == 0);
            REQUIRE( J["images"][1].count("uri") == 0);

            REQUIRE( J["images"][0]["bufferView"].get<uint32_t>() == 0);
            REQUIRE( J["images"][1]["bufferView"].get<uint32_t>() == 1);

            REQUIRE( J.count("bufferViews") == 1);
            REQUIRE( J.count("buffers") == 1);


            REQUIRE( J["bufferViews"].size() == 2);
            REQUIRE( J["buffers"].size() == 1);

            REQUIRE( J["buffers"][0].count("uri") == 1);
            REQUIRE( J["buffers"][0]["uri"].get<std::string>() == "data:application/octet-stream;base64,AAAAAAAAAAAAAAAAAAAAAP////////////////////8=");


            auto & B1 = M.images[0].getBufferView();
            auto & B2 = M.images[1].getBufferView();
            REQUIRE( B1.byteLength == 16);
            REQUIRE( B2.byteLength == 16);

            auto * b1 = static_cast<unsigned char*>(B1.data());
            auto * b2 = static_cast<unsigned char*>(B2.data());

            for(size_t i=0;i<16;i++)
            {
                REQUIRE( b1[i] == 0 );
                REQUIRE( b2[i] == 0xFF );
            }
        }

    }

}



