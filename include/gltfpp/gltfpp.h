#include<nlohmann/json.hpp>
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

## To Do:
* [`accessor`](#reference-accessor)
   * [`sparse`](#reference-sparse)
      * [`indices`](#reference-indices)
      * [`values`](#reference-values)
* [`animation`](#reference-animation)
   * [`animation sampler`](#reference-animation-sampler)
   * [`channel`](#reference-channel)
      * [`target`](#reference-target)
* [`image`](#reference-image)
* [`material`](#reference-material)
   * [`normalTextureInfo`](#reference-normaltextureinfo)
   * [`occlusionTextureInfo`](#reference-occlusiontextureinfo)
   * [`pbrMetallicRoughness`](#reference-pbrmetallicroughness)

* [`sampler`](#reference-sampler)

* [`skin`](#reference-skin)
* [`texture`](#reference-texture)
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

class Buffer
{
    public:
        int32_t              byteLength;
        std::vector<uint8_t> m_data;
};

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
    std::vector<int32_t> children;
    int32_t camera=-1;
    int32_t skin=-1;

    std::array<float,16> matrix     = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    std::array<float,3>  scale       = {1,1,1};
    std::array<float,4>  rotation    = {0,0,0,1};
    std::array<float,3>  translation = {0,0,0};

    int32_t mesh=-1;

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

    // attributes	object	A dictionary object, where each key corresponds to mesh attribute semantic and each value is the index of the accessor containing attribute's data.	white_check_mark Yes
    // indices	integer	The index of the accessor that contains the indices.	No
    // material	integer	The index of the material to apply to this primitive when rendering.	No
    // mode	integer	The type of primitives to render.	No, default: 4
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
                std::cout << b.dump(4) << std::endl;
                auto B = b.get<Accessor>();

                accessors.emplace_back( std::move(B) );
                accessors.back()._parent = this;
            }
        }

        if(J.count("nodes") == 1)
        {
            for(auto & b : J["nodes"] )
            {
                std::cout << b.dump(4) << std::endl;
                auto B = b.get<Node>();

                nodes.emplace_back( std::move(B) );
                nodes.back()._parent = this;
            }
        }

        if(J.count("meshes") == 1)
        {
            for(auto & b : J["meshes"] )
            {
                std::cout << b.dump(4) << std::endl;
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
                std::cout << b.dump(4) << std::endl;
                auto B = b.get<Scene>();

                scenes.emplace_back( std::move(B) );
                scenes.back()._parent = this;
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
};


inline Node* Node::getChild(int32_t childIndex)
{
    return &_parent->nodes[ children[childIndex] ];
}

inline Accessor& Primitive::getAccessor(PrimitiveAttribute attr)
{
    const auto acessorIndex = (&attributes.POSITION)[ static_cast<int32_t>(attr)];
    return _parent->accessors[acessorIndex];
}

inline Node* Scene::getRootNode(int32_t rootSceneNode)
{
    return &_parent->nodes[ nodes[rootSceneNode] ];
}


}
