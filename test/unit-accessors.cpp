#define UGLTF_USE_SPAN
#include "catch.hpp"
#include <ugltf/ugltf.h>
#include <iostream>
#include <fstream>
#include <set>

#include <regex>

// this should be deprecated
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
        auto a2 = Bv.createNewAccessor(sizeof(vec3) , 10, uGLTF::AccessorType::VEC2, uGLTF::ComponentType::UNSIGNED_INT);

        THEN("The byte offset of the first elements of each span is equal to sizeof(vec3) ")
        {
            vec3 A = {0x11223344, 0xaabbccdd, 0x1a2a3a4a };
            vec2 B = {0x44332211, 0x1a2a3a4a };

            M.accessors[a1].setValue(0, A);
            M.accessors[a2].setValue(0, B);


            auto * raw_bytes = static_cast<unsigned char*>(Bv.data());

            {
                vec3 a;
                vec2 b;

                memcpy(&a, raw_bytes                , sizeof(a));
                memcpy(&b, raw_bytes+sizeof(vec3)   , sizeof(b));

                REQUIRE( A == a );
                REQUIRE( B == b );
            }

        }
    }

}



SCENARIO("Testing Accessors get/set values")
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
        auto a2 = Bv.createNewAccessor(sizeof(vec3) , 10, uGLTF::AccessorType::VEC2, uGLTF::ComponentType::UNSIGNED_INT);

        auto & acc1 = M.accessors.at(a1);
        auto & acc2 = M.accessors.at(a2);

        REQUIRE( acc1.byteOffset == 0);
        REQUIRE( acc2.byteOffset == sizeof(vec3));

        THEN("Testing setValue/getValue")
        {
            vec3 A = {0x11223344, 0xaabbccdd, 0x1a2a3a4a };
            vec2 B = {0x44332211, 0x1a2a3a4a };

            acc1.setValue(0, A);
            acc2.setValue(0, B);

            {
                vec3 a;
                vec2 b;

                acc1.getValue(0, a);
                acc2.getValue(0, b);

                REQUIRE( A == a );
                REQUIRE( B == b );
            }
        }

        THEN("Testing setValues")
        {
            std::vector<vec3> Av;
            std::vector<vec2> Bv;

            Av.push_back( {0x1, 0x2, 0x3 } );
            Av.push_back( {0x4, 0x5, 0x6 } );

            Bv.push_back( {0xa, 0xb } );
            Bv.push_back( {0xc, 0xd } );

            acc1.setValues(0, &Av[0], 2);
            acc2.setValues(0, &Bv[0], 2);


            {
                std::vector<vec3> Av2;
                std::vector<vec2> Bv2;

                Av2.resize(2);
                Bv2.resize(2);

                acc1.getValues(0, &Av2[0], 2);
                acc2.getValues(0, &Bv2[0], 2);

                REQUIRE( Av2[0][0] == Av[0][0] );
                REQUIRE( Av2[0][1] == Av[0][1] );
                REQUIRE( Av2[0][2] == Av[0][2] );

                REQUIRE( Av2[1][0] == Av[1][0] );
                REQUIRE( Av2[1][1] == Av[1][1] );
                REQUIRE( Av2[1][2] == Av[1][2] );

                REQUIRE( Bv2[0][0] == Bv[0][0] );
                REQUIRE( Bv2[0][1] == Bv[0][1] );
                REQUIRE( Bv2[1][0] == Bv[1][0] );
                REQUIRE( Bv2[1][1] == Bv[1][1] );
            }
        }
    }

}



SCENARIO("Copying Accessors")
{
    GIVEN("A model with a single bufferView and two accessors (vec3 and vec2) interleaved within the view")
    {
        uGLTF::GLTFModel M;
        uGLTF::GLTFModel M2;



        using vec3 = std::array<uint32_t,3>;

        {
            auto & B = M.newBuffer();

            auto bv_i = B.createNewBufferView( 10*sizeof(vec3), uGLTF::BufferViewTarget::ARRAY_BUFFER, sizeof(vec3), 1 );

            auto & Bv = M.bufferViews[ bv_i ];

            auto a1 = Bv.createNewAccessor(0            , 10, uGLTF::AccessorType::VEC3, uGLTF::ComponentType::UNSIGNED_INT);
            REQUIRE( a1 == 0);
            auto & acc1 = M.accessors.at(a1);

            REQUIRE( acc1.byteOffset == 0);


            for(uint32_t i=0;i<10;i++)
            {
                vec3 A = {i, i, i };
                acc1.setValue(i, A);
            }

            std::vector<vec3> tmp;

            tmp.resize( acc1.count );
            acc1.memcpy_all(tmp.data());

            for(uint32_t i=0;i<10;i++)
            {
                auto v = acc1.getValue<vec3>(i);

                vec3 A = {i, i, i };

                REQUIRE( v[0] == A[0] );
                REQUIRE( v[1] == A[1] );
                REQUIRE( v[2] == A[2] );

                REQUIRE( tmp[i][0] == A[0] );
                REQUIRE( tmp[i][1] == A[1] );
                REQUIRE( tmp[i][2] == A[2] );


            }

        }

        {
            auto & B = M2.newBuffer();
            auto bv_i = B.createNewBufferView( 10*sizeof(vec3), uGLTF::BufferViewTarget::ARRAY_BUFFER, sizeof(vec3), 1 );

            auto & Bv = M2.bufferViews[ bv_i ];

            auto a1 = Bv.createNewAccessor(0            , 10, uGLTF::AccessorType::VEC3, uGLTF::ComponentType::UNSIGNED_INT);

            auto & acc1 = M2.accessors.at(a1);

            REQUIRE( a1 == 0);
            REQUIRE( acc1.byteOffset == 0);
            acc1.copyDataFrom( M.accessors[0] );


            for(uint32_t i=0;i<10;i++)
            {
                auto v = acc1.getValue<vec3>(i);

                vec3 A = {i, i, i };
                REQUIRE( v[0] == A[0] );
                REQUIRE( v[1] == A[1] );
                REQUIRE( v[2] == A[2] );
            }

        }

    }

}
