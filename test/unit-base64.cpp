#include "catch.hpp"
#include <ugltf/ugltf.h>
#include <iostream>
#include <fstream>
#include <set>

SCENARIO("Base64 Decode 2")
{
    std::string enc = "AAAAAAAAAAAAAIA/AAAAAAAAAAAAAIA/AAAAAAAAAAAAAIA/AAAAAAAAAAAAAIA/AACAPwAAAAAAAAAAAACAPwAAAAAAAAAAAACAPwAAAAAAAAAAAACAPwAAAAAAAAAAAAAAAAAAgD8AAAAAAAAAAAAAgD8AAAAAAAAAAAAAgD8AAAAAAAAAAAAAgD8AAAAAAAAAAAAAgL8AAAAAAAAAAAAAgL8AAAAAAAAAAAAAgL8AAAAAAAAAAAAAgL8AAAAAAACAvwAAAAAAAAAAAACAvwAAAAAAAAAAAACAvwAAAAAAAAAAAACAvwAAAAAAAAAAAAAAAAAAAAAAAIC/AAAAAAAAAAAAAIC/AAAAAAAAAAAAAIC/AAAAAAAAAAAAAIC/AAAAvwAAAL8AAAA/AAAAPwAAAL8AAAA/AAAAvwAAAD8AAAA/AAAAPwAAAD8AAAA/AAAAPwAAAD8AAAA/AAAAPwAAAL8AAAA/AAAAPwAAAD8AAAC/AAAAPwAAAL8AAAC/AAAAvwAAAD8AAAA/AAAAPwAAAD8AAAA/AAAAvwAAAD8AAAC/AAAAPwAAAD8AAAC/AAAAPwAAAL8AAAA/AAAAvwAAAL8AAAA/AAAAPwAAAL8AAAC/AAAAvwAAAL8AAAC/AAAAvwAAAL8AAAA/AAAAvwAAAD8AAAA/AAAAvwAAAL8AAAC/AAAAvwAAAD8AAAC/AAAAvwAAAL8AAAC/AAAAvwAAAD8AAAC/AAAAPwAAAL8AAAC/AAAAPwAAAD8AAAC/AADAQAAAAAAAAKBAAAAAAAAAwED+/38/AACgQP7/fz8AAIBAAAAAAAAAoEAAAAAAAACAQAAAgD8AAKBAAACAPwAAAEAAAAAAAACAPwAAAAAAAABAAACAPwAAgD8AAIA/AABAQAAAAAAAAIBAAAAAAAAAQEAAAIA/AACAQAAAgD8AAEBAAAAAAAAAAEAAAAAAAABAQAAAgD8AAABAAACAPwAAAAAAAAAAAAAAAP7/fz8AAIA/AAAAAAAAgD/+/38/AAABAAIAAwACAAEABAAFAAYABwAGAAUACAAJAAoACwAKAAkADAANAA4ADwAOAA0AEAARABIAEwASABEAFAAVABYAFwAWABUA";

    auto ret = uGLTF::_fromBase64( &enc[0], &enc[ enc.size() ] );

    REQUIRE( ret.size() == 840 );
}

SCENARIO("Test Base64 decode")
{
    std::string orig = "Hello! This is a test.";
    std::string enc = "SGVsbG8hIFRoaXMgaXMgYSB0ZXN0Lg==";

    auto ret = uGLTF::_fromBase64( &enc[0], &enc[ enc.size() ] );

    REQUIRE( ret.size() == orig.size() );
    auto x = orig.begin();
    for(auto & r : ret)
    {
        REQUIRE( r == *x++);
    }
}

SCENARIO("Test Base64 encode")
{
    std::string orig = "Hello! This is a test.";
    std::string enc = "SGVsbG8hIFRoaXMgaXMgYSB0ZXN0Lg==";

    auto ret = uGLTF::_toBase64(&orig[0], &orig[ orig.size() ] );

    REQUIRE( ret.size() == enc.size() );
    auto x = enc.begin();
    for(auto & r : ret)
    {
        REQUIRE( r == *x++);
    }
}


SCENARIO("Test Base64 encode 2")
{
    float orig[] = {1,2,3,4,5};


    auto enc = uGLTF::_toBase64(&orig[0], &orig[ 5 ] );


    auto dec = uGLTF::_fromBase64( &enc[0], &enc[ enc.size() ] );

    float dec2[] = {9,9,9,9,9};

    std::memcpy(dec2,dec.data(), sizeof(float)*5);

    REQUIRE( orig[0] == Approx(dec2[0] ));
    REQUIRE( orig[1] == Approx(dec2[1] ));
    REQUIRE( orig[2] == Approx(dec2[2] ));
    REQUIRE( orig[3] == Approx(dec2[3] ));
    REQUIRE( orig[4] == Approx(dec2[4] ));
}
