#include "catch.hpp"
#include <ugltf/ugltf.h>
#include <iostream>
#include <fstream>
#include <set>

#include <regex>


// this should be deprecated
SCENARIO("Writing GLB2")
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



        WHEN("We write the GLB file")
        {
            std::ofstream out("writeGLB.glb");
            M.writeGLB( out );
            out.close();

            THEN("We can read it back in")
            {
                std::ifstream in("writeGLB.glb");
                uGLTF::GLTFModel M2;
                REQUIRE( M2.load(in) );


                THEN("There is only one buffer")
                {
                    REQUIRE( M2.buffers.size()     == 1);
                }
                THEN("All teh buffer views point to that buffer")
                {
                    REQUIRE( M2.bufferViews.size() == 2);
                    REQUIRE( M2.bufferViews[0].buffer == 0 );
                    REQUIRE( M2.bufferViews[1].buffer == 0 );

                    REQUIRE( M2.bufferViews[0].byteOffset == 0);
                    REQUIRE( M2.bufferViews[1].byteOffset == M.buffers[0].byteLength);
                }

                THEN("The accessor values are correct")
                {
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
    }
}

