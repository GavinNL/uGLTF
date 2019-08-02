#include "catch.hpp"
#include <gltfpp/gltfpp.h>
#include <iostream>
#include <fstream>


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
    #define TEST_MODEL "/home/gavin/Projects/gltfpp/share/gltfpp/CesiumMan.glb"

    gltfpp::GLTFModel M;
    std::ifstream in(TEST_MODEL);
    M.load(in);
    in.close();


    GIVEN("A GLTFModel and an input stream")
    {

        THEN("We can access the meshes")
        {
            REQUIRE( M.meshes.size() == 1);

            REQUIRE( M.meshes[0].primitives.size() == 1);

            REQUIRE( M.meshes[0].primitives[0].has(gltfpp::PrimitiveAttribute::POSITION) );
            REQUIRE( M.meshes[0].primitives[0].has(gltfpp::PrimitiveAttribute::NORMAL) );
            REQUIRE( M.meshes[0].primitives[0].has(gltfpp::PrimitiveAttribute::WEIGHTS_0) );
            REQUIRE( M.meshes[0].primitives[0].has(gltfpp::PrimitiveAttribute::TEXCOORD_0) );
            REQUIRE( M.meshes[0].primitives[0].has(gltfpp::PrimitiveAttribute::JOINTS_0) );

            REQUIRE( !M.meshes[0].primitives[0].has(gltfpp::PrimitiveAttribute::COLOR_0) );
            REQUIRE( !M.meshes[0].primitives[0].has(gltfpp::PrimitiveAttribute::TEXCOORD_1) );

            REQUIRE( M.meshes[0].primitives[0].hasIndices() );

            THEN("We can access the accesssors of the mesh primitives")
            {
                auto & p =  M.meshes[0].primitives[0].getAccessor(gltfpp::PrimitiveAttribute::POSITION);
                auto & n =  M.meshes[0].primitives[0].getAccessor(gltfpp::PrimitiveAttribute::NORMAL);
                auto & w =  M.meshes[0].primitives[0].getAccessor(gltfpp::PrimitiveAttribute::WEIGHTS_0);
                auto & t =  M.meshes[0].primitives[0].getAccessor(gltfpp::PrimitiveAttribute::TEXCOORD_0);
                auto & j =  M.meshes[0].primitives[0].getAccessor(gltfpp::PrimitiveAttribute::JOINTS_0);

                REQUIRE( p.count == n.count );
                REQUIRE( p.count == w.count );
                REQUIRE( p.count == t.count );
                REQUIRE( p.count == j.count );

                THEN("We can alias the buffer data")
                {

                }
            }

            THEN("we can access the Scenes")
            {
                REQUIRE( M.scenes.size() == 1 );

                THEN("We can get the root node of the scene")
                {
                    for(auto & scene : M.scenes )
                    {
                        std::cout << "Scene: " << scene.name << std::endl;
                        uint32_t si=0;
                        for(auto & node : scene.nodes)
                        {
                            std::cout << "  Root Node: " << node << std::endl;
                            auto * N = scene.getRootNode(si++);

                            std::cout << "      Children: " << N->children.size() << std::endl;
                            int i=0;
                            N->depthFirstTraverse( [&](gltfpp::Node & N)
                                         {
                                            std::cout << N.name  << "  mesh: " << N.mesh  << "  skin: " << N.skin << std::endl;
                                         }
                                        );


                            N->getChild()
                            std::cout << "Total depth: " << i << std::endl;

                        }
                    }

                }
            }

        }





    }
}
