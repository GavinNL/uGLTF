#ifndef GLTFPP_H
#define GLTFPP_H

#ifndef JSON_INCLUDE
#define JSON_INCLUDE <nlohmann/json.hpp>
#endif

#include JSON_INCLUDE
#include <iostream>

/*


## Done
* [`asset`](#reference-asset)
* [`buffer`](#reference-buffer)
* [`bufferView`](#reference-bufferview)
* [`mesh`](#reference-mesh)
   * [`primitive`](#reference-primitive)
* [`node`](#reference-node)
* [`scene`](#reference-scene)
* [`animation`](#reference-animation)
   * [`animation sampler`](#reference-animation-sampler)
   * [`channel`](#reference-channel)
      * [`target`](#reference-target)
* [`image`](#reference-image)
* [`sampler`](#reference-sampler)
* [`texture`](#reference-texture)
* [`skin`](#reference-skin)
*
## To Do:
* [`accessor`](#reference-accessor)
   * [`sparse`](#reference-sparse)
      * [`indices`](#reference-indices)
      * [`values`](#reference-values)
* [`material`](#reference-material)
   * [`normalTextureInfo`](#reference-normaltextureinfo)
   * [`occlusionTextureInfo`](#reference-occlusiontextureinfo)
   * [`pbrMetallicRoughness`](#reference-pbrmetallicroughness)

* [`textureInfo`](#reference-textureinfo)
* [`camera`](#reference-camera)
   * [`orthographic`](#reference-orthographic)
   * [`perspective`](#reference-perspective)
   *
* [`extension`](#reference-extension)
* [`extras`](#reference-extras)
* [`glTF`](#reference-gltf) (root object)



*/

namespace gltfpp
{

class GLTFModel;


template<typename T>
class aspan
{
public:
    using value_type = T;

    aspan(void * data, size_t size, size_t stride) : _begin( static_cast<unsigned char*>(data) ),
        _size(size), _stride(stride)
    {

    }

    value_type & operator[](size_t i) const
    {
        return *reinterpret_cast<value_type*>(_begin + _stride*i);
    }
    value_type & operator[](size_t i)
    {
        return *reinterpret_cast<value_type*>(_begin + _stride*i);
    }

    value_type* begin()
    {
        return reinterpret_cast<value_type*>(_begin);
    }

    value_type* end()
    {
        return &this->operator[](_size);
    }

    value_type & back()
    {
        return this->operator[](_size-1);
    }
    value_type const & back() const
    {
        return this->operator[](_size-1);
    }
    value_type & front()
    {
        return this->operator[](0);
    }
    value_type const & front() const
    {
        return this->operator[](0);
    }

    size_t size() const
    {
        return _size;
    }

    size_t stride() const
    {
        return _stride;
    }

public:
    unsigned char* _begin;
    size_t         _size;
    size_t         _stride;
};

template<typename T>
inline T _getValue(nlohmann::json const & obj, const std::string & key, T const &default_val)
{
    try {
        auto it = obj.find(key);
        if( it != obj.end() )
        {
            return it->get<T>();
        }
    } catch (...) {
        return default_val;
    }
    return default_val;
}

class Buffer
{
    public:
        int32_t              byteLength;
        std::vector<uint8_t> m_data;
};

enum class BufferViewTarget : int32_t
{
    UNKNOWN              = 0,
    ARRAY_BUFFER         = 34962,
    ELEMENT_ARRAY_BUFFER = 34963
};

class BufferView
{
public:
    int32_t          buffer    ;
    int32_t          byteLength = 0;
    int32_t          byteOffset = 0;
    int32_t          byteStride = 0;
    BufferViewTarget target;

    /**
     * @brief getBuffer
     * @return
     *
     * Returns a reference to the buffer.
     * The buffer is just a vector of raw uint8
     */
    Buffer & getBuffer();
    Buffer const & getBuffer() const;

    template<typename T>
    aspan<T> getSpan();

    void* data();

private:
    GLTFModel * _parent;
    friend class GLTFModel;
};

inline void from_json(const nlohmann::json & j, BufferView & B)
{
    B.buffer     = _getValue(j, "buffer"    , 0u);
    B.target     = static_cast<BufferViewTarget>(_getValue(j, "target"    , 0u));
    B.byteLength = _getValue(j, "byteLength", 0u);
    B.byteOffset = _getValue(j, "byteOffset", 0u);
    B.byteStride = _getValue(j, "byteStride", 0u);
}



enum class AccessorType : int32_t
{
    UNKNOWN=0,
    SCALAR,
    VEC2,
    VEC3,
    VEC4,
    MAT2,
    MAT3,
    MAT4
};

enum class ComponentType : int32_t
{
    BYTE            = 5120,
    UNSIGNED_BYTE   = 5121,
    SHORT           = 5122,
    UNSIGNED_SHORT  = 5123,
    UNSIGNED_INT    = 5125,
    FLOAT           = 5126,
    DOUBLE          = 5130
};

class Accessor
{
    public:
        int32_t       bufferView;
        int32_t       bufferOffset;
        ComponentType componentType;
        bool          normalized;
        int32_t       count;
        AccessorType  type;

        std::vector<double> min;
        std::vector<double> max;

        std::string  name;

        template<typename T>
        aspan<T> getSpan();



        size_t componentSize() const
        {
            switch(componentType)
            {
                case ComponentType::BYTE          : return 1;
                case ComponentType::UNSIGNED_BYTE : return 1;
                case ComponentType::SHORT         : return 2;
                case ComponentType::UNSIGNED_SHORT: return 2;
                case ComponentType::UNSIGNED_INT  : return 4;
                case ComponentType::FLOAT         : return 4;
                case ComponentType::DOUBLE        : return 8;
            }
        }
        /**
         * @brief accessorSize
         * @return
         *
         * Returns the size of the datatype stored in this accessor.. for example
         * if this accessor is storing a 3-component vector of shorts, it will return
         * 3*sizeof(short) = 6;
         */
        size_t accessorSize() const
        {
            auto actualDataSize = componentSize();
            switch(type)
            {
                case AccessorType::UNKNOWN: return actualDataSize*0;
                case AccessorType::SCALAR : return actualDataSize*1;
                case AccessorType::VEC2   : return actualDataSize*2;
                case AccessorType::VEC3   : return actualDataSize*3;
                case AccessorType::VEC4   : return actualDataSize*4;
                case AccessorType::MAT2   : return actualDataSize*4;
                case AccessorType::MAT3   : return actualDataSize*9;
                case AccessorType::MAT4   : return actualDataSize*16;
            }
        }

    private:
        GLTFModel * _parent;
        friend class GLTFModel;
};

inline void from_json(const nlohmann::json & j, Accessor & B)
{
    B.bufferView     = _getValue(j, "bufferView"   , 0);
    B.bufferOffset   = _getValue(j, "bufferOffset" , 0);
    B.componentType  = static_cast<ComponentType>(_getValue(j, "componentType", 0));
    B.normalized     = _getValue(j, "normalized"   , false);
    B.count          = _getValue(j, "count"   , 0);

    B.name           = _getValue(j, "name", std::string(""));
    B.min            = _getValue(j, "min", std::vector<double>());
    B.max            = _getValue(j, "max", std::vector<double>());
    auto type        = _getValue(j, "type", std::string("UNKNOWN"));

    if( type == "SCALAR") B.type = AccessorType::SCALAR;
    if( type == "VEC2")   B.type = AccessorType::VEC2;
    if( type == "VEC3")   B.type = AccessorType::VEC3;
    if( type == "VEC4")   B.type = AccessorType::VEC4;
    if( type == "MAT2")   B.type = AccessorType::MAT2;
    if( type == "MAT3")   B.type = AccessorType::MAT3;
    if( type == "MAT4")   B.type = AccessorType::MAT4;

}
struct Transform
{
    std::array<float,3> scale       = {1,1,1};
    std::array<float,4> rotation    = {0,0,0,1};
    std::array<float,3> translation = {0,0,0};
};

class Node
{
public:
    int32_t mesh=-1;
    int32_t camera=-1;
    int32_t skin=-1;

    std::array<float,16> matrix      = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    std::array<float,3>  scale       = {1,1,1};
    std::array<float,4>  rotation    = {0,0,0,1};
    std::array<float,3>  translation = {0,0,0};

    std::vector<int32_t> children;



    std::vector<float> weights;
    std::string        name;

    /**
     * @brief hasMatrix
     * @return
     * If this returns true, you should use the Node.matrix as your transform,
     * otherwise use the .scale, .rotation and .translation
     */
    bool hasMatrix() const
    {
        return _hasMatrix;
    }
    bool hasTransforms() const
    {
        return _hasTransforms;
    }
    bool hasMesh() const
    {
        return mesh != -1;
    }
    bool hasCamera() const
    {
        return camera != -1;
    }
    bool hasSkin() const
    {
        return skin != -1;
    }

    size_t childCount() const
    {
        return children.size();
    }
    /**
     * @brief getChild
     * @param childIndex
     * @return
     *
     * Returns a pointer to the child node.
     * Requirement: childIndex < children.size()
     */
    Node* getChild(int32_t childIndex);


    template<typename Callable_t>
    void depthFirstTraverse(Callable_t && C)
    {
        C(*this);
        int32_t childIndex=0;
        for(auto & i : children)
        {
            auto * N = getChild(childIndex++);
            N->depthFirstTraverse(C);
        }
    }

    template<typename Callable_t>
    void descendTree(Callable_t && C)
    {
        C(*this);
        int32_t childIndex=0;
        for(auto & i : children)
        {
            auto * N = getChild(childIndex++);
            N->descendTree(C);
        }
    }
private:
    bool _hasMatrix=false;
    bool _hasTransforms=false;
    GLTFModel * _parent;
    friend void from_json(const nlohmann::json & j, Node & B);
    friend class GLTFModel;
};

inline void from_json(const nlohmann::json & j, Node & B)
{
    B.camera     = _getValue(j, "camera"   , -1);
    B.skin       = _getValue(j, "skin" , -1 );
    B.mesh       = _getValue(j, "mesh"  , -1 );

    B.matrix      =     _getValue(j, "matrix", std::array<float,16>{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1});
    B.scale       =     _getValue(j, "scale", std::array<float,3>{1,1,1});
    B.rotation    =     _getValue(j, "rotation", std::array<float,4>{0,0,0,1});
    B.translation =     _getValue(j, "translation", std::array<float,3>{0,0,0});

    if( j.count("matrix") == 1)
        B._hasMatrix = true;

    if( j.count("rotation") == 1)
        B._hasTransforms = true;
    if( j.count("scale") == 1)
        B._hasTransforms = true;
    if( j.count("translation") == 1)
        B._hasTransforms = true;

    B.name           = _getValue(j, "name", std::string(""));
    B.weights        = _getValue(j, "weights", std::vector<float>());
    B.children       = _getValue(j, "children", std::vector<int32_t>());
}

enum class PrimitiveAttribute
{
    POSITION   = 0,
    NORMAL	   = 1,
    TANGENT	   = 2,
    TEXCOORD_0 = 3,
    TEXCOORD_1 = 4,
    COLOR_0	   = 5,
    JOINTS_0   = 6,
    WEIGHTS_0  = 7
};

enum class PrimitiveMode : int32_t
{
    POINTS             = 0,
    LINES              = 1,
    LINE_LOOP          = 2,
    LINE_STRIP         = 3,
    TRIANGLES          = 4,
    TRIANGLE_STRIP     = 5,
    TRIANGLE_FAN       = 6
};

class Primitive
{
public:
    struct
    {
        // node these must be in this order otherwise the has() method wont work
        int32_t POSITION  = -1;
        int32_t NORMAL	  = -1;
        int32_t TANGENT	  = -1;
        int32_t TEXCOORD_0= -1;
        int32_t TEXCOORD_1= -1;
        int32_t COLOR_0	  = -1;
        int32_t JOINTS_0  = -1;
        int32_t WEIGHTS_0 = -1;
    } attributes;

    int32_t       indices = -1;
    PrimitiveMode mode = PrimitiveMode::TRIANGLES;
    int32_t       material;

    bool has(PrimitiveAttribute attr) const
    {
        return -1 != (&attributes.POSITION)[ static_cast<int32_t>(attr)];
    }
    bool hasIndices() const
    {
        return indices!=-1;
    }

    Accessor& getAccessor(PrimitiveAttribute attr);
    Accessor const & getAccessor(PrimitiveAttribute attr) const;

    Accessor& getIndexAccessor();
    Accessor const & getIndexAccessor() const;

    template<typename T>
    aspan<T> getSpan(PrimitiveAttribute attr)
    {
        auto & A = getAccessor(attr);

        return A.getSpan<T>();
    }

    // targets	object [1-*]	An array of Morph Targets, each Morph Target is a dictionary mapping attributes (only POSITION, NORMAL, and TANGENT supported) to their deviations in the Morph Target.	No
    // extensions	object	Dictionary object with extension-specific objects.	No
    // extras	any	Application-specific data.	No
private:
    GLTFModel * _parent;
    friend class GLTFModel;
};

inline void from_json(const nlohmann::json & j, Primitive & B)
{
    B.attributes.POSITION = _getValue(j["attributes"], "POSITION", -1);
    B.attributes.NORMAL = _getValue(j["attributes"], "NORMAL", -1);
    B.attributes.TANGENT = _getValue(j["attributes"], "TANGENT", -1);
    B.attributes.TEXCOORD_0 = _getValue(j["attributes"], "TEXCOORD_0", -1);
    B.attributes.TEXCOORD_1 = _getValue(j["attributes"], "TEXCOORD_1", -1);
    B.attributes.COLOR_0 = _getValue(j["attributes"], "COLOR_0", -1);
    B.attributes.JOINTS_0 = _getValue(j["attributes"], "JOINTS_0", -1);
    B.attributes.WEIGHTS_0 = _getValue(j["attributes"], "WEIGHTS_0", -1);

    B.indices = _getValue(j, "indices", -1);
    B.material = _getValue(j, "material", -1);

    B.mode = static_cast<PrimitiveMode>( _getValue(j, "mode", 4) );

}


class Mesh
{
public:
    std::vector<Primitive> primitives;
    std::vector<float>     weights;
    std::string name;

   // primitives	primitive [1-*]	An array of primitives, each defining geometry to be rendered with a material.	white_check_mark Yes
    //weights	number [1-*]	Array of weights to be applied to the Morph Targets.	No

//    extensions	object	Dictionary object with extension-specific objects.	No
//    extras	any	Application-specific data.	No
private:
    GLTFModel * _parent;
    friend class GLTFModel;
};

inline void from_json(const nlohmann::json & j, Mesh & B)
{
    B.name           = _getValue(j, "name", std::string(""));
    B.primitives     = _getValue(j, "primitives"   , std::vector<Primitive>() );

    B.weights        = _getValue(j, "weights", std::vector<float>());
}


class Scene
{
public:
    std::vector<int32_t> nodes;	//integer [1-*]	The indices of each root node.	No
    std::string name;          //The user-defined name of this object.	No
    //extensions	object	Dictionary object with extension-specific objects.	No
    //extras	any	Application-specific data.	No

    Node* getRootNode(int32_t rootSceneNode);
private:
    GLTFModel * _parent;
    friend class GLTFModel;
};

inline void from_json(const nlohmann::json & j, Scene & B)
{
    B.name           = _getValue(j, "name", std::string(""));
    //B.primitives     = _getValue(j, "primitives"   , std::vector<Primitive>() );

    B.nodes        = _getValue(j, "nodes", std::vector<int32_t>());
}

class Skin
{
public:
    int32_t              inverseBindMatrices = -1;
    std::vector<int32_t> joints;	//integer [1-*]	The indices of each root node.	No
    std::string          name;          //The user-defined name of this object.	No
    int32_t              skeleton=-1;
    //extensions	object	Dictionary object with extension-specific objects.	No
    //extras	any	Application-specific data.	No

    Node* getRootNode();
private:
    GLTFModel * _parent;
    friend class GLTFModel;
};

inline void from_json(const nlohmann::json & j, Skin & B)
{
    B.inverseBindMatrices = _getValue(j, "inverseBindMatrices", -1);
    B.name                = _getValue(j, "name", std::string(""));
    B.joints              = _getValue(j, "joints", std::vector<int32_t>());
    B.skeleton            = _getValue(j, "skeleton", -1);
    //B.primitives      = _getValue(j, "primitives"   , std::vector<Primitive>() );

}


enum class AnimationInterpolation : int32_t
{
    STEP,
    LINEAR,
    CUBIC
};

enum class AnimationPath
{
    TRANSLATE,
    ROTATION,
    SCALE,
    WEIGHTS
};

/**
 * @brief The AnimationSampler class
 *
 * An animation sampler is basically two arrays,
 *
 * a time array and a value array.
 *
 * The time/value arrays describe a linear spline
 * to interpolate the value.
 *
 *
 * t = [                         ]
 * v = [                         ]
 */
class AnimationSampler
{
public:
    int32_t                input  = -1;
    int32_t                output = -1;
    AnimationInterpolation interpolation = AnimationInterpolation::LINEAR;

    /**
     * @brief getInputData
     * @return
     *
     * Returns the span of data for the input (eg time)
     * This value is always a floating point balue
     */
    aspan<float> getInputSpan()
    {
        return getInputAccessor().getSpan<float>();
    }

    template<typename T>
    aspan<T> getOutputSpan()
    {
        return getOutputAccessor().getSpan<T>();
    }

    Accessor& getInputAccessor();
    Accessor& getOutputAccessor();

private:
    GLTFModel * _parent;
    friend class GLTFModel;
};

inline void from_json(const nlohmann::json & j, AnimationSampler & B)
{
    B.input = _getValue(j, "input", -1);
    B.output = _getValue(j, "output", -1);

    auto interpolation = _getValue( j, "interpolation", std::string() );

    if( interpolation == "STEP")   B.interpolation = AnimationInterpolation::STEP;
    if( interpolation == "LINEAR") B.interpolation = AnimationInterpolation::LINEAR;
    if( interpolation == "CUBIC")  B.interpolation = AnimationInterpolation::CUBIC;
}



class AnimationChannel
{
public:
    int32_t              sampler = -1;

    struct
    {
        int32_t       node;
        AnimationPath path;
    } target;

private:
    GLTFModel * _parent;
    friend class GLTFModel;
};

inline void from_json(const nlohmann::json & j, AnimationChannel & B)
{
    B.sampler = _getValue(j, "sampler", -1);

    auto path = _getValue(j["target"], "path", std::string() );

    if( path == "translation") B.target.path = AnimationPath::TRANSLATE;
    if( path == "rotation")    B.target.path = AnimationPath::ROTATION;
    if( path == "scale")       B.target.path = AnimationPath::SCALE;
    if( path == "weights")     B.target.path = AnimationPath::WEIGHTS;

    B.target.node = _getValue(j["target"], "node", -1);
    //B.primitives      = _getValue(j, "primitives"   , std::vector<Primitive>() );

}

class Animation
{
public:
    std::vector<AnimationChannel> channels;
    std::vector<AnimationSampler> samplers;
    std::string                   name;          //The user-defined name of this object.	No


    /**
     * @brief getFrame
     * @param nodeTransformations
     * @param t
     *
     * This method will fill the nodeTransformations vector with
     * the transformation matrices for this animation at time t.
     *
     * nodeTransformations.size() = 16 * GLTF.nodes.size()
     *
     * Note that not all nodes may be animated.
     */
    void getFrame( std::vector<float> & nodeTransformations, float t );
private:
    GLTFModel * _parent;
    friend class GLTFModel;

};

inline void from_json(const nlohmann::json & j, Animation & B)
{
    B.channels            = _getValue(j, "channels", std::vector<AnimationChannel>());
    B.samplers            = _getValue(j, "samplers", std::vector<AnimationSampler>());
    B.name                = _getValue(j, "name", std::string(""));
}


class Image
{
public:
    // mimeType	string	The image's MIME type.	No
    // bufferView	integer	The index of the bufferView that contains the image. Use this instead of the image's uri property.	No

    std::string uri;
    std::string mimeType;
    int32_t     bufferView;
    std::string name;

    aspan<uint8_t> data();

    BufferView       & getBufferView();
    BufferView const & getBufferView() const;
private:
    GLTFModel * _parent;
    friend class GLTFModel;

};

inline void from_json(const nlohmann::json & j, Image & B)
{
    B.uri         = _getValue(j, "uri", std::string(""));
    B.mimeType    = _getValue(j, "mimeType", std::string(""));
    B.bufferView  = _getValue(j, "bufferView", -1);
    B.name        = _getValue(j, "name", std::string(""));
}

class Texture
{
public:
    int32_t     sampler;
    int32_t     source;
    std::string name;

private:
    GLTFModel * _parent;
    friend class GLTFModel;
};

inline void from_json(const nlohmann::json & j, Texture & B)
{
    B.source  = _getValue(j, "source", -1);
    B.sampler  = _getValue(j, "sampler", -1);
    B.name        = _getValue(j, "name", std::string(""));
}

enum class Filter
{
    UNKNOWN                = -1,
    NEAREST                =  9728,
    LINEAR                 =  9729,
    NEAREST_MIPMAP_NEAREST =  9984,
    LINEAR_MIPMAP_NEAREST  =  9985,
    NEAREST_MIPMAP_LINEAR  =  9986,
    LINEAR_MIPMAP_LINEAR   =  9987
};

enum class WrapMode
{
    CLAMP_TO_EDGE   = 33071,
    MIRRORED_REPEAT = 33648,
    REPEAT          = 10497
};

class Sampler
{
public:
    Filter magFilter;
    Filter minFilter;
    WrapMode wrapS;
    WrapMode wrapT;
    std::string name;

private:
    GLTFModel * _parent;
    friend class GLTFModel;
};

inline void from_json(const nlohmann::json & j, Sampler & B)
{
    B.magFilter  = static_cast<Filter>(_getValue(j, "magFilter", -1) );
    B.minFilter  = static_cast<Filter>(_getValue(j, "minFilter", -1) );
    B.wrapS      = static_cast<WrapMode>(_getValue(j, "wrapS", 10497));
    B.wrapT      = static_cast<WrapMode>(_getValue(j, "wrapT", 10497));
    B.name       = _getValue(j, "name", std::string(""));
}



class GLTFModel
{
public:
    struct header_t
    {
        uint32_t magic;
        uint32_t version;
        uint32_t length;
    };

    struct chunk_t
    {
        uint32_t             chunkLength;
        uint32_t             chunkType;
        std::vector<uint8_t> chunkData;
    };

    void load( std::istream & i)
    {
        auto header = _readHeader(i);
        auto jsonChunk = _readChunk(i);

        jsonChunk.chunkData.push_back(0);
        auto J = _parseJson(  reinterpret_cast<char*>(jsonChunk.chunkData.data()) );

        if(J.count("buffers") == 1)
        {
            buffers = _readBuffers(i, J["buffers"]);
        }


        if(J.count("bufferViews") == 1)
        {
            for(auto & b : J["bufferViews"] )
            {
                auto B = b.get<BufferView>();

                bufferViews.emplace_back( std::move(B) );
                bufferViews.back()._parent = this;
            }
        }

        if(J.count("accessors") == 1)
        {
            for(auto & b : J["accessors"] )
            {
              //  std::cout << b.dump(4) << std::endl;
                auto B = b.get<Accessor>();

                accessors.emplace_back( std::move(B) );
                accessors.back()._parent = this;
            }
        }

        if(J.count("nodes") == 1)
        {
            for(auto & b : J["nodes"] )
            {
                //std::cout << b.dump(4) << std::endl;
                auto B = b.get<Node>();

                nodes.emplace_back( std::move(B) );
                nodes.back()._parent = this;
            }
        }

        if(J.count("meshes") == 1)
        {
            for(auto & b : J["meshes"] )
            {
              //  std::cout << b.dump(4) << std::endl;
                auto B = b.get<Mesh>();

                meshes.emplace_back( std::move(B) );
                meshes.back()._parent = this;

                for(auto & p : meshes.back().primitives )
                {
                    p._parent = this;
                }
            }
        }

        if(J.count("scenes") == 1)
        {
            for(auto & b : J["scenes"] )
            {
               // std::cout << b.dump(4) << std::endl;
                auto B = b.get<Scene>();

                scenes.emplace_back( std::move(B) );
                scenes.back()._parent = this;
            }
        }

        if(J.count("skins") == 1)
        {
            for(auto & b : J["skins"] )
            {
              //  std::cout << b.dump(4) << std::endl;
                auto B = b.get<Skin>();
               //
                skins.emplace_back( std::move(B) );
                skins.back()._parent = this;
            }
        }

        if(J.count("animations") == 1)
        {
            for(auto & b : J["animations"] )
            {
               // std::cout << b.dump(4) << std::endl;
                auto B = b.get<Animation>();
               //
                animations.emplace_back( std::move(B) );
                animations.back()._parent = this;

                for(auto & s : animations.back().channels)
                    s._parent = this;
                for(auto & s : animations.back().samplers)
                    s._parent = this;
            }
        }
        if(J.count("images") == 1)
        {
            for(auto & b : J["images"] )
            {
               // std::cout << b.dump(4) << std::endl;
                auto B = b.get<Image>();
               //
                images.emplace_back( std::move(B) );
                images.back()._parent = this;
            }
        }
        if(J.count("textures") == 1)
        {
            for(auto & b : J["textures"] )
            {
               // std::cout << b.dump(4) << std::endl;
                auto B = b.get<Texture>();
               //
                textures.emplace_back( std::move(B) );
                textures.back()._parent = this;
            }
        }
        if(J.count("samplers") == 1)
        {
            for(auto & b : J["samplers"] )
            {
               // std::cout << b.dump(4) << std::endl;
                auto B = b.get<Sampler>();
               //
                samplers.emplace_back( std::move(B) );
                samplers.back()._parent = this;
            }
        }
    }

    static header_t _readHeader(std::istream & in)
    {
        header_t h{0,0,0};

        if(in.read(reinterpret_cast<char*>(&h), sizeof h))
        {
            return h;
        }
        return h;
    }

    static chunk_t _readChunk(std::istream & in)
    {
        chunk_t h{0,0, {} };

        if(in.read(reinterpret_cast<char*>(&h), sizeof(uint32_t)*2))
        {
            if( h.chunkType == 0x4E4F534A || h.chunkType == 0x004E4942)
            {
                h.chunkData.resize(h.chunkLength);
                in.read( reinterpret_cast<char*>(&h.chunkData[0]), h.chunkLength);
            }
            return h;
        }
        return h;
    }

    /**
     * @brief _parseJson
     * @param data
     * @return
     *
     * Parse the raw json string data into a Json object.
     */
    static nlohmann::json _parseJson( char * data)
    {
        nlohmann::json J;

        return J.parse(data);
    }

    /**
     * @brief _extractBuffers
     * @param buffersChunk
     * @param jBuffers
     *
     * Extract each of the buffers from the buffersDataChunk and add them
     * to the buffers array.
     */
    static std::vector<Buffer> _readBuffers(std::istream & in, nlohmann::json const & jBuffers)
    {
        std::vector<Buffer> outputBuffers;

        uint32_t offset=0;

        chunk_t h;
        if(in.read(reinterpret_cast<char*>(&h), sizeof(uint32_t)*2))
        {
            if( h.chunkType == 0x4E4F534A || h.chunkType == 0x004E4942)
            {

                for(auto & b : jBuffers)
                {
                    Buffer B;
                    B.m_data.resize( b["byteLength"].get<uint32_t>() );
                    B.byteLength = b["byteLength"].get<int32_t>();
                    in.read( reinterpret_cast<char*>(B.m_data.data()), B.m_data.size());

                    outputBuffers.emplace_back( std::move(B) );

                    offset += B.m_data.size();
                }

            }
        }


        return outputBuffers;
    }

    /**
     * @brief _extractBuffers
     * @param buffersChunk
     * @param jBuffers
     *
     * Extract each of the buffers from the buffersDataChunk and add them
     * to the buffers array.
     */
    static std::vector<Buffer> _extractBuffers(chunk_t const & buffersChunk, nlohmann::json const & jBuffers)
    {
        std::vector<Buffer> outputBuffers;

        uint32_t offset=0;

        for(auto & b : jBuffers)
        {
            Buffer B;
            B.m_data.resize( b["byteLength"].get<uint32_t>() );

            memcpy( B.m_data.data(), buffersChunk.chunkData.data()+offset, B.m_data.size());

            outputBuffers.emplace_back( std::move(B) );

            offset += B.m_data.size();
        }

        return outputBuffers;
    }




public:
    std::vector<Accessor>   accessors;
    std::vector<Buffer>     buffers;
    std::vector<BufferView> bufferViews;
    std::vector<Node>       nodes;
    std::vector<Mesh>       meshes;
    std::vector<Scene>      scenes;
    std::vector<Skin>       skins;
    std::vector<Animation>  animations;
    std::vector<Image>      images;
    std::vector<Texture>    textures;
    std::vector<Sampler>    samplers;
};


inline Node* Node::getChild(int32_t childIndex)
{
    return &_parent->nodes[ children[childIndex] ];
}

Accessor& Primitive::getIndexAccessor()
{
    if( hasIndices() )
    {
        return _parent->accessors[ indices ];
    }
    throw std::runtime_error("This primitive does not have an index buffer");
}

Accessor const & Primitive::getIndexAccessor() const
{
    if( hasIndices() )
    {
        return _parent->accessors[ indices ];
    }
    throw std::runtime_error("This primitive does not have an index buffer");
}

inline Accessor& Primitive::getAccessor(PrimitiveAttribute attr)
{
    const auto acessorIndex = (&attributes.POSITION)[ static_cast<int32_t>(attr)];
    return _parent->accessors[acessorIndex];
}

Accessor const & Primitive::getAccessor(PrimitiveAttribute attr) const
{
    const auto acessorIndex = (&attributes.POSITION)[ static_cast<int32_t>(attr)];
    return _parent->accessors[acessorIndex];
}

inline Node* Scene::getRootNode(int32_t rootSceneNode)
{
    return &_parent->nodes[ nodes[rootSceneNode] ];
}

inline Accessor& AnimationSampler::getInputAccessor()
{
    return _parent->accessors[input];
}

inline Accessor& AnimationSampler::getOutputAccessor()
{
    return _parent->accessors[output];
}


Buffer &BufferView::getBuffer()
{
    return _parent->buffers[ buffer ];
}

Buffer const & BufferView::getBuffer() const
{
    return _parent->buffers[ buffer ];
}

void* BufferView::data()
{
    return _parent->buffers[ buffer ].m_data.data() + byteOffset;
}

template<typename T>
aspan<T> Accessor::getSpan()
{
    auto & bv = _parent->bufferViews[ bufferView ];

    auto stride = accessorSize();

    if( stride < sizeof(T) )
    {
        throw std::runtime_error( std::string("The stride listed in the accessor, ") + std::to_string(stride) + ", is less size of the template parameter, " + std::to_string( sizeof(T)) + ". Your data will overlap");
    }

    return
    aspan<T>( bv.data(),
              count,
              stride);
}



template<typename T>
aspan<T> BufferView::getSpan()
{
    auto d  =  _parent->buffers[ buffer ].m_data.data() + byteOffset;

    auto stride = sizeof(T);

    // byteStride not given, so data is tightly packed
    if( byteStride <= 0) // the stride is defined so its likely an attribute byffer
    {
        stride = sizeof(T);
    }
    else
    {
        if( static_cast<size_t>(byteStride) < sizeof(T) )
        {
            // warning: the defined byteStride is less than the requested
            //          stride, consequitive data will overlap.
        }
    }

    size_t count = byteLength / stride;
    return aspan<T>(d, count, stride);
}

aspan<uint8_t> Image::data()
{
    return _parent->bufferViews[ bufferView ].getSpan<uint8_t>();
}

BufferView       & Image::getBufferView()
{
    return _parent->bufferViews[ bufferView ];
}

BufferView const & Image::getBufferView() const
{
    return _parent->bufferViews[ bufferView ];
}

}

#endif
