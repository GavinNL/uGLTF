#include "catch.hpp"
#include <gltfpp/gltfpp.h>
#include <iostream>
#include <fstream>
#include <set>

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

SCENARIO("Buffer View")
{
    gltfpp::GLTFModel M;

    M.buffers.emplace_back( gltfpp::Buffer() );
    M.bufferViews.emplace_back( gltfpp::BufferView() );

    M.buffers[0].m_data.resize(1024);
    M.buffers[0].byteLength = 1024;

    M.bufferViews[0].buffer = 0;
    M.bufferViews[0].byteLength = 512;
    M.bufferViews[0].byteOffset = 0;
    M.bufferViews[0].byteStride = 3*sizeof(float);

    M.accessors.emplace_back( gltfpp::Accessor() );
    M.accessors[0].type = gltfpp::AccessorType::VEC3;
    M.accessors[0].bufferView = 0;
    M.accessors[0].componentType = gltfpp::ComponentType::FLOAT;


   // auto & B = M.bufferViews[0].getBuffer();
   // REQUIRE( &B == &M.buffers[0] );


}

SCENARIO( "Read Header" )
{
    #define TEST_MODEL "/home/gavin/Projects/gltfpp/share/gltfpp/CesiumMan.glb"


    gltfpp::GLTFModel M;
    std::ifstream in(TEST_MODEL);
    GIVEN("A GLTFModel and an input stream")
    {

        THEN("We can read the header")
        {
            auto h = M._readHeader(in);

            REQUIRE( h.version == 2);
            REQUIRE( h.magic   == 0x46546C67);
            REQUIRE( h.length  == 491144);

            THEN("We can read the JSON chunk")
            {
                auto cJ = M._readChunk(in);

                REQUIRE( cJ.chunkType == 0x4E4F534A);
                REQUIRE( cJ.chunkLength == 27708);
                REQUIRE( cJ.chunkData.size() == cJ.chunkLength);

                THEN("We can parse the JSON")
                {
                    cJ.chunkData.push_back(0);
                    auto J = M._parseJson( reinterpret_cast<char*>(cJ.chunkData.data()) );

                    REQUIRE(J.count("asset") == 1 );
                    std::cout << J["asset"].dump(4) << std::endl;

                    std::cout << J["buffers"].dump(4) << std::endl;
                }

                THEN("We can read the Buffer chunk")
                {
                    auto cB = M._readChunk(in);

                    REQUIRE( cB.chunkType == 0x004E4942);
                    REQUIRE( cB.chunkLength == 491144-27708-3*sizeof(uint32_t)-4*sizeof(uint32_t));
                    REQUIRE( cB.chunkData.size() == cB.chunkLength);
                }
            }
        }

        in.close();
    }
}

SCENARIO( "Extracting Buffers" )
{
    #define TEST_MODEL "/home/gavin/Projects/gltfpp/share/gltfpp/CesiumMan.glb"

    gltfpp::GLTFModel M;
    std::ifstream in(TEST_MODEL);
    GIVEN("A GLTFModel and an input stream")
    {

        THEN("We can read the header")
        {
            auto h = M._readHeader(in);

            REQUIRE( h.version == 2);
            REQUIRE( h.magic   == 0x46546C67);
            REQUIRE( h.length  == 491144);

            THEN("We can read and parse the JSON chunk")
            {
                auto cJ = M._readChunk(in);

                REQUIRE( cJ.chunkType == 0x4E4F534A);
                REQUIRE( cJ.chunkLength == 27708);
                REQUIRE( cJ.chunkData.size() == cJ.chunkLength);


                cJ.chunkData.push_back(0);
                auto J = M._parseJson( reinterpret_cast<char*>(cJ.chunkData.data()) );


                REQUIRE( J.count("asset") == 1 );


                THEN("We can read the Buffer chunk")
                {
                    auto cB = M._readChunk(in);

                    REQUIRE( cB.chunkType == 0x004E4942);
                    REQUIRE( cB.chunkLength == 491144-27708-3*sizeof(uint32_t)-4*sizeof(uint32_t));
                    REQUIRE( cB.chunkData.size() == cB.chunkLength);


                    THEN("We can extract the buffers")
                    {
                        auto buffers = M._extractBuffers(cB, J["buffers"]);

                        REQUIRE(buffers.size() == 1);

                    }
                }
            }
        }

        in.close();
    }
}


SCENARIO( "Loading " )
{
    #define TEST_MODEL "/home/gavin/Projects/gltfpp/share/gltfpp/BoomBox.glb"

    gltfpp::GLTFModel M;
    std::ifstream in(TEST_MODEL);
    M.load(in);
    in.close();


    GIVEN("A GLTFModel and an input stream")
    {

        THEN("We can read the buffers")
        {
            for(auto & B : M.buffers)
            {
                REQUIRE( B.byteLength != 0);
                REQUIRE( B.byteLength == B.m_data.size() );
            }
        }

        THEN("We can read the bufferViews")
        {
            for(auto & B : M.bufferViews)
            {
                REQUIRE( B.byteLength != 0);
            }
        }

        THEN("We can read the Cameras")
        {
            for(auto & C : M.cameras)
            {
                REQUIRE( C.type == gltfpp::CameraType::PERSPECTIVE);

                float M[16];
                C.writeMatrix(M);
            }
        }

        THEN("We can extract image data")
        {
            for(auto & I : M.images)
            {
                auto img = I.data();

                REQUIRE( I.bufferView >= 0 );

                THEN("We can get the bufferView")
                {
                    auto & Bv = I.getBufferView();

                    REQUIRE( Bv.buffer != -1 );

                    THEN("We can get the buffer")
                    {
                        auto & B = Bv.getBuffer();

                        REQUIRE(B.byteLength != 0);
                    }
                }
            }

          //  REQUIRE(img.size()==40);

        }

        THEN("We can access the meshes")
        {
            THEN("We can access the accesssors of the mesh primitives")
            {
                gltfpp::PrimitiveAttribute attrs[] = {
                    gltfpp::PrimitiveAttribute::POSITION  ,
                    gltfpp::PrimitiveAttribute::NORMAL	  ,
                    gltfpp::PrimitiveAttribute::TANGENT	  ,
                    gltfpp::PrimitiveAttribute::TEXCOORD_0,
                    gltfpp::PrimitiveAttribute::TEXCOORD_1,
                    gltfpp::PrimitiveAttribute::COLOR_0	  ,
                    gltfpp::PrimitiveAttribute::JOINTS_0  ,
                    gltfpp::PrimitiveAttribute::WEIGHTS_0
                };
                for(auto & mesh : M.meshes)
                {

                    std::set<size_t> vertexCounts;


                    for(auto & primitive : mesh.primitives)
                    {
                        if( primitive.material != -1)
                        {
                            REQUIRE_NOTHROW( M.materials.at(primitive.material)) ;
                        }

                        if( primitive.hasIndices() )
                        {
                            auto & acc = primitive.getIndexAccessor();

                            auto span = acc.getSpan<uint8_t>();

                            if( acc.accessorSize() == 2 )
                            {
                                REQUIRE( span.stride() >= 2 );
                            }
                            else if( acc.accessorSize() == 4 )
                            {
                                REQUIRE( span.stride() >= 4);
                            }
                            else {
                                REQUIRE(false);
                            }
                        }


                        for(auto & a : attrs)
                        {
                            if( primitive.has(a) )
                            {
                                auto & p  =  primitive.getAccessor(a);

                                auto span = primitive.getSpan<uint8_t>(a);

                                THEN("The stride must be larger than the accessorSize...otherwise data will be aliased")
                                {
                                    REQUIRE( span.stride() >= p.accessorSize() );
                                }
                                vertexCounts.insert( span.size() );
                            }
                        }
                        THEN("All attriubutes must have the same count")
                        {
                            REQUIRE( vertexCounts.size() == 1 );
                        }
                    }

                }
            }
        }



    }
}
