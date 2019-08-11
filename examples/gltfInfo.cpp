#include <ugltf/ugltf.h>
#include <fstream>

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

        std::cout << "Root Node: " << M.scenes[0].nodes[0] << std::endl;
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
                std::cout << INDENT INDENT "Hash: " << std::hex << p.getIDType() << std::dec << std::endl;
                printAttributes( p );
            }
        }


        std::cout << "Materials: " << M.meshes.size() << std::endl;
        i=0;
        for(auto & m : M.materials)
        {
            std::cout << INDENT << i++ << std::endl;
            std::cout << INDENT "PBR: " << m.hasPBR() << std::endl;

            if(m.hasPBR())
            {
                std::cout << INDENT INDENT "baseColorFactor       : " << m.pbrMetallicRoughness.baseColorFactor[0] << ", "
                                                                      << m.pbrMetallicRoughness.baseColorFactor[1] << ", "
                                                                      << m.pbrMetallicRoughness.baseColorFactor[2] << '\n';

                std::cout << INDENT INDENT "metallicFactor        : " << m.pbrMetallicRoughness.metallicFactor   << '\n';
                std::cout << INDENT INDENT "roughnessFactor        : " << m.pbrMetallicRoughness.roughnessFactor << '\n';

                if(m.pbrMetallicRoughness.hasBaseColorTexture())
                    std::cout << INDENT INDENT "BaseColorTexture        : " << m.pbrMetallicRoughness.baseColorTexture << '\n';
                if(m.pbrMetallicRoughness.hasMetallicRoughnessTexture())
                    std::cout << INDENT INDENT "MetallicRoughnessTexture: " << m.pbrMetallicRoughness.metallicRoughnessTexture << '\n';

            }
            if( m.hasNormalTexture() )
                std::cout << INDENT "normalTexture: "    << m.normalTexture << '\n';
            if( m.hasEmissiveTexture() )
                std::cout << INDENT "emissiveTexture: "    << m.emissiveTexture << '\n';
            if( m.hasOcclusionTexture() )
                std::cout << INDENT "occlusionTexture: "    << m.occlusionTexture << '\n';

            //m.pbrMetallicRoughness.
        }

    }

    return 42;
}

