#include "catch.hpp"
#include <ugltf/ugltf.h>
#include <iostream>
#include <fstream>
#include <set>

#include <regex>


SCENARIO("Testing Accessors in a BufferView")
{
    GIVEN("A model with a single bufferView and two accessors (vec3 and vec2) interleaved within the view")
    {
        uGLTF::GLTFModel M;

        auto & B = M.newBuffer();

        using vec3 = std::array<uint32_t,3>;
        using vec2 = std::array<uint32_t,2>;

        auto bv_i = B.createNewBufferView( 10*(sizeof(vec3)+sizeof(vec2)), uGLTF::BufferViewTarget::ARRAY_BUFFER, sizeof(vec3)+sizeof(vec2), 1 );

        auto & Bv = M.bufferViews[ bv_i ];

        // the accessors will be interleaved in the bufferView:
        //   [vec3, vec2, vec3, vec2...]
        auto a1 = Bv.createNewAccessor(0            , 10, uGLTF::AccessorType::VEC3, uGLTF::ComponentType::UNSIGNED_INT);
        auto a2 = Bv.createNewAccessor(sizeof(vec3) , 10, uGLTF::AccessorType::VEC3, uGLTF::ComponentType::UNSIGNED_INT);

        auto V1 = M.accessors[a1].getSpan<vec3>();
        auto V2 = M.accessors[a2].getSpan<vec2>();

        THEN("The strides of each accessor's span is equal to the stride of the bufferView")
        {
            REQUIRE( V1.stride() == Bv.byteStride );
            REQUIRE( V2.stride() == Bv.byteStride );

            REQUIRE( V1.size() == 10 );
            REQUIRE( V2.size() == 10 );
        }
        THEN("The byte offset of the first elements of each span is equal to sizeof(vec3) ")
        {
            vec3 A = {0x11223344, 0xaabbccdd, 0x1a2a3a4a };
            vec2 B = {0x44332211, 0x1a2a3a4a };

            V1.set(0, A);
            V2.set(0, B);


            auto * raw_bytes = static_cast<unsigned char*>(Bv.data());

            {
                vec3 a;
                vec2 b;

                memcpy(&a, raw_bytes                , sizeof(vec3));
                memcpy(&b, raw_bytes+sizeof(vec3)   , sizeof(vec3));

                REQUIRE( A == a );
                REQUIRE( B == b );
            }

        }
    }




}

