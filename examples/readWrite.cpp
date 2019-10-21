#include <ugltf/ugltf.h>


// THis is not included with ugltf, but was
// downloaded by the CMakeLists.txt file specifically for building
// this app
#define STB_IMAGE_IMPLEMENTATION
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

    if( argc >= 3)
    {
        uGLTF::GLTFModel M_in;

        //std::ifstream in( argv[1] );
        std::ifstream in(argv[1] );



        M_in.load(in);


        std::ofstream fout(argv[2]);
        M_in.writeGLB( fout);

        return 0;
    }

    return  1;
}

