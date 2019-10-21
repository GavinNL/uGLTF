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

std::vector<uint8_t> toRawData(uGLTF::Accessor const & A)
{
    std::vector<uint8_t> d;

    auto srcData = static_cast<uint8_t const*>( A.getBufferView().data() );
    auto stride  = A.getBufferView().byteStride==0 ? A.componentSize() : A.getBufferView().byteStride;

    //auto compSize = A.componentSize();
    auto accessorSize = A.accessorSize();
    auto count = A.count;

    d.resize( count * accessorSize );

    spdlog::info("Copying {} items of size {} ", count, accessorSize);

    auto dstData = d.data();
    while(count--)
    {
        std::memcpy( dstData, srcData, accessorSize);

        srcData += stride;
        dstData += accessorSize;
    }
    return d;
}

int copyAnimations2(uGLTF::GLTFModel const & src, uGLTF::GLTFModel & dst )
{
    for(auto & a : src.animations)
    {
        uGLTF::Buffer & newBuffer = dst.newBuffer();

        uGLTF::Animation & newA = dst.newAnimation();

        for(auto & s : a.samplers)
        {
            uGLTF::AnimationSampler & newS = newA.newSampler();

            // get the accessor for the input value
            auto & input  = s.getInputAccessor();
            auto & output = s.getOutputAccessor();

            newS.input  = newBuffer.createNewAccessor(input);
            newS.output = newBuffer.createNewAccessor(output);
            newS.interpolation = s.interpolation;
        }

        for(auto & c : a.channels)
        {
            newA.channels.push_back(c);
        }

    }

    dst._setParents(&dst);
    return 0;
}


int copyAnimations(uGLTF::GLTFModel const & src, uGLTF::GLTFModel & dst )
{

    std::vector< uGLTF::Accessor >   & newAccessors   = dst.accessors;
    std::vector< uGLTF::BufferView > & newBufferViews = dst.bufferViews;
    std::vector< uGLTF::Buffer  >    & newBuffers     = dst.buffers;

    for(auto & a : src.animations)
    {
        uGLTF::Animation & newA = dst.animations.emplace_back();

        uGLTF::Buffer & newBuffer = newBuffers.emplace_back();

        for(auto & s : a.samplers)
        {
            uGLTF::AnimationSampler & newS = newA.samplers.emplace_back();

            {
                // get the accessor for the input value
                auto input  = s.getInputAccessor();
                // and make a copy of the data
                auto rawData = toRawData(input);

                // merge the hew data into the buffer
                newBuffer.m_data.insert( newBuffer.m_data.end(), rawData.begin(), rawData.end() );

                uGLTF::BufferView & newBufferView    = newBufferViews.emplace_back();

                newBufferView.buffer     = newBuffers.size()-1;
                newBufferView.byteLength = rawData.size();
                newBufferView.byteOffset = newBuffer.m_data.size();
                newBufferView.byteStride = 0;
                newBufferView.target     = uGLTF::BufferViewTarget::UNKNOWN; // unknown since this isn't actually drawn


                uGLTF::Accessor   & newInputAccessor = newAccessors.emplace_back();
                newInputAccessor = input;

                newInputAccessor.bufferView = newBufferViews.size()-1;
                newInputAccessor.byteOffset = 0; // we will have a unique buffer view for each

                newS.input = newAccessors.size()-1;
            }

            {
                // get the accessor for the input value
                auto input  = s.getOutputAccessor();
                // and make a copy of the data
                auto rawData = toRawData(input);

                // merge the hew data into the buffer
                newBuffer.m_data.insert( newBuffer.m_data.end(), rawData.begin(), rawData.end() );

                uGLTF::BufferView & newBufferView    = newBufferViews.emplace_back();

                newBufferView.buffer     = newBuffers.size()-1;
                newBufferView.byteLength = rawData.size();
                newBufferView.byteOffset = newBuffer.m_data.size();
                newBufferView.byteStride = 0;
                newBufferView.target     = uGLTF::BufferViewTarget::UNKNOWN; // unknown since this isn't actually drawn


                uGLTF::Accessor   & newInputAccessor = newAccessors.emplace_back();
                newInputAccessor = input;

                newInputAccessor.bufferView = newBufferViews.size()-1;
                newInputAccessor.byteOffset = 0; // we will have a unique buffer view for each

                newS.output = newAccessors.size()-1;
            }

        }

        for(auto & c : a.channels)
        {
            newA.channels.push_back(c);
        }


        newBuffer.byteLength = newBuffer.m_data.size();

    }

    dst._setParents(&dst);
    return 0;
}


void makeDouble(uGLTF::GLTFModel & M, std::string out)
{
    auto m2 = M;
    copyAnimations2(M, m2);

    std::ofstream fout(out);
    m2.writeGLB( fout);

}

int main(int argc, char **argv)
{
//    uGLTF::GLTFModel M;
//    uGLTF::GLTFModel M2;

//    std::ifstream in("/home/globo/Other_Projects/models/test_blend.glb");

//    M.load(in);

//    copyAnimations(M, M2);
//    return 0;
    if( argc >= 2)
    {
        uGLTF::GLTFModel M_in;

        //std::ifstream in( argv[1] );
        std::ifstream in("/home/globo/Other_Projects/uGLTF/build/test/BoxAnimated.glb");

        M_in.load(in);

        uGLTF::GLTFModel M = M_in;
        copyAnimations2(M_in, M);

        //makeDouble(M, "test.glb");
        //return 0;

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
            std::cout << INDENT << "Name: " << m.name << '\n';
            std::cout << INDENT << i++ <<": [";
            for(auto n : m.children)
            {
                std::cout << n << ", ";
            }
            std::cout << "]\n";

            std::cout << INDENT INDENT << "T " << m.translation[0] << ", "
                              << m.translation[1] << ", "
                              << m.translation[2] << "\n";
            std::cout << INDENT INDENT << "R " << m.rotation[0] << ", "
                              << m.rotation[1] << ", "
                              << m.rotation[2] << ", "
                              << m.rotation[3] << "\n";
            std::cout << INDENT INDENT << "S " << m.scale[0] << ", "
                              << m.scale[1] << ", "
                              << m.scale[2] << "\n";
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
                std::cout << INDENT << "   Channel: " <<  i++
                          << "  Node: " <<  c.target.node
                          << "  Sampler: " <<  c.sampler
                          << "  Path: " <<  to_string(c.target.path) << std::endl;
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
                          << INDENT INDENT << "Time Interval     : " << s.getInputSpan().front() << ", " << s.getInputSpan().back()
                          << INDENT INDENT << "Interpolation : " << to_string(s.interpolation)<< ", " << s.getInputSpan().back() << std::endl;

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

    return 42;
}

