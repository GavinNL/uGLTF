#include "catch.hpp"
#include <ugltf/ugltf.h>
#include <iostream>
#include <fstream>
#include <set>

#include <regex>


SCENARIO("Aspan")
{
    uGLTF::GLTFModel M;

    std::vector<uint8_t> data;
    data.resize(24);

    struct Vec3
    {
        uint32_t x,y,z;
    };


    uGLTF::aspan<Vec3>  span( data.data(),2,sizeof(Vec3));

    REQUIRE( span.size() == 2);
    REQUIRE( static_cast<void*>(&span[0]) == static_cast<void*>(&data[0]) );

    REQUIRE( static_cast<void*>(&span.front()) == static_cast<void*>(&data.front()) );
    REQUIRE( static_cast<void*>(&span.back())  == static_cast<void*>(&data[12]) );

    REQUIRE( static_cast<void*>(&span[0].x) == static_cast<void*>(&data[0]) );
    REQUIRE( static_cast<void*>(&span[0].y) == static_cast<void*>(&data[4]) );
    REQUIRE( static_cast<void*>(&span[0].z) == static_cast<void*>(&data[8]) );

    REQUIRE( static_cast<void*>(&span[1].x) == static_cast<void*>(&data[12]) );
    REQUIRE( static_cast<void*>(&span[1].y) == static_cast<void*>(&data[16]) );
    REQUIRE( static_cast<void*>(&span[1].z) == static_cast<void*>(&data[20]) );

    data[16] = 0xAA;
    data[17] = 0xBB;
    data[18] = 0xCC;
    data[19] = 0xDD;

    REQUIRE( span[1].y == 0xDDCCBBAA);

    THEN("We can loop using begin()/end() ")
    {
        uint32_t count=0;
        for(auto & b : span)
        {
            assert(&b);
            count++;
        }
        REQUIRE(count==2);
    }

    WHEN("We create a span of const vec3")
    {
        uGLTF::aspan<const Vec3>  spanOfConst( data.data(),2,sizeof(Vec3));

        THEN("We can loop through it like it was a vector")
        {
            auto it = spanOfConst.begin();
            while( it != spanOfConst.end() )
            {
                it++;
            }
        }
    }

    WHEN("We can make the non-const span const")
    {
        const auto & constSpan = span;

        THEN("We can loop through it as if it was a regular container")
        {
            for(auto & v : constSpan)
            {
                assert(&v);
            }
        }
    }

}


SCENARIO( "Read Header" )
{
    uGLTF::GLTFModel M;
    std::ifstream in("CesiumMan.glb");
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
                 //   std::cout << J["asset"].dump(4) << std::endl;

//                    std::cout << J["buffers"].dump(4) << std::endl;
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
    uGLTF::GLTFModel M;
    std::ifstream in("CesiumMan.glb");
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
                    M._readBuffers(in, J["buffers"]);

                    // auto cB = M._readChunk(in);
                    //
                    // REQUIRE( cB.chunkType == 0x004E4942);
                    // REQUIRE( cB.chunkLength == 491144-27708-3*sizeof(uint32_t)-4*sizeof(uint32_t));
                    // REQUIRE( cB.chunkData.size() == cB.chunkLength);
                    //
                    //
                    // THEN("We can extract the buffers")
                    // {
                    //     auto buffers = M._extractBuffers(cB, J["buffers"]);
                    //
                    //     REQUIRE(buffers.size() == 1);
                    //
                    // }
                }
            }
        }

        in.close();
    }
}


SCENARIO( "Loading GLB files" )
{
    std::vector< std::string > models = {"BoomBox.glb", "BrainStem.glb", "CesiumMan.glb"};

    for(auto & modelName : models )
    {




    //GIVEN("A GLTFModel and an input stream")
    {
        uGLTF::GLTFModel M;
        std::ifstream in(modelName);
        M.load(in);
        in.close();

        //("We can read the buffers")
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

        //("We can read the Cameras")
        {
            for(auto & C : M.cameras)
            {
                REQUIRE( C.type == uGLTF::CameraType::PERSPECTIVE);

                float M[16];
                C.writeMatrix(M);
            }
        }

        //("We can read the Materials")
        {
            for(auto & C : M.materials)
            {
                if( C.hasPBR() )
                {
                    REQUIRE( std::isfinite(C.pbrMetallicRoughness.metallicFactor) );
                }
                if( C.hasNormalTexture() )
                {
                    REQUIRE( C.normalTexture.index != std::numeric_limits<uint32_t>::max() );
                }
            }
        }

        //("We can extract image data")
        {
            for(auto & I : M.images)
            {
                auto img_c = I.getSpan();
                REQUIRE( img_c.size() > 0 );
                REQUIRE( img_c.stride() == 1);
            }

          //  REQUIRE(img.size()==40);

        }

        //("We can access the meshes")
        {
            //("We can access the accesssors of the mesh primitives")
            {
                uGLTF::PrimitiveAttribute attrs[] = {
                    uGLTF::PrimitiveAttribute::POSITION  ,
                    uGLTF::PrimitiveAttribute::NORMAL	  ,
                    uGLTF::PrimitiveAttribute::TANGENT	  ,
                    uGLTF::PrimitiveAttribute::TEXCOORD_0,
                    uGLTF::PrimitiveAttribute::TEXCOORD_1,
                    uGLTF::PrimitiveAttribute::COLOR_0	  ,
                    uGLTF::PrimitiveAttribute::JOINTS_0  ,
                    uGLTF::PrimitiveAttribute::WEIGHTS_0
                };
                for(auto & mesh : M.meshes)
                {
                    for(auto & primitive : mesh.primitives)
                    {
                        std::set<size_t> vertexCounts;

                        if( primitive.material != std::numeric_limits<uint32_t>::max() )
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

                                //("The stride must be larger than the accessorSize...otherwise data will be aliased")
                                {
                                    REQUIRE( span.stride() >= p.accessorSize() );
                                }
                                vertexCounts.insert( span.size() );

                                size_t count=0;
                                for(auto & v : span)
                                {
                                    assert(&v);
                                    count++;
                                }
                                REQUIRE( count == span.size() );
                                REQUIRE( count == p.count );
                            }
                        }
                        //("All attriubutes must have the same count")
                        {
                            REQUIRE( vertexCounts.size() == 1 );
                        }
                    }

                }
            }
        }


        // Animation
        {
            for(auto & A : M.animations )
            {
                for(auto & Ch : A.channels )
                {
                    REQUIRE( Ch.target.node >= 0 );
                    REQUIRE( Ch.sampler <= A.samplers.size() );
                }
                for(auto & Sm : A.samplers )
                {
                    REQUIRE( Sm.input  < M.accessors.size() );
                    REQUIRE( Sm.output < M.accessors.size() );

                    auto & timeAcc = Sm.getInputAccessor();
                    auto time      = Sm.getInputSpan();

                    REQUIRE( timeAcc.type == uGLTF::AccessorType::SCALAR );
                    REQUIRE( timeAcc.componentType == uGLTF::ComponentType::FLOAT);
                    REQUIRE( timeAcc.count == time.size() );


                    {
                        auto const & Smc = Sm;

                        auto & timeAccc = Smc.getInputAccessor();
//                        auto timec      = Smc.getInputSpan();




                        REQUIRE( timeAccc.type == uGLTF::AccessorType::SCALAR );
                        REQUIRE( timeAccc.componentType == uGLTF::ComponentType::FLOAT);
                        REQUIRE( timeAccc.count == time.size() );
                    }

                }
            }
        }

        // Skins
        {
            for(auto & A : M.skins )
            {

                if( A.hasInverseBindMatrices() )
                {
                    auto acc = A.getInverseBindMatricesAccessor();

                    auto span = A.getInverseBindMatricesSpan< std::array<float,16> >();

                    REQUIRE( span.size() == A.joints.size() );

                    REQUIRE( acc.componentType   == uGLTF::ComponentType::FLOAT);
                    REQUIRE( acc.componentSize() == sizeof(float));
                    REQUIRE( acc.accessorSize()  == 16*sizeof(float));

                    { //
                        auto const &Ac = A;
                        auto acc_c  = Ac.getInverseBindMatricesAccessor() ;
                        auto span_c = Ac.getInverseBindMatricesSpan< const std::array<float,16> >();

                        REQUIRE(span_c.size() == A.joints.size() );

                        REQUIRE( acc_c.componentType   == uGLTF::ComponentType::FLOAT);
                        REQUIRE( acc_c.componentSize() == sizeof(float));
                        REQUIRE( acc_c.accessorSize()  == 16*sizeof(float));
                    }
                }
            }
        }

       }

    }
}

SCENARIO("Creating new BufferViews")
{
    GIVEN("Given a Model and a single buffer")
    {
        uGLTF::GLTFModel M;

        THEN("We can create a new buffer")
        {
            auto & buff = M.newBuffer();
            REQUIRE( M.buffers.size() == 1);


            THEN("We can create a new BufferView")
            {
                auto viewIndex1 = buff.createNewBufferView(1020);

                auto & view = M.bufferViews[viewIndex1];

                REQUIRE( view.buffer == 0);
                REQUIRE( view.target == uGLTF::BufferViewTarget::UNKNOWN);
                REQUIRE( view.byteLength == 1020);
                REQUIRE( view.byteOffset == 0);
                REQUIRE( view.byteStride == 0);

                WHEN("We create another buffer view")
                {
                    auto viewIndex2 = buff.createNewBufferView(1024, 8);

                    auto & view2 = M.bufferViews[viewIndex2];

                    REQUIRE( view2.buffer == 0);
                    REQUIRE( view2.target == uGLTF::BufferViewTarget::UNKNOWN);
                    REQUIRE( view2.byteLength == 1024);
                    REQUIRE( view2.byteOffset == 1024); // because we set the alignment
                    REQUIRE( view2.byteStride == 0);
                }

            }
        }
    }
}

SCENARIO("Creating new Accessors from bufferViews")
{
    GIVEN("Given a Model and a single buffer")
    {
        uGLTF::GLTFModel M;

        THEN("We can create a new buffer")
        {
            auto & buff = M.newBuffer();
            REQUIRE( M.buffers.size() == 1);


            THEN("We can create a new BufferView")
            {
                auto viewIndex1 = buff.createNewBufferView(1000);

                auto & view = M.bufferViews[viewIndex1];

                REQUIRE( view.buffer == 0);
                REQUIRE( view.target == uGLTF::BufferViewTarget::UNKNOWN);
                REQUIRE( view.byteLength == 1000);
                REQUIRE( view.byteOffset == 0);
                REQUIRE( view.byteStride == 0);


                THEN("We can create new accessors")
                {
                    auto aIndex = view.createNewAccessor(0, 10, uGLTF::AccessorType::VEC2, uGLTF::ComponentType::FLOAT);

                    auto & a = M.accessors[aIndex];

                    REQUIRE( a.count          == 10);
                    REQUIRE( a.type           == uGLTF::AccessorType::VEC2);
                    REQUIRE( a.componentType  == uGLTF::ComponentType::FLOAT);
                    REQUIRE( a.bufferView     == 0);

                }
            }
        }
    }
}


SCENARIO("Copying data from one accessor to another")
{
    GIVEN("Given a Model and a single buffer")
    {
        uGLTF::GLTFModel M;

        THEN("We can create a new buffer")
        {
            auto & buff = M.newBuffer();
            REQUIRE( M.buffers.size() == 1);


            THEN("We can create a new BufferView")
            {
                auto viewIndex1 = buff.createNewBufferView(1000);

                auto & view = M.bufferViews[viewIndex1];

                REQUIRE( view.buffer == 0);
                REQUIRE( view.target == uGLTF::BufferViewTarget::UNKNOWN);
                REQUIRE( view.byteLength == 1000);
                REQUIRE( view.byteOffset == 0);
                REQUIRE( view.byteStride == 0);


                THEN("We can create new accessors")
                {
                    auto aIndex = view.createNewAccessor(0, 2, uGLTF::AccessorType::VEC2, uGLTF::ComponentType::UNSIGNED_INT);

                    auto & a = M.accessors[aIndex];

                    REQUIRE( a.count          == 2);
                    REQUIRE( a.type           == uGLTF::AccessorType::VEC2);
                    REQUIRE( a.componentType  == uGLTF::ComponentType::UNSIGNED_INT);
                    REQUIRE( a.bufferView     == 0);

                    auto S = a.getSpan< std::array<uint32_t,2> >();
                    S[0][0] = 1;
                    S[0][1] = 2;
                    S[1][0] = 3;
                    S[1][1] = 4;


                    THEN("We create a new accessor with different strides")
                    {
                        auto bIndex = view.createNewAccessor(16, 2, uGLTF::AccessorType::VEC2, uGLTF::ComponentType::UNSIGNED_INT);
                        auto & b = M.accessors[bIndex];

                        b.copyDataFrom( M.accessors[aIndex] );

                        THEN("The values are the same")
                        {
                            auto T = b.getSpan< std::array<uint32_t,2> >();
                            REQUIRE( T[0][0] == 1 );
                            REQUIRE( T[0][1] == 2 );
                            REQUIRE( T[1][0] == 3 );
                            REQUIRE( T[1][1] == 4 );

                            THEN("We can calculatte the min and max")
                            {
                                b.calculateMinMax();

                                REQUIRE( b.min.size() == 2);
                                REQUIRE( b.max.size() == 2);

                                REQUIRE( b.min[0] == Approx(1) );
                                REQUIRE( b.min[1] == Approx(2) );
                                REQUIRE( b.max[0] == Approx(3) );
                                REQUIRE( b.max[1] == Approx(4) );
                            }
                        }
                    }
                }
            }
        }
    }
}


SCENARIO("Loading BoxAnimated.gltf")
{
    uGLTF::GLTFModel M;
    std::ifstream in("BoxAnimated.gltf");
    M.load(in);
}


SCENARIO("Loading BoxTextured.gltf")
{
    GIVEN("A GLTF model")
    {
        uGLTF::GLTFModel M;

        WHEN("We load a GLTF embedded file")
        {
            std::ifstream in("BoxTextured.gltf");
            REQUIRE( M.load(in) );

            THEN("The embedded buffers are loaded into the m_data variable")
            {
                REQUIRE( M.buffers.size() == 1);
                REQUIRE( M.buffers[0].byteLength == 840);
                REQUIRE( M.buffers[0].m_data.size() == 840);
                REQUIRE( M.buffers[0].uri == "");
            }


            THEN("The images are loaded into the m_imageData variable")
            {
                REQUIRE( M.images.size() == 1);
                REQUIRE( M.images[0].bufferView == std::numeric_limits<uint32_t>::max() );
                REQUIRE( M.images[0].m_imageData.size() == 23516);
            }
        }
    }
}
