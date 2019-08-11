#include <ugltf/ugltf.h>
#include <fstream>

#define INDENT "  "

void printAttributes(uGLTF::Primitive const & P)
{
    uGLTF::PrimitiveAttribute attr[]=
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

        std::cout << "Meshs: " << M.meshes.size() << std::endl;
        size_t i=0;
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
                std::cout << INDENT INDENT "BaseColorTexture        : " << m.pbrMetallicRoughness.hasBaseColorTexture() << std::endl;
                std::cout << INDENT INDENT "MetallicRoughnessTexture: " << m.pbrMetallicRoughness.hasMetallicRoughnessTexture() << std::endl;
            }
            std::cout << INDENT "NormalTexture: " << m.hasNormalTexture() << std::endl;
            std::cout << INDENT "EmissiveTexture: " << m.hasEmissiveTexture() << std::endl;
            std::cout << INDENT "OcclusionTexture: " << m.hasOcclusionTexture() << std::endl;

            //m.pbrMetallicRoughness.
        }

    }

    return 42;
}

