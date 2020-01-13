#include "catch.hpp"
#include <ugltf/ugltf.h>
#include <iostream>
#include <fstream>
#include <set>
#include <algorithm>
#include <spdlog/spdlog.h>

void printVector(std::vector<uint32_t> const & vec)
{
    for(auto & v : vec)
        std::cout << v << ", ";
    std::cout << std::endl;
}

template<typename T, typename U>
void REORDER(std::vector<T>& vA, std::vector<U>& vOrder)
{
    assert(vA.size() == vOrder.size());

    // for all elements to put in place
    for( size_t i = 0; i < vA.size() - 1; ++i )
    {
        // while the element i is not yet in place
        while( i != vOrder[i] )
        {
            // swap it with the element at its final place
            int alt = vOrder[i];
            std::swap( vA[i], vA[alt] );
            std::swap( vOrder[i], vOrder[alt] );
        }
    }
}

void _replaceNode(uGLTF::GLTFModel & M, std::vector<uint32_t>  & _map)
{
    for(auto & N : M.nodes)
    {
        for(auto & c : N.children)
        {
            c = _map[c];
        }
    }

    for(auto & S : M.skins)
    {
        for(auto & n : S.joints)
        {
            n = _map[n];
        }
    }

    for(auto & A : M.animations)
    {
        for(auto & C : A.channels)
        {
            C.target.node = _map[ C.target.node];
        }
    }

    for(auto & S : M.scenes)
    {
        for(auto & n : S.nodes)
        {
            n = _map[n];
        }
    }

    REORDER( M.nodes, _map);

    //reorder( _map.begin(). _map.end(), M.nodes.begin() );
}
void changeNodeIndex(uGLTF::GLTFModel & M, uint32_t prevNodeIndex, uint32_t newNodeIndex)
{
    assert( prevNodeIndex != newNodeIndex);

    std::vector<uint32_t> old;
    for(uint32_t i=0;i<M.nodes.size();i++)
        old.push_back(i);


    auto New = old;

    printVector(old);

    New.erase( New.begin() + prevNodeIndex);
    New.insert( New.begin() + newNodeIndex, prevNodeIndex);
    printVector(New);

    std::vector<uint32_t> _map = old;
    for(auto i=0u;i<New.size();i++)
    {
        //_replaeNode( M, New[i], old[i] );
        std::cout << i << ". Nodes " << New[i] << " should be replaced by " << old[i] << std::endl;
        _map[ New[i] ] = old[i];

    }
    _replaceNode(M, _map);

    //for(auto)
}

SCENARIO("Writing a GLB file")
{
    GIVEN("A GLTF model with a texture")
    {
        uGLTF::GLTFModel M;

        std::ifstream in("BrainStem.glb");

        REQUIRE( M.load(in) );

        WHEN("We save the model as a GLB file")
        {
            changeNodeIndex(M, 3, 15);
        }
    }
}
