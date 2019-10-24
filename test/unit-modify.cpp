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
