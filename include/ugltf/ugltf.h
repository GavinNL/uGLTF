#ifndef GLTFPP_H
#define GLTFPP_H

#ifndef JSON_INCLUDE
#define JSON_INCLUDE <nlohmann/json.hpp>
#endif

#include JSON_INCLUDE
#include <iostream>
#include <type_traits>

#ifndef UGLTF_NAMESPACE
    #define UGLTF_NAMESPACE uGLTF
#endif

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
* [`camera`](#reference-camera)
   * [`orthographic`](#reference-orthographic)
   * [`perspective`](#reference-perspective)
* [`material`](#reference-material)
   * [`normalTextureInfo`](#reference-normaltextureinfo)
   * [`occlusionTextureInfo`](#reference-occlusiontextureinfo)
   * [`pbrMetallicRoughness`](#reference-pbrmetallicroughness)
* [`textureInfo`](#reference-textureinfo)

## To Do:
* [`accessor`](#reference-accessor)
   * [`sparse`](#reference-sparse)
      * [`indices`](#reference-indices)
      * [`values`](#reference-values)

   *
* [`extension`](#reference-extension)
* [`extras`](#reference-extras)
* [`glTF`](#reference-gltf) (root object)



*/

namespace UGLTF_NAMESPACE
{

using json = nlohmann::json;

class GLTFModel;

/**
 * @brief The aspan class
 *
 * An aspan (Aliased Span) is similar to a string_view or span (from C++20), but the underlying
 * contigious array data can be alised to a different type as well as custom strides.
 */
template<typename T>
class aspan
{
public:
    using base_value_type = typename std::remove_cv<T>::type;

    using value_type = T;

    using const_value_type = const typename std::remove_cv<value_type>::type;


    /**
     * @brief The _iterator class
     * Iterator for non-const data
     */
    template<typename V>
    class _iterator :  public std::iterator<std::random_access_iterator_tag, V>
    {
            using char_type  = unsigned char;
            using value_type = V;
            char_type     * p;
            std::ptrdiff_t stride;

          public:

            _iterator(char_type* x, std::ptrdiff_t _stride) : p(x), stride(_stride) {}
            _iterator(const _iterator& mit) : p(mit.p), stride(mit.stride) {}

            // return the number of elements between two iterators
            bool operator<(const _iterator & other) const
            {
                return p < other.p;
            }

            std::ptrdiff_t operator-(const _iterator & other) const
            {
                return (p - other.p) / stride;
            }
            _iterator operator-(int inc) const
            {
                return _iterator( p - inc*stride, stride);
            }
            _iterator operator+(int inc) const
            {
                return _iterator( p + inc*stride, stride);
            }

            _iterator& operator--() {p-=stride;return *this;}
            _iterator  operator--(int) {_iterator tmp(*this); operator--(); return tmp;}

            _iterator& operator++() {p+=stride;return *this;}
            _iterator operator++(int) {_iterator tmp(*this); operator++(); return tmp;}

            bool operator==(const _iterator& rhs) const {return p==rhs.p;}
            bool operator!=(const _iterator& rhs) const {return p!=rhs.p;}

            value_type & operator*()
            {
                return *static_cast<value_type*>(static_cast<void*>(p));
            }

            value_type const & operator*() const
            {
                return *static_cast<value_type const*>(static_cast<void const*>(p));
            }
     };
    template<typename V>
    class _const_iterator :  public std::iterator<std::random_access_iterator_tag, V>
    {
            using char_type  = const unsigned char;
            using value_type = const V;
            char_type     * p;
            std::ptrdiff_t stride;

          public:

            _const_iterator(char_type* x, std::ptrdiff_t _stride) : p(x), stride(_stride) {}
            _const_iterator(const _const_iterator& mit) : p(mit.p), stride(mit.stride) {}

            // return the number of elements between two iterators
            bool operator<(const _const_iterator & other) const
            {
                return p < other.p;
            }

            std::ptrdiff_t operator-(const _const_iterator & other) const
            {
                return (p - other.p) / stride;
            }
            _const_iterator operator-(int inc) const
            {
                return _iterator( p - inc*stride, stride);
            }
            _const_iterator operator+(int inc) const
            {
                return _iterator( p + inc*stride, stride);
            }

            _const_iterator& operator--() {p-=stride;return *this;}
            _const_iterator  operator--(int) {_const_iterator tmp(*this); operator--(); return tmp;}

            _const_iterator& operator++() {p+=stride;return *this;}
            _const_iterator operator++(int) {_const_iterator tmp(*this); operator++(); return tmp;}

            bool operator==(const _const_iterator& rhs) const {return p==rhs.p;}
            bool operator!=(const _const_iterator& rhs) const {return p!=rhs.p;}

            value_type & operator*() const
            {
                return *static_cast<value_type const*>(static_cast<void const*>(p));
            }
            value_type & operator*()
            {
                return *static_cast<value_type const*>(static_cast<void const*>(p));
            }
     };

    using iterator       = _iterator<value_type>;
    using const_iterator = _const_iterator<base_value_type>;
    using void_type      = typename std::conditional< std::is_const<value_type>::value, const void, void>::type;
    using char_type      = typename std::conditional< std::is_const<value_type>::value, const unsigned char, unsigned char>::type;

    aspan(void_type * data, size_t size, size_t stride) :
        _begin( static_cast<char_type*>(data) ),
        _size(size),
        _stride(stride)
    {

    }

    const_value_type & operator[](size_t i) const
    {
        return *reinterpret_cast<value_type*>(_begin + _stride*i);
    }
    value_type & operator[](size_t i)
    {
        return *reinterpret_cast<value_type*>(_begin + _stride*i);
    }

    const_iterator begin() const
    {
        return const_iterator( _begin, _stride);
    }
    const_iterator end() const
    {
        return const_iterator( _begin + _stride*size(), _stride);
    }

    typename std::conditional< std::is_const<value_type>::value, const_iterator, iterator>::type
    begin()
    {
        using __it = typename std::conditional< std::is_const<value_type>::value, const_iterator, iterator>::type;
        return __it( _begin, _stride);
    }

    typename std::conditional< std::is_const<value_type>::value, const_iterator, iterator>::type
    end()
    {
        using __it = typename std::conditional< std::is_const<value_type>::value, const_iterator, iterator>::type;
        return __it( _begin + _stride*size(), _stride);
    }

//    iterator end()
//    {
//        return iterator( _begin + _stride*size(), _stride);
//    }

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
    char_type*     _begin;
    size_t         _size;
    size_t         _stride;
};

template<typename T>
inline T _getValue(json const & obj, const std::string & key, T const &default_val)
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

static std::vector<uint8_t> _parseURI(const std::string & uri)
{
    std::vector<uint8_t> out;
    static bool init=false;
    static std::vector<int32_t> T(256,-1);
    if(!init)
    {
        init = true;
        for (int32_t i=0; i<64; i++)
        {
            T[ static_cast<size_t>("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/"[i]) ] = i;
        }
    }


    int val=0, valb=-8;

    auto size = uri.size();
    //for (unsigned char c : in)
    for(size_t i=0;i<size;i++)
    {
        unsigned char c = static_cast<unsigned char>(uri[i]);

        if (T[c] == -1)
            break;

        val = (val<<6) + T[c];
        valb += 6;
        if (valb>=0)
        {
            out.push_back( static_cast<uint8_t>( (val>>valb)&0xFF) );
            valb-=8;
        }
    }
    return out;
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


struct Asset
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
            {"copyright" , p.copyright}
           };
}

inline void from_json(const json & j, Asset & B)
{
    B.version   = _getValue(j,   "version",   std::string("") );
    B.generator = _getValue(j, "generator", std::string("") );
    B.copyright = _getValue(j, "copyright", std::string("") );
}

class Accessor;
class BufferView;

class Buffer
{
    public:
        uint32_t             byteLength;
        std::vector<uint8_t> m_data;


        /**
         * @brief createNewAccessor
         * @param count
         * @param type
         * @param comp
         * @return
         *
         * Create a new accessor within this buffer. This will also create a
         * unique bufferView for the accessor. The buffer's vector data
         * will be expanded
         *
         * Retuns the accessor index
         */
        size_t createNewAccessor(size_t count, AccessorType type, ComponentType comp);

        /**
         * @brief createNewAccessor
         * @param A
         * @return
         *
         * Create a new accessor and copy the data from A
         *
         * Retuns the accessor index
         */
        size_t createNewAccessor(Accessor const & A);


private:
    GLTFModel * _parent;
    friend class GLTFModel;
};


class BufferView
{
public:
    uint32_t          buffer    ;
    uint32_t          byteLength = 0;
    uint32_t          byteOffset = 0;
    uint32_t          byteStride = 0;
    BufferViewTarget  target;

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

    template<typename T>
    aspan<typename std::add_const<T>::type > getSpan() const;

    void* data();

    void const* data() const;

private:
    GLTFModel * _parent;
    friend class GLTFModel;
    friend class Buffer;

};

inline void to_json(json& j, const BufferView & p)
{
   j = json{
            {"buffer"    , p.buffer},
            {"target"    , static_cast<uint32_t>(p.target) },
            {"byteLength", p.byteLength},
            {"byteOffset", p.byteOffset},
            {"byteStride", p.byteStride}
           };
}

inline void from_json(const json & j, BufferView & B)
{
    B.buffer     = _getValue(j, "buffer"    , 0u);
    B.target     = static_cast<BufferViewTarget>(_getValue(j, "target"    , 0u));
    B.byteLength = _getValue(j, "byteLength", 0u);
    B.byteOffset = _getValue(j, "byteOffset", 0u);
    B.byteStride = _getValue(j, "byteStride", 0u);
}





inline std::string to_string(const AccessorType & p)
{
    switch(p)
    {
        case AccessorType::UNKNOWN: return std::string("UNKNOWN");
        case AccessorType::SCALAR:  return std::string("SCALAR");
        case AccessorType::VEC2:    return std::string("VEC2");
        case AccessorType::VEC3:    return std::string("VEC3");
        case AccessorType::VEC4:    return std::string("VEC4");
        case AccessorType::MAT2:    return std::string("MAT2");
        case AccessorType::MAT3:    return std::string("MAT3");
        case AccessorType::MAT4:    return std::string("MAT4");
    }
    return std::string("UNKNOWN");
}



inline std::string to_string(const ComponentType & p)
{
    switch(p)
    {
        case ComponentType::BYTE          : return std::string("BYTE");
        case ComponentType::UNSIGNED_BYTE : return std::string("UNSIGNED_BYTE");
        case ComponentType::SHORT         : return std::string("SHORT");
        case ComponentType::UNSIGNED_SHORT: return std::string("UNSIGNED_SHORT");
        case ComponentType::UNSIGNED_INT  : return std::string("UNSIGNED_INT");
        case ComponentType::FLOAT         : return std::string("FLOAT");
        case ComponentType::DOUBLE        : return std::string("DOUBLE");
    }
    return std::string("UNKNOWN");
}

enum class CameraType
{
    PERSPECTIVE,
    ORTHOGRAPHIC
};

class Camera
{
public:
    std::string name;//	string	The user-defined name of this object.	No
    CameraType type;//	string	Specifies if the camera uses a perspective or orthographic projection.	white_check_mark Yes

    union
    {
        struct
        {
            float aspectRatio;
            float yfov;
            float zfar;
            float znear;
        } perspective;

        struct
        {
            float xmag;
            float ymag;
            float zfar;
            float znear;
        } orthographic;

    };

    /**
     * @brief writeMatrix
     * @param _mat4
     *
     * Takes the parameters of the perspective or orthograph structs and
     * writes the matrix values.
     *
     * _mat4 must be an array of at least 16 floats.
     */
    void writeMatrix(float * _mat4) const
    {
        std::array< std::array<float, 4>, 4> & M = *reinterpret_cast<std::array< std::array<float, 4>, 4>*>(_mat4);

        M[0][1] = M[0][2] = M[0][3] =
        M[1][0] = M[1][2] = M[1][3] =
        M[2][0] = M[2][1] =
        M[3][0] = M[3][1] = M[3][2] = 0.0f;

        if( type == CameraType::PERSPECTIVE)
        {
            auto a = perspective.aspectRatio;
            auto y = perspective.yfov;
            auto n = perspective.znear;
            auto inv_tan = 1.0f / ( a * std::tan(0.5f * y) );

            M[0][0] = inv_tan / a;
            M[1][1] = inv_tan;

            M[3][3] = 0;
            M[3][2] = -1;

            if( std::isinf( perspective.zfar ))
            {
                M[2][2] = -1;
                M[3][2] = -2*n;
            }
            else
            {
                auto f = perspective.zfar;
                M[2][2] = 1.0f / (n-f);
                M[3][2] = -2*f*n / M[2][2];
                M[2][2] *= f+n;
            }
        }
        else
        {
            auto r = orthographic.xmag;
            auto t = orthographic.ymag;
            auto f = orthographic.zfar;
            auto n = orthographic.znear;

            auto inv_nf = 1.0f / (n-f);

            M[0][0] = 1.0f / r;
            M[1][1] = 1.0f / t;
            M[2][2] = 2.0f * inv_nf;
            M[2][3] = (f+n)*inv_nf;

            M[3][3] = -1;
        }
    }
};

inline void to_json( json & j, Camera const & B)
{
    switch( B.type )
    {
        case CameraType::ORTHOGRAPHIC:
            j["type"] = "orthographic";
            j["orthographic"]["xmag"]  = B.orthographic.xmag;
            j["orthographic"]["ymag"]  = B.orthographic.ymag;
            j["orthographic"]["zfar"]  = B.orthographic.zfar;
            j["orthographic"]["znear"] = B.orthographic.znear;
            break;
        case CameraType::PERSPECTIVE:
            j["type"] = "perspective";
            j["perspective"]["aspectRatio"]  = B.perspective.aspectRatio;
            j["perspective"]["yfov"]  = B.perspective.yfov;

            if( !std::isinf(B.perspective.zfar) )
                j["perspective"]["zfar"]  = B.perspective.zfar;

            j["perspective"]["znear"] = B.perspective.znear;
            break;
    }

    if( B.name.size() )
        j["name"] = B.name;
}

inline void from_json(const json & j, Camera & B)
{
    auto type     = _getValue(j, "type", std::string(""));
    B.name        = _getValue(j, "name", std::string(""));

    if( type == "perspective")
    {
        B.type = CameraType::PERSPECTIVE;
        B.perspective.aspectRatio = _getValue(j["perspective"], "aspectRatio", 0.0f);
        B.perspective.yfov        = _getValue(j["perspective"], "yfov", 0.0f);
        B.perspective.zfar        = _getValue(j["perspective"], "zfar", INFINITY);
        B.perspective.znear       = _getValue(j["perspective"], "znear", 0.0f);
    }
    else if( type == "orthographic")
    {
        B.type = CameraType::ORTHOGRAPHIC;
        B.orthographic.xmag  = _getValue(j["orthographic"], "xmag", 0.0f);
        B.orthographic.ymag  = _getValue(j["orthographic"], "ymag", 0.0f);
        B.orthographic.zfar  = _getValue(j["orthographic"], "zfar", INFINITY);
        B.orthographic.znear = _getValue(j["orthographic"], "znear", 0.0f);
    }
    else
    {
        throw std::runtime_error("Camera type is not defined in the gltf asset");
    }

}

class Accessor
{
    public:
        uint32_t       bufferView=std::numeric_limits<uint32_t>::max();
        uint32_t       byteOffset=0;
        uint32_t       count=0;
        ComponentType  componentType;
        AccessorType   type;
        bool           normalized;

        std::vector<double> min;
        std::vector<double> max;

        std::string  name;

        /**
         * @brief getSpan
         * @return
         *
         * Returns a typed container span to the raw data.
         * This method will throw an error if sizeof(T) < componentSize().
         *
         * The span can be accessed as if it was a vector. Note that
         * a span's data may not be contigious.
         */
        template<typename T>
        aspan<T> getSpan();

        template<typename T>
        aspan< typename std::add_const<T>::type > getSpan() const;

        BufferView const & getBufferView() const;
        BufferView & getBufferView();



        /**
         * @brief copyDataFrom
         * @param A
         *
         * Copies data from accessor A to this accessor.
         */
        void copyDataFrom(Accessor const & A);

        /**
         * @brief componentSize
         * @return
         *
         * Returns the component size. That is the size, in bytes, of teh base datatype.
         */
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
            return 0u;
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
            return 0;
        }

    private:
        GLTFModel * _parent;
        friend class GLTFModel;
        friend class Buffer;
};

inline void to_json(json& j, const Accessor & p)
{
   j = json{
            {"buffer"    , p.bufferView},
            {"byteOffset", p.byteOffset},
            {"normalized", p.normalized},
            {"count"     , p.count},
            {"name"      , p.name},
            {"type"      , to_string(p.type)},
            {"componentType"      , to_string(p.componentType)}
           };

   if(p.min.size())
       j["min"] = p.min;
   if(p.max.size())
       j["max"] = p.max;
}

inline void from_json(const json & j, Accessor & B)
{
    B.bufferView     = _getValue(j, "bufferView"   , 0u);
    B.byteOffset     = _getValue(j, "byteOffset" , 0u);
    B.componentType  = static_cast<ComponentType>(_getValue(j, "componentType", 0u));
    B.normalized     = _getValue(j, "normalized"   , false);
    B.count          = _getValue(j, "count"   , 0u);

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

class Mesh;
class Skin;
class Camera;

class Node
{
public:
    uint32_t mesh    = std::numeric_limits<uint32_t>::max();
    uint32_t camera  = std::numeric_limits<uint32_t>::max();
    uint32_t skin    = std::numeric_limits<uint32_t>::max();

    std::array<float,16> matrix      = {1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1};
    std::array<float,3>  scale       = {1,1,1};
    std::array<float,4>  rotation    = {0,0,0,1};
    std::array<float,3>  translation = {0,0,0};

    std::vector<uint32_t> children;



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
        return mesh != std::numeric_limits<uint32_t>::max();
    }
    bool hasCamera() const
    {
        return camera != std::numeric_limits<uint32_t>::max();
    }
    bool hasSkin() const
    {
        return skin != std::numeric_limits<uint32_t>::max();
    }
    size_t childCount() const
    {
        return children.size();
    }

    /**
     * @brief getMesh
     * @return
     *
     * Returns a reference to the mesh. Throws an error if the mesh is not defined
     */
    Mesh & getMesh();
    Mesh const & getMesh() const;

    /**
     * @brief getSkin
     * @return
     *
     * Returns a reference to the skin. Throws an error if the skin is not defined.
     */
    Skin & getSkin();
    Skin const & getSkin() const;

    /**
     * @brief getCamera
     * @return
     *
     * Returns a reference to the camera. Throws an error if the camera is not defined.
     */
    Camera & getCamera();
    Camera const & getCamera() const;


    /**
     * @brief getChild
     * @param childIndex
     * @return
     *
     * Returns a pointer to the child node.
     * Requirement: childIndex < children.size()
     */
    Node* getChild(size_t childIndex);
    Node const * getChild(size_t childIndex) const;


    template<typename Callable_t>
    void depthFirstTraverse(Callable_t && C)
    {
        C(*this);
        uint32_t childIndex=0;
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
        uint32_t childIndex=0;
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
    friend void from_json(const json & j, Node & B);
    friend class GLTFModel;
};

inline void to_json(json& j, const Node & p)
{
    if( p.mesh    != std::numeric_limits<uint32_t>::max())
        j["mesh"] = p.mesh;

    if( p.camera  != std::numeric_limits<uint32_t>::max())
        j["camera"] = p.camera;

    if( p.skin    != std::numeric_limits<uint32_t>::max())
        j["camera"] = p.camera;

    if( p.hasMatrix() )
        j["matrix"] = p.matrix;

    if( p.hasTransforms())
    {
        j["scale"] = p.scale      ;
        j["rotation"] = p.rotation   ;
        j["translation"] = p.translation;
    }

    if( p.childCount())
    {
        j["children"] = p.children;
    }

    if( p.weights.size() )
        j["weights"] = p.weights;

    if( p.name.length() )
        j["name"] = p.name;
}

inline void from_json(const json & j, Node & B)
{
    B.camera      = _getValue(j, "camera", std::numeric_limits<uint32_t>::max() );
    B.skin        = _getValue(j, "skin"  , std::numeric_limits<uint32_t>::max() );
    B.mesh        = _getValue(j, "mesh"  , std::numeric_limits<uint32_t>::max() );

    B.matrix      = _getValue(j, "matrix", std::array<float,16>{1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1});
    B.scale       = _getValue(j, "scale", std::array<float,3>{1,1,1});
    B.rotation    = _getValue(j, "rotation", std::array<float,4>{0,0,0,1});
    B.translation = _getValue(j, "translation", std::array<float,3>{0,0,0});

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
    B.children       = _getValue(j, "children", std::vector<uint32_t>());
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

inline std::string to_string(const PrimitiveAttribute & p)
{
    switch(p)
    {
        case PrimitiveAttribute::POSITION  : return std::string("POSITION");
        case PrimitiveAttribute::NORMAL	   : return std::string("NORMAL");
        case PrimitiveAttribute::TANGENT   : return std::string("TANGENT");
        case PrimitiveAttribute::TEXCOORD_0: return std::string("TEXCOORD_0");
        case PrimitiveAttribute::TEXCOORD_1: return std::string("TEXCOORD_1");
        case PrimitiveAttribute::COLOR_0   : return std::string("COLOR_0");
        case PrimitiveAttribute::JOINTS_0  : return std::string("JOINTS_0");
        case PrimitiveAttribute::WEIGHTS_0 : return std::string("WEIGHTS_0");
    }
    return std::string("UNKNOWN");
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


inline std::string to_string(const PrimitiveMode & p)
{
    switch(p)
    {
        case PrimitiveMode::POINTS            :     return std::string("PrimitiveMode::POINTS");
        case PrimitiveMode::LINES             :     return std::string("PrimitiveMode::LINES");
        case PrimitiveMode::LINE_LOOP         :     return std::string("PrimitiveMode::LINE_LOOP");
        case PrimitiveMode::LINE_STRIP        :     return std::string("PrimitiveMode::LINE_STRIP");
        case PrimitiveMode::TRIANGLES         :     return std::string("PrimitiveMode::TRIANGLES");
        case PrimitiveMode::TRIANGLE_STRIP    :     return std::string("PrimitiveMode::TRIANGLE_STRIP");
        case PrimitiveMode::TRIANGLE_FAN      :     return std::string("PrimitiveMode::TRIANGLE_FAN");
    }
    return std::string("UNKNOWN");
}

class Primitive
{
public:
    struct
    {
        // node these must be in this order otherwise the has() method wont work
        uint32_t POSITION   = std::numeric_limits<uint32_t>::max();//-1;
        uint32_t NORMAL	    = std::numeric_limits<uint32_t>::max();//-1;
        uint32_t TANGENT	= std::numeric_limits<uint32_t>::max();//-1;
        uint32_t TEXCOORD_0 = std::numeric_limits<uint32_t>::max();//-1;
        uint32_t TEXCOORD_1 = std::numeric_limits<uint32_t>::max();//-1;
        uint32_t COLOR_0	= std::numeric_limits<uint32_t>::max();//-1;
        uint32_t JOINTS_0   = std::numeric_limits<uint32_t>::max();//-1;
        uint32_t WEIGHTS_0  = std::numeric_limits<uint32_t>::max();//-1;
    } attributes;

    uint32_t       indices  = std::numeric_limits<uint32_t>::max();
    uint32_t       material = std::numeric_limits<uint32_t>::max();
    PrimitiveMode  mode     = PrimitiveMode::TRIANGLES;


    template <class T>
    static inline void hash_combine(std::size_t& seed, const T& v)
    {
        std::hash<T> hasher;
        seed ^= hasher(v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
    }

    /**
     * @brief getIDType
     * @return
     *
     * Returns a hash of the primitive. The hash of two primitives will be
     * equal if:
     *  1. They both contain the same attributes (position, normal, etc)
     *  2. All attribute's use the same accessorType and componentType
     *  3. They use the same primitive mode.
     *
     * This can be used to see if two primitives can be combined.
     *
     * It can also be used to determine if a pipeline can handle
     * this type of mesh input.
     */
    size_t getIDType() const
    {
        size_t h = 0;

        for(uint32_t i = 0; i < 8; i++)
        {
            auto attr = static_cast<PrimitiveAttribute>(i);

            if( has(attr) )
            {
                auto & A = getAccessor(attr);

                hash_combine(h,  static_cast<uint32_t>(A.type) );
                hash_combine(h,  static_cast<uint32_t>(A.componentType) );
            }
        }

        if(hasIndices())
        {
            auto & A = getIndexAccessor();
            hash_combine(h,  static_cast<uint32_t>(A.type) );
            hash_combine(h,  static_cast<uint32_t>(A.componentType) );
        }

        hash_combine(h, static_cast<PrimitiveMode>(mode) );
        return h;
    }
    bool has(PrimitiveAttribute attr) const
    {
        return std::numeric_limits<uint32_t>::max() != (&attributes.POSITION)[ static_cast<uint32_t>(attr)];
    }
    bool hasIndices() const
    {
        return indices!=std::numeric_limits<uint32_t>::max();
    }
    bool hasMaterial() const
    {
        return material!=std::numeric_limits<uint32_t>::max();
    }

    /**
     * @brief getAccessor
     * @param attr
     * @return
     *
     * Gets the accessor of the input attributes. You can use this
     * to determine the component type and accessor type.
     */
    Accessor& getAccessor(PrimitiveAttribute attr);
    Accessor const & getAccessor(PrimitiveAttribute attr) const;

    /**
     * @brief getIndexAccessor
     * @return
     *
     * Returns a reference to the accessor used for index buffers.
     */
    Accessor& getIndexAccessor();
    Accessor const & getIndexAccessor() const;

    /**
     * @brief getSpan
     * @param attr
     * @return
     *
     * Returns a span to the attribute so that you can
     * access the data like it was an array
     */
    template<typename T>
    aspan<T> getSpan(PrimitiveAttribute attr)
    {
        auto & A = getAccessor(attr);

        return A.getSpan<T>();
    }

    template<typename T>
    aspan<T> getSpan(PrimitiveAttribute attr) const
    {
        auto & A = getAccessor(attr);

        return A.getSpan<T>();
    }

    /**
     * @brief getSpan
     * @param attr
     * @return
     *
     * Returns a span to the index accessor so that you can
     * access the data like it was an array. sizeof(T) must be equal to 2 or 4
     */
    template<typename T>
    aspan<T> getIndexSpan()
    {
        static_assert( sizeof(T)==2 || sizeof(T)==4, "Size of template parameter is not in accordance with GLTF2.0");
        auto & A = getIndexAccessor();

        return A.getSpan<T>();
    }

    template<typename T>
    aspan<T> getIndexSpan() const
    {
        static_assert( sizeof(T)==2 || sizeof(T)==4, "Size of template parameter is not in accordance with GLTF2.0");
        auto & A = getIndexAccessor();

        return A.getSpan<T>();
    }
private:
    GLTFModel * _parent;
    friend class GLTFModel;
};

inline void to_json(json& j, const Primitive & p)
{

    if(p.has(PrimitiveAttribute::POSITION  ) ) j["attributes"]["POSITION"  ] =  p.attributes.POSITION;
    if(p.has(PrimitiveAttribute::NORMAL    ) ) j["attributes"]["NORMAL"    ] =  p.attributes.NORMAL;
    if(p.has(PrimitiveAttribute::TANGENT   ) ) j["attributes"]["TANGENT"   ] =  p.attributes.TANGENT;
    if(p.has(PrimitiveAttribute::TEXCOORD_0) ) j["attributes"]["TEXCOORD_0"] =  p.attributes.TEXCOORD_0;
    if(p.has(PrimitiveAttribute::TEXCOORD_1) ) j["attributes"]["TEXCOORD_1"] =  p.attributes.TEXCOORD_1;
    if(p.has(PrimitiveAttribute::COLOR_0   ) ) j["attributes"]["COLOR_0"   ] =  p.attributes.COLOR_0;
    if(p.has(PrimitiveAttribute::JOINTS_0  ) ) j["attributes"]["JOINTS_0"  ] =  p.attributes.JOINTS_0;
    if(p.has(PrimitiveAttribute::WEIGHTS_0 ) ) j["attributes"]["WEIGHTS_0" ] =  p.attributes.WEIGHTS_0;

    if(p.hasIndices())
        j["indices" ] =  p.attributes.WEIGHTS_0;

    if( p.hasMaterial() )
        j["material"] = p.material;
}

inline void from_json(const json & j, Primitive & B)
{
    B.attributes.POSITION   = _getValue(j["attributes"], "POSITION",   std::numeric_limits<uint32_t>::max() );
    B.attributes.NORMAL     = _getValue(j["attributes"], "NORMAL",     std::numeric_limits<uint32_t>::max() );
    B.attributes.TANGENT    = _getValue(j["attributes"], "TANGENT",    std::numeric_limits<uint32_t>::max() );
    B.attributes.TEXCOORD_0 = _getValue(j["attributes"], "TEXCOORD_0", std::numeric_limits<uint32_t>::max() );
    B.attributes.TEXCOORD_1 = _getValue(j["attributes"], "TEXCOORD_1", std::numeric_limits<uint32_t>::max() );
    B.attributes.COLOR_0    = _getValue(j["attributes"], "COLOR_0",    std::numeric_limits<uint32_t>::max() );
    B.attributes.JOINTS_0   = _getValue(j["attributes"], "JOINTS_0",   std::numeric_limits<uint32_t>::max() );
    B.attributes.WEIGHTS_0  = _getValue(j["attributes"], "WEIGHTS_0",  std::numeric_limits<uint32_t>::max() );

    B.indices  = _getValue(j, "indices" , std::numeric_limits<uint32_t>::max() );
    B.material = _getValue(j, "material", std::numeric_limits<uint32_t>::max() );

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

inline void to_json(json& j, const Mesh & p)
{

    if( p.primitives.size() )
        j["primitives"] = p.primitives;

    if( p.name.length() )
        j["name"] = p.name;

    if( p.weights.size() )
        j["weights"] = p.weights;
}

inline void from_json(const json & j, Mesh & B)
{
    B.name           = _getValue(j, "name", std::string(""));
    B.primitives     = _getValue(j, "primitives"   , std::vector<Primitive>() );

    B.weights        = _getValue(j, "weights", std::vector<float>());
}


class Scene
{
public:
    std::vector<uint32_t> nodes;	//integer [1-*]	The indices of each root node.	No
    std::string name;           //The user-defined name of this object.	No

    //extensions	object	Dictionary object with extension-specific objects.	No
    //extras	any	Application-specific data.	No

    /**
     * @brief getRootNode
     * @param index
     * @return
     *
     * Returns the pointer to the node index.
     *
     * index must be less than nodes.size();
     */
    Node* operator[](size_t i);

private:
    GLTFModel * _parent;
    friend class GLTFModel;
};

inline void to_json(json& j, const Scene & p)
{
    if( p.name.size() )
        j["name"] = p.name;

    if( p.nodes.size() )
        j["nodes"] = p.nodes;
}

inline void from_json(const json & j, Scene & B)
{
    B.name   = _getValue(j, "name", std::string(""));
    B.nodes  = _getValue(j, "nodes", std::vector<uint32_t>());
}

class Skin
{
public:
    uint32_t              inverseBindMatrices = std::numeric_limits<uint32_t>::max();
    std::vector<uint32_t> joints;	//integer [1-*]	The indices of each root node.	No
    std::string           name;          //The user-defined name of this object.	No
    int32_t               skeleton=-1;
    //extensions	object	Dictionary object with extension-specific objects.	No
    //extras	any	Application-specific data.	No

    Node* getRootNode();

    bool hasInverseBindMatrices() const
    {
        return inverseBindMatrices!=std::numeric_limits<uint32_t>::max();
    }

    Accessor& getInverseBindMatricesAccessor();
    Accessor const & getInverseBindMatricesAccessor() const;

    template<typename T>
    aspan<T> getInverseBindMatricesSpan()
    {
        auto & A = getInverseBindMatricesAccessor();

        assert( sizeof(T) == A.accessorSize() );
        return A.getSpan<T>();
    }

    template<typename T>
    aspan< typename std::add_const<T>::type > getInverseBindMatricesSpan() const
    {
        auto & A = getInverseBindMatricesAccessor();

        assert( sizeof(T) == A.accessorSize() );
        return A.getSpan< typename std::add_const<T>::type >();
    }

private:
    GLTFModel * _parent;
    friend class GLTFModel;
};

inline void to_json(json& j, const Skin & p)
{
    if(p.hasInverseBindMatrices())
        j["inverseBindMatrices"] = p.inverseBindMatrices;

    if( p.name.size())
        j["name"] = p.name;

    if( p.joints.size())
        j["joints"] = p.joints;

    if( p.skeleton != -1)
        j["skeleton"] = p.skeleton;
}

inline void from_json(const json & j, Skin & B)
{
    B.inverseBindMatrices = _getValue(j, "inverseBindMatrices", std::numeric_limits<uint32_t>::max());
    B.name                = _getValue(j, "name", std::string(""));
    B.joints              = _getValue(j, "joints", std::vector<uint32_t>());
    B.skeleton            = _getValue(j, "skeleton", -1);
    //B.primitives      = _getValue(j, "primitives"   , std::vector<Primitive>() );

}


enum class AnimationInterpolation : int32_t
{
    STEP,
    LINEAR,
    CUBICSPLINE
};
inline std::string to_string(const AnimationInterpolation & p)
{
    switch(p)
    {
        case AnimationInterpolation::STEP:   return std::string("STEP");
        case AnimationInterpolation::LINEAR: return std::string("LINEAR");
        case AnimationInterpolation::CUBICSPLINE:  return std::string("CUBICSPLINE");
    }
    return "UNKNOWN";
}
enum class AnimationPath
{
    TRANSLATION,
    ROTATION,
    SCALE,
    WEIGHTS
};

inline std::string to_string(const AnimationPath & p)
{
    switch(p)
    {
        case AnimationPath::TRANSLATION: return std::string("TRANSLATION");
        case AnimationPath::ROTATION:    return std::string("ROTATION");
        case AnimationPath::SCALE:       return std::string("SCALE");
        case AnimationPath::WEIGHTS:     return std::string("WEIGHTS");
    }
    return "UNKNOWN";
}

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
    uint32_t                input  = std::numeric_limits<uint32_t>::max();
    uint32_t                output = std::numeric_limits<uint32_t>::max();
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

    aspan<const float> getInputSpan() const
    {
        return getInputAccessor().getSpan<const float>();
    }

    template<typename T>
    aspan<T> getOutputSpan()
    {
        return getOutputAccessor().getSpan<T>();
    }

    //template<typename T>
    //aspan<T> getOutputSpan() const
    //{
    //    return getOutputAccessor().getSpan<T>();
    //}
    template<typename T>
    aspan< typename std::add_const<T>::type > getOutputSpan() const
    {
        return getOutputAccessor().getSpan< typename std::add_const<T>::type >();
    }

    Accessor& getInputAccessor();
    Accessor& getOutputAccessor();

    Accessor const & getInputAccessor() const ;
    Accessor const & getOutputAccessor() const;
private:
    GLTFModel * _parent;
    friend class GLTFModel;
    friend class Animation;
};

inline void to_json(json& j, const AnimationSampler & p)
{
    j["input"]  = p.input;
    j["output"] = p.output;
    j["interpolation"] = to_string(p.interpolation);
}

inline void from_json(const json & j, AnimationSampler & B)
{
    B.input = _getValue(j,  "input",  std::numeric_limits<uint32_t>::max() );
    B.output = _getValue(j, "output", std::numeric_limits<uint32_t>::max() );

    auto interpolation = _getValue( j, "interpolation", std::string() );

    if( interpolation == "STEP")         B.interpolation = AnimationInterpolation::STEP;
    if( interpolation == "LINEAR")       B.interpolation = AnimationInterpolation::LINEAR;
    if( interpolation == "CUBICSPLINE")  B.interpolation = AnimationInterpolation::CUBICSPLINE;
}



class AnimationChannel
{
public:
    uint32_t              sampler = std::numeric_limits<uint32_t>::max();

    struct
    {
        uint32_t      node = std::numeric_limits<uint32_t>::max();
        AnimationPath path;
    } target;

private:
    GLTFModel * _parent;
    friend class GLTFModel;
    friend class Animation;
};


inline void to_json(json& j, const AnimationChannel & p)
{
    j["sampler"] = p.sampler;
    j["target"]["path"] = to_string(p.target.path);
    j["target"]["node"] = p.target.node;
}

inline void from_json(const json & j, AnimationChannel & B)
{
    B.sampler = _getValue(j, "sampler", std::numeric_limits<uint32_t>::max());

    auto path = _getValue(j["target"], "path", std::string() );

    if( path == "translation") B.target.path = AnimationPath::TRANSLATION;
    if( path == "rotation")    B.target.path = AnimationPath::ROTATION;
    if( path == "scale")       B.target.path = AnimationPath::SCALE;
    if( path == "weights")     B.target.path = AnimationPath::WEIGHTS;

    B.target.node = _getValue(j["target"], "node", std::numeric_limits<uint32_t>::max());
    //B.primitives      = _getValue(j, "primitives"   , std::vector<Primitive>() );

}

class Animation
{
public:
    std::vector<AnimationChannel> channels;
    std::vector<AnimationSampler> samplers;
    std::string                   name;          //The user-defined name of this object.	No

    AnimationSampler & newSampler()
    {
        auto & S = samplers.emplace_back();
        S._parent = _parent;
    }
    AnimationChannel & newChannel()
    {
        auto & S = channels.emplace_back();
        S._parent = _parent;
    }


private:
    GLTFModel * _parent;
    friend class GLTFModel;

};

inline void to_json(json& j, const Animation & p)
{
    j["channels"] = p.channels;
    j["samplers"] = p.samplers;
    if(p.name.size()) j["name"]     = p.name;
}

inline void from_json(const json & j, Animation & B)
{
    B.channels            = _getValue(j, "channels", std::vector<AnimationChannel>());
    B.samplers            = _getValue(j, "samplers", std::vector<AnimationSampler>());
    B.name                = _getValue(j, "name", std::string(""));
}


class Image
{
public:
    // MimeType	string	The image's MIME type.	No
    // bufferView	integer	The index of the bufferView that contains the image. Use this instead of the image's uri property.	No

    uint32_t    bufferView = std::numeric_limits<uint32_t>::max();
    std::string uri;
    std::string mimeType;
    std::string name;

    /**
     * @brief getSpan
     * @return
     *
     * Returns the span of the data. Image data will always be
     * contigious.
     *
     * Note: The data returned in the span is ENCODED data based on the
     * mimeType. It is YOUR responsibility to decode the data.
     *
     * For example using stb:
     *
     * auto span = Img.getSpan();
     *
     * int width,height,channels;
     *
     * auto decodedData = stbi_load_from_memory( &span[0], span.size(), &width, &height, &channels,4 );
     */
    aspan<uint8_t> getSpan();
    aspan<const uint8_t> getSpan() const;

    aspan<uint8_t> data();

    BufferView       & getBufferView();
    BufferView const & getBufferView() const;
private:
    GLTFModel * _parent;
    friend class GLTFModel;

};

inline void to_json(json& j, const Image & p)
{
    if( p.uri.size())
        j["uri"] = p.uri;

    if(p.mimeType.size())
        j["mimeType"] = p.mimeType;

    if(p.bufferView != std::numeric_limits<uint32_t>::max())
        j["bufferView"] = p.bufferView;

    if( p.name.size() )
        j["name"] = p.name;
}

inline void from_json(const json & j, Image & B)
{
    B.uri         = _getValue(j, "uri", std::string(""));
    B.mimeType    = _getValue(j, "mimeType", std::string(""));
    B.bufferView  = _getValue(j, "bufferView", std::numeric_limits<uint32_t>::max());
    B.name        = _getValue(j, "name", std::string(""));
}

class Image;

class Sampler;

class Texture
{
public:
    uint32_t     sampler=std::numeric_limits<uint32_t>::max();
    uint32_t     source =std::numeric_limits<uint32_t>::max();
    std::string name;

    Sampler & getSampler();
    Sampler const & getSampler() const;

    Image & getImage();
    Image const & getImage() const;

private:
    GLTFModel * _parent;
    friend class GLTFModel;
};

inline void to_json(json& j, const Texture & p)
{
    if(p.source != std::numeric_limits<uint32_t>::max())
        j["source"] = p.source;

    if(p.sampler != std::numeric_limits<uint32_t>::max())
        j["sampler"] = p.sampler;

    if( p.name.size() )
        j["name"] = p.name;
}


inline void from_json(const json & j, Texture & B)
{
    B.source  = _getValue(j, "source",  std::numeric_limits<uint32_t>::max() );
    B.sampler = _getValue(j, "sampler", std::numeric_limits<uint32_t>::max() );
    B.name    = _getValue(j, "name", std::string(""));
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


class Sampler
{
public:
    Filter   magFilter;
    Filter   minFilter;
    WrapMode wrapS;
    WrapMode wrapT;

    std::string name;
};

inline void to_json(json& j, const Sampler & p)
{
    j["magFilter"] = static_cast<uint32_t>(p.magFilter);
    j["minFilter"] = static_cast<uint32_t>(p.minFilter);
    j["wrapS"]     = static_cast<uint32_t>(p.wrapS);
    j["wrapT"]     = static_cast<uint32_t>(p.wrapT);

    if( p.name.size() )
        j["name"] = p.name;
}

inline void from_json(const json & j, Sampler & B)
{
    B.magFilter    = static_cast<Filter>( _getValue(j, "magFilter", 9729u) ); // magic values defined by spec
    B.minFilter    = static_cast<Filter>( _getValue(j, "minFilter", 9729u) ); // magic values defined by spec
    B.wrapS        = static_cast<WrapMode>( _getValue(j, "wrapS",  10497u) ); // magic values defined by spec
    B.wrapT        = static_cast<WrapMode>( _getValue(j, "wrapT",  10497u) ); // magic values defined by spec
    B.name    = _getValue(j, "name", std::string(""));
}

class TextureInfo
{
public:
    uint32_t index    = std::numeric_limits<uint32_t>::max();
    uint32_t texCoord = 0;

    float   scale    = std::nanf("");        // used for other textures
    float   strength = std::nanf("");;     // used for occlusion Textures

    operator bool() const
    {
        return index != std::numeric_limits<uint32_t>::max();;
    }
};


inline void to_json( json & j, TextureInfo const & B)
{
    j["index"]    = B.index   ;// = _getValue(j, "index",   std::numeric_limits<uint32_t>::max());
    j["texCoord"] = B.texCoord;// = _getValue(j, "texCoord", 0u );

    if( !std::isnan(B.scale) )
        j["scale"] = B.scale;

    if( !std::isnan(B.strength) )
        j["strength"] = B.strength;
}

inline void from_json(const json & j, TextureInfo & B)
{
    B.index    = _getValue(j, "index",   std::numeric_limits<uint32_t>::max());
    B.texCoord = _getValue(j, "texCoord", 0u );

    if( j.count("scale") == 1)
    {
        B.scale = j["scale"].get<float>();
    }
    else if( j.count("strength") == 1)
    {
        B.strength = j["strength"].get<float>();
    }
    else
    {
        B.strength = 1.0f;
    }
}


enum class MaterialAlphaMode
{
    OPAQUE,
    MASK,
    BLEND
};

std::string to_string(MaterialAlphaMode d)
{
    switch(d)
    {
        case MaterialAlphaMode::OPAQUE:      return std::string("OPAQUE");
        case MaterialAlphaMode::MASK  :      return std::string("MASK");
        case MaterialAlphaMode::BLEND :      return std::string("BLEND");
        default: return std::string("OPAQUE");
    }

}
class Material
{
public:

    struct
    {
        std::array<float, 4> baseColorFactor = {1,1,1,1};
        TextureInfo          baseColorTexture;
        float                metallicFactor  = 1.0f;
        float                roughnessFactor = 1.0f;
        TextureInfo          metallicRoughnessTexture;

        operator bool() const
        {
            return _has;
        }

        bool hasBaseColorTexture() const
        {
            return static_cast<bool>(baseColorTexture);
        }
        bool hasMetallicRoughnessTexture() const
        {
            return static_cast<bool>(metallicRoughnessTexture);
        }
    private:
        bool              _has=false;
        friend void from_json(const json & j, Material & B);
    } pbrMetallicRoughness;

    TextureInfo          normalTexture;
    TextureInfo          occlusionTexture;
    TextureInfo          emissiveTexture;

    std::array<float,3>  emissiveFactor = {0,0,0};

    MaterialAlphaMode alphaMode = MaterialAlphaMode::OPAQUE;
    float alphaCutoff = 0.5f;
    bool  doubleSided = false;

    bool hasPBR() const
    {
        return static_cast<bool>(pbrMetallicRoughness);
    }
    bool hasNormalTexture() const
    {
        return static_cast<bool>(normalTexture);
    }
    bool hasOcclusionTexture() const
    {
        return static_cast<bool>(occlusionTexture);
    }
    bool hasEmissiveTexture() const
    {
        return static_cast<bool>(emissiveTexture);
    }
private:
  //  GLTFModel * _parent;
    friend class GLTFModel;
};

inline void to_json(json & j, Material const & B)
{
    if( B.hasPBR() )
    {

        if( B.pbrMetallicRoughness.hasBaseColorTexture())
            j["pbrMetallicRoughness"]["baseColorTexture"]          = B.pbrMetallicRoughness.baseColorTexture;

        if( B.pbrMetallicRoughness.hasMetallicRoughnessTexture())
            j["pbrMetallicRoughness"]["metallicRoughnessTexture"]  = B.pbrMetallicRoughness.metallicRoughnessTexture;

        j["pbrMetallicRoughness"]["baseColorFactor"] = B.pbrMetallicRoughness.baseColorFactor;
        j["pbrMetallicRoughness"]["roughnessFactor"] = B.pbrMetallicRoughness.roughnessFactor;
        j["pbrMetallicRoughness"]["metallicFactor"]  = B.pbrMetallicRoughness.metallicFactor;
    }

    if( B.hasNormalTexture() )
        j["normalTexture"] = B.normalTexture;

    if( B.hasEmissiveTexture() )
        j["emissiveTexture"] = B.emissiveTexture;

    if( B.hasOcclusionTexture() )
        j["occlusionTexture"] = B.occlusionTexture;

    j["emissiveFactor"] = B.emissiveFactor;

    j["alphaMode"]   = to_string(B.alphaMode);
    j["alphaCutoff"] = B.alphaCutoff;
    j["doubleSided"] = B.doubleSided;

}

inline void from_json(const json & j, Material & B)
{
    B.pbrMetallicRoughness._has = false;
    if( j.count("pbrMetallicRoughness")==1)
    {
        B.pbrMetallicRoughness.baseColorFactor = _getValue( j["pbrMetallicRoughness"], "baseColorFactor", std::array<float,4>({1,1,1,1}));
        B.pbrMetallicRoughness.metallicFactor  = _getValue( j["pbrMetallicRoughness"], "metallicFactor" , 1.0f);
        B.pbrMetallicRoughness.roughnessFactor = _getValue( j["pbrMetallicRoughness"], "roughnessFactor", 1.0f);

        B.pbrMetallicRoughness.baseColorTexture          = _getValue(j["pbrMetallicRoughness"], "baseColorTexture", TextureInfo{});
        B.pbrMetallicRoughness.metallicRoughnessTexture  = _getValue(j["pbrMetallicRoughness"], "metallicRoughnessTexture", TextureInfo{});
        B.pbrMetallicRoughness._has = true;
    }

    B.emissiveFactor = _getValue(j, "emissiveFactor", std::array<float,3>({0,0,0}));
    B.emissiveTexture = _getValue(j, "emissiveTexture", TextureInfo{});

    B.normalTexture    = _getValue(j, "normalTexture", TextureInfo{});
    B.occlusionTexture = _getValue(j, "occlusionTexture", TextureInfo{});

    B.alphaCutoff        = _getValue(j, "alphaCutoff", 0.5f);
    B.doubleSided        = _getValue(j, "doubleSided", false);

    auto alphaMode = _getValue(j, "alphaMode", std::string("OPAQUE"));

    if( alphaMode == "OPAQUE")
        B.alphaMode = MaterialAlphaMode::OPAQUE;
    if( alphaMode == "MASK")
        B.alphaMode = MaterialAlphaMode::MASK;
    if( alphaMode == "BLEND")
        B.alphaMode = MaterialAlphaMode::BLEND;

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

    GLTFModel()
    {
    }

    explicit GLTFModel( std::istream & i )
    {
        load(i);
    }

    GLTFModel(GLTFModel && other)
    {
         accessors   = std::move(  other.accessors);
         buffers     = std::move(  other.buffers);
         bufferViews = std::move(  other.bufferViews);
         nodes       = std::move(  other.nodes);
         meshes      = std::move(  other.meshes);
         scenes      = std::move(  other.scenes);
         skins       = std::move(  other.skins);
         animations  = std::move(  other.animations);
         images      = std::move(  other.images);
         textures    = std::move(  other.textures);
         samplers    = std::move(  other.samplers);
         cameras     = std::move(  other.cameras);
         materials   = std::move(  other.materials);

         _setParents(this);
    }
    GLTFModel(GLTFModel const & other)
    {
         accessors   = other.accessors;
         buffers     = other.buffers;
         bufferViews = other.bufferViews;
         nodes       = other.nodes;
         meshes      = other.meshes;
         scenes      = other.scenes;
         skins       = other.skins;
         animations  = other.animations;
         images      = other.images;
         textures    = other.textures;
         samplers    = other.samplers;
         cameras     = other.cameras;
         materials   = other.materials;

         _setParents(this);
    }

    GLTFModel & operator=(GLTFModel const & other)
    {
         if( this != &other)
         {
             accessors   = other.accessors;
             buffers     = other.buffers;
             bufferViews = other.bufferViews;
             nodes       = other.nodes;
             meshes      = other.meshes;
             scenes      = other.scenes;
             skins       = other.skins;
             animations  = other.animations;
             images      = other.images;
             textures    = other.textures;
             samplers    = other.samplers;
             cameras     = other.cameras;
             materials   = other.materials;

             _setParents(this);
         }
         return *this;
    }
    GLTFModel & operator=(GLTFModel && other)
    {
         if( this != &other)
         {
             accessors   = std::move(  other.accessors);
             buffers     = std::move(  other.buffers);
             bufferViews = std::move(  other.bufferViews);
             nodes       = std::move(  other.nodes);
             meshes      = std::move(  other.meshes);
             scenes      = std::move(  other.scenes);
             skins       = std::move(  other.skins);
             animations  = std::move(  other.animations);
             images      = std::move(  other.images);
             textures    = std::move(  other.textures);
             samplers    = std::move(  other.samplers);
             cameras     = std::move(  other.cameras);
             materials   = std::move(  other.materials);

             _setParents(this);
         }
         return *this;
    }

    void _setParents(GLTFModel * parent)
    {
        for(auto & v :  buffers) { v._parent=parent;}
        for(auto & v :  bufferViews) { v._parent=parent;}
        for(auto & v :  accessors  ) { v._parent=parent;}

        for(auto & v :  nodes      ) { v._parent=parent;}
        for(auto & v :  scenes     ) { v._parent=parent;}
        for(auto & v :  skins      ) { v._parent=parent;}

        for(auto & v :  images     ) { v._parent=parent;}
        for(auto & v :  textures   ) { v._parent=parent;}
        //for(auto & v :  samplers   ) { v._parent=this;};
        //for(auto & v :  cameras    ) { v._parent=this;};
        //for(auto & v :  materials  ) { v._parent=this;};

        for(auto & v :  meshes     )
        {
            v._parent=parent;
            for(auto & p :  v.primitives )
            {
                p._parent=parent;
            }
        }
        for(auto & v :  animations     )
        {
            v._parent=parent;
            for(auto & p :  v.samplers )
            {
                p._parent=parent;
            }
            for(auto & p :  v.channels )
            {
                p._parent=parent;
            }
        }
    }


    bool load( std::istream & i)
    {
        auto header = _readHeader(i);
        auto jsonChunk = _readChunk(i);

        if(header.magic != 0x46546C67)
        {
            return false;
        }
        if( header.version < 2)
        {
            return false;
        }
        jsonChunk.chunkData.push_back(0);
        _json = _parseJson(  reinterpret_cast<char*>(jsonChunk.chunkData.data()) );
        auto & J = _json;

        if(J.count("asset") == 1)
        {
            asset = J["asset"].get<Asset>();
        }

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
                //samplers.back()._parent = this;
            }
        }
        if(J.count("cameras") == 1)
        {
            for(auto & b : J["cameras"] )
            {
               // std::cout << b.dump(4) << std::endl;
                auto B = b.get<Camera>();
               //
                cameras.emplace_back( std::move(B) );
            }
        }
        if(J.count("materials") == 1)
        {
            for(auto & b : J["materials"] )
            {
                //std::cout << b.dump(4) << std::endl;
                auto B = b.get<Material>();
               ////
                materials.emplace_back( std::move(B) );
            }
        }
        return true;
    }




    /**
     * @brief generateJSON
     * @return
     *
     * Generates the JSON component of the GLB file.
     */
    json generateJSON() const
    {
        json root;

        auto & J = root;

        J["asset"]       =  asset;
        J["accessors"]   =  accessors;
        J["bufferViews"] =  bufferViews;
        J["nodes"]       =  nodes;
        J["meshes"]      =  meshes;
        J["scenes"]      =  scenes;
        J["skins"]       =  skins;
        J["animations"]  =  animations;
        J["images"]      =  images;
        J["textures"]    =  textures;
        J["samplers"]    =  samplers;
        J["cameras"]     =  cameras;
        J["materials"]   =  materials;

        return root;
    }

    /**
     * @brief writeGLB
     * @param out
     *
     * Writes the GLTF file to an output stream in GLB format
     */
    void writeGLB(std::ostream & out) const
    {
        uint32_t magic   = 0x46546C67;
        uint32_t version = 2;
        uint32_t length  = 12;

        auto j = generateJSON();

        std::string j_str = j.dump(4);
        std::cout << j_str << std::endl;
        length += 8;
        length += j_str.size();

        for(auto & c : buffers)
        {
            length += c.m_data.size() + 8;
        }

        out.write( reinterpret_cast<char*>(&magic  ) , sizeof(magic));
        out.write( reinterpret_cast<char*>(&version) , sizeof(version));
        out.write( reinterpret_cast<char*>(&length ) , sizeof(length));

        {
            uint32_t chunkLength = static_cast<uint32_t>(j_str.size());
            uint32_t chunkType   = 0x4E4F534A; // json

            out.write( reinterpret_cast<char*>(&chunkLength) , sizeof(chunkLength));
            out.write( reinterpret_cast<char*>(&chunkType ) , sizeof(chunkType));
            out.write( j_str.data(), j_str.size());
        }

        for(auto & c : buffers)
        {
            _writeVectorBuffer( c.m_data, out );
        }

    }


    Buffer & newBuffer()
    {
        auto & b = buffers.emplace_back();
        b._parent = this;
        return b;
    }

    Animation& newAnimation()
    {
        auto & b = animations.emplace_back();
        b._parent = this;
        return b;
    }

    template<typename T>
    static void _writeVectorBuffer(const std::vector<T> & chunk, std::ostream & out)
    {
        _writeBuffer(chunk.data(), sizeof(T)*chunk.size(), out);
    }
    /**
     * @brief _writeBuffer
     * @param data
     * @param bytes
     * @param out
     *
     * Writes a buffer to the stream using the glb format specifications
     */
    static void _writeBuffer(const void * data, size_t bytes, std::ostream &out )
    {
        uint32_t chunkLength = static_cast<uint32_t>(bytes);
        uint32_t chunkType   = 0x004E4942;

        char four[4];
        memcpy(four,&chunkLength,4);
        out.write( four, 4 );
        memcpy(four,&chunkType,4);
        out.write( four, 4 );
        out.write( static_cast<char const*>(data), bytes);
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
    static json _parseJson( char * data)
    {
        json J;

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
    static std::vector<Buffer> _readBuffers(std::istream & in, json const & jBuffers)
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
    static std::vector<Buffer> _extractBuffers(chunk_t const & buffersChunk, json const & jBuffers)
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
    Asset                   asset;
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
    std::vector<Camera>     cameras;
    std::vector<Material>   materials;

    json                    _json;
};


inline Node* Node::getChild(size_t childIndex)
{
    return &_parent->nodes.at(  children[ childIndex ]  );
}

inline Node const* Node::getChild(size_t childIndex) const
{
    return &_parent->nodes.at(  children[ childIndex ]  );
}

inline Accessor& Primitive::getIndexAccessor()
{
    if( hasIndices() )
    {
        return _parent->accessors[ indices ];
    }
    throw std::runtime_error("This primitive does not have an index buffer");
}

inline Accessor const & Primitive::getIndexAccessor() const
{
    if( hasIndices() )
    {
        return _parent->accessors.at( static_cast<size_t>(indices) );
    }
    throw std::runtime_error("This primitive does not have an index buffer");
}

inline Accessor& Primitive::getAccessor(PrimitiveAttribute attr)
{
    const auto acessorIndex = (&attributes.POSITION)[ static_cast<int32_t>(attr)];
    return _parent->accessors.at( static_cast<size_t>(acessorIndex) );
}

inline Accessor const & Primitive::getAccessor(PrimitiveAttribute attr) const
{
    const auto acessorIndex = (&attributes.POSITION)[ static_cast<int32_t>(attr)];
    return _parent->accessors.at( static_cast<size_t>(acessorIndex) );
}

inline Node* Scene::operator[](size_t i)
{
    return &_parent->nodes.at( static_cast<size_t>(nodes.at(i)) );
}

inline Accessor& AnimationSampler::getInputAccessor()
{
    return _parent->accessors.at( static_cast<size_t>(input) );
}

inline Accessor& AnimationSampler::getOutputAccessor()
{
    return _parent->accessors.at( static_cast<size_t>(output) );
}

inline Accessor const& AnimationSampler::getInputAccessor() const
{
    return _parent->accessors.at( static_cast<size_t>(input) );
}

inline Accessor const & AnimationSampler::getOutputAccessor() const
{
    return _parent->accessors.at( static_cast<size_t>(output) );
}

inline Buffer &BufferView::getBuffer()
{
    return _parent->buffers.at( static_cast<size_t>(buffer) );
}

inline Buffer const & BufferView::getBuffer() const
{
    return _parent->buffers.at( static_cast<size_t>(buffer) );
}

inline void* BufferView::data()
{
    return getBuffer().m_data.data() + byteOffset;
}

inline void const * BufferView::data() const
{
    return getBuffer().m_data.data() + byteOffset;
}

inline BufferView const & Accessor::getBufferView() const
{
    return _parent->bufferViews.at( static_cast<size_t>(bufferView) );
}

inline BufferView &       Accessor::getBufferView()
{
    return _parent->bufferViews.at( static_cast<size_t>(bufferView) );
}

template<typename T>
inline aspan<T> Accessor::getSpan()
{
    auto & bv = getBufferView();

    auto stride = accessorSize();

    if( stride < sizeof(T) )
    {
        throw std::runtime_error( std::string("The stride listed in the accessor, ") + std::to_string(stride) + ", is less size of the template parameter, " + std::to_string( sizeof(T)) + ". Your data will overlap");
    }

    return
    aspan<T>( static_cast<unsigned char*>(bv.data())+byteOffset,
              count,
              stride);
}

template<typename T>
inline aspan< typename std::add_const<T>::type > Accessor::getSpan() const
{
    auto & bv = getBufferView();

    auto stride = accessorSize();

    if( stride < sizeof(T) )
    {
        throw std::runtime_error( std::string("The stride listed in the accessor, ") + std::to_string(stride) + ", is less size of the template parameter, " + std::to_string( sizeof(T)) + ". Your data will overlap");
    }

    return
    aspan<typename std::add_const<T>::type>( static_cast<const unsigned char*>(bv.data())+byteOffset,
              count,
              stride);
}


template<typename T>
inline aspan<T> BufferView::getSpan()
{
    auto d  = getBuffer().m_data.data() + byteOffset;

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

    auto count = static_cast<size_t>(byteLength) / stride;
    return aspan<T>(d, count, stride);
}

template<typename T>
inline aspan< typename std::add_const<T>::type > BufferView::getSpan() const
{
    auto d  = getBuffer().m_data.data() + byteOffset;

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

    auto count = static_cast<size_t>(byteLength) / stride;
    return aspan< typename std::add_const<T>::type >(d, count, stride);
}

inline aspan<uint8_t> Image::getSpan()
{
    return getBufferView().getSpan<uint8_t>();
}

inline aspan<const uint8_t> Image::getSpan() const
{
    return getBufferView().getSpan<const uint8_t>();
}

inline BufferView       & Image::getBufferView()
{
    return _parent->bufferViews.at( static_cast<size_t>(bufferView ) );
}

inline BufferView const & Image::getBufferView() const
{
    return _parent->bufferViews.at( static_cast<size_t>(bufferView ) );
}


inline Mesh const   & Node::getMesh() const
{
    return _parent->meshes.at( static_cast<size_t>(mesh) );
}
inline Skin const   & Node::getSkin() const
{
    return _parent->skins.at(static_cast<size_t>(skin));
}
inline Camera const & Node::getCamera() const
{
    return _parent->cameras.at(static_cast<size_t>(camera));
}
inline Mesh         & Node::getMesh()
{
    return _parent->meshes.at( static_cast<size_t>(mesh));
}
inline Skin         & Node::getSkin()
{
    return _parent->skins.at( static_cast<size_t>(skin));
}
inline Camera       & Node::getCamera()
{
    return _parent->cameras.at(static_cast<size_t>(camera));
}

inline Sampler       & Texture::getSampler()
{
    return _parent->samplers.at( static_cast<size_t>(sampler));
}

inline Sampler const & Texture::getSampler() const
{
    return _parent->samplers.at( static_cast<size_t>(sampler));
}

inline Image       & Texture::getImage()
{
    return _parent->images.at( static_cast<size_t>(source));
}

inline Image const & Texture::getImage() const
{
    return _parent->images.at( static_cast<size_t>(source));
}

inline Accessor&        Skin::getInverseBindMatricesAccessor()
{
    return _parent->accessors.at( static_cast<size_t>(inverseBindMatrices ));
}

inline Accessor const & Skin::getInverseBindMatricesAccessor() const
{
    return _parent->accessors.at( static_cast<size_t>(inverseBindMatrices ));
}

void Accessor::copyDataFrom(Accessor const & A)
{
    uint8_t * dst       = static_cast<uint8_t*>( getBufferView().data() );

    uint8_t const * src = static_cast<uint8_t const*>( A.getBufferView().data() ) + A.byteOffset;

    size_t srcStride=0;
    // byte stride has not been set by the bufferView, so
    // to get to the next element,
    if( A.getBufferView().byteStride == 0)
    {
        srcStride = A.accessorSize();
    }
    else
    {
        if( A.getBufferView().byteStride % A.accessorSize() != 0)
        {
            throw std::runtime_error("The byteStride for the bufferView is not a multiple of the AccessorSize!");
        }
    }

    auto totalBytesForData = A.accessorSize() * A.count;
    if( getBufferView().byteLength < totalBytesForData   )
    {
        throw std::runtime_error("This destinationAccessor does not have enough space for the data");
    }

    auto count = A.count;
    auto elmentSize = A.accessorSize();
    auto dstStride = elmentSize;

    while(count--)
    {
        std::memcpy( dst, src, elmentSize);

        dst += dstStride;
        src += srcStride;
    }

}

inline size_t Buffer::createNewAccessor(Accessor const & A)
{
    auto i = createNewAccessor(A.count, A.type, A.componentType);

    auto & B = _parent->accessors[i];
    B.min = A.min;
    B.max = A.max;
    B.normalized = A.normalized;
    B.name = A.name;

    B.copyDataFrom(A);

    return i;
}

inline size_t Buffer::createNewAccessor(size_t count, AccessorType type, ComponentType comp)
{
    // reserve data for the new accessor
    auto offset = m_data.size();


    auto & newBufferView = _parent->bufferViews.emplace_back();
    auto & newAccessor   = _parent->accessors.emplace_back();

    newBufferView._parent = _parent;
    newAccessor._parent = _parent;

    newAccessor.bufferView = _parent->bufferViews.size()-1;
    newAccessor.byteOffset=0; // byte offset from start of bufferview?
    newAccessor.count=count;
    newAccessor.componentType = comp;
    newAccessor.type = type;
    //newAccessor.normalized;

    auto bytes = newAccessor.accessorSize() * count;

    newBufferView.buffer = _parent->buffers.size()-1;
    newBufferView.byteLength = bytes;
    newBufferView.byteOffset = offset;
    newBufferView.byteStride = 0;

    m_data.insert( m_data.end(), bytes, 0);
    byteLength += bytes;

    return _parent->accessors.size()-1;
}

}

#endif
