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


                THEN("We can alias the Position buffer Data")
                {
                    using value_type = std::array<float,3>;

                    auto P = p.data<value_type>();

                    auto &  bufferView = M.bufferViews[ p.bufferView ];
                    auto &  buffer = M.buffers[ M.bufferViews[ p.bufferView ].buffer].m_data;

                    REQUIRE( P.size() == p.count );
                    REQUIRE( reinterpret_cast<unsigned char *>(&P[0])   == &buffer[ bufferView.byteOffset ] );
                    REQUIRE( reinterpret_cast<unsigned char *>(P.end()) == &buffer[ bufferView.byteOffset + p.count * bufferView.byteStride] );
                }
                THEN("We can alias the Normal buffer Data")
                {
                    using value_type = std::array<float,3>;

                    auto P = n.data<value_type>();

                    auto &  bufferView = M.bufferViews[ n.bufferView ];
                    auto &  buffer     = M.buffers[ M.bufferViews[ n.bufferView ].buffer].m_data;

                    REQUIRE( P.size() == n.count );
                    REQUIRE( reinterpret_cast<unsigned char *>(&P[0])   == &buffer[ bufferView.byteOffset ] );
                    REQUIRE( reinterpret_cast<unsigned char *>(P.end()) == &buffer[ bufferView.byteOffset + p.count * bufferView.byteStride] );
                }
                THEN("We can alias the Texcoord buffer Data")
                {
                    using value_type = std::array<float,2>;

                    auto P = t.data<value_type>();

                    auto &  bufferView = M.bufferViews[ t.bufferView ];
                    auto &  buffer     = M.buffers[ M.bufferViews[ n.bufferView ].buffer].m_data;

                    REQUIRE( P.size() == n.count );
                    REQUIRE( reinterpret_cast<unsigned char *>(&P[0])   == &buffer[ bufferView.byteOffset ] );
                    REQUIRE( reinterpret_cast<unsigned char *>(P.end()) == &buffer[ bufferView.byteOffset + p.count * bufferView.byteStride] );
                }
                THEN("We can alias the Weights buffer Data")
                {
                    using value_type = std::array<float,4>;

                    auto P = w.data<value_type>();

                    auto &  bufferView = M.bufferViews[ w.bufferView ];
                    auto &  buffer = M.buffers[ M.bufferViews[ w.bufferView ].buffer].m_data;

                    REQUIRE( P.size() == w.count );
                    REQUIRE( reinterpret_cast<unsigned char *>(&P[0])   == &buffer[ bufferView.byteOffset ] );
                    REQUIRE( reinterpret_cast<unsigned char *>(P.end()) == &buffer[ bufferView.byteOffset + p.count * bufferView.byteStride] );
                }
                THEN("We can alias the Weights buffer Data")
                {
                    using value_type = std::array<uint16_t,4>;

                    auto P = j.data<value_type>();

                    auto &  bufferView = M.bufferViews[ j.bufferView ];
                    auto &  buffer = M.buffers[ M.bufferViews[ j.bufferView ].buffer].m_data;

                    REQUIRE( P.size() == j.count );
                    REQUIRE( reinterpret_cast<unsigned char *>(&P[0])   == &buffer[ bufferView.byteOffset ] );
                    REQUIRE( reinterpret_cast<unsigned char *>(P.end()) == &buffer[ bufferView.byteOffset + p.count * bufferView.byteStride] );
                }
            }

            THEN("we can access the Scenes")
            {
                REQUIRE( M.scenes.size() == 1 );

                THEN("We can get the root node of the scene")
                {
                    for(auto & scene : M.scenes )
                    {
                       // std::cout << "Scene: " << scene.name << std::endl;
                        uint32_t si=0;
                        for(auto & node : scene.nodes)
                        {
                           // std::cout << "  Root Node: " << node << std::endl;
                            auto * N = scene.getRootNode(si++);

                           // std::cout << "      Children: " << N->children.size() << std::endl;
                            int i=0;
                            N->depthFirstTraverse( [&](gltfpp::Node & N)
                                         {
                                           // std::cout << N.name  << "  mesh: " << N.mesh  << "  skin: " << N.skin << std::endl;
                                         }
                                        );



                         //   std::cout << "Total depth: " << i << std::endl;

                        }
                    }

                }

                THEN("We can extract animations")
                {
                    auto & A = M.animations[0];

                    std::set<int32_t> nodes;

                    // A channel is basically a spline for
                    // a particular node's translation/scale/rotation property
                    for(auto & C : A.channels)
                    {
                        // The node that this channel will be modifiying;
                        auto & N = M.nodes[ C.target.node ];

                        // The sampler is the spline data.
                        auto & S = A.samplers[ C.sampler ];

                        // what path of the node is it affecting? translation/scale/rotation/weights?
                        switch( C.target.path )
                        {
                            case gltfpp::AnimationPath::TRANSLATE:
                            {
                                auto  T = S.getInputData();
                                auto  V = S.getOutputData< std::array<float,3> >();

                                // We want to find the transformation T(t), where t is
                                // some floating point value T <= t <= T.back()
                                //
                                // let i be the index such that T[i] < t < T[i+1]
                                //
                                // then:
                                //    mixValue = ( t - t[i] ) / ( t[i+1] - t[i] )
                                //
                                //    finalTranslation = lerp( V[i], V[i+1], mixValue);


                                REQUIRE(T.size() == V.size());
                            }
                            break;
                            case gltfpp::AnimationPath::SCALE:
                            {
                                auto  T = S.getInputData();
                                auto  V = S.getOutputData< std::array<float,3> >();

                                REQUIRE(T.size() == V.size());
                            }
                            break;

                            case gltfpp::AnimationPath::ROTATION:
                            {
                                auto  T = S.getInputData();
                                auto  V = S.getOutputData< std::array<float,4> >();

                                // We want to find the transformation T(t), where t is
                                // some floating point value T <= t <= T.back()
                                //
                                // let i be the index such that T[i] < t < T[i+1]
                                //
                                // then:
                                //    mixValue = ( t - t[i] ) / ( t[i+1] - t[i] )
                                //
                                //    finalRotation = slerp( V[i], V[i+1], mixValue);

                                REQUIRE(T.size() == V.size());
                            }
                            break;
                            case gltfpp::AnimationPath::WEIGHTS:   break;
                        }


                        nodes.insert( C.target.node );

                    }

                    auto nodes_size = nodes.size();

                    for(auto & n : nodes)
                    {
                        std::cout << "Node: " << n << std::endl;
                    }

                    auto & T = M.animations[0].samplers[0].getInputAccessor();


                    auto t = M.animations[0].samplers[0].getInputData();

                    t.size();

                   // auto v = M.animations[0].samplers[0].getOutputData();
                    std::cout << T.name << std::endl;
                }
            }

        }





    }
}
