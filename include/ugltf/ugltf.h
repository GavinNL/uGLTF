#ifndef UGLTF_H
#define UGLTF_H

#ifndef UGLTF_JSON_INCLUDE
    #define UGLTF_JSON_INCLUDE <nlohmann/json.hpp>
#endif

#include UGLTF_JSON_INCLUDE

#include <iostream>
#include <type_traits>

#ifndef UGLTF_NAMESPACE
    #define UGLTF_NAMESPACE uGLTF
    #define UGLTF_JSON_CLASS nlohmann::json
#endif

#if 1
#define TRACE(...)
#else
#include <spdlog/spdlog.h>
#define TRACE(...) spdlog::info(__VA_ARGS__)
#endif


#ifndef UGLTF_PRIVATE
    #define UGLTF_PRIVATE private
#endif

#ifndef UGLTF_PROTECTED
    #define UGLTF_PROTECTED protected
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

using json = UGLTF_JSON_CLASS;

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


    using void_type      = typename std::conditional< std::is_const<value_type>::value, const void, void>::type;
    using char_type      = typename std::conditional< std::is_const<value_type>::value, const unsigned char, unsigned char>::type;

    aspan(void_type * data, size_t size, size_t stride) :
        _begin( static_cast<char_type*>(data) ),
        _size(size),
        _stride(stride)
    {

    }


    value_type  back() const
    {
        return get( size() - 1);
    }

    value_type front() const
    {
        return get( 0);
    }


    size_t size() const
    {
        return _size;
    }

    size_t stride() const
    {
        return _stride;
    }

    /**
     * @brief get
     * @param index
     * @return
     *
     * Returns the value of the data stored in index n.
     *
     * You should use this method rather than indexing using the []
     * operator as this does not invoke undefined behaviour
     */
    value_type get(size_t i) const
    {
        typename std::remove_cv<value_type>::type c;
        std::memcpy(&c, _begin+_stride*i, sizeof(value_type));
        return c;
    }

    /**
     * @brief set
     * @param i
     * @param v
     *
     * Sets the value at index, i, to v.
     *
     * You should use this method rather than indexing using the []
     * operator as this does not invoke undefined behaviour
     */
    void set(size_t i, value_type const & v)
    {
        std::memcpy(_begin+_stride*i, &v, sizeof(value_type));
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


// https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp

inline bool is_base64(unsigned char c) {
  return (isalnum(c) || (c == '+') || (c == '/'));
}

inline std::string _toBase64( void const* src, void const* src_end)
{
    static const std::string base64_chars =
                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                 "abcdefghijklmnopqrstuvwxyz"
                 "0123456789+/";

    auto bytes_to_encode     = static_cast<unsigned char const*>(src);
    auto bytes_to_encode_end = static_cast<unsigned char const*>(src_end);

    unsigned int in_len = bytes_to_encode_end - bytes_to_encode;

  std::string ret;

  int i = 0;
  int j = 0;
  unsigned char char_array_3[3];
  unsigned char char_array_4[4];

  while (in_len--) {
    char_array_3[i++] = *(bytes_to_encode++);
    if (i == 3) {
      char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
      char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
      char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
      char_array_4[3] = char_array_3[2] & 0x3f;

      for(i = 0; (i <4) ; i++)
        ret += base64_chars[char_array_4[i]];
      i = 0;
    }
  }

  if (i)
  {
    for(j = i; j < 3; j++)
      char_array_3[j] = '\0';

    char_array_4[0] = ( char_array_3[0] & 0xfc) >> 2;
    char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
    char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);

    for (j = 0; (j < i + 1); j++)
      ret += base64_chars[char_array_4[j]];

    while((i++ < 3))
      ret += '=';

  }

  return ret;

}

inline std::vector<uint8_t> _fromBase64( void const* src, void const* src_end)
{
    static const std::string base64_chars =
                 "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                 "abcdefghijklmnopqrstuvwxyz"
                 "0123456789+/";

    auto bytes_to_encode     = static_cast<unsigned char const*>(src);
    auto bytes_to_encode_end = static_cast<unsigned char const*>(src_end);

    auto encoded_string = bytes_to_encode;
    int in_len = bytes_to_encode_end - bytes_to_encode;
  //int in_len = encoded_string.size();
  int i = 0;
  int j = 0;
  int in_ = 0;
  unsigned char char_array_4[4], char_array_3[3];
  std::vector<uint8_t> ret;

  while (in_len-- && ( encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
    char_array_4[i++] = encoded_string[in_]; in_++;
    if (i ==4) {
      for (i = 0; i <4; i++)
        char_array_4[i] = base64_chars.find(char_array_4[i]);

      char_array_3[0] = ( char_array_4[0] << 2       ) + ((char_array_4[1] & 0x30) >> 4);
      char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
      char_array_3[2] = ((char_array_4[2] & 0x3) << 6) +   char_array_4[3];

      for (i = 0; (i < 3); i++)
        ret.push_back( char_array_3[i] );
      i = 0;
    }
  }

  if (i) {
    for (j = 0; j < i; j++)
      char_array_4[j] = base64_chars.find(char_array_4[j]);

    char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
    char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);

    for (j = 0; (j < i - 1); j++)
        ret.push_back( char_array_3[j] );
  }

  return ret;
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
    INT             = 5124,
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

#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2)    << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif
}

class Accessor;
class BufferView;


/**
 * @brief The Buffer class
 *
 * The Buffer class is used to store raw data. It does not
 * hold any type information.
 */
class Buffer
{
    public:
        std::string          uri;
        uint32_t             byteLength;

        std::vector<uint8_t> m_data;

        /**
         * @brief createNewBufferView
         * @param bytes
         * @param alignment
         * @return
         *
         * Create a new bufferView from this buffer. The data will be
         * appended to the end of the buffer.
         */
        size_t createNewBufferView(size_t bytes, BufferViewTarget target, size_t stride, size_t alignment);


        /**
         * @brief createNewAccessor
         * @param byteOffset
         * @param count
         * @param type
         * @param comp
         * @return
         *
         * Creates a new accessor AND bufferView within this buffer. This createsa  bufferView which
         * holds only a single accessor
         *
         * Returns the index of the Accessor
         */
        size_t createNewAccessor(size_t count,  BufferViewTarget target, uint32_t bufferViewStride, uint32_t bufferViewAlignment, AccessorType type, ComponentType comp);



        void append(Buffer const & B)
        {
            // add this buffer to the end of the new buffer
            m_data.insert( m_data.end(), B.m_data.begin(), B.m_data.end() );
            byteLength = m_data.size();
        }
private:
    GLTFModel * _parent = nullptr;
    friend class GLTFModel;
};


inline void to_json(json& j, const Buffer & p)
{
   j = json{
            {"byteLength", p.m_data.size()}
           };
}

inline void from_json(const json & j, Buffer & B)
{
    std::string uri = _getValue(j, "uri"    , std::string(""));

    B.byteLength     = _getValue(j, "byteLength"    , 0u);

    if( uri != "" )
    {
        auto comma = std::find( uri.begin(), uri.end(), ',');
        if( comma == uri.end() ) // is a file
        {
            // uri is a file
            B.uri = std::move(uri);
        }
        else
        {
            //image/png
            //image/jpeg
            auto colon     = std::find( uri.begin(), comma, ':');
            auto semicolon = std::find( uri.begin(), comma, ';');

            if( colon != comma && semicolon != comma )
            {
                auto mimeType = std::string(colon+1, semicolon);

                assert( mimeType == "application/octet-stream");
            }

            TRACE("Decoding Base64 buffer");
            // uri is base64 encoded
            auto i = std::distance(uri.begin(), comma);
            B.m_data = _fromBase64( &uri[i+1], &uri.back() +1 );

            assert( B.m_data.size() == B.byteLength);
        }
    }
    B.m_data.resize(B.byteLength);
#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif
}

/**
 * @brief The BufferView class
 *
 * The BufferView class specifies a subset of a buffer. It is
 * essentially the buffer reference and the offset from the start
 * of the buffer.
 *
 * The BufferView can also have a BufferViewTarget which specificies
 * whether this view can be used for Vertex data or Element data.
 *
 */
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

    /**
     * @brief getSpan
     * @return
     *
     * Returns an typed view into this buffer view
     * The returned aspan behaves like a fixed length array
     *
     * Note: You probably shouldn't use this
     */
    //template<typename T>
    //aspan<T> getSpan();
    //
    //template<typename T>
    //aspan<typename std::add_const<T>::type > getSpan() const;

    /**
     * @brief data
     * @return
     *
     * Returns a pointer to the BufferView's data.
     */
    void* data();
    void const* data() const;


    /**
     * @brief createNewAccessor
     * @param byteOffset
     * @param count
     * @param type
     * @param comp
     * @return
     *
     * Create a new accessor in this buffer view at the specified byteOffset. Returns the accessor index
     * which must be looked up in the MODEL.accessors array.
     *
     * Multiple accessors are allowed to overlap in the BufferView.
     */
    size_t createNewAccessor(size_t byteOffset, size_t count, AccessorType type, ComponentType comp);

private:
    GLTFModel * _parent = nullptr;
    friend class GLTFModel;
    friend class Buffer;

};

inline void to_json(json& j, const BufferView & p)
{
   j = json{
            {"buffer"    , p.buffer},

            {"byteLength", p.byteLength},
            {"byteOffset", p.byteOffset}

           };

   if( p.byteStride)
    j["byteStride"] = p.byteStride;

   if( p.target != BufferViewTarget::UNKNOWN)
   {
       j["target"] = static_cast<uint32_t>(p.target);

   }
}

inline void from_json(const json & j, BufferView & B)
{
    B.buffer     = _getValue(j, "buffer"    , 0u);
    B.target     = static_cast<BufferViewTarget>(_getValue(j, "target"    , 0u));
    B.byteLength = _getValue(j, "byteLength", 0u);
    B.byteOffset = _getValue(j, "byteOffset", 0u);
    B.byteStride = _getValue(j, "byteStride", 0u);

#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif
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
        case ComponentType::INT           : return std::string("INT");
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

#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif

}

/**
 * @brief The Accessor class
 *
 * "An accessor defines a method for retrieving data as typed arrays from a bufferView" - GLTF Specification
 *
 * Accessors are the main way to access typed data for various purposes such as vertex attributes,
 * element indices, animation samplers, etc.
 */
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
         * @brief memcpy_all
         * @param dst
         *
         * Copy all the accessor values into the dst
         * buffer.
         */
        void memcpy_all( void * dst ) const
        {
            auto & bv = getBufferView();

            auto * src_c = static_cast<const unsigned char*>(bv.data()) + byteOffset;
            auto * dst_c = static_cast< unsigned char*>( dst );

            auto aSize = accessorSize();

            auto stride = bv.byteStride;
            if( stride == 0)
            {
                stride = aSize;
            }
            for(size_t i=0;i<count;i++)
            {
                std::memcpy(dst_c + i * aSize,
                            src_c + i * stride, aSize );

            }
        }

        /**
         * @brief calculateMinMax
         *
         * Calculate the min and maximum values for this accessor
         */
        void calculateMinMax()
        {
#define DOMINMAX(T, N) \
{\
    auto S = getSpan< std::array<T, N> >();\
    auto l_min = S.front();\
    auto l_max = S.front();\
    auto size = S.size();\
    for(size_t i=0;i<size;i++)\
    {\
        l_min = _min<T,N>( S.get(i), l_min);\
        l_max = _max<T,N>( S.get(i), l_max);\
    }\
    for(size_t i=0;i< N;i++)\
    {\
        min.push_back( static_cast<T>(l_min[i]) );\
        max.push_back( static_cast<T>(l_max[i]) );\
    }\
    break;\
}\


            min.clear();
            max.clear();
            switch (componentType)
            {
                case ComponentType::BYTE:
                    switch (type)
                    {
                        case AccessorType::UNKNOWN: break;
                        case AccessorType::SCALAR:
                            DOMINMAX(std::int8_t,1)
                        case AccessorType::VEC2:
                            DOMINMAX(std::int8_t,2)
                        case AccessorType::VEC3:
                            DOMINMAX(std::int8_t,3)
                        case AccessorType::VEC4:
                            DOMINMAX(std::int8_t,4)
                        case AccessorType::MAT2: break;
                        case AccessorType::MAT3: break;
                        case AccessorType::MAT4: break;
                    }
                    break;
                case ComponentType::UNSIGNED_BYTE :
                    switch (type)
                    {
                        case AccessorType::UNKNOWN: break;
                        case AccessorType::SCALAR:
                            DOMINMAX(std::uint8_t,1)
                        case AccessorType::VEC2:
                            DOMINMAX(std::uint8_t,2)
                        case AccessorType::VEC3:
                            DOMINMAX(std::uint8_t,3)
                        case AccessorType::VEC4:
                            DOMINMAX(std::uint8_t,4)
                        case AccessorType::MAT2: break;
                        case AccessorType::MAT3: break;
                        case AccessorType::MAT4: break;
                    }
                    break;
                case ComponentType::SHORT         :
                    switch (type)
                    {
                        case AccessorType::UNKNOWN: break;
                        case AccessorType::SCALAR:
                            DOMINMAX(std::int16_t,1)
                        case AccessorType::VEC2:
                            DOMINMAX(std::int16_t,2)
                        case AccessorType::VEC3:
                            DOMINMAX(std::int16_t,3)
                        case AccessorType::VEC4:
                            DOMINMAX(std::int16_t,4)
                        case AccessorType::MAT2: break;
                        case AccessorType::MAT3: break;
                        case AccessorType::MAT4: break;
                    }
                    break;
                case ComponentType::UNSIGNED_SHORT:
                    switch (type)
                    {
                        case AccessorType::UNKNOWN: break;
                        case AccessorType::SCALAR:
                            DOMINMAX(std::uint16_t,1)
                        case AccessorType::VEC2:
                            DOMINMAX(std::uint16_t,2)
                        case AccessorType::VEC3:
                            DOMINMAX(std::uint16_t,3)
                        case AccessorType::VEC4:
                            DOMINMAX(std::uint16_t,4)
                        case AccessorType::MAT2: break;
                        case AccessorType::MAT3: break;
                        case AccessorType::MAT4: break;
                    }
                    break;
                case ComponentType::INT  :
                    switch (type)
                    {
                        case AccessorType::UNKNOWN: break;
                        case AccessorType::SCALAR:
                            DOMINMAX(std::int32_t,1)
                        case AccessorType::VEC2:
                            DOMINMAX(std::int32_t,2)
                        case AccessorType::VEC3:
                            DOMINMAX(std::int32_t,3)
                        case AccessorType::VEC4:
                            DOMINMAX(std::int32_t,4)
                        case AccessorType::MAT2: break;
                        case AccessorType::MAT3: break;
                        case AccessorType::MAT4: break;
                    }
                    break;
                case ComponentType::UNSIGNED_INT  :
                    switch (type)
                    {
                        case AccessorType::UNKNOWN: break;
                        case AccessorType::SCALAR:
                            DOMINMAX(std::uint32_t,1)
                        case AccessorType::VEC2:
                            DOMINMAX(std::uint32_t,2)
                        case AccessorType::VEC3:
                            DOMINMAX(std::uint32_t,3)
                        case AccessorType::VEC4:
                            DOMINMAX(std::uint32_t,4)
                        case AccessorType::MAT2: break;
                        case AccessorType::MAT3: break;
                        case AccessorType::MAT4: break;
                    }
                    break;
                case ComponentType::FLOAT         :
                    switch (type)
                    {
                        case AccessorType::UNKNOWN: break;
                        case AccessorType::SCALAR:
                            DOMINMAX(float,1)
                        case AccessorType::VEC2:
                            DOMINMAX(float,2)
                        case AccessorType::VEC3:
                            DOMINMAX(float,3)
                        case AccessorType::VEC4:
                            DOMINMAX(float,4)
                        case AccessorType::MAT2: break;
                        case AccessorType::MAT3: break;
                        case AccessorType::MAT4: break;
                    }
                    break;
                case ComponentType::DOUBLE        :
                    switch (type)
                    {
                        case AccessorType::UNKNOWN: break;
                        case AccessorType::SCALAR:
                            DOMINMAX(double,1)
                        case AccessorType::VEC2:
                            DOMINMAX(double,2)
                        case AccessorType::VEC3:
                            DOMINMAX(double,3)
                        case AccessorType::VEC4:
                            DOMINMAX(double,4)
                        case AccessorType::MAT2: break;
                        case AccessorType::MAT3: break;
                        case AccessorType::MAT4: break;
                    }
                    break;

            }
        }


        template<typename T>
        /**
         * @brief getValue
         * @param index
         * @return
         *
         * Gets the value of the accessor at index.
         *
         * A.getValue<glm::vec3>(3);
         */
        inline T getValue(size_t index) const
        {
            T dst;
            auto * src = _getData(index);
            auto as = accessorSize();
            assert( as == sizeof(T) );
            std::memcpy(&dst, src,sizeof(T) );
            return dst;
        }

        /**
         * @brief getValue
         * @param index
         * @param dst
         *
         * Gets the value at index and copies it into dst.
         */
        template<typename T>
        inline void getValue(size_t index, T & dst) const
        {
            auto * src = _getData(index);
            auto as = accessorSize();
            assert( as == sizeof(T) );
            std::memcpy(&dst, src,sizeof(T) );
        }


        /**
         * @brief getValues
         * @param startIndex
         * @param dst
         * @param totalElements
         * @return
         *
         * Gets totalElements of accessors and copies it into the
         * random-access memory starting at dst,
         *
         * dst must be a pointer to an array of type T, where type T
         * is a type which is compatible with the accessor.
         */
        template<typename T>
        inline size_t getValues(size_t startIndex, T * dst, size_t totalElements) const
        {
            auto * src = _getData(startIndex);
            auto as = accessorSize();
            assert( as == sizeof(T) );
            auto stride = getBufferView().byteStride;

            if( startIndex + totalElements > count )
            {
                totalElements = count - startIndex;
            }

            for(uint32_t i=0;i<totalElements;i++)
            {
                std::memcpy(dst, src, sizeof(T) );
                dst++;
                src+=stride;
            }

            return totalElements;
        }


        /**
         * @brief setValue
         * @param index
         * @param src
         *
         * Sets the accessor value at index.
         */
        template<typename T>
        inline void setValue(size_t index, T const & src)
        {
            if( index < count)
            {
                auto * dst = _getData(index);
                auto as = accessorSize();
                assert( as == sizeof(T) );

                std::memcpy(dst, &src, sizeof(T) );
            }
            else
            {
                throw std::out_of_range("Index out of range");
            }
        }

        /**
         * @brief setValues
         * @param startIndex
         * @param src
         * @param totalElements
         * @return
         *
         * Copies totalElements from the array src[] into the accessor
         * starting at startIndex.
         */
        template<typename T>
        inline size_t setValues(size_t startIndex, T const * src, size_t totalElements)
        {
            auto * dst = _getData(startIndex);
            auto as = accessorSize();
            assert( as == sizeof(T) );
            auto stride = getBufferView().byteStride;

            if( startIndex + totalElements > count )
            {
                throw std::out_of_range("Index out of range");
            }

            for(uint32_t i=0;i<totalElements;i++)
            {
                std::memcpy(dst, src, sizeof(T) );
                dst += stride;
                src++;
            }

            return totalElements;
        }


        /**
         * @brief copyDataFrom
         * @param A
         *
         * Copies data from accessor A to this accessor.
         *
         * Will throw an error if:
         *  - acessors types are different (eg: vec3 vs vec2)
         *  - accessors componentTypes are different (eg: float vs uint)
         *  - the accessor is not large enough to hold all of values in A.
         *
         * All the values from A are copied over, but the .count values is
         * not updated for this accessor.
         */
        void copyDataFrom(Accessor const & A);

        /**
         * @brief componentSize
         * @return
         *
         * Returns the component size. That is the size, in bytes, of the base datatype.
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
                case ComponentType::INT           : return 4;
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
        GLTFModel * _parent = nullptr;
        friend class GLTFModel;
        friend class Buffer;
        friend class BufferView;

        template<typename T, size_t N>
        std::array<T, N> _min( std::array<T, N> a, std::array<T, N> b)
        {
            std::array<T, N> m;
            for(size_t i=0;i<N;i++) m[i] = std::numeric_limits<T>::max();
            for(size_t i=0;i<N;i++)
            {
                m[i] = std::min( a[i], b[i]);
            }
            return m;
        }

        template<typename T, size_t N>
        std::array<T, N> _max( std::array<T, N> a, std::array<T, N> b)
        {
            std::array<T, N> m;
            for(size_t i=0;i<N;i++) m[i] = std::numeric_limits<T>::lowest();
            for(size_t i=0;i<N;i++)
            {
                m[i] = std::max( a[i], b[i]);
            }
            return m;
        }

        uint8_t * _getData(size_t index);
        uint8_t const * _getData(size_t index) const;
};

inline void to_json(json& j, const Accessor & p)
{
   j = json{
            {"bufferView"    , p.bufferView},
            {"byteOffset", p.byteOffset},
            {"normalized", p.normalized},
            {"count"     , p.count},
            {"name"      , p.name},
            {"type"      , to_string(p.type)},
            {"componentType"      , static_cast<uint32_t>(p.componentType)}
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

#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif

}


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

private:
    bool _hasMatrix=false;
    bool _hasTransforms=false;
    GLTFModel * _parent = nullptr;
    friend void from_json(const json & j, Node & B);
    friend class GLTFModel;
};

inline void to_json(json& j, const Node & p)
{
    if( p.mesh    != std::numeric_limits<uint32_t>::max())
        j["mesh"] = p.mesh;

    if( p.camera  != std::numeric_limits<uint32_t>::max())
        j["camera"] = p.camera;

    if( p.hasMatrix() )
        j["matrix"] = p.matrix;

    if( p.hasSkin())
        j["skin"] = p.skin;

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

#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif
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

/**
 * @brief The Primitive class
 *
 * The Primitive class represents an actual mesh that can be rendered in some way.
 *
 * It contains a set of attributes which must be looked up in the .accessors array
 *
 */
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
    GLTFModel * _parent = nullptr;
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
        j["indices" ] =  p.indices;

    if( p.hasMaterial() )
        j["material"] = p.material;

    j["mode"] = static_cast<uint32_t>(p.mode);
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

#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif

}

/**
 * @brief The Mesh class
 *
 * The Mesh class defines a list or primitives that can be rendered.
 *
 * Each primitive can have a different material.
 */
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
    GLTFModel * _parent = nullptr;
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

#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif
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
    GLTFModel * _parent = nullptr;
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
    GLTFModel * _parent = nullptr;
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

#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif

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
        case AnimationPath::TRANSLATION: return std::string("translation");
        case AnimationPath::ROTATION:    return std::string("rotation");
        case AnimationPath::SCALE:       return std::string("scale");
        case AnimationPath::WEIGHTS:     return std::string("weights");
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
    GLTFModel * _parent = nullptr;
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

#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif
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
    GLTFModel * _parent = nullptr;
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
        return S;
    }
    AnimationChannel & newChannel()
    {
        auto & S = channels.emplace_back();
        S._parent = _parent;
        return S;
    }


private:
    GLTFModel * _parent = nullptr;
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

#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif
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

    std::vector<uint8_t> m_imageData; // if bufferView is not given


    BufferView       &  getBufferView();
    BufferView const &  getBufferView() const;

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

private:
    GLTFModel * _parent = nullptr;
    friend class GLTFModel;

};

inline void to_json(json& j, const Image & p)
{
    // if the image has data, then we should write it out as
    // encoded data.
    if( p.m_imageData.size() )
    {
        if( p.mimeType == "")
        {
            j["uri"] = "data:application/octet-stream;base64," + _toBase64( &p.m_imageData[0], &p.m_imageData[ p.m_imageData.size() ]);
        }
        else
        {
            j["uri"] = "data:" + p.mimeType +";base64," + _toBase64( &p.m_imageData[0], &p.m_imageData[ p.m_imageData.size() ]);
        }
    }
    else
    {
        if( p.uri.size())
            j["uri"] = p.uri;
    }

    if(p.mimeType.size())
        j["mimeType"] = p.mimeType;

    if(p.bufferView != std::numeric_limits<uint32_t>::max())
        j["bufferView"] = p.bufferView;

    if( p.name.size() )
        j["name"] = p.name;
}

inline void from_json(const json & j, Image & B)
{
    std::string uri  = _getValue(j, "uri", std::string(""));
    B.mimeType    = _getValue(j, "mimeType", std::string(""));
    B.bufferView  = _getValue(j, "bufferView", std::numeric_limits<uint32_t>::max());
    B.name        = _getValue(j, "name", std::string(""));

    if( uri != "" )
    {
        auto comma = std::find( uri.begin(), uri.end(), ',');
        if( comma == uri.end() ) // is a file
        {
            // uri is a file
            B.uri = uri;
        }
        else
        {
            //image/png
            //image/jpeg
            auto colon     = std::find( uri.begin(), comma, ':');
            auto semicolon = std::find( uri.begin(), comma, ';');

            if( colon != comma && semicolon != comma )
            {
                if( B.mimeType == "")
                {
                    B.mimeType = std::string(colon+1, semicolon);

                    assert( B.mimeType == "image/png" ||
                            B.mimeType == "image/jpeg");
                }

            }

            TRACE("Decoding Base64 buffer");
            // uri is base64 encoded
            auto i = std::distance(uri.begin(), comma);
            B.m_imageData = _fromBase64( &uri[i]+1, &uri.back()+1 );
        }
    }

#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif
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
    GLTFModel * _parent = nullptr;
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


#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif
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

#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif
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


#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif
}


enum class MaterialAlphaMode
{
    OPAQUE,
    MASK,
    BLEND
};

inline std::string to_string(MaterialAlphaMode d)
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
  //  GLTFModel * _parent = nullptr;
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
    if( B.alphaMode == MaterialAlphaMode::MASK)
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

#if defined PRINT_CONV
    std::cout << "=======================" << std::endl;
    std::cout << "original: " << std::endl;
    std::cout << j.dump(2) << std::endl;
    std::cout << "=======================" << std::endl;
    std::cout << "mine: " << std::endl;
    json J;
    to_json(J,B);
    std::cout << J.dump(2);
    std::cout << "=======================" << std::endl;
#endif

}


class GLTFModel
{
public:
    struct header_t
    {
        uint32_t magic   = 0x46546C67;
        uint32_t version = 2;
        uint32_t length  = 12;
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
         extensions  = std::move( other.extensions );
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
         extensions  = other.extensions;

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
             extensions  = other.extensions;
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
             extensions  = std::move(  other.extensions );

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

    bool fromJSON(json const & J)
    {
        if(J.count("asset") == 1)
        {
            asset = J["asset"].get<Asset>();
        }

        if(J.count("buffers") == 1)
        {
            buffers = J["buffers"].get< std::vector<Buffer> >();
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

        _setParents(this);

        if( J.count("extensions") == 1)
        {
            extensions = J["extensions"];
        }

        return true;
    }

    bool load( std::istream & i)
    {
        bool isGLB = false;
        auto firstChar = i.peek();
        if( firstChar == 0x67 && firstChar !=  ' ' && firstChar != '{' )
        {
            // it's a GLB file.
            auto header    = _readHeader(i);
            if(header.magic != 0x46546C67) return false;
            if(header.version < 2)         return false;


            auto jsonChunk = _readChunk(i);
            jsonChunk.chunkData.push_back(0);

            _json = _parseJson(  reinterpret_cast<char*>(jsonChunk.chunkData.data()) );

            isGLB = true;
        }
        else  // is pure json file
        {
            i >> _json;
        }


        auto & J = _json;

        fromJSON(J);

        if( isGLB)
        {
            _readGLBBuffers( i, buffers, J["buffers"] );
        }
        return true;
    }



    //=========================================================================================
    // MODIFICATION methods
    //
    // Methods which modify an already set up GLTFModel
    //=========================================================================================
    /**
     * @brief mergeBuffers
     *
     * Merges all the buffers into a single buffer.
     * This is required for saving to a GLB format.
     */
    void mergeBuffers()
    {
        std::vector<uint8_t> newBuffer;

        if(buffers.size() <= 1)
            return;

        uint32_t i=0;
        for(auto & b : buffers)
        {
            uint32_t byteOffset = static_cast<uint32_t>( newBuffer.size() );

            // add this buffer to the end of the new buffer
            newBuffer.insert( newBuffer.end(), b.m_data.begin(), b.m_data.end() );

            // loop through all the buffer views
            // and check if that view is referencing the current buffer, i,
            // if it is, set it to reference buffer 0 and update it's byteOffset
            for(auto & v : bufferViews)
            {
                if( v.buffer == i )
                {
                    v.buffer      = 0;
                    v.byteOffset += byteOffset;
                }
            }
            i++;
        }
        buffers.resize(1);
        buffers[0].byteLength = newBuffer.size();
        buffers[0].m_data = std::move(newBuffer);
    }

    /**
     * @brief convertImagesToBuffers
     *
     * Converts all images which have local image storage (ie: the data is
     * not stored in a bufferView.
     *
     * All images which are converted at stored in a single newly created buffer.
     *
     */
    void convertImagesToBuffers()
    {
        auto & newB = newBuffer();

        for(auto & b : images)
        {
            if( b.m_imageData.size() )
            {
                auto imageBufferViewIndex = newB.createNewBufferView( b.m_imageData.size() , BufferViewTarget::UNKNOWN, 0, 1);

                void * bufferViewData = bufferViews[imageBufferViewIndex].data();

                std::memcpy( bufferViewData, b.m_imageData.data(), b.m_imageData.size() );

                b.bufferView = imageBufferViewIndex;
                b.m_imageData.clear();
                b.uri.clear();
            }

        }
    }




    //=========================================================================================
    // Generation Methods
    //
    // Methods which modify an already set up GLTFModel
    //=========================================================================================

    /**
     * @brief getMergedBuffer
     * @return
     *
     * Returns a single buffer which is the merge of
     * all the current buffers. This does not
     * update the bufferViews.
     */
    Buffer generateMergedBuffer() const
    {
        Buffer newBuffer;

        for(auto & b : buffers)
        {
            newBuffer.append(b);
        }
        return newBuffer;
    }
    /**
     * @brief getMergedBufferViews
     * @return
     *
     * Returns a new vector of BufferViews which have been
     * updated to all point to single merged buffer.
     */
    std::vector<BufferView> generateMergedBufferViews() const
    {
        if(buffers.size() <= 1)
            return bufferViews;

        std::vector<BufferView> merged = bufferViews;

        uint32_t i=0;
        uint32_t byteOffset = 0;
        for(auto & b : buffers)
        {
            // loop through all the buffer views
            // and check if that view is referencing the current buffer, i,
            // if it is, set it to reference buffer 0 and update it's byteOffset
            for(auto & v : merged)
            {
                if( v.buffer == i )
                {
                    v.buffer      = 0;
                    v.byteOffset += byteOffset;
                }
            }

            byteOffset = static_cast<uint32_t>( b.byteLength );
            i++;
        }
        return merged;
    }

    /**
     * @brief generateJSON
     * @return
     *
     * Generates the JSON component of the GLB file.
     */
    json generateJSON(bool _writeEmbeddedBuffers) const
    {
        json root;

        auto & J = root;

        J["asset"]       =  asset;

        if( accessors.size() )
            J["accessors"]   =  accessors;

        if( buffers.size() )
            J["buffers"] =  buffers;

        if( bufferViews.size() )
            J["bufferViews"] =  bufferViews;

        if(nodes.size())
            J["nodes"]       =  nodes;

        if(meshes.size())
            J["meshes"]      =  meshes;

        if( scenes.size())
            J["scenes"]      =  scenes;

        if( skins.size())
            J["skins"]       =  skins;

        if( animations.size() )
            J["animations"]  =  animations;

        if( images.size() )
            J["images"]      =  images;

        if(textures.size())
            J["textures"]    =  textures;

        if( samplers.size() )
            J["samplers"]    =  samplers;

        if( cameras.size() )
            J["cameras"]     =  cameras;

        if( materials.size() )
            J["materials"]   =  materials;

        J["extensions"] = extensions;

        if( _writeEmbeddedBuffers)
        {
            writeEmbeddedBuffers(J);
        }
        return root;
    }



    /**
     * @brief writeGLB
     * @param out
     *
     * Writes the Model to a GLB file.
     */
    void writeGLB(std::ostream & out) const
    {
        // generate the JSON with
        auto j = generateJSON(false);

        auto bv = generateMergedBufferViews();
        auto b  = generateMergedBuffer();

        j.erase("buffers");
        j["buffers"][0] = b;
        j["bufferViews"] = bv;

        _writeGLB(out, j, b);
    }

    /**
     * @brief writeEmbedded
     * @param out
     *
     * Writes the Model as a .gltf JSON file with embedded buffers
     * encoded in base64.
     */
    void writeEmbeddedGLTF(std::ostream & out) const
    {
        auto j = generateJSON(true);
        out << j.dump(4);
    }


    /**
     * @brief writeEmbeddedBuffers
     *
     * Populates the GLTF["buffers"] property with the the embedded data
     *
     * {
     *     "buffers" : {
     *        [
     *          {
     *            "byteLength":  xxx,
     *            "uri" : "data:application/octet-stream;base64NERDH3245HJ23K2J3NHBDH5J23HKJ25NBKJ"
     *          }
     *        ]
     *     }
     * }
     */
    void writeEmbeddedBuffers( json & j ) const
    {
        j["buffers"].clear();
        uint32_t i=0;
        for(auto & b : buffers)
        {
            j["buffers"][i]["byteLength"] = b.m_data.size();
            j["buffers"][i]["uri"] = std::string("data:application/octet-stream;base64,") + _toBase64( &b.m_data[0] , &b.m_data[ b.m_data.size() ]);
            i++;
        }
    }




    //=========================================================================================
    // Object Creation Methods
    //
    // Methods which can create new objects
    //=========================================================================================
    /**
     * @brief newImage
     * @return
     *
     * Creates a new image
     */
    Image & newImage()
    {
        auto & b = images.emplace_back();
        b._parent = this;
        return b;
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
    json                    extensions;
    json                    _json;

UGLTF_PROTECTED:
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
     * @brief _readGLBBuffers
     * @param in
     * @param buffers
     * @param jBuffers
     *
     * Reads the GLB raw data buffers from the input stream, in. "in" must
     * be pointing to the start of a GLB chunk which contains buffers.
     */
    static void _readGLBBuffers( std::istream & in, std::vector<Buffer> & buffers, json const & jBuffers )
    {
        if( buffers.size() != jBuffers.size())
        {
            throw std::runtime_error("In a GLB file there can only be 1 buffer");
        }
        chunk_t h;
        uint32_t i=0;
        if(in.read(reinterpret_cast<char*>(&h), sizeof(uint32_t)*2))
        {
            if( h.chunkType == 0x4E4F534A || h.chunkType == 0x004E4942)
            {
                for(auto & b : jBuffers)
                {
                    Buffer & B   = buffers[i];
                    B.byteLength = b["byteLength"].get<int32_t>();
                    B.m_data.resize( B.byteLength );

                    // read exactly byteLength bytes from the input stream
                    in.read( reinterpret_cast<char*>(B.m_data.data()), B.m_data.size());

                    i++;
                }

            }
        }
    }

    static void _writeGLB(std::ostream & out, json const & J, Buffer const & B)
    {
        uint32_t magic   = 0x46546C67;
        uint32_t version = 2;
        uint32_t length  = 12;

        std::string j_str = J.dump();

        // make sure that the string is aligned to a 4-byte boundary
        // according to the specifications
        if( j_str.size() %4 != 0)
        {
            j_str.insert( j_str.end(), 4-j_str.size()%4, ' ');
            assert( j_str.size() % 4 == 0);
        }

        length += 8;
        length += j_str.size();

        uint32_t bufferChunkSize=8;
        bufferChunkSize += B.m_data.size();

        if(bufferChunkSize%4!=0)
            bufferChunkSize += 4-bufferChunkSize%4;

        TRACE("Buffer Chunk Size   : {}", bufferChunkSize);
        TRACE("Buffer Chunk%4 Check: {}", bufferChunkSize%4);

        length += bufferChunkSize;

        out.write( reinterpret_cast<char*>(&magic  ) , sizeof(magic));
        out.write( reinterpret_cast<char*>(&version) , sizeof(version));
        out.write( reinterpret_cast<char*>(&length ) , sizeof(length));

        // Write the JSON chunk.
        {
            uint32_t chunkLength = static_cast<uint32_t>(j_str.size());
            uint32_t chunkType   = 0x4E4F534A; // json

            out.write( reinterpret_cast<char*>(&chunkLength) , sizeof(chunkLength));
            out.write( reinterpret_cast<char*>(&chunkType )  , sizeof(chunkType));

            out.write( j_str.data(), j_str.size());
        }

        // write the Buffers chunk.
        {
            uint32_t chunkLength = 0;//
            uint32_t chunkType   = 0x004E4942; // bin

            chunkLength += B.m_data.size();

            TRACE("Total Buffer bytes: {}", chunkLength);
            // Make sure that the total data is aligned to a 4byte boundary.
            uint32_t padding=0;
            if( chunkLength % 4!=0)
            {
                padding = 4-chunkLength%4;
                chunkLength += padding;
                TRACE("Adding padding: {}", padding);
            }

            out.write( reinterpret_cast<char*>(&chunkLength) , sizeof(chunkLength));
            out.write( reinterpret_cast<char*>(&chunkType )  , sizeof(chunkType));
            out.write( reinterpret_cast<char const*>(B.m_data.data()) , B.m_data.size() );

            if(padding)
            {
                std::array<char, 8> _padd = {};
                out.write( _padd.data() , padding );
            }

        }
    }
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

    auto stride = bv.byteStride;

    // if the bufferView's stride is zero, that means the accessor's data
    // is tightly packed
    if( stride == 0 )
    {
        auto accSize = accessorSize();

        if( accSize < sizeof(T) )
        {
            throw std::runtime_error( std::string("The stride listed in the accessor, ") + std::to_string(stride) + ", is less size of the template parameter, " + std::to_string( sizeof(T)) + ". Your data will overlap");
        }

        stride = accSize;
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

    auto stride = bv.byteStride;

    // if the bufferView's stride is zero, that means the accessor's data
    // is tightly packed
    if( stride == 0 )
    {
        auto accSize = accessorSize();

        if( accSize < sizeof(T) )
        {
            throw std::runtime_error( std::string("The stride listed in the accessor, ") + std::to_string(stride) + ", is less size of the template parameter, " + std::to_string( sizeof(T)) + ". Your data will overlap");
        }

        stride = accSize;
    }
    else  // the buffer view is not packed
    {

    }

    return
    aspan<typename std::add_const<T>::type>( static_cast<const unsigned char*>(bv.data())+byteOffset,
              count,
              stride);
}


inline aspan<uint8_t> Image::getSpan()
{
    if( bufferView != std::numeric_limits<uint32_t>::max() )
    {
        auto & Bv = _parent->bufferViews.at( static_cast<size_t>(bufferView ) );

        return aspan<uint8_t>( Bv.data(), Bv.byteLength, 1);// aspan(uint8_t)
        //return Bv.getSpan<uint8_t>();
    }

    if( m_imageData.size() > 0)
        return aspan<uint8_t>( m_imageData.data(), m_imageData.size(), 1);

    throw std::runtime_error("Image Data has not been loaded yet.");
}

inline aspan<const uint8_t> Image::getSpan() const
{
    if( bufferView != std::numeric_limits<uint32_t>::max() )
    {
        auto & Bv = _parent->bufferViews.at( static_cast<size_t>(bufferView ) );

        return aspan<uint8_t const>( Bv.data(), Bv.byteLength, 1);// aspan(uint8_t)
        //return Bv.getSpan<uint8_t const>();
    }

    if( m_imageData.size() > 0)
        return aspan<uint8_t const>( m_imageData.data(), m_imageData.size(), 1);

    throw std::runtime_error("Image Data has not been loaded yet.");
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

uint8_t* Accessor::_getData(size_t index)
{
    auto & v = getBufferView();
    uint8_t * dst = static_cast<uint8_t*>( v.data() ) + byteOffset + index * v.byteStride;
    return dst;
}

uint8_t const * Accessor::_getData(size_t index) const
{
    auto & v = getBufferView();
    uint8_t const * dst = static_cast<uint8_t const*>( getBufferView().data() ) + byteOffset + index * v.byteStride;
    return dst;
}


inline void Accessor::copyDataFrom(Accessor const & A)
{
    if( componentType != A.componentType )
        throw std::runtime_error( std::string("Component types are not the same: " + to_string(componentType) + " != " + to_string(A.componentType)) );

    if( type != A.type )
        throw std::runtime_error( std::string("Accessor types are not the same: " + to_string(type) + " != " + to_string(A.type)) );

    if( count < A.count)
        throw std::runtime_error( std::string("Not enough room in source accessor to copy data. Requires: " + std::to_string(A.count) + ", available: " + std::to_string(count)) );

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

    //auto totalBytesForData = A.accessorSize() * A.count;
    //if( getBufferView().byteLength < totalBytesForData   )
    //{
    //    throw std::runtime_error("This destination Accessor does not have enough space for the data");
    //}

    auto count = A.count;

    auto elmentSize = A.accessorSize();

    // get the byte offset;
    uint8_t * dst       = static_cast<uint8_t*>( getBufferView().data() ) + byteOffset;
    uint8_t const * src = static_cast<uint8_t const*>( A.getBufferView().data() ) + A.byteOffset;
    auto dstStride      = elmentSize;

    while(count--)
    {
//        TRACE("Copying src[{}] to dst[{}]", std::distance(srcStart,src), std::distance(dstStart,dst));
        std::memcpy( dst, src, elmentSize);

        dst += dstStride;
        src += srcStride;

    }

}

inline size_t Buffer::createNewAccessor(size_t count,  BufferViewTarget target, uint32_t bufferViewStride, uint32_t bufferViewAlignment,AccessorType type, ComponentType comp)
{
   // size_t alignment = 1;

    Accessor a;
    a.type = type;
    a.componentType = comp;

    auto bytes = a.accessorSize() * count;

    auto bv_i = createNewBufferView( bytes, target, bufferViewStride, bufferViewAlignment);

    return _parent->bufferViews[bv_i].createNewAccessor(0, count, type, comp);
}

// create a new buffer view by expanding the current
// buffer and placing the bytes at the end.
inline size_t Buffer::createNewBufferView(size_t bytes, BufferViewTarget target, size_t stride, size_t alignment)
{
    _parent->bufferViews.push_back(BufferView());//.emplace_back();
    BufferView & Bv = _parent->bufferViews.back();
    Bv._parent = _parent;
    Bv.target  = target;

    size_t i=0;
    for(auto & buffer : _parent->buffers)
    {
        if(&buffer == this)
        {
            break;
        }
        ++i;
    }

    if( alignment != 0 )
    {
        if( m_data.size() % alignment != 0)
        {
            m_data.insert(m_data.end(), alignment - m_data.size()%alignment, 0);
        }
    }

    Bv.buffer     = i;
    Bv.byteLength = bytes;
    Bv.byteOffset = m_data.size();
    Bv.byteStride = stride;

    // expand the buffer.
    m_data.insert( m_data.end(), bytes, 0);
    byteLength += bytes;
    return _parent->bufferViews.size()-1;
}

inline size_t BufferView::createNewAccessor(size_t byteOffset, size_t count, AccessorType type, ComponentType comp)
{
    Accessor _temp;
    _temp.type = type;
    _temp.componentType = comp;
    _temp.count = count;
    _temp.byteOffset = byteOffset; // byteOffset from start of view.

    // calculate the total number of bytes required
    auto bytes = _temp.componentSize() * count;

    if( byteOffset+bytes > byteLength)
    {
        throw std::runtime_error("Not enough room in the buffer view to allocate this accessor. Required " + std::to_string(byteOffset+bytes));
        exit(1);
    }

    uint32_t i=0;
    for(auto & v : _parent->bufferViews)
    {
        if( &v == this) break;
        ++i;
    }


    auto & aa = _parent->accessors.emplace_back();
    aa._parent = _parent;
    aa.type = type;
    aa.componentType = comp;
    aa.count = count;
    aa.byteOffset = byteOffset; // byteOffset from start of view.
    aa.bufferView = i;

    return _parent->accessors.size()-1;
}

}

#endif
