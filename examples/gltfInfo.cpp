#include <ugltf/ugltf.h>


// THis is not included with ugltf, but was
// downloaded by the CMakeLists.txt file specifically for building
// this app
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <fstream>
#include <set>
#define INDENT "  "

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
            out += to_string(p) + ",";
        }
    }
    out.pop_back();
    std::cout << INDENT INDENT "Attributes: " << out << std::endl;
}

int main(int argc, char **argv)
{

    if( argc >= 2)
    {
        uGLTF::GLTFModel M;

        std::ifstream in( argv[1] );

        M.load(in);

        std::cout << "Asset: " << argv[1] << std::endl;

        std::cout << "Nodes: " << M.nodes.size() << std::endl;


        std::cout << "Root Nodes: " ;
        for(auto & r : M.scenes[0].nodes)
        {
            std::cout << " " << r;
        }
        std::cout << std::endl;
        size_t i=0;
        for(auto & m : M.nodes)
        {
            std::cout << INDENT << i++ <<": [";
            for(auto n : m.children)
            {
                std::cout << n << ", ";
            }
            std::cout << "]\n";
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
                }
                std::cout << INDENT INDENT "Vertices: " << p.getAccessor(uGLTF::PrimitiveAttribute::POSITION).count << std::endl;
                std::cout << INDENT INDENT "Hash: " << std::hex << p.getIDType() << std::dec << std::endl;
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
///                std::cout << INDENT << "Channel:           : " <<  i++ << std::endl;
///                std::cout << INDENT INDENT << "Node              : " <<  c.target.node<< std::endl;
///                std::cout << INDENT INDENT << "Path              : " <<  to_string(c.target.path) << std::endl;
            }

            std::cout << INDENT << "Animated Nodes    : ";// << m.channels.size() << std::endl;
            for(auto & n : nodes)
            {
                std::cout << n << ", ";
            }
            std::cout << std::endl;
            std::cout << INDENT << "Samplers          : " << m.samplers.size() << std::endl;
            for(auto & s : m.samplers)
            {

                std::cout << INDENT INDENT << to_string(s.getOutputAccessor().type) << INDENT << "Total Frames      : " << s.getOutputAccessor().count
                          << INDENT INDENT << "Time Interval     : " << s.getInputSpan().front() << ", " << s.getInputSpan().back() << std::endl;

            }

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
                auto data = I.getSpan();
                std::cout << INDENT INDENT << "img data size     : " << data.size() << std::endl;

                int x,y,comp;
                auto * img = stbi_load_from_memory( &data[0], data.size(), &x, &y, &comp, 3);

                std::cout << INDENT INDENT << "Dimensions  e     : " << x << " x " << y << std::endl;

                stbi_image_free(img);
            }

            //auto & S = m.getSampler();
            //S.magFilter
            //std::cout << INDENT << "img data size     : " << data.size() << std::endl;

        }



        //=====================================================================================================

    }

    return 42;
}

