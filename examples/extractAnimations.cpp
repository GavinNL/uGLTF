#include <ugltf/ugltf.h>
#include <spdlog/spdlog.h>
#include <lyra/lyra.hpp>
#include <fstream>
#include <set>

std::optional<uint32_t> findNode( const std::string & name, uGLTF::GLTFModel const & M1)
{
    uint32_t i=0;
    for(auto & n : M1.nodes)
    {
        if( n.name == name)
        {
            return i;
        }
        i++;
    }

    return {};
}

std::vector<uint32_t> getAnimatedNodes(uGLTF::Animation const & A)
{
    std::set<uint32_t> s;
    for(auto & c : A.channels)
    {
        s.insert( c.target.node );
    }
    std::vector<uint32_t> x(s.size());
    for(auto c : s)
    {
        x.push_back(c);
    }
    return x;

}
bool checkAnimations(uGLTF::GLTFModel const & M1, uGLTF::GLTFModel & M2 )
{
    bool out=true;



    for(auto & a : M1.animations)
    {
        auto animatedNodes = getAnimatedNodes(a);

        uint32_t j=0;
        for(auto  n : animatedNodes)
        {
            auto & N = M1.nodes[n];

            auto i = findNode( N.name, M2 );
            if(!i)
            {
                spdlog::error("Node {} does not exist in model 2", N.name);
                out = false;
            }
            else {
                spdlog::info(" M1[{}] == M2[{}] ", j, *i);
            }
            j++;
        }
    }
    return out;
}


int main(int argc, char **argv)
{
    if( argc == 1)
    {
        std::cout << "Extract or combine animations" << std::endl << std::endl;;

        std::cout << "Usage:\n\n";
        std::cout << "Extract all the animations into a separate glb file:\n\n";
        std::cout << "The new glb file will be invalid because the animations will be referencing\n"
                     "nodes that do not exist:\n\n";
        std::cout << "   ./extractAnimations input.glb  output_animations_only.glb\n\n\n";
        std::cout << "Copy all the animations from input1 into input2 and store it in output_combined.glb (including mesh data):\n\n";
        std::cout << "   ./extractAnimations input1.glb  input2.glb output_combined.glb\n\n";
        return 0;
    }

    //int width = 0;
    std::string name;
    //bool doIt = false;
    std::string command;
    bool show_help=false;
    std::vector<std::string> inputs;
    std::string output;
    std::string input;

    auto cli
        = lyra::help(show_help) // <2>
        |  lyra::opt( output, "output" )
            ["-o"]["--output"]
            ("Output GLB file")
        |  lyra::opt( input, "input" )
            ["-i"]["--input"]
            ("The main input file that all other animations will be added to. This file should contain the mesh")
        | lyra::arg( inputs, "files" )
            ("A list of GLB files which contain animations.");

    // Parse the program arguments:
    auto result = cli.parse({ argc, argv });

    // Check that the arguments where valid:
    if (!result)
    {
        std::cerr << "Error in command line: " << result.errorMessage() << std::endl;
        std::cerr << cli << "\n"; // <1>
        exit(1);
    }

    // Show the help when asked for.
    if (show_help) // <2>
    {
        std::cout << cli << "\n";
        exit(0);
    }
    for(auto & i : inputs)
    {
        std::cout << i << std::endl;
    }

    {
        uGLTF::GLTFModel M_in1;



        {
            std::ifstream in1( input);
            M_in1.load(in1);
        }

        for(auto & i1 : inputs)
        {
            uGLTF::GLTFModel M_in2;
            std::ifstream in2( i1 );
            M_in2.load(in2);

            if( checkAnimations(M_in1, M_in2) )
            {
                uint32_t i=0;
                for(auto & A : M_in2.animations)
                {
                    uGLTF::copyAnimation(M_in1, M_in2, i++);
                    M_in1.animations.back().name = i1;
                    assert(&A);
                }
            }
        }


        std::ofstream fout( output );
        M_in1.writeGLB( fout);
    }
    return 0;



    return  1;
}

