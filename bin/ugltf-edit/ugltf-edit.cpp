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

int stripAnimation(std::string input, std::string output)
{
    spdlog::set_level(spdlog::level::debug);
        uGLTF::GLTFModel M_in1;
        std::ifstream in1( input);

        if( !in1)
            return 1;

        M_in1.load(in1);

        spdlog::info("Stripping all animations from {}", input);
        spdlog::info("Total Animations: {}", M_in1.animations.size());

        spdlog::info("Total Buffers: {} ", M_in1.buffers.size());
        spdlog::info("Splitting Buffers");
        M_in1.splitBuffers();
        spdlog::info("Total Buffers: {} ", M_in1.buffers.size());


        std::set<uint32_t> bufferViewsToKeep;

        uint32_t accessorCount=0;
        uint32_t bufferViewCount=0;
        std::map< uint32_t , uint32_t > oldToNewAccessors;

        std::map< uint32_t , uint32_t > oldToNewBufferViews;

        std::vector<uGLTF::Accessor>    newAccessors;
        std::vector<uGLTF::BufferView>  newBufferViews;
        std::vector<uGLTF::Buffer>      newBuffers;

        auto checkBufferview = [&](uint32_t bufferViewIndex)
        {
            auto & oldToNew = oldToNewBufferViews;

            if( bufferViewIndex != std::numeric_limits<uint32_t>::max() )
            {
                if( oldToNew.count( bufferViewIndex ) == 0)
                {
                    oldToNew[ bufferViewIndex ] = bufferViewCount++;
                    newBufferViews.push_back(  M_in1.bufferViews[bufferViewIndex] );
                    spdlog::debug("     BufferView Index Changed: {} --> {}", bufferViewIndex, bufferViewCount-1);

                    return bufferViewCount-1;
                }
                else
                {
                    spdlog::debug("     BufferView Index Changed: {} --> {}", bufferViewIndex, oldToNew[bufferViewIndex]);
                    return oldToNew[bufferViewIndex];
                }
            }
            else
            {
                return bufferViewIndex;
            }
        };

        auto checkAccessor = [&](uint32_t accessorIndex)
        {
            auto & oldToNew = oldToNewAccessors;

            if( accessorIndex != std::numeric_limits<uint32_t>::max() )
            {
                if( oldToNew.count( accessorIndex ) == 0)
                {
                    oldToNew[ accessorIndex ] = accessorCount++;
                    newAccessors.push_back(  M_in1.accessors[accessorIndex] );
                    spdlog::debug("  Accessor Index Changed: {} --> {}", accessorIndex, accessorCount-1);
                    newAccessors.back().bufferView = checkBufferview( M_in1.accessors[accessorIndex].bufferView);
                    return accessorCount-1;
                }
                else
                {
                    spdlog::debug("  Accessor Index Changed: {} --> {}", accessorIndex, oldToNew[accessorIndex]);
                    return oldToNew[accessorIndex];
                }
            }
            else
            {
                return accessorIndex;
            }
        };



        for(auto & A : M_in1.meshes)
        {
            for(auto & S : A.primitives)
            {
                S.indices               = checkAccessor(S.indices);
                S.attributes.POSITION   = checkAccessor(S.attributes.POSITION  );
                S.attributes.NORMAL	    = checkAccessor(S.attributes.NORMAL	   );
                S.attributes.TANGENT    = checkAccessor(S.attributes.TANGENT   );
                S.attributes.TEXCOORD_0 = checkAccessor(S.attributes.TEXCOORD_0);
                S.attributes.TEXCOORD_1 = checkAccessor(S.attributes.TEXCOORD_1);
                S.attributes.COLOR_0    = checkAccessor(S.attributes.COLOR_0   );
                S.attributes.JOINTS_0   = checkAccessor(S.attributes.JOINTS_0  );
                S.attributes.WEIGHTS_0  = checkAccessor(S.attributes.WEIGHTS_0 );
            }
        }

        for(auto & A : M_in1.skins)
        {
            A.inverseBindMatrices = checkAccessor(A.inverseBindMatrices);
        }

        for(auto & A : M_in1.images)
        {
            if( A.m_imageData.size() == 0)
            {
                A.bufferView = checkBufferview(A.bufferView);
            }
        }

        M_in1.bufferViews = newBufferViews;
        M_in1.accessors = newAccessors;
        M_in1.animations.clear();
        //M_in1.images.clear();
        //M_in1.textures.clear();

        M_in1.mergeBuffers();
        spdlog::info("Total Buffers: {} ", M_in1.buffers.size());


        std::ofstream fout( output );
        M_in1.writeGLB( fout );

        return 0;
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

    bool stripAnimations=false;
    bool mergeAnimations=false;

    auto cli
        = lyra::help(show_help) // <2>
        |  lyra::opt( output, "output" )
            ["-o"]["--output"]
            ("Output GLB file")
        |  lyra::opt( mergeAnimations )
            ["--mergeAnimations"]
            ("Merge all the animations from the input list. Node animations are mapped by name, so each GLTF file must contain nodes that have the same name.")
        |  lyra::opt( stripAnimations )
            ["-s"]["--stripAnimations"]
            ("Strip all the animations from the GLB file.")
        | lyra::arg( inputs, "files" )
            ("A list of input GLB files");

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

    if( stripAnimations)
    {
        if( inputs.size() == 1)
            return stripAnimation( inputs.front(), output);
        else
        {
            spdlog::error("To strip animations, only 1 input file must be listed");
            exit(1);
        }
    }


    if( mergeAnimations )
    {
        uGLTF::GLTFModel M_in1;

        spdlog::info("reading");
        inputs.clear();
        for (std::string i1; std::getline(std::cin, i1);)
        {
            std::cout << i1 << std::endl;
            spdlog::info(i1);
            inputs.push_back(i1);
        }
        std::reverse( inputs.begin(), inputs.end());

        {
            std::ifstream in1( inputs.back() );
            M_in1.load(in1);
            inputs.pop_back();
        }



        while(inputs.size())
        {
            std::string i1 = inputs.back();

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
            inputs.pop_back();
        }

        std::ofstream fout( output );
        M_in1.writeGLB( fout);
    }
    return 0;



    return  1;
}

