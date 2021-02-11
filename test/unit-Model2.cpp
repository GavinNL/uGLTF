#include "catch.hpp"
#include <ugltf/ugltf2.h>
#include <iostream>
#include <fstream>
#include <set>

#include <regex>

SCENARIO("Test json ref")
{
    ugltf::json j;

    j["str"] = "hello this is a test";

    auto & ref = j.at("str");
    auto     p = ref.get_ptr<std::string*>();

    REQUIRE( *p == "hello this is a test");
}

SCENARIO("TEST LOAD")
{
    ugltf::GLTFModel2 UModel;
    std::ifstream in("CesiumMan.glb");
    REQUIRE( UModel.load(in) );
    in.close();
}

SCENARIO("TEST DATA CHUNK WRITE")
{
    ugltf::GLTFModel2 M;
    M.bufferViews.emplace_back();
    M.bufferViews[0].data.resize(507);

    auto cs = ugltf::GLTFModel2::_calculateBufferChunkSize(M.bufferViews,8);
    REQUIRE( cs[0] == 512 );

    std::ostringstream SS;
    ugltf::GLTFModel2::_writeBufferViewsToBufferChunk(SS, M.bufferViews, 8);

    auto raw = SS.str();
    REQUIRE( raw.size() == 512 + 8);
    uint32_t type;
    uint32_t siz;
    std::memcpy( &siz , &raw[0], sizeof(uint32_t));
    std::memcpy( &type, &raw[4], sizeof(uint32_t));

    REQUIRE( siz == 512);
}


SCENARIO("Test Full Write")
{
    ugltf::GLTFModel2 M;
    ugltf::GLTFModel2 M_in;

    M.bufferViews.emplace_back();

    M.bufferViews[0].data.resize(507);

    auto cs = ugltf::GLTFModel2::_calculateBufferChunkSize(M.bufferViews,8);
    REQUIRE( cs[0] == 512);

    std::ofstream out("testOut.glb");
    M.writeGLB(out, M);
    out.close();

    std::ifstream in("testOut.glb");
    REQUIRE( M_in.load(in) );
    in.close();

    std::cout << M_in._json.dump(4) << std::endl;
    REQUIRE( M_in._json["buffers"].size()           == 1  );
    REQUIRE( M_in._json["buffers"][0]["byteLength"] == 508);

    REQUIRE( M_in.bufferViews[0].data.size() == 507);
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

    //std::cout << ugltf::GLTFModel2::generateJson(M).dump(4) << std::endl;
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

SCENARIO("Test read-write-read3")
{
    ugltf::GLTFModel2 UModel;
    std::ifstream in("CesiumMan.glb");
    REQUIRE( UModel.load(in) );
    in.close();

    std::ofstream out("CesiumMan2.glb");
    UModel.writeGLB(out, UModel);
    out.close();
}

