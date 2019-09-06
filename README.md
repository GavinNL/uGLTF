# uGLTF - Micro GLTF

uGLTF is a small header-only library with the only dependency being the json parser: [nlohmann/json](https://github.com/nlohmann/json) (not included, but automatically downloaded for unit tests/examples).

## Usage

Simply copy the `ugltf.h` header file to your project include path and you are
good to go.


## Compile

```Bash
cd SRC_FOLDER
mkdir build
cd build
cmake ..
cmake --build .

ctest

```

# Features

* enum classes for all constants
*

```C++

uGLTF::GLTFModel M;
std::ifstream in("mygltf.glb");
M.load(in);

```

The GLTFModel class contains all the properties defined by the GLTF 2.0 Specification.
They are public members and can be accessed without getters.

Helper functions are created to get references to other objects which are normally defined only by an index.

## Enum Classes for integer/string constants

 All integer or string constants are wrapped around an `enum class`. These can
 all be statically cast to their base integer type.
 eg:
 * PrimitiveAttribute
 * PrimitiveMode
 * AnimationPath
 * AnimationInterpolation
 * BufferViewTarget
 * AccessorType
 * AccessorComponent
 * ImageFilter
 * ImageWrapMode

## Optional Properties

Most classes have a `operator bool()` specified so that you can check if that
property is specified.

Classes also have a `hasPROPERTY()` method specified.

Properties with integer index values are set to -1 if they are not specified.

## aspan<T>

The `aspan<T>` is a template class which creates a typed view around raw data.
Since `buffers`, `bufferViews` don't have a C++ type, aspan can be used to access the
typed information from each of these:

Accessors, Buffers, BufferViews and Images all have a method called `getSpan<T>()`.
This allows you to get a typed vector-like view into the actual data.

```C++
gltfpp::Model M;
// load model

auto positions =  M.Accessors[0].getSpan<glm::vec3>();

// access positions as if it was a vector
for(auto & p : positions)
{
    // access p.x, p.y, p.z
}

```

An exception will be thrown if you attempt to use a template type that does not
match the Accessor's type/component constants. The requirement for the template
parameter is that its size must be less than the stride defined by the GLTF asset.


## Mesh data

Mesh data can be accessed for each mesh primitive using the following:

```C++

for(auto & mesh : gltf.meshes)
{
    for(auto & primitive : mesh.primitives)
    {
       if( primitive.has(uGLTF::PrimitiveAttribute::POSITION))
       {
           auto positions = primitive.getSpan< std::array<float,3> >();

           for(auto & p : positions)
           {
              // access p[0], p[1], p[2]
           }
       }
       if( primitive.hasIndices() )
       {
         if( primitive.getIndexAccessor().accessorSize() == 2 )
         {
           auto index = primitive.getIndexSpan<uint16_t>();
         }
         else
         {
           auto index = primitive.getIndexSpan<uint32_t>();
         }

       }
    }
}

```
