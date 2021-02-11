#ifndef UGLTF_2_H
#define UGLTF_2_H

/*=================================================================================

A C++17 implementation of ugltf.

1. Optional keys are represented by std::optional<>
2. You do not have to manage the "buffers"
   - BufferViews now contains a "data" member
   - BufferViews automatically get placed in a Buffer[0] when serialized

=================================================================================*/

#include <nlohmann/json.hpp>
#include <optional>

#ifndef UGLTF_NAMESPACE
    #define UGLTF_NAMESPACE ugltf
    #define UGLTF_JSON_CLASS nlohmann::json
#endif

#ifndef UGLTF_PRIVATE
    #define UGLTF_PRIVATE private
#endif

#ifndef UGLTF_PROTECTED
    #define UGLTF_PROTECTED protected
#endif

namespace UGLTF_NAMESPACE
{
using json = UGLTF_JSON_CLASS;

using AccessorIndex = uint32_t;

template<typename T>
void readKey( json const & Obj, std::string key, T & required)
{
    required = Obj.find(key)->get<T>();
}
template<typename T>
void readKey( json const & Obj, std::string key, T & required, T const & defaultV)
{
    auto it = Obj.find(key);
    if( it == Obj.end() )
    {
        required = defaultV;
    }
    else
    {
        required = it->get<T>();
    }
}

template<typename T>
void readKey( json const & Obj, std::string const& key, std::optional<T> & opt)
{
    auto it = Obj.find(key);
    if( it == Obj.end() )
    {
        opt.reset();
    }
    else
    {
        opt = it->get<T>();
    }
}

template<typename T>
void readKey( json const & Obj, std::string const& key, std::vector<T> & opt)
{
    auto it = Obj.find(key);
    if( it == Obj.end() )
    {
        opt.clear();
    }
    else
    {
        opt = it->get< std::vector<T> >();
    }
}

template<typename T>
void writeKey( json & Obj, std::string const& key, T const & required)
{
    Obj[key] = required;
}

template<typename T>
void writeKey( json & Obj, std::string const& key, std::optional<T> const & opt)
{
    if( opt.has_value() )
    {
        Obj[key] = *opt;
    }
}

template<typename T>
void writeKey( json & Obj, std::string const& key, std::vector<T> const & opt)
{
    if( opt.size() )
    {
        Obj[key] = opt;
    }
}

struct Base_J
{
    std::string name;
    json        extra;
    json        extension;
};
inline void _writeBaseJ(json & J, Base_J const &B)
{
    if( B.name.size() )
    {
        J["name"] = B.name;
    }
    if( B.extra.size() )
    {
        J["extra"] = B.extra;
    }
    if( B.extension.size() )
    {
        J["extension"] = B.extension;
    }
}
inline void _readBaseJ(json const & J, Base_J &B)
{
    if( J.count("name"))
        B.name = J.at("name").get<std::string>();
    if( J.count("extra"))
        B.extra = J.at("extra").get<std::string>();
    if( J.count("extension"))
        B.extension = J.at("extension");
}


struct Asset : public Base_J
{
    std::string version = "2.0";
    std::string generator;
    std::string copyright;

};

inline void to_json(json& j, const Asset & p)
{
   j = json{
            {"version"   , p.version},
            {"generator" , p.generator},
            {"copyright" , p.copyright},
           };
   _writeBaseJ(j,p);
}



inline void from_json(const json & j, Asset & B)
{
    readKey(j, "version", B.version);
    if( j.count("generator"))
        B.generator = j.at("generator").get<std::string>();
    if( j.count("copyright"))
        B.copyright = j.at("copyright").get<std::string>();

    _readBaseJ(j,B);
}

enum class BufferViewTarget : uint32_t
{
    UNKNOWN              = 0,
    ARRAY_BUFFER         = 34962,
    ELEMENT_ARRAY_BUFFER = 34963
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

NLOHMANN_JSON_SERIALIZE_ENUM( AccessorType, {
    {AccessorType::SCALAR, "SCALAR"},
  {AccessorType::VEC2, "VEC2"},
  {AccessorType::VEC3, "VEC3"},
  {AccessorType::VEC4, "VEC4"},
  {AccessorType::MAT2, "MAT2"},
  {AccessorType::MAT3, "MAT3"},
  {AccessorType::MAT4, "MAT4"}
})

enum class ComponentType : int32_t
{
    BYTE            = 5120,
    UNSIGNED_BYTE   = 5121,
    SHORT           = 5122,
    UNSIGNED_SHORT  = 5123,
    INT             = 5124,
    UNSIGNED_INT    = 5125,
    FLOAT           = 5126,
    DOUBLE          = 5130
};
inline void to_json(json& j, const ComponentType & p)
{
   j = static_cast<int32_t>(p);
}

inline void from_json(const json & j, ComponentType & B)
{
    int e = j.get<int>();
    B = static_cast<ComponentType>(e);
}

/**
 * @brief The BufferView struct
 *
 */
struct BufferView : public Base_J
{
    std::vector<uint8_t> data;
    uint32_t             byteStride = 0;
    BufferViewTarget     target = BufferViewTarget::UNKNOWN;
};

struct Accessor : public Base_J
{
    uint32_t       bufferView=std::numeric_limits<uint32_t>::max();
    uint32_t       byteOffset=0;
    uint32_t       count=0;
    ComponentType  componentType;
    AccessorType   type;
    bool           normalized = false;

    std::vector<double> min;
    std::vector<double> max;
};

inline void to_json(json& j, const Accessor & p)
{
    _writeBaseJ(j,p);
    writeKey(j, "componentType" , p.componentType);
    writeKey(j, "type"          , p.type);
    writeKey(j, "count"         , p.count);

    writeKey(j, "bufferView", p.bufferView);
    writeKey(j, "byteOffset", p.byteOffset);
    writeKey(j, "normalized", p.normalized);
    writeKey(j, "min"       , p.min);
    writeKey(j, "max"       , p.max);
}

inline void from_json(const json & j, Accessor & p)
{
    readKey(j, "count"         , p.count);
    readKey(j, "componentType" , p.componentType);
    readKey(j, "type"         , p.type);

    readKey(j, "bufferView", p.bufferView);
    readKey(j, "byteOffset", p.byteOffset, 0u);
    readKey(j, "normalized"    , p.normalized, false);
    readKey(j, "min"       , p.min);
    readKey(j, "max"       , p.max);
}


class Image : public Base_J
{
public:
    // MimeType	string	The image's MIME type.	No
    // bufferView	integer	The index of the bufferView that contains the image. Use this instead of the image's uri property.	No
    uint32_t    bufferView; // if bufferView is not given
    std::string mimeType;
};

inline void to_json(json& j, const Image & p)
{
    _writeBaseJ(j,p);
    writeKey(j, "bufferView", p.bufferView);
    writeKey(j, "mimeType", p.mimeType);
}

inline void from_json(const json & j, Image & p)
{
    readKey(j, "bufferView", p.bufferView);
    readKey(j, "mimeType", p.mimeType);
}

enum class AnimationPath
{
    TRANSLATION,
    ROTATION,
    SCALE,
    WEIGHTS
};

NLOHMANN_JSON_SERIALIZE_ENUM( AnimationPath, {
    {AnimationPath::TRANSLATION, "translation"},
  {AnimationPath::ROTATION, "rotation"},
  {AnimationPath::SCALE, "scale"},
    {AnimationPath::WEIGHTS, "weights"}
})

class AnimationChannel : public Base_J
{
public:
    uint32_t       samplerIndex = std::numeric_limits<uint32_t>::max();

    struct
    {
        uint32_t      node = std::numeric_limits<uint32_t>::max();
        AnimationPath path;
    } target;
};

inline void to_json(json& j, const AnimationChannel & p)
{
    _writeBaseJ(j,p);
    j["target"] = json::object();
    writeKey(j, "sampler", p.samplerIndex);
    writeKey(j.at("target"), "node", p.target.node);
    writeKey(j.at("target"), "path", p.target.path);
}

inline void from_json(const json & j, AnimationChannel & p)
{
    readKey(j, "sampler", p.samplerIndex);
    readKey(j.at("target"), "node", p.target.node);
    readKey(j.at("target"), "path", p.target.path);
}

enum class AnimationInterpolation : int32_t
{
    STEP,
    LINEAR,
    CUBICSPLINE
};

NLOHMANN_JSON_SERIALIZE_ENUM( AnimationInterpolation, {
    {AnimationInterpolation::STEP, "STEP"},
  {AnimationInterpolation::LINEAR, "LINEAR"},
  {AnimationInterpolation::CUBICSPLINE, "CUBICSPLINE"}
})

class AnimationSampler  : public Base_J
{
public:
    AccessorIndex          input  = std::numeric_limits<uint32_t>::max();
    AccessorIndex          output = std::numeric_limits<uint32_t>::max();
    AnimationInterpolation interpolation = AnimationInterpolation::LINEAR;
};

inline void to_json(json& j, const AnimationSampler & p)
{
    _writeBaseJ(j,p);
    writeKey(j, "input", p.input);
    writeKey(j, "output", p.output);
    writeKey(j, "interpolation", p.interpolation);
}

inline void from_json(const json & j, AnimationSampler & p)
{
    readKey(j, "input", p.input);
    readKey(j, "output", p.output);

    p.interpolation = AnimationInterpolation::LINEAR;
    std::optional<AnimationInterpolation> intrp;
    readKey(j, "interpolation", intrp);

    if( intrp.has_value())
        p.interpolation = *intrp;
}


class Animation : public Base_J
{
public:
    std::vector<AnimationChannel> channels;
    std::vector<AnimationSampler> samplers;
};

inline void to_json(json& j, const Animation & p)
{
    _writeBaseJ(j,p);
    writeKey(j, "channels", p.channels);
    writeKey(j, "samplers", p.samplers);
}

inline void from_json(const json & j, Animation & p)
{
    readKey(j, "channels", p.channels);
    readKey(j, "samplers", p.samplers);
}


class Node : public Base_J
{
public:
    std::optional<uint32_t> mesh  ;
    std::optional<uint32_t> camera;
    std::optional<uint32_t> skin  ;

    std::optional< std::array<float,16> > matrix     ;// = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    std::optional< std::array<float,3>  > scale      ;// = {1,1,1};
    std::optional< std::array<float,4>  > rotation   ;// = {0,0,0,1};
    std::optional< std::array<float,3>  > translation;// = {0,0,0};

    std::vector<uint32_t> children;

    std::vector<float> weights;
};

inline void to_json(json& j, const Node & p)
{
    _writeBaseJ(j,p);
    writeKey( j, "mesh"       , p.mesh        );
    writeKey( j, "camera"     , p.camera      );
    writeKey( j, "skin"       , p.skin        );
    writeKey( j, "matrix"     , p.matrix      );// = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    writeKey( j, "scale"      , p.scale       );// = {1,1,1};
    writeKey( j, "rotation"   , p.rotation    );// = {0,0,0,1};
    writeKey( j, "translation", p.translation );// = {0,0,0};
    writeKey( j, "children"   , p.children    );
    writeKey( j, "weights"    , p.weights     );
}

inline void from_json(const json & j, Node & p)
{
    readKey( j, "mesh"       , p.mesh        );
    readKey( j, "camera"     , p.camera      );
    readKey( j, "skin"       , p.skin        );
    readKey( j, "matrix"     , p.matrix      );// = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    readKey( j, "scale"      , p.scale       );// = {1,1,1};
    readKey( j, "rotation"   , p.rotation    );// = {0,0,0,1};
    readKey( j, "translation", p.translation );// = {0,0,0};
    readKey( j, "children"   , p.children    );
    readKey( j, "weights"    , p.weights     );
}

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

struct PrimitiveTargets
{
    // node these must be in this order otherwise the has() method wont work
    std::optional<AccessorIndex> POSITION   ;//= std::numeric_limits<uint32_t>::max();//-1;
    std::optional<AccessorIndex> NORMAL     ;//= std::numeric_limits<uint32_t>::max();//-1;
    std::optional<AccessorIndex> TANGENT	 ;//= std::numeric_limits<uint32_t>::max();//-1;
    std::optional<AccessorIndex> TEXCOORD_0 ;//= std::numeric_limits<uint32_t>::max();//-1;
    std::optional<AccessorIndex> TEXCOORD_1 ;//= std::numeric_limits<uint32_t>::max();//-1;
    std::optional<AccessorIndex> COLOR_0	 ;//= std::numeric_limits<uint32_t>::max();//-1;
    std::optional<AccessorIndex> JOINTS_0   ;//= std::numeric_limits<uint32_t>::max();//-1;
    std::optional<AccessorIndex> WEIGHTS_0  ;//= std::numeric_limits<uint32_t>::max();//-1;
};

inline void to_json(json& j, const PrimitiveTargets & p)
{
    writeKey( j , "POSITION"  , p.POSITION        );
    writeKey( j , "NORMAL"    , p.NORMAL      );
    writeKey( j , "TANGENT"   , p.TANGENT        );
    writeKey( j , "TEXCOORD_0", p.TEXCOORD_0      );// = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    writeKey( j , "TEXCOORD_1", p.TEXCOORD_1       );// = {1,1,1};
    writeKey( j , "COLOR_0"   , p.COLOR_0    );// = {0,0,0,1};
    writeKey( j , "JOINTS_0"  , p.JOINTS_0 );// = {0,0,0};
    writeKey( j , "WEIGHTS_0" , p.WEIGHTS_0    );
}

inline void from_json(const json & j, PrimitiveTargets & p)
{
    readKey( j , "POSITION"  , p.POSITION        );
    readKey( j , "NORMAL"    , p.NORMAL      );
    readKey( j , "TANGENT"   , p.TANGENT        );
    readKey( j , "TEXCOORD_0", p.TEXCOORD_0      );// = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    readKey( j , "TEXCOORD_1", p.TEXCOORD_1       );// = {1,1,1};
    readKey( j , "COLOR_0"   , p.COLOR_0    );// = {0,0,0,1};
    readKey( j , "JOINTS_0"  , p.JOINTS_0 );// = {0,0,0};
    readKey( j , "WEIGHTS_0" , p.WEIGHTS_0    );
}


class Primitive : public Base_J
{
public:
    PrimitiveTargets                attributes;
    std::optional<AccessorIndex>    indices;
    std::optional<uint32_t>         material;
    std::vector<PrimitiveTargets>   targets;
    PrimitiveMode                   mode     = PrimitiveMode::TRIANGLES;
};

inline void to_json(json& j, const Primitive & p)
{
    _writeBaseJ(j,p);
    j["attributes"] = json::object();
    writeKey( j, "attributes", p.attributes);

    writeKey( j , "indices"  , p.indices );
    writeKey( j , "material" , p.material);
    writeKey( j , "mode"     , p.mode    );
}

inline void from_json(const json & j, Primitive & p)
{
    readKey( j , "attributes", p.attributes);
    readKey( j , "targets"   , p.targets);

    readKey( j , "indices"  , p.indices );
    readKey( j , "material" , p.material);
    readKey( j , "mode"     , p.mode    , PrimitiveMode::TRIANGLES);
}

class Mesh : public Base_J
{
public:
    std::vector<Primitive> primitives;
    std::vector<float>      weights;
};

inline void to_json(json& j, const Mesh & p)
{
    _writeBaseJ(j,p);
    writeKey( j, "primitives"       , p.primitives        );
    writeKey( j, "weights"     , p.weights      );
}

inline void from_json(const json & j, Mesh & p)
{
    readKey( j, "primitives"       , p.primitives        );
    readKey( j, "weights"     , p.weights      );
}


class Skin : public Base_J
{
public:
    std::optional<AccessorIndex>     inverseBindMatrices = std::numeric_limits<uint32_t>::max();
    std::vector<uint32_t>            joints;	//integer [1-*]	The indices of each root node.	No
    std::optional<int32_t>           skeleton;
};

inline void to_json(json& j, const Skin & p)
{
    _writeBaseJ(j,p);
    writeKey( j, "inverseBindMatrices"       , p.inverseBindMatrices        );
    writeKey( j, "joints"     , p.joints      );
    writeKey( j, "skeleton"     , p.skeleton      );
}

inline void from_json(const json & j, Skin & p)
{
    readKey( j, "inverseBindMatrices"       , p.inverseBindMatrices        );
    readKey( j, "joints"     , p.joints      );
    readKey( j, "skeleton"     , p.skeleton      );
}

enum class Filter : uint32_t
{
    UNKNOWN                =  0,
    NEAREST                =  9728,
    LINEAR                 =  9729,
    NEAREST_MIPMAP_NEAREST =  9984,
    LINEAR_MIPMAP_NEAREST  =  9985,
    NEAREST_MIPMAP_LINEAR  =  9986,
    LINEAR_MIPMAP_LINEAR   =  9987
};

enum class WrapMode : uint32_t
{
    CLAMP_TO_EDGE   = 33071,
    MIRRORED_REPEAT = 33648,
    REPEAT          = 10497
};


class Sampler : public Base_J
{
public:
    Filter   magFilter;
    Filter   minFilter;
    WrapMode wrapS;
    WrapMode wrapT;
};

inline void to_json(json& j, const Sampler & p)
{
    _writeBaseJ(j,p);
    writeKey( j, "magFilter"  , p.magFilter        );
    writeKey( j, "minFilter" , p.minFilter      );
    writeKey( j, "wrapS"     , p.wrapS      );
    writeKey( j, "wrapT"     , p.wrapT      );
}

inline void from_json(const json & j, Sampler & p)
{
    readKey( j, "magFilter" , p.magFilter  );
    readKey( j, "minFilter" , p.minFilter  );
    readKey( j, "wrapS"     , p.wrapS      , WrapMode::REPEAT);
    readKey( j, "wrapT"     , p.wrapT      , WrapMode::REPEAT);
}

class Texture : public Base_J
{
public:
    std::optional<uint32_t>   samplerIndex;   // "sampler"
    std::optional<uint32_t>   sourceTexture ; // "source"
};

inline void to_json(json& j, const Texture & p)
{
    _writeBaseJ(j,p);
    writeKey( j, "sampler"  , p.samplerIndex        );
    writeKey( j, "source" , p.sourceTexture      );
}

inline void from_json(const json & j, Texture & p)
{
    readKey( j, "sampler"       , p.samplerIndex        );
    readKey( j, "source"     , p.sourceTexture      );
}

class Scene : public Base_J
{
public:
    std::vector<uint32_t> nodes;	//integer [1-*]	The indices of each root node.	No
};

inline void to_json(json& j, const Scene & p)
{
    _writeBaseJ(j,p);
    writeKey( j, "nodes"  , p.nodes        );
}

inline void from_json(const json & j, Scene & p)
{
    readKey( j, "nodes"       , p.nodes        );
}




class TextureInfo : public Base_J
{
public:
    uint32_t index    = std::numeric_limits<uint32_t>::max();
    uint32_t texCoord = 0;
};
inline void to_json(json& j, const TextureInfo & p)
{
    _writeBaseJ(j,p);
    writeKey( j, "index"  , p.index );
    writeKey( j, "texCoord"  , p.texCoord );
}

inline void from_json(const json & j, TextureInfo & p)
{
    readKey( j, "index"    , p.index        );
    readKey( j, "texCoord" , p.texCoord, 0u  );
    //readKey( j, "scale"    , p.scale,    1.0f  );
}

class NormalTextureInfo : public Base_J
{
public:
    uint32_t index    = std::numeric_limits<uint32_t>::max();
    uint32_t texCoord = 0;
    float    scale    = 1.0f;        // used for other textures
};
inline void to_json(json& j, const NormalTextureInfo & p)
{
    _writeBaseJ(j,p);
    writeKey( j, "index"  , p.index );
    writeKey( j, "texCoord"  , p.texCoord );
    writeKey( j, "scale"  , p.scale );
}

inline void from_json(const json & j, NormalTextureInfo & p)
{
    readKey( j, "index"    , p.index        );
    readKey( j, "texCoord" , p.texCoord, 0u  );
    readKey( j, "scale"    , p.scale,    1.0f  );
}

class OcclusionTextureInfo : public Base_J
{
public:
    uint32_t index    = std::numeric_limits<uint32_t>::max();
    uint32_t texCoord = 0;
    float    strength = 1.0f;        // used for other textures
};

inline void to_json(json& j, const OcclusionTextureInfo & p)
{
    _writeBaseJ(j,p);
    writeKey( j, "index"  , p.index );
    writeKey( j, "texCoord"  , p.texCoord );
    writeKey( j, "strength"  , p.strength );
}

inline void from_json(const json & j, OcclusionTextureInfo & p)
{
    readKey( j, "index"    , p.index        );
    readKey( j, "texCoord" , p.texCoord, 0u  );
    readKey( j, "strength"    , p.strength,    1.0f  );
}

struct PBRMetallicRoughness : public Base_J
{
    std::array<float, 4>        baseColorFactor = {1,1,1,1};
    std::optional<TextureInfo>  baseColorTexture;
    float                       metallicFactor  = 1.0f;
    float                       roughnessFactor = 1.0f;
    std::optional<TextureInfo>  metallicRoughnessTexture;
};

inline void to_json(json& j, const PBRMetallicRoughness & p)
{
    _writeBaseJ(j,p);
    writeKey( j, "baseColorFactor"  , p.baseColorFactor );
    writeKey( j, "baseColorTexture"  , p.baseColorTexture );
    writeKey( j, "metallicFactor"  , p.metallicFactor );
    writeKey( j, "roughnessFactor"  , p.roughnessFactor );
    writeKey( j, "metallicRoughnessTexture"  , p.metallicRoughnessTexture );
}

inline void from_json(const json & j, PBRMetallicRoughness & p)
{
    readKey( j, "baseColorFactor"  , p.baseColorFactor, std::array<float, 4>{1,1,1,1} );
    readKey( j, "baseColorTexture"  , p.baseColorTexture );
    readKey( j, "metallicFactor"  , p.metallicFactor, 1.0f );
    readKey( j, "roughnessFactor"  , p.roughnessFactor, 1.0f );
    readKey( j, "metallicRoughnessTexture"  , p.metallicRoughnessTexture );
}

enum class MaterialAlphaMode
{
    OPAQUE,
    MASK,
    BLEND
};
NLOHMANN_JSON_SERIALIZE_ENUM( MaterialAlphaMode, {
    {MaterialAlphaMode::OPAQUE, "OPAQUE"},
  {MaterialAlphaMode::MASK, "MASK"},
  {MaterialAlphaMode::BLEND, "BLEND"}
})


class Material : public Base_J
{
public:
    std::optional<PBRMetallicRoughness> pbrMetallicRoughness;

    std::optional<NormalTextureInfo>     normalTexture;
    std::optional<OcclusionTextureInfo>  occlusionTexture;
    std::optional<TextureInfo>           emissiveTexture;

    std::array<float,3>  emissiveFactor = {0,0,0};

    MaterialAlphaMode alphaMode = MaterialAlphaMode::OPAQUE;
    float alphaCutoff = 0.5f;
    bool  doubleSided = false;
};
inline void to_json(json& j, const Material & p)
{
    _writeBaseJ(j,p);
    writeKey( j, "pbrMetallicRoughness"  , p.pbrMetallicRoughness );
    writeKey( j, "normalTexture"  , p.normalTexture );
    writeKey( j, "occlusionTexture"  , p.occlusionTexture );
    writeKey( j, "emissiveTexture"  , p.emissiveTexture );

    writeKey( j, "emissiveFactor"  , p.emissiveFactor );
    writeKey( j, "alphaMode"     , p.alphaMode );
    writeKey( j, "alphaCutoff"  , p.alphaCutoff );
    writeKey( j, "doubleSided"  , p.doubleSided );
}

inline void from_json(const json & j, Material & p)
{
    readKey( j, "pbrMetallicRoughness"  , p.pbrMetallicRoughness );
    readKey( j, "normalTexture"  , p.normalTexture );
    readKey( j, "occlusionTexture"  , p.occlusionTexture );
    readKey( j, "emissiveTexture"  , p.emissiveTexture );

    readKey( j, "emissiveFactor"  , p.emissiveFactor, std::array<float,3>{0,0,0} );
    readKey( j, "alphaMode"       , p.alphaMode     , MaterialAlphaMode::OPAQUE);
    readKey( j, "alphaCutoff"     , p.alphaCutoff   , 0.5f);
    readKey( j, "doubleSided"     , p.doubleSided   , false);
}




enum class CameraType
{
    PERSPECTIVE,
    ORTHOGRAPHIC
};

NLOHMANN_JSON_SERIALIZE_ENUM( CameraType, {
    {CameraType::PERSPECTIVE, "perspective"},
  {CameraType::ORTHOGRAPHIC, "orthographic"}
})

struct OrthoCamera
{
    float xmag;
    float ymag;
    float zfar;
    float znear;
};
inline void to_json(json& j, const OrthoCamera & p)
{
    writeKey( j, "xmag"  , p.xmag );
    writeKey( j, "ymag"  , p.ymag );
    writeKey( j, "zfar"  , p.zfar );
    writeKey( j, "znear" , p.znear );
}

inline void from_json(const json & j, OrthoCamera & p)
{
    readKey( j, "xmag"  , p.xmag );
    readKey( j, "ymag"  , p.ymag );
    readKey( j, "zfar"  , p.zfar );
    readKey( j, "znear" , p.znear );
}

struct PerspectiveCamera
{
    float aspectRatio;
    float yfov;
    float zfar;
    float znear;
};
inline void to_json(json& j, const PerspectiveCamera & p)
{
    writeKey( j, "aspectRatio"  , p.aspectRatio );
    writeKey( j, "yfov"  , p.yfov );
    writeKey( j, "zfar"  , p.zfar );
    writeKey( j, "znear" , p.znear );
}

inline void from_json(const json & j, PerspectiveCamera & p)
{
    readKey( j, "aspectRatio"  , p.aspectRatio );
    readKey( j, "yfov"  , p.yfov );
    readKey( j, "zfar"  , p.zfar );
    readKey( j, "znear" , p.znear );
}

class Camera : public Base_J
{
public:
    CameraType type;//	string	Specifies if the camera uses a perspective or orthographic projection.	white_check_mark Yes

    std::optional<OrthoCamera>       orthographic;
    std::optional<PerspectiveCamera> perspective;

};

inline void to_json( json & j, Camera const & B)
{
    writeKey(j, "orthographic", B.orthographic);
    writeKey(j, "perspective" , B.perspective );
    writeKey(j, "type" , B.type );
    _writeBaseJ(j, B);
}

inline void from_json(const json & j, Camera & B)
{
    readKey(j, "type"        , B.type);
    readKey(j, "perspective" , B.perspective);
    readKey(j, "orthographic", B.orthographic);

    _readBaseJ(j,B);
}


class GLTFModel2
{
public:
    struct header_t
    {
        uint32_t magic   = 0x46546C67;
        uint32_t version = 2;
        uint32_t length  = 12;
    };

    GLTFModel2()
    {
    }

    bool load( std::istream & i)
    {
        auto firstChar = i.peek();
        if( firstChar == 0x67 && firstChar !=  ' ' && firstChar != '{' )
        {
            // it's a GLB file.
            std::array<uint32_t, 3> header;
            i.read(reinterpret_cast<char*>(&header), sizeof(header));

            if(header[0] != 0x46546C67) return false;
            if(header[1] < 2)          return false;


            std::array<uint32_t, 2> chunkHeader;

            {
                i.read(reinterpret_cast<char*>(&chunkHeader), sizeof(chunkHeader));
                std::string jsonStr;
                assert( chunkHeader[0] > 0);
                jsonStr.resize( chunkHeader[0] );
                i.read(reinterpret_cast<char*>(&jsonStr[0]), chunkHeader[0] );
                _json  = json::parse(jsonStr);
            }
            {
                i.read(reinterpret_cast<char*>(&chunkHeader), sizeof(chunkHeader));
                std::vector<uint8_t> buffer;
                assert( chunkHeader[0] > 0);

                buffer.resize( chunkHeader[0] );
                i.read(reinterpret_cast<char*>(&buffer[0]), chunkHeader[0] );

                bufferViews = _convertBufferToBufferViews(buffer, _json.at("bufferViews"));
            }
            _build(_json);
            return true;
        }
        else  // is pure json file
        {
            return false;
            ///i >> _json;
        }
    }

    static void writeGLB( std::ostream & out, GLTFModel2 const & M)
    {
        // generate the json component of the GLTF asset
        auto J = generateJson(M, 4);
        std::string Jstr = J.dump(4);

        // The size of the json data must be aligned to a size of 4
        if( Jstr.size() %4 != 0)
        {
            Jstr.insert( Jstr.end(), 4-Jstr.size()%4, ' ');
        }
        assert( Jstr.size() % 4 == 0);

        // get a vector of the size of each buffer view aligned to a 4 byte boundry
        auto cs = GLTFModel2::_calculateBufferChunkSize(M.bufferViews, 4);

        header_t h;
        // calculate the total length of the glb file
        h.length = static_cast<uint32_t>( Jstr.size() +            // size of the json data
                   + std::accumulate(cs.begin(),cs.end(), size_t(0)) ) // size of the binary
                   + sizeof(header_t)                               // size of the main header
                   + 2 * ( sizeof(uint32_t) + sizeof(uint32_t) )  ; // headers of the 2 chunks

        // write the header
        out.write( reinterpret_cast<char const*>(&h), sizeof(h) );


        // write the json chunk
        uint32_t u32v = static_cast<uint32_t>( Jstr.size() );
        out.write( reinterpret_cast<char const*>(&u32v), sizeof(uint32_t) );

        u32v = 0x4E4F534A;
        out.write( reinterpret_cast<char const*>(&u32v), sizeof(uint32_t));

        out.write(Jstr.data(), Jstr.size() );

        // write out all the data in teh buffer views into a single buffer
        _writeBufferViewsToBufferChunk(out, M.bufferViews, 4);
    }

    static json generateJson(GLTFModel2 const & M, size_t bufferAlignment=8)
    {
        json J;

         writeKey(J, "scene"       , M.scene);
         writeKey(J, "asset"       , M.asset      );
         writeKey(J, "accessors"   , M.accessors  );
         writeKey(J, "nodes"       , M.nodes      );
         writeKey(J, "meshes"      , M.meshes     );
         writeKey(J, "scene"       , M.scene      );
         writeKey(J, "scenes"      , M.scenes     );
         writeKey(J, "skins"       , M.skins      );
         writeKey(J, "animations"  , M.animations );
         writeKey(J, "images"      , M.images     );
         writeKey(J, "textures"    , M.textures   );
         writeKey(J, "samplers"    , M.samplers   );
         writeKey(J, "cameras"     , M.cameras    );
         writeKey(J, "materials"   , M.materials  );
         if( M.extensions.is_object() )
         {
            writeKey(J, "extensions"  , M.extensions );
         }

         J["bufferViews"] = _generateJSONBufferViews(M.bufferViews, bufferAlignment);

         auto b = _calculateBufferChunkSize(M.bufferViews, bufferAlignment);

         J["buffers"][0]["byteLength"] = std::accumulate(b.begin(), b.end(), 0);
         return J;
    }


    std::vector<BufferView> _convertBufferToBufferViews(std::vector<uint8_t> const & buffer, json const & bvJ)
    {
        std::vector<BufferView> views;
        for(auto & bv : bvJ)
        {
            BufferView b2;

            uint32_t byteLength=0;
            std::optional<uint32_t>         byteStride;
            std::optional<uint32_t>         byteOffset;
            std::optional<BufferViewTarget> target;

            readKey(bv, "byteLength", byteLength);
            readKey(bv, "byteOffset", byteOffset);
            readKey(bv, "byteStride", byteStride);
            readKey(bv, "target"    , target);

            if(byteStride.has_value()) b2.byteStride = *byteStride;
            if(target.has_value())     b2.target = *target;

            if(byteOffset.has_value())
            {
                byteOffset = *byteOffset;
            }

            b2.data.resize( byteLength );
            std::memcpy( &b2.data[0], &buffer[*byteOffset], byteLength);
            views.push_back( std::move(b2));
        }
        return views;
    }
public:
    Asset                   asset;

    std::vector<BufferView> bufferViews;
    std::vector<Accessor>   accessors;
    std::vector<Node>       nodes;
    std::vector<Mesh>       meshes;
    std::optional<uint32_t>  scene;
    std::vector<Scene>      scenes;
    std::vector<Skin>       skins;
    std::vector<Animation>  animations;
    std::vector<Image>      images;
    std::vector<Texture>    textures;
    std::vector<Sampler>    samplers;
    std::vector<Camera>      cameras;
    std::vector<Material>   materials;
    json                     extensions;
    json                     _json;

    /**
     * @brief _calculateBufferChunkSize
     * @param bufferViews
     * @return
     *
     * returns the number of bytes each bufferview SHOULD write out.
     *
     * Given N bufferViews with actual data.
     *
     * +----------------------+
     * | BV1                  |
     * +----------------------+
     *
     * +-----------+
     * | BV2       |
     * +-----------+
     *
     * +------+
     * | BV3  |
     * +------+
     *
     * Calculate the total number of bytes each bufferView
     * must write out to keep a proper alignment
     * +----------------------+--+-----------+-----+------+--+
     * | BV1                  |PP| BV2       |ppppp| BV3  |PP|
     * +----------------------+--+-----------+-----+------+--+
     */
    static std::vector<size_t> _calculateBufferChunkSize( std::vector<BufferView> const & bufferViews, size_t alignment)
    {
        std::vector<size_t> bufferViewSizes;
        for(auto & b : bufferViews)
        {
            size_t chunkSize = b.data.size();

            if( chunkSize % alignment != 0)
            {
                chunkSize += ( alignment - chunkSize%alignment);
            }
            bufferViewSizes.push_back(chunkSize);
        }
        return bufferViewSizes;
    }
    /**
     * @brief _generateJSONBufferViews
     * @param Bv
     * @param alignment
     * @return
     *
     * Given the BufferViews with actual data.
     * Generate the GLTF json bufferView array with the apporpriate
     * offsets to keep the data properly aligned
     *
     */
    static json _generateJSONBufferViews(std::vector<BufferView> const & Bv, size_t alignment)
    {
        auto writeOutSizes = _calculateBufferChunkSize(Bv, alignment);
        json J;

        size_t i=0;
        uint32_t bufferSize=0;
        uint32_t offset=0;
        for(auto & b : Bv)
        {
            json j;
            auto bs = static_cast<uint32_t>( b.data.size() );

            j["buffer"]     = 0;
            j["byteLength"] = bs;
            j["byteOffset"] = offset;
            if( b.byteStride >= 4)
                j["byteStride"] = b.byteStride;
            if( b.target != BufferViewTarget::UNKNOWN)
                j["target"] = static_cast<uint32_t>(b.target);

            bufferSize += bs;
            offset += writeOutSizes[i];
            i++;
            J.push_back(j);
        }
        return J;
    }


    /**
     * @brief writeBufferView
     * @param out
     * @param data
     * @param bvJ
     *
     * Given the BufferView with actual data. Write out out all the bufferView data
     * to a single binary buffer chunk. THis includes
     * the GLTF header for the chunk.
     */
    static size_t _writeBufferViewsToBufferChunk( std::ostream & out, std::vector<BufferView> const & data, size_t alignment)
    {
        auto bufferViewWriteOutSizes = _calculateBufferChunkSize(data, alignment);

        uint32_t chunkLength = 0;//
        uint32_t chunkType   = 0x004E4942; // bin

        chunkLength = static_cast<uint32_t>( std::accumulate(bufferViewWriteOutSizes.begin(), bufferViewWriteOutSizes.end(), 0) );

        out.write( reinterpret_cast<char*>(&chunkLength) , sizeof(chunkLength));
        out.write( reinterpret_cast<char*>(&chunkType )  , sizeof(chunkType));

        size_t bytesWritten=0;
        auto s = data.size();
        size_t zero=0;
        for(size_t i=0;i<s;i++)
        {
            out.write( reinterpret_cast<char const*>(data[i].data.data())  , data[i].data.size() );
            bytesWritten += data[i].data.size();

            auto diff = bufferViewWriteOutSizes[i] - data[i].data.size();
            if(diff)
            {
                out.write( reinterpret_cast<char const*>(&zero), diff );
                bytesWritten += diff;
            }
        }
        return bytesWritten;
    }

    /**
     * @brief _build
     * @param J
     *
     * Given the JSON component of a GLTF asset.
     * build each of the components
     */
    void _build(json const & J)
    {
        readKey(J, "asset", asset);
        readKey(J, "nodes", nodes);
        readKey(J, "meshes", meshes);
        readKey(J, "scenes", scenes);
        readKey(J, "skins", skins);
        readKey(J, "animations", animations);
        readKey(J, "accessors", accessors);
        readKey(J, "images", images);
        readKey(J, "textures", textures);
        readKey(J, "samplers", samplers);
        readKey(J, "cameras", cameras);
        readKey(J, "materials", materials);
        readKey(J, "scene", scene);
        if(J.count("extensions"))
        {
            extensions = J.at("extensions");
        }
    }
};


}

#endif

