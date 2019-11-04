#include <ugltf/ugltf.h>

#include <fstream>

/**
 * @brief copyAnimations2
 * @param src
 * @param dst
 * @return
 *
 * Copies all the animations from src to dst. The dst buffer will be increased
 * and three new bufferViews will be created, one for the input accessors, float (time), and
 * two for the output accessors, vec3 (scale/translation) and vec4 (rotation).
 */
int copyAnimations2(uGLTF::GLTFModel const & src, uGLTF::GLTFModel & dst )
{
    TRACE("Buffers Before: {} ", dst.buffers.size() );
    TRACE("BufferViews Before: {} ", dst.bufferViews.size() );
    TRACE("Accessors Before: {} ", dst.accessors.size() );

    // create a new buffer per animation. This is where
    // all the accessors will be stored.
    if( dst.buffers.size() ==0)
        dst.newBuffer();

    // Loop through all the animations in the source model.
    for(auto & a : src.animations)
    {
        TRACE("====================");
        TRACE("  Copying Animation: {}", a.name);
        TRACE("    - Channels: {}", a.channels.size() );
        TRACE("    - Samplers: {}", a.samplers.size() );

        // create a new buffer per animation. This is where
        // all the accessors will be stored.
        uGLTF::Buffer & newBuffer = dst.buffers.front();// dst.newBuffer();

        // create the new animation in the destination model
        uGLTF::Animation & dstAnimation = dst.newAnimation();

        {
            dstAnimation.name = a.name;

            // Loop  through all the samplers and count the total number of
            // of vec3 elements, quat elements and float elements.
            // We will need this to create 3 buffer views, one for each type (time, translate/scale, rotation)
            uint32_t inputCount=0;
            uint32_t vec3Count=0;
            uint32_t vec4Count=0;

            for(auto & srcSampler : a.samplers)
            {
                auto & input  = srcSampler.getInputAccessor();
                inputCount += input.count;

                auto & output = srcSampler.getOutputAccessor();
                TRACE("Sampler Type: {} --> {} ", to_string(input.type), to_string(output.type));

                if( output.type == uGLTF::AccessorType::VEC4 )
                {
                    vec4Count += output.count;
                }
                else if(output.type == uGLTF::AccessorType::VEC3)
                {
                    vec3Count += output.count;
                }
                else
                {
                    throw std::runtime_error("Error, accessor type is incorrect");
                }
            }

            TRACE("Input Count: {}", inputCount);
            TRACE("vec3Count : {}", vec3Count);
            TRACE("vec4Count : {}", vec4Count);

            // so there are inputCount * sizeof(float) bytes needed to hold
            // all the input accessor values

            // this buffer view should ahve enough space to hold all the input (time values) accessors
            // for all the samplers.
            auto inputBufferViewIndex = newBuffer.createNewBufferView( inputCount * sizeof(float), uGLTF::BufferViewTarget::UNKNOWN, 4, 1);

            // Ouptut buffers, one for vec3 elements and one for vec4 elements.
            auto outputBufferViewIndexVec3 = newBuffer.createNewBufferView( vec3Count * sizeof(float) * 3, uGLTF::BufferViewTarget::UNKNOWN, 4, 1);
            auto outputBufferViewIndexVec4 = newBuffer.createNewBufferView( vec4Count * sizeof(float) * 4, uGLTF::BufferViewTarget::UNKNOWN, 4, 1);

            TRACE("inputBufferViewIndex : {}", inputBufferViewIndex);
            TRACE("outputBufferViewIndexVec3 : {}", outputBufferViewIndexVec3);
            TRACE("outputBufferViewIndexVec4 : {}", outputBufferViewIndexVec4);


            TRACE("inputBufferView  float size : {}"     ,  dst.bufferViews[inputBufferViewIndex     ].byteLength);
            TRACE("outputBufferView Vec3 size  : {}",  dst.bufferViews[outputBufferViewIndexVec3].byteLength);
            TRACE("outputBufferView Vec4 size  : {}",  dst.bufferViews[outputBufferViewIndexVec4].byteLength);

            uint32_t accessorInputOffset      = 0;
            uint32_t accessorOutputOffsetVec3 = 0;
            uint32_t accessorOutputOffsetVec4 = 0;


            // Now looop through all the samplers
            for(auto & srcSampler : a.samplers)
            {
                auto & dstSampler = dstAnimation.newSampler();

                {
                    auto srcCount     = srcSampler.getInputAccessor().count;
                    auto srcType      = srcSampler.getInputAccessor().type;
                    auto srcCompType  = srcSampler.getInputAccessor().componentType;

                    // Create a new input accessor in the destination model to hold the time values.
                    auto dstInputAccessorIndex  = dst.bufferViews[inputBufferViewIndex].createNewAccessor(accessorInputOffset, srcCount, srcType, srcCompType);
                    accessorInputOffset += srcCount * dst.accessors[dstInputAccessorIndex].accessorSize();

                    // copy the time values from the src accessor into the new accessor.
                    dst.accessors[dstInputAccessorIndex].copyDataFrom( srcSampler.getInputAccessor() );
                    dst.accessors[dstInputAccessorIndex].calculateMinMax();
                    dstSampler.input = dstInputAccessorIndex;

                }

                {
                    auto srcCount     = srcSampler.getOutputAccessor().count;
                    auto srcType      = srcSampler.getOutputAccessor().type;
                    auto srcCompType  = srcSampler.getOutputAccessor().componentType;

                    // determine which bufferView we want to add the data to, the vec3 view or the vec4.
                    auto bufferViewIndex    = srcType == uGLTF::AccessorType::VEC3 ? outputBufferViewIndexVec3 : outputBufferViewIndexVec4;

                    // reference to the byteOffset for the view
                    auto & bufferViewOffset = srcType == uGLTF::AccessorType::VEC3 ? accessorOutputOffsetVec3 : accessorOutputOffsetVec4;

                    // create the new accessor and copy the old data over
                    auto dstOutputAccessorIndex  = dst.bufferViews[bufferViewIndex].createNewAccessor(bufferViewOffset, srcCount, srcType, srcCompType);
                    bufferViewOffset += srcCount * dst.accessors[dstOutputAccessorIndex].accessorSize();

                    dst.accessors[dstOutputAccessorIndex].copyDataFrom( srcSampler.getOutputAccessor() );
                    dst.accessors[dstOutputAccessorIndex].calculateMinMax();
                    dstSampler.output = dstOutputAccessorIndex;
                }

                dstSampler.interpolation = srcSampler.interpolation;
            }
        }

        for(auto & c : a.channels)
        {
            dstAnimation.channels.push_back(c);
        }

    }

    dst._setParents(&dst);

    TRACE("Buffers After: {} ", dst.buffers.size() );
    TRACE("BufferViews After: {} ", dst.bufferViews.size() );
    TRACE("Accessors After: {} ", dst.accessors.size() );
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

    // Copy all animations into a blank glb file.
    // This glb file will be invalid because the animations
    // will be referencing nodes that do not exist
    if( argc == 3)
    {
        uGLTF::GLTFModel M_in;
        uGLTF::GLTFModel M_out;

        std::ifstream in(argv[1] );

        M_in.load(in);

        copyAnimations2(M_in, M_out);

        std::ofstream fout(argv[2]);
        M_out.writeGLB( fout);

        return 0;
    }

    if( argc == 4)
    {
        uGLTF::GLTFModel M_in1;
        uGLTF::GLTFModel M_in2;


        {
            std::ifstream in1(argv[1] );
            M_in1.load(in1);
        }

        {
            std::ifstream in2(argv[2] );
            M_in2.load(in2);
        }

        copyAnimations2(M_in1, M_in2);

        std::ofstream fout(argv[3]);
        M_in2.writeGLB( fout);

        return 0;
    }
    return  1;
}

