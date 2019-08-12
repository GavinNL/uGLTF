#include "catch.hpp"
#include <ugltf/ugltf.h>
#include <iostream>
#include <fstream>
#include <set>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_NO_INCLUDE_JSON
#define TINYGLTF_NO_STB_IMAGE_WRITE
// #define TINYGLTF_NOEXCEPTION // optional. disable exception handling.
#include "tiny_gltf.h"



SCENARIO( "Loading " )
{
    std::vector< std::string > models = {"BoomBox.glb", "BrainStem.glb", "CesiumMan.glb"};

    for(auto & modelName : models )
    {


    //GIVEN("A GLTFModel and an input stream")
    {
        uGLTF::GLTFModel UModel;
        std::ifstream in(modelName);
        UModel.load(in);
        in.close();

        tinygltf::Model TModel;
        tinygltf::TinyGLTF loader;
        std::string err;
        std::string warn;

        bool ret = loader.LoadBinaryFromFile(&TModel, &err, &warn, modelName.c_str() ); // for binary glTF(.glb)

        REQUIRE(ret==true);


        auto _bufferCompare = [&]( uGLTF::Buffer & U, tinygltf::Buffer & T)
        {
            REQUIRE( T.data.size() == U.m_data.size() );
            REQUIRE( true == std::equal(T.data.begin(), T.data.end(), U.m_data.begin()) );

            return true;
        };

        auto _bufferViewCompare = [&]( uGLTF::BufferView & U, tinygltf::BufferView & T)
        {
            REQUIRE(U.buffer == T.buffer);
            if( U.target != uGLTF::BufferViewTarget::UNKNOWN)
                REQUIRE(static_cast<int>(U.target) == T.target);
            REQUIRE(static_cast<size_t>(U.byteLength) == T.byteLength);
            REQUIRE(static_cast<size_t>(U.byteOffset) == T.byteOffset);
            REQUIRE(static_cast<size_t>(U.byteStride) == T.byteStride);

            auto & Uv = U.getBuffer();
            auto & Tv = TModel.buffers[T.buffer];

            REQUIRE( true == _bufferCompare(Uv,Tv) );
            return true;
        };

        auto _accessorsCompare = [&]( uGLTF::Accessor & U, tinygltf::Accessor & T)
        {
            std::map<int, int> TUmap;

            TUmap[TINYGLTF_TYPE_VEC2   ] = static_cast<int>(uGLTF::AccessorType::VEC2) ;//(2)
            TUmap[TINYGLTF_TYPE_VEC3   ] = static_cast<int>(uGLTF::AccessorType::VEC3) ;//(3)
            TUmap[TINYGLTF_TYPE_VEC4   ] = static_cast<int>(uGLTF::AccessorType::VEC4) ;//(4)
            TUmap[TINYGLTF_TYPE_MAT2   ] = static_cast<int>(uGLTF::AccessorType::MAT2) ;//(32 + 2)
            TUmap[TINYGLTF_TYPE_MAT3   ] = static_cast<int>(uGLTF::AccessorType::MAT3) ;//(32 + 3)
            TUmap[TINYGLTF_TYPE_MAT4   ] = static_cast<int>(uGLTF::AccessorType::MAT4) ;//(32 + 4)
            TUmap[TINYGLTF_TYPE_SCALAR ] = static_cast<int>(uGLTF::AccessorType::SCALAR) ;//(64 + 1)
            //TUmap[TINYGLTF_TYPE_VECTOR ] = static_cast<int>(uGLTF::AccessorType::) ;//(64 + 4)
            //TUmap[TINYGLTF_TYPE_MATRIX ] = static_cast<int>(uGLTF::AccessorType::VEC2) ;//(64 + 16)

            REQUIRE( static_cast<int>(U.type)       == TUmap[T.type] );
            REQUIRE( static_cast<int>(U.bufferView) == T.bufferView);

            REQUIRE( U.count == T.count );
            REQUIRE( U.normalized == T.normalized);
            REQUIRE( static_cast<int>(U.componentType) == T.componentType);

            REQUIRE( U.byteOffset == T.byteOffset );

            REQUIRE( true == _bufferViewCompare( U.getBufferView(), TModel.bufferViews[T.bufferView] ) );



            { // CHECK THE SPAN
                auto byteSpan = U.getSpan<uint8_t>();

                auto & TBv = TModel.bufferViews[ T.bufferView];
                auto & TB  = TModel.buffers[ TBv.buffer ];

                auto TByteStride = T.ByteStride( TBv);

                REQUIRE( byteSpan.size() == T.count );
                REQUIRE( byteSpan._stride == TByteStride );
                REQUIRE( byteSpan[0] == TB.data[ TBv.byteOffset + U.byteOffset]);

                REQUIRE( byteSpan[1] == TB.data[ TBv.byteOffset + U.byteOffset + TByteStride]);

                if( U.type == uGLTF::AccessorType::VEC3 && U.componentType == uGLTF::ComponentType::FLOAT)
                {
                    auto byteSpanVec3f = U.getSpan< std::array<uint32_t, 3> >();

                    REQUIRE( byteSpanVec3f.size() == T.count);

                    uint32_t i=0;
                    for(auto & v : byteSpanVec3f)
                    {
                        auto & vT = *reinterpret_cast<std::array<uint32_t,3>* >(&TB.data[ TBv.byteOffset + U.byteOffset + TByteStride*i]);

                        REQUIRE( vT[0] == v[0] );
                        REQUIRE( vT[1] == v[1] );
                        REQUIRE( vT[2] == v[2] );
                        i++;
                    }
                }
                if( U.type == uGLTF::AccessorType::VEC4 && U.componentType == uGLTF::ComponentType::FLOAT)
                {
                    auto byteSpanVec3f = U.getSpan< std::array<uint32_t, 4> >();

                    REQUIRE( byteSpanVec3f.size() == T.count);

                    uint32_t i=0;
                    for(auto & v : byteSpanVec3f)
                    {
                        auto & vT = *reinterpret_cast<std::array<uint32_t,4>* >(&TB.data[ TBv.byteOffset + U.byteOffset +  TByteStride*i]);

                        REQUIRE( vT[0] == v[0] );
                        REQUIRE( vT[1] == v[1] );
                        REQUIRE( vT[2] == v[2] );
                        REQUIRE( vT[3] == v[3] );
                        i++;
                    }
                }
                if( U.type == uGLTF::AccessorType::VEC4 && U.componentType == uGLTF::ComponentType::UNSIGNED_BYTE)
                {
                    auto byteSpanVec3f = U.getSpan< std::array<uint8_t, 4> >();

                    REQUIRE( byteSpanVec3f.size() == T.count);

                    uint32_t i=0;
                    for(auto & v : byteSpanVec3f)
                    {
                        auto & vT = *reinterpret_cast<std::array<uint8_t,4>* >(&TB.data[ TBv.byteOffset + U.byteOffset +  TByteStride*i]);

                        REQUIRE( vT[0] == v[0] );
                        REQUIRE( vT[1] == v[1] );
                        REQUIRE( vT[2] == v[2] );
                        REQUIRE( vT[3] == v[3] );
                        i++;
                    }
                }
                if( U.type == uGLTF::AccessorType::VEC4 && U.componentType == uGLTF::ComponentType::UNSIGNED_SHORT)
                {
                    auto byteSpanVec3f = U.getSpan< std::array<uint16_t, 4> >();

                    REQUIRE( byteSpanVec3f.size() == T.count);

                    uint32_t i=0;
                    for(auto & v : byteSpanVec3f)
                    {
                        auto & vT = *reinterpret_cast<std::array<uint16_t,4>* >(&TB.data[ TBv.byteOffset + U.byteOffset +  TByteStride*i]);

                        REQUIRE( vT[0] == v[0] );
                        REQUIRE( vT[1] == v[1] );
                        REQUIRE( vT[2] == v[2] );
                        REQUIRE( vT[3] == v[3] );
                        i++;
                    }
                }
                if( U.type == uGLTF::AccessorType::SCALAR && U.componentType == uGLTF::ComponentType::UNSIGNED_SHORT)
                {
                    auto byteSpanVec3f = U.getSpan< uint16_t >();

                    REQUIRE( byteSpanVec3f.size() == T.count);

                    uint32_t i=0;
                    for(auto & v : byteSpanVec3f)
                    {
                        auto & vT = *reinterpret_cast<uint16_t* >(&TB.data[ TBv.byteOffset + U.byteOffset +  TByteStride*i]);

                        REQUIRE( vT == v );
                        i++;
                    }
                }
                if( U.type == uGLTF::AccessorType::SCALAR && U.componentType == uGLTF::ComponentType::UNSIGNED_INT)
                {
                    auto byteSpanVec3f = U.getSpan< uint32_t >();

                    REQUIRE( byteSpanVec3f.size() == T.count);

                    uint32_t i=0;
                    for(auto & v : byteSpanVec3f)
                    {
                        auto & vT = *reinterpret_cast<uint32_t* >(&TB.data[ TBv.byteOffset + U.byteOffset +  TByteStride*i]);

                        REQUIRE( vT == v );
                        i++;
                    }
                }
            }



            return true;
        };

        auto _primitiveCompare = [&]( uGLTF::Primitive & U, tinygltf::Primitive & T)
        {
            REQUIRE( static_cast<int>(U.mode) == T.mode );

            if( T.indices != -1 )
            {
                REQUIRE( U.hasIndices() == true);
                REQUIRE( U.indices == T.indices );

                auto & Uia = U.getIndexAccessor();
                auto & Tia = TModel.accessors[T.indices];

                REQUIRE( _accessorsCompare(Uia, Tia) == true);
            }

            if( T.material != -1)
            {
                REQUIRE( U.material == T.material );
            }

            for(auto & Tattr : T.attributes )
            {
                if( Tattr.first == "POSITION" )
                {
                    REQUIRE( U.has( uGLTF::PrimitiveAttribute::POSITION   ) == true) ;
                    _accessorsCompare( U.getAccessor(uGLTF::PrimitiveAttribute::POSITION),
                                       TModel.accessors[ Tattr.second ] );
                }
                if( Tattr.first == "NORMAL" )
                {
                    REQUIRE( U.has( uGLTF::PrimitiveAttribute::NORMAL	 ) == true) ;
                    _accessorsCompare( U.getAccessor(uGLTF::PrimitiveAttribute::NORMAL),
                                       TModel.accessors[ Tattr.second ] );
                }
                if( Tattr.first == "TANGENT" )
                {
                    REQUIRE( U.has( uGLTF::PrimitiveAttribute::TANGENT	 ) == true) ;
                    _accessorsCompare( U.getAccessor(uGLTF::PrimitiveAttribute::TANGENT),
                                       TModel.accessors[ Tattr.second ] );
                }
                if( Tattr.first == "TEXCOORD_0" )
                {
                    REQUIRE( U.has( uGLTF::PrimitiveAttribute::TEXCOORD_0 ) == true) ;
                    _accessorsCompare( U.getAccessor(uGLTF::PrimitiveAttribute::TEXCOORD_0),
                                       TModel.accessors[ Tattr.second ] );
                }
                if( Tattr.first == "TEXCOORD_1" )
                {
                    REQUIRE( U.has( uGLTF::PrimitiveAttribute::TEXCOORD_1 ) == true) ;
                    _accessorsCompare( U.getAccessor(uGLTF::PrimitiveAttribute::TEXCOORD_1),
                                       TModel.accessors[ Tattr.second ] );
                }
                if( Tattr.first == "COLOR_0" )
                {
                    REQUIRE( U.has( uGLTF::PrimitiveAttribute::COLOR_0	 ) == true) ;
                    _accessorsCompare( U.getAccessor(uGLTF::PrimitiveAttribute::COLOR_0),
                                       TModel.accessors[ Tattr.second ] );
                }
                if( Tattr.first == "JOINTS_0" )
                {
                    REQUIRE( U.has( uGLTF::PrimitiveAttribute::JOINTS_0   ) == true) ;
                    _accessorsCompare( U.getAccessor(uGLTF::PrimitiveAttribute::JOINTS_0),
                                       TModel.accessors[ Tattr.second ] );
                }
                if( Tattr.first == "WEIGHTS_0" )
                {
                    REQUIRE( U.has( uGLTF::PrimitiveAttribute::WEIGHTS_0  ) == true) ;
                    _accessorsCompare( U.getAccessor(uGLTF::PrimitiveAttribute::WEIGHTS_0),
                                       TModel.accessors[ Tattr.second ] );
                }
            }


            return true;
        };

        auto _meshCompare = [&]( uGLTF::Mesh & U, tinygltf::Mesh & T)
        {
            REQUIRE( U.primitives.size() == T.primitives.size() );
            REQUIRE( U.weights.size() == T.weights.size() );

            auto Tpi = T.primitives.begin();
            for(auto & UP : U.primitives)
            {
                auto & TP = *Tpi;

                REQUIRE( true == _primitiveCompare(UP, TP) );

                Tpi++;
            }

            return true;
        };


        // Read the buffers
        {
            REQUIRE( TModel.buffers.size() == UModel.buffers.size() );
            auto  tIt = TModel.buffers.begin();
            for(auto & UBuf : UModel.buffers)
            {
                auto & TBuf = *tIt;

                REQUIRE( true == _bufferCompare(UBuf, TBuf) );
                tIt++;
            }
        }

        // Read the bufferViews
        {
            REQUIRE( TModel.bufferViews.size() == UModel.bufferViews.size() );
            auto  tIt = TModel.bufferViews.begin();
            for(auto & UBuf : UModel.bufferViews)
            {
                auto & TBuf = *tIt;

                REQUIRE( true == _bufferViewCompare(UBuf, TBuf) );

                tIt++;
            }
        }

        // Read the Accesors
        {
            REQUIRE( TModel.accessors.size() == UModel.accessors.size() );

            auto  tIt = TModel.accessors.begin();
            for(auto & UBuf : UModel.accessors)
            {
                auto & TBuf = *tIt;

                REQUIRE( true == _accessorsCompare(UBuf, TBuf) );

                tIt++;
            }
        }

        // Read the Mesh
        {
            REQUIRE( TModel.meshes.size() == UModel.meshes.size() );

            auto  tIt = TModel.meshes.begin();
            for(auto & UBuf : UModel.meshes)
            {
                auto & TBuf = *tIt;

                REQUIRE( true == _meshCompare(UBuf, TBuf) );

                tIt++;
            }
        }
#if 0
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
                    REQUIRE( C.normalTexture.index >= 0 );
                }
            }
        }

        //("We can extract image data")
        {
            for(auto & I : M.images)
            {
                REQUIRE( I.bufferView >= 0 );

                //("We can get the bufferView")
                {
                    auto & Bv = I.getBufferView();

                    REQUIRE( Bv.buffer != -1 );

                    //("We can get the buffer")
                    {
                        auto & B = Bv.getBuffer();

                        REQUIRE(B.byteLength != 0);

                        //("We can get the span of the image")

                            auto img = I.getSpan();

                            REQUIRE( img.size() > 0 );
                            REQUIRE( img.stride() == 1);

                            auto const & cI = I;

                            auto img_c = cI.getSpan();

                            REQUIRE( img_c.size() > 0 );
                            REQUIRE( img_c.stride() == 1);

                    }


                }



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

                                //("The stride must be larger than the accessorSize...otherwise data will be aliased")
                                {
                                    REQUIRE( span.stride() >= p.accessorSize() );
                                }
                                vertexCounts.insert( span.size() );

                                size_t count=0;
                                for(auto & v : span)
                                {
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
                }


            }
        }
#endif
       }

    }
}
