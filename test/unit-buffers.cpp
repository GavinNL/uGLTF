#include "catch.hpp"
#include <ugltf/ugltf.h>
#include <iostream>
#include <fstream>
#include <set>

#include <regex>


// this should be deprecated
SCENARIO("Test Writing a GLB with multiple buffers")
{
    using vec3 = std::array<uint32_t,3>;

    GIVEN("A model with a single bufferView and two accessors (vec3 and vec2) interleaved within the view")
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

        THEN("When we save the GLB file")
        {
            std::ofstream out("unit-buffers.glb");
            M.writeGLB( out );
        }
        THEN("When we read them back in")
        {
            uGLTF::GLTFModel M2;
            std::ifstream in("unit-buffers.glb");

            REQUIRE( M2.load(in) );

            REQUIRE(M2.buffers.size() == 2);
            REQUIRE(M2.bufferViews.size() == 2);
            REQUIRE(M2.accessors.size() == 2);

            auto & A1 = M2.accessors[0];
            auto & A2 = M2.accessors[1];

            REQUIRE( A1.count == 2);
            REQUIRE( A2.count == 2);

            REQUIRE( A1.getValue<vec3>(0) == temp);
            REQUIRE( A1.getValue<vec3>(1) == temp);

            REQUIRE( A2.getValue<vec3>(0) == temp2);
            REQUIRE( A2.getValue<vec3>(1) == temp2);

        }

        THEN("When we save the GLTF file")
        {
            std::ofstream out("unit-buffers.gltf");
            M.writeEmbeddedGLTF( out );
        }
        THEN("When we read them back in")
        {
            uGLTF::GLTFModel M2;
            std::ifstream in("unit-buffers.gltf");

            REQUIRE( M2.load(in) );

            REQUIRE(M2.buffers.size() == 2);
            REQUIRE(M2.bufferViews.size() == 2);
            REQUIRE(M2.accessors.size() == 2);

            auto & A1 = M2.accessors[0];
            auto & A2 = M2.accessors[1];

            REQUIRE( A1.count == 2);
            REQUIRE( A2.count == 2);

            REQUIRE( A1.getValue<vec3>(0) == temp);
            REQUIRE( A1.getValue<vec3>(1) == temp);

            REQUIRE( A2.getValue<vec3>(0) == temp2);
            REQUIRE( A2.getValue<vec3>(1) == temp2);

        }
    }

}

