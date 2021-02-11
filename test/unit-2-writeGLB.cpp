#include "catch.hpp"
#include <ugltf/ugltf2.h>
#include <iostream>
#include <fstream>
#include <set>

#include <regex>


ugltf::json createBufferViewJson( std::vector<ugltf::BufferView> const & V, size_t firstBuffer=0)
{
    ugltf::json j = ugltf::json::array();

    size_t offset = 0;

    for(auto & x : V)
    {
        ugltf::json bv;
        bv["buffer"]     = 0 + firstBuffer;
        bv["byteLength"] = x.data.size();
        bv["byteOffset"] = offset;
        if( x.byteStride)
            bv["byteStride"] = x.byteStride;
        bv["target"]    = static_cast<uint32_t>(x.target);

        j.push_back(bv);
        offset += x.data.size();
    }
    return j;
}


SCENARIO("Test")
{
    ugltf::GLTFModel2 M;

    auto & bv1 = M.bufferViews.emplace_back();
    auto & bv2 = M.bufferViews.emplace_back();

    bv1.data.resize(1024);
    bv2.data.resize(512);

    auto J = createBufferViewJson(M.bufferViews);

    std::cout << J.dump(4) << std::endl;
    REQUIRE( J.size() == 2);

}


SCENARIO("test enums")
{
    ugltf::json J;

    J = "MAT4";
    REQUIRE( J.get<ugltf::AccessorType>() == ugltf::AccessorType::MAT4);
}

SCENARIO("TEST LOAD")
{
    ugltf::GLTFModel2 UModel;
    std::ifstream in("BoomBox.glb");
    REQUIRE( UModel.load(in) );
    in.close();

    REQUIRE( UModel.bufferViews[0].data.size() == 3285844 );
    REQUIRE( UModel.bufferViews[1].data.size() == 4471451 );
    REQUIRE( UModel.bufferViews[2].data.size() == 2845923 );
    REQUIRE( UModel.bufferViews[3].data.size() == 132833 );
    REQUIRE( UModel.bufferViews[4].data.size() == 28600 );
    REQUIRE( UModel.bufferViews[5].data.size() == 42900 );
    REQUIRE( UModel.bufferViews[6].data.size() == 57200 );
    REQUIRE( UModel.bufferViews[7].data.size() == 42900 );
    REQUIRE( UModel.bufferViews[8].data.size() == 36216 );

    REQUIRE(UModel.nodes.size() == 1);
    REQUIRE(UModel.nodes[0].rotation.has_value() == true);
    REQUIRE(UModel.nodes[0].scale.has_value() == false);
    REQUIRE(UModel.nodes[0].translation.has_value() == false);

    REQUIRE(UModel.meshes.size() == 1);
    REQUIRE(UModel.meshes[0].primitives.size() == 1);
    REQUIRE(UModel.meshes[0].primitives[0].attributes.POSITION.has_value() );
    REQUIRE(UModel.meshes[0].primitives[0].attributes.NORMAL.has_value() );
    REQUIRE(UModel.meshes[0].primitives[0].attributes.TANGENT.has_value() );
    REQUIRE(UModel.meshes[0].primitives[0].attributes.TEXCOORD_0.has_value() );

    REQUIRE(*UModel.meshes[0].primitives[0].attributes.POSITION   == 3 );
    REQUIRE(*UModel.meshes[0].primitives[0].attributes.NORMAL     == 1 );
    REQUIRE(*UModel.meshes[0].primitives[0].attributes.TANGENT    == 2 );
    REQUIRE(*UModel.meshes[0].primitives[0].attributes.TEXCOORD_0 == 0 );

    REQUIRE( UModel.accessors[0].bufferView == 4);
    REQUIRE( UModel.accessors[1].bufferView == 5);
    REQUIRE( UModel.accessors[2].bufferView == 6);
    REQUIRE( UModel.accessors[3].bufferView == 7);
    REQUIRE( UModel.accessors[4].bufferView == 8);

    REQUIRE( UModel.accessors[0].count == 3575);
    REQUIRE( UModel.accessors[1].count == 3575);
    REQUIRE( UModel.accessors[2].count == 3575);
    REQUIRE( UModel.accessors[3].count == 3575);
    REQUIRE( UModel.accessors[4].count == 18108);

    REQUIRE( static_cast<uint32_t>(UModel.accessors[0].componentType) == 5126);
    REQUIRE( static_cast<uint32_t>(UModel.accessors[1].componentType) == 5126);
    REQUIRE( static_cast<uint32_t>(UModel.accessors[2].componentType) == 5126);
    REQUIRE( static_cast<uint32_t>(UModel.accessors[3].componentType) == 5126);
    REQUIRE( static_cast<uint32_t>(UModel.accessors[4].componentType) == 5123);

    REQUIRE( UModel.accessors[0].type == ugltf::AccessorType::VEC2);
    REQUIRE( UModel.accessors[1].type == ugltf::AccessorType::VEC3);
    REQUIRE( UModel.accessors[2].type == ugltf::AccessorType::VEC4);
    REQUIRE( UModel.accessors[3].type == ugltf::AccessorType::VEC3);
    REQUIRE( UModel.accessors[4].type == ugltf::AccessorType::SCALAR);

    REQUIRE( UModel.materials[0].emissiveTexture->index == 3 );
    REQUIRE( UModel.materials[0].normalTexture->index == 2 );
    REQUIRE( UModel.materials[0].occlusionTexture->index == 1 );
    REQUIRE( UModel.materials[0].alphaMode == ugltf::MaterialAlphaMode::OPAQUE);
    REQUIRE( UModel.materials[0].pbrMetallicRoughness->baseColorTexture->index == 0 );
    REQUIRE( UModel.materials[0].pbrMetallicRoughness->metallicRoughnessTexture->index == 1 );

    REQUIRE( UModel.textures.size() == 4);
    REQUIRE( *UModel.textures[0].sourceTexture == 0);
    REQUIRE( *UModel.textures[1].sourceTexture == 1);
    REQUIRE( *UModel.textures[2].sourceTexture == 2);
    REQUIRE( *UModel.textures[3].sourceTexture == 3);


    REQUIRE( UModel.images.size() == 4);
    REQUIRE( UModel.images[0].bufferView == 0);
    REQUIRE( UModel.images[1].bufferView == 1);
    REQUIRE( UModel.images[2].bufferView == 2);
    REQUIRE( UModel.images[3].bufferView == 3);

    REQUIRE( UModel.images[0].mimeType == "image/png");
    REQUIRE( UModel.images[1].mimeType == "image/png");
    REQUIRE( UModel.images[2].mimeType == "image/png");
    REQUIRE( UModel.images[3].mimeType == "image/png");
    std::cout << UModel._json.dump(4) << std::endl;

    std::cout << " ----------------------------------" << std::endl;
    std::cout << ugltf::GLTFModel2::generateJson(UModel).dump(4) << std::endl;
}


SCENARIO("Test read-write-read")
{
    ugltf::GLTFModel2 UModel;
    std::ifstream in("BoomBox.glb");
    REQUIRE( UModel.load(in) );
    in.close();

    std::ostringstream out;

    UModel.writeGLB(out, UModel);


    std::istringstream in2(out.str());
    ugltf::GLTFModel2 UModel2;
    REQUIRE( UModel2.load(in2) );

}

SCENARIO("Test read-write-read2")
{
    ugltf::GLTFModel2 UModel;
    std::ifstream in("BoomBox.glb");
    REQUIRE( UModel.load(in) );
    in.close();

    std::ofstream out("BoomBox2.glb");
    UModel.writeGLB(out, UModel);
    out.close();
}

SCENARIO("Test Write")
{
    ugltf::GLTFModel2 M;

    M.bufferViews.emplace_back();
    M.bufferViews.emplace_back();
    M.bufferViews.emplace_back();

    M.bufferViews[0].data.resize(507);
    M.bufferViews[1].data.resize(1033);
    M.bufferViews[2].data.resize(283);

    auto cs = ugltf::GLTFModel2::_calculateBufferChunkSize(M.bufferViews,8);
    REQUIRE( cs[0] == 512);
    REQUIRE( cs[1] == 1040);

    auto j = ugltf::GLTFModel2::_generateJSONBufferViews(M.bufferViews, 8);

    std::cout << j.dump(4) << std::endl;
    REQUIRE( j.size() == 3);
    REQUIRE( j[0].at("byteLength") == 507);
    REQUIRE( j[1].at("byteLength") == 1033);
    REQUIRE( j[2].at("byteLength") == 283);

    REQUIRE( j[0].at("byteOffset") == 0);
    REQUIRE( j[1].at("byteOffset") == 512);
    REQUIRE( j[2].at("byteOffset") == 1552);

    std::ostringstream SS;
    ugltf::GLTFModel2::_writeBufferViewsToBufferChunk(SS, M.bufferViews, 8);

    auto strdata = SS.str();
    REQUIRE( strdata.size() == 8+cs[0]+cs[1]+cs[2] );

    std::cout << ugltf::GLTFModel2::generateJson(M).dump(4) << std::endl;
}


