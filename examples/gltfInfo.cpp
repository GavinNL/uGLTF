#include <ugltf/ugltf.h>


// THis is not included with ugltf, but was
// downloaded by the CMakeLists.txt file specifically for building
// this app
//#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fstream>
#include <set>
#define INDENT "  "

#include <spdlog/spdlog.h>
#include <lyra/lyra.hpp>
#include <spdlog/fmt/fmt.h>

void printAttributes(uGLTF::Primitive const & P)
{
    uGLTF::PrimitiveAttribute attr[] =
    {
        uGLTF::PrimitiveAttribute::POSITION  ,
        uGLTF::PrimitiveAttribute::NORMAL	 ,
        uGLTF::PrimitiveAttribute::TANGENT	 ,
        uGLTF::PrimitiveAttribute::TEXCOORD_0,
        uGLTF::PrimitiveAttribute::TEXCOORD_1,
        uGLTF::PrimitiveAttribute::COLOR_0	 ,
        uGLTF::PrimitiveAttribute::JOINTS_0  ,
        uGLTF::PrimitiveAttribute::WEIGHTS_0 ,
    };

    std::string out;
    for(auto & p : attr)
    {
        if( P.has(p) )
        {

            auto & A = P.getAccessor(p);


            out += to_string(p) + "(" + to_string( A.componentType ) + ")  ";
        }
    }
    out.pop_back();
    std::cout << INDENT INDENT "Attributes: " << out << std::endl;
}

std::string to_string( std::array<float,3> const & F)
{
    return fmt::format("{},{},{}", F[0], F[1], F[2]);
}
std::string to_string( std::array<float,4> const & F)
{
    return fmt::format("{},{},{},{}", F[0], F[1], F[2], F[3]);
}

std::string to_string( uGLTF::Node const & N)
{
    return fmt::format("T:{:<30} R:{:<30} S:{:<30}", to_string(N.translation), to_string(N.rotation), to_string(N.scale));
}

void printNodeHierarchy( uGLTF::GLTFModel const & M, uGLTF::Node const & N, std::string indent="")
{
    auto dist = std::distance(&M.nodes[0], &N);
    std::cout << indent << fmt::format("[{:2}] {}", dist, N.name) << '\n';
    indent += "   ";
    for(auto & c : N.children)
    {
        printNodeHierarchy(M, M.nodes[c], indent);
    }
}

int printInfo(std::string const & filename,
              bool printBuffers,
              bool printAnimationDetail)
{

  //  if( argc >= 2)
    {
        uGLTF::GLTFModel M_in;

        std::ifstream in( filename );

        M_in.load(in);
        auto & M = M_in;

        std::cout << "Asset: " << filename << std::endl;

        std::cout << "Nodes: " << M.nodes.size() << std::endl;


        for(auto & s : M.scenes)
        {
            std::cout << "Root Nodes: " ;
            for(auto & r : s.nodes)
            {
                std::cout << " " << r;
            }
            std::cout << std::endl;
        }

        for(auto & s : M.scenes)
        {
            for(auto r : s.nodes)
            {
                printNodeHierarchy( M, M.nodes[r]);
            }
        }
        size_t i=0;
        for(auto & m : M.nodes)
        {
            std::cout << INDENT << fmt::format("NODE NAME [{:2}] : {:<30}", i++, m.name) << ": ";
            std::cout << "  CHILDREN : " << " [";
            for(auto n : m.children)
            {
                std::cout << n << ", ";
            }
            std::cout << "] ";

            if( m.hasMesh() )
                std::cout << "MESH: " << m.mesh << "  ";

            if( m.hasSkin() )
                std::cout << "SKIN: " << m.skin << "  ";

            std::cout << "\n";
            std::cout << INDENT << "NODE TRANSLATION: " << to_string(m.translation) << '\n';
            std::cout << INDENT << "NODE ROTATION   : " << to_string(m.rotation) << '\n';
            std::cout << INDENT << "NODE SCALE      : " << to_string(m.scale) << '\n';



        }

        std::cout << "Meshs: " << M.meshes.size() << std::endl;
        i=0;
        for(auto & m : M.meshes)
        {
            std::cout << INDENT << i++ << std::endl;
            std::cout << INDENT "Name: " << m.name << std::endl;
            std::cout << INDENT "Primitives: " << m.primitives.size() << std::endl;

            size_t pi = 0;
            for(auto & p : m.primitives)
            {
                std::cout << INDENT << pi++ << std::endl;
                std::cout << INDENT INDENT "Mode: " << to_string(p.mode) << std::endl;
                if( p.hasIndices() )
                {
                    std::cout << INDENT INDENT "Elements: " << p.getIndexAccessor().count << std::endl;
                    std::cout << INDENT INDENT "Type    : " << to_string(p.getIndexAccessor().componentType) << std::endl;
                }
                std::cout << INDENT INDENT "Vertices: " << p.getAccessor(uGLTF::PrimitiveAttribute::POSITION).count << std::endl;
                std::cout << INDENT INDENT "Hash: " << std::hex << p.getIDType() << std::dec << std::endl;
                std::cout << INDENT INDENT "Material : " << p.material << std::endl;
                printAttributes( p );

            }
        }


        std::cout << "Materials: " << M.meshes.size() << std::endl;
        i=0;
        for(auto & m : M.materials)
        {
            std::cout << INDENT << i++ << std::endl;
            //std::cout << INDENT "PBR: " << m.hasPBR() << std::endl;
            if( m.hasNormalTexture() )
            {
                std::cout << INDENT "normalTexture: \n";
                std::cout << INDENT "     index: "    << m.normalTexture.index << '\n';
                std::cout << INDENT "  texCoord: "    << m.normalTexture.texCoord << '\n';
                std::cout << INDENT "     scale: "    << m.normalTexture.scale << '\n';
            }

            if( m.hasEmissiveTexture() )
            {
                std::cout << INDENT "emissiveTexture: \n";
                std::cout << INDENT "     index: "    << m.emissiveTexture.index << '\n';
                std::cout << INDENT "  texCoord: "    << m.emissiveTexture.texCoord << '\n';
                std::cout << INDENT "     scale: "    << m.emissiveTexture.scale << '\n';
            }

            if( m.hasOcclusionTexture() )
            {
                std::cout << INDENT "occlusionTexture: \n";
                std::cout << INDENT "     index: "    << m.occlusionTexture.index << '\n';
                std::cout << INDENT "  texCoord: "    << m.occlusionTexture.texCoord << '\n';
                std::cout << INDENT "  strength: "    << m.occlusionTexture.strength << '\n';
            }

            std::cout << INDENT "emissiveFactor          : " << m.emissiveFactor[0] << ", "
                                                             << m.emissiveFactor[1] << ", "
                                                             << m.emissiveFactor[2] << ", "
                                                             << m.emissiveFactor[3] << '\n';
            if(m.hasPBR())
            {
                std::cout << INDENT "PBR: \n";
                std::cout << INDENT INDENT "baseColorFactor       : " << m.pbrMetallicRoughness.baseColorFactor[0] << ", "
                                                                      << m.pbrMetallicRoughness.baseColorFactor[1] << ", "
                                                                      << m.pbrMetallicRoughness.baseColorFactor[2] << ", "
                                                                      << m.pbrMetallicRoughness.baseColorFactor[3] << '\n';

                std::cout << INDENT INDENT "metallicFactor        : " << m.pbrMetallicRoughness.metallicFactor   << '\n';
                std::cout << INDENT INDENT "roughnessFactor       : " << m.pbrMetallicRoughness.roughnessFactor << '\n';

                if(m.pbrMetallicRoughness.hasBaseColorTexture())
                    std::cout << INDENT INDENT "BaseColorTexture        : " << m.pbrMetallicRoughness.baseColorTexture.index << '\n';
                if(m.pbrMetallicRoughness.hasMetallicRoughnessTexture())
                    std::cout << INDENT INDENT "MetallicRoughnessTexture: " << m.pbrMetallicRoughness.metallicRoughnessTexture << '\n';

            }


            //m.pbrMetallicRoughness.
        }

        //=====================================================================================================
        // Skin/Skeleton
        //=====================================================================================================
        std::cout << "Skins: " << M.skins.size() << std::endl;
        i=0;

        for(auto & m : M.skins)
        {
            std::cout << INDENT << "Name              : " << m.name << std::endl;
            std::cout << INDENT << "Root Skeleton Node: " << m.skeleton << std::endl;
            std::cout << INDENT << "Joint Nodes       : " ;
            for(auto & j : m.joints)
                std::cout << j << ' ';
            std::cout << std::endl;


            auto & inverseBindMatrices = m.getInverseBindMatricesAccessor();
            std::cout << INDENT << "Total inv Bind Mat: " << inverseBindMatrices.count << std::endl;

        }
        //=====================================================================================================




        //=====================================================================================================
        // Animations
        //=====================================================================================================
        std::cout << "Animations: " << M.animations.size() << std::endl;
        i=0;

        for(auto & m : M.animations)
        {
            std::cout << INDENT << "Name              : " << m.name << std::endl;
            std::cout << INDENT << "Channels          : " << m.channels.size() << std::endl;

            uint32_t i=0;
            std::set<uint32_t> nodes;
            for(auto & c : m.channels)
            {
                nodes.insert(c.target.node);
            }


            std::cout << INDENT << "Animated Nodes    : ";// << m.channels.size() << std::endl;
            for(auto & n : nodes)
            {
                std::cout << n << ", ";
            }
            std::cout << std::endl;

            if( printAnimationDetail )
            {
                for(auto & c : m.channels)
                {
                    std::cout << INDENT << "   Channel: " <<  i++
                              << "  Node: " <<  c.target.node
                              << "  Sampler: " <<  c.sampler
                              << "  Path: " <<  to_string(c.target.path) << std::endl;
                }

                std::cout << INDENT << "Samplers          : " << m.samplers.size() << std::endl;
                for(auto & s : m.samplers)
                {
                    auto & iA = s.getInputAccessor();

                   // auto & oA = s.getOutputAccessor();
                    std::cout << INDENT INDENT << s.input << "  " << to_string(s.getOutputAccessor().type) << INDENT << "Total Frames      : " << s.getOutputAccessor().count
                              << INDENT INDENT << "Time Interval     : " << iA.getValue<float>(0) << ", " << iA.getValue<float>(iA.count-1)
                              << INDENT INDENT << "Interpolation : " << to_string(s.interpolation) << std::endl;
                              //<< INDENT INDENT << "input Accessor Size : " << iA.accessorSize() << std::endl;
                }
            }
            std::cout << "\n\n";
        }

        //=====================================================================================================



        //=====================================================================================================
        // Textures
        //=====================================================================================================
        std::cout << "Textures: " << M.textures.size() << std::endl;
        i=0;

        for(auto & m : M.textures)
        {
            std::cout << INDENT << "Source: " << m.source << std::endl;
        }

        //=====================================================================================================
        // Images
        //=====================================================================================================
        std::cout << "Images: " << M.textures.size() << std::endl;
        i=0;

        for(auto & I : M.images)
        {
            std::cout << INDENT << "Name              : " << I.name << std::endl;

            std::cout << INDENT INDENT << "img mime type     : " << I.mimeType << std::endl;

            {
//                auto data = I.getSpan();

                auto & bufferView = I.getBufferView();
                std::cout << INDENT INDENT << "img data size     : " << bufferView.byteLength << std::endl;

                int x,y,comp;
                auto * img = stbi_load_from_memory( static_cast<const stbi_uc*>(bufferView.data()) , bufferView.byteLength , &x, &y, &comp, 3);

                std::cout << INDENT INDENT << "Dimensions  e     : " << x << " x " << y << std::endl;

                stbi_image_free(img);
            }

            //auto & S = m.getSampler();
            //S.magFilter
            //std::cout << INDENT << "img data size     : " << data.size() << std::endl;

        }

        //=====================================================================================================
        if( printBuffers )
        {
            std::cout << "Buffers: " << M.buffers.size() << std::endl;
            i=0;

            for(auto & I : M.buffers)
            {
                std::cout << INDENT << "Bytes             : " << I.byteLength << std::endl;
            }
            //=====================================================================================================
            std::cout << "BufferViews: " << M.textures.size() << std::endl;
            i=0;

            for(auto & I : M.bufferViews)
            {
                std::cout << INDENT << i++
                          << INDENT << "Buffer : " << I.buffer
                          << INDENT << "Bytes  : " << I.byteLength
                          << INDENT << "Offset : " << I.byteOffset
                          << INDENT << "Stride : " << I.byteStride << std::endl;

            }
            //=====================================================================================================
            std::cout << "Accessors: " << M.accessors.size() << std::endl;
            i=0;

            for(auto & I : M.accessors)
            {
                std::cout << INDENT << i++
                          << INDENT << "bufferView : " << I.bufferView
                          << INDENT << "type  : " << to_string(I.type)
                          << INDENT << "byteOffset : " << I.byteOffset
                          << INDENT << "count : " << I.count << std::endl;

            }
        }
    }

    return 42;
}

int main(int argc, char ** argv)
{
 //   int width = 0;
    std::string name;
    bool printNodes = false;
    std::string fileName;
    bool show_help = false;

    bool printAnimationDetail = false;
    bool printBuffers = false;

    auto cli
        = lyra::help(show_help).description(
                    "This is a combined sample CLI parser. It takes varied options"
                    "and arguments.")
//        | lyra::opt( width, "width" )
//            ["-w"]["--width"]
//            ("How wide should it be?")
//        | lyra::opt( name, "name" )
//            ["-n"]["--name"]
//            ("By what name should I be known")
        | lyra::opt( printNodes )
            ["--printNodes"]
            ("Print only the Nodes" )
        | lyra::opt( printBuffers )
            ["--printBuffers"]
            ("Print information about the Buffers and BufferViews" )
        | lyra::opt( printAnimationDetail )
            ["--printAnimationDetail"]
            ("Prints the Animations samplers and channel information" )
        | lyra::arg( fileName, "input GLTF/GLB file" )
            ("Which GLTF asset to read");


    auto result = cli.parse( { argc, argv } );
    if ( !result )
    {
        std::cerr << "Error in command line: " << result.errorMessage() << std::endl;

        exit(1);
    }
    if (show_help or !result)
    {
        std::cout << cli << "\n";
        return 0;
    }

    if( printNodes )
    {

    }
    else
    {
        printInfo( fileName, printBuffers, printAnimationDetail );
    }
    std::cout << fileName << std::endl;

    return 0;
}
