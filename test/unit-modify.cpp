#include "catch.hpp"
#include <ugltf/ugltf.h>
#include <iostream>
#include <fstream>
#include <set>

SCENARIO("Merging Buffers")
{
    GIVEN("A GLTF Model with multiple buffers and multiple buffer views")
    {
        uGLTF::GLTFModel M;

        M.newBuffer();
        M.newBuffer();

        REQUIRE( M.buffers.size() == 2);

        auto & B1 = M.buffers[0];
        auto & B2 = M.buffers[1];

        B1.createNewBufferView(1024);
        B1.createNewBufferView(512);


        B2.createNewBufferView(1024);
        B2.createNewBufferView(512);

        REQUIRE( M.bufferViews.size() == 4);

        REQUIRE( M.bufferViews[0].buffer     == 0);
        REQUIRE( M.bufferViews[0].byteOffset == 0);
        REQUIRE( M.bufferViews[0].byteLength == 1024);

        REQUIRE( M.bufferViews[1].buffer     == 0);
        REQUIRE( M.bufferViews[1].byteOffset == 1024);
        REQUIRE( M.bufferViews[1].byteLength == 512);

        REQUIRE( M.bufferViews[2].buffer     == 1);
        REQUIRE( M.bufferViews[2].byteOffset == 0);
        REQUIRE( M.bufferViews[2].byteLength == 1024);

        REQUIRE( M.bufferViews[3].buffer     == 1);
        REQUIRE( M.bufferViews[3].byteOffset == 1024);
        REQUIRE( M.bufferViews[3].byteLength == 512);


        WHEN("We merge buffers")
        {
            M.mergeBuffers();

            THEN("There will be only one buffer in the model")
            {
                REQUIRE( M.buffers.size()==1 );
            }
            THEN("The byte length will the sum of all the buffers")
            {
                REQUIRE( M.buffers[0].byteLength == 3*1024);
            }

            THEN("All buffer views will have their buffer reference set to 0")
            {
                REQUIRE( M.bufferViews[0].buffer     == 0);
                REQUIRE( M.bufferViews[1].buffer     == 0);
                REQUIRE( M.bufferViews[2].buffer     == 0);
                REQUIRE( M.bufferViews[3].buffer     == 0);
            }

            THEN("All buffer views will have thhe same byteLength as before")
            {
                REQUIRE( M.bufferViews[0].byteLength == 1024);
                REQUIRE( M.bufferViews[1].byteLength == 512);
                REQUIRE( M.bufferViews[2].byteLength == 1024);
                REQUIRE( M.bufferViews[3].byteLength == 512);
            }

            THEN("All buffer views will have their byteOffset updated")
            {
                REQUIRE( M.bufferViews[0].byteOffset == 0);
                REQUIRE( M.bufferViews[1].byteOffset == 1024);
                REQUIRE( M.bufferViews[2].byteOffset == 1536);
                REQUIRE( M.bufferViews[3].byteOffset == 2560);
            }
        }
    }
}


SCENARIO("ImageData to Buffer Views")
{
    GIVEN("A GLTF Model with multiple buffers and multiple buffer views and ImageData which is not stored in the Buffer")
    {
        uGLTF::GLTFModel M;

        M.newBuffer();
        M.newBuffer();

        auto & I = M.newImage();
        I.m_imageData.push_back(0xAA);
        I.m_imageData.push_back(0xBB);
        I.m_imageData.push_back(0xCC);
        I.m_imageData.push_back(0xDD);
        I.m_imageData.push_back(0x11);
        I.m_imageData.push_back(0x22);
        I.m_imageData.push_back(0x33);
        I.m_imageData.push_back(0x44);


        REQUIRE( M.buffers.size() == 2);

        auto & B1 = M.buffers[0];
        auto & B2 = M.buffers[1];

        B1.createNewBufferView(1024);
        B1.createNewBufferView(512);

        B2.createNewBufferView(1024);
        B2.createNewBufferView(512);

        REQUIRE( M.bufferViews.size() == 4);

        REQUIRE( M.bufferViews[0].buffer     == 0);
        REQUIRE( M.bufferViews[0].byteOffset == 0);
        REQUIRE( M.bufferViews[0].byteLength == 1024);

        REQUIRE( M.bufferViews[1].buffer     == 0);
        REQUIRE( M.bufferViews[1].byteOffset == 1024);
        REQUIRE( M.bufferViews[1].byteLength == 512);

        REQUIRE( M.bufferViews[2].buffer     == 1);
        REQUIRE( M.bufferViews[2].byteOffset == 0);
        REQUIRE( M.bufferViews[2].byteLength == 1024);

        REQUIRE( M.bufferViews[3].buffer     == 1);
        REQUIRE( M.bufferViews[3].byteOffset == 1024);
        REQUIRE( M.bufferViews[3].byteLength == 512);


        WHEN("We convert the Image data into Buffer Data")
        {
            M.convertImagesToBuffers();

            THEN("A new buffer is created")
            {
                REQUIRE(M.buffers.size() == 3);
                REQUIRE(M.bufferViews.size() == 5);
            }

            THEN("The local image data is cleared")
            {
                REQUIRE(M.images[0].m_imageData.size() == 0);
            }

            THEN("The image points to a bufferView")
            {
                REQUIRE(M.images[0].bufferView == 4);
            }

            THEN("The image data is the same")
            {
                auto d = M.images[0].getSpan();
                REQUIRE( d[0] == 0xAA);
                REQUIRE( d[1] == 0xBB);
                REQUIRE( d[2] == 0xCC);
                REQUIRE( d[3] == 0xDD);
                REQUIRE( d[4] == 0x11);
                REQUIRE( d[5] == 0x22);
                REQUIRE( d[6] == 0x33);
                REQUIRE( d[7] == 0x44);
            }


        }
#if 0
        WHEN("We merge buffers")
        {
            M.mergeBuffers();

            THEN("There will be only one buffer in the model")
            {
                REQUIRE( M.buffers.size()==1 );
            }
            THEN("The byte length will the sum of all the buffers")
            {
                REQUIRE( M.buffers[0].byteLength == 3*1024);
            }

            THEN("All buffer views will have their buffer reference set to 0")
            {
                REQUIRE( M.bufferViews[0].buffer     == 0);
                REQUIRE( M.bufferViews[1].buffer     == 0);
                REQUIRE( M.bufferViews[2].buffer     == 0);
                REQUIRE( M.bufferViews[3].buffer     == 0);
            }

            THEN("All buffer views will have thhe same byteLength as before")
            {
                REQUIRE( M.bufferViews[0].byteLength == 1024);
                REQUIRE( M.bufferViews[1].byteLength == 512);
                REQUIRE( M.bufferViews[2].byteLength == 1024);
                REQUIRE( M.bufferViews[3].byteLength == 512);
            }

            THEN("All buffer views will have their byteOffset updated")
            {
                REQUIRE( M.bufferViews[0].byteOffset == 0);
                REQUIRE( M.bufferViews[1].byteOffset == 1024);
                REQUIRE( M.bufferViews[2].byteOffset == 1536);
                REQUIRE( M.bufferViews[3].byteOffset == 2560);
            }
        }
    #endif
    }
}
