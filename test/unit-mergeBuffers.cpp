#include "catch.hpp"
#include <ugltf/ugltf.h>
#include <iostream>
#include <fstream>
#include <set>

#include <regex>


// this should be deprecated
SCENARIO("Merging Buffers")
{
    using vec3 = std::array<uint32_t,3>;

    GIVEN("A model with a two BufferViews and two Buffers")
    {
        uGLTF::GLTFModel M;

        vec3 temp  = {0x11223344, 0xaabbccdd, 0x1a2a3a4a };
        vec3 temp2 = {0xabcdef12, 0x12efcdab, 0x1f2e3e4d };

        // Create two buffers
        #define TOTAL_SIZE 2
        {
            auto & B = M.newBuffer();

            auto bv_i = B.createNewBufferView( TOTAL_SIZE*sizeof(vec3), uGLTF::BufferViewTarget::ARRAY_BUFFER, sizeof(vec3), 1 );

            auto & Bv = M.bufferViews[ bv_i ];

            // the accessors will be interleaved in the bufferView:
            auto a1 = Bv.createNewAccessor(0, TOTAL_SIZE, uGLTF::AccessorType::VEC3, uGLTF::ComponentType::UNSIGNED_INT);

            auto & A = M.accessors[a1];

            for(uint32_t i=0;i<TOTAL_SIZE;i++)
                A.setValue(i, temp);
        }
        {
            auto & B = M.newBuffer();

            auto bv_i = B.createNewBufferView( TOTAL_SIZE*sizeof(vec3), uGLTF::BufferViewTarget::ARRAY_BUFFER, sizeof(vec3), 1 );

            auto & Bv = M.bufferViews[ bv_i ];

            // the accessors will be interleaved in the bufferView:
            auto a1 = Bv.createNewAccessor(0, TOTAL_SIZE, uGLTF::AccessorType::VEC3, uGLTF::ComponentType::UNSIGNED_INT);

            auto & A = M.accessors[a1];

            for(uint32_t i=0;i<TOTAL_SIZE;i++)
                A.setValue(i, temp2);
        }


        REQUIRE( M.buffers.size()     == 2);
        REQUIRE( M.bufferViews.size() == 2);


        REQUIRE( M.bufferViews[0].byteOffset == 0);
        REQUIRE( M.bufferViews[1].byteOffset == 0);

        THEN("When we get the merged buffer views")
        {
            auto bv = M.generateMergedBufferViews();

            REQUIRE( bv[0].byteOffset == 0);
            REQUIRE( bv[1].byteOffset == M.buffers[0].byteLength);
        }

        THEN("When we get the merged buffer")
        {
            auto bv = M.generateMergedBuffer();

            REQUIRE( bv.byteLength == M.buffers[0].byteLength+M.buffers[1].byteLength);
        }
    }
}

