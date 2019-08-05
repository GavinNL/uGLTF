# GLTF++

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
match the Accessor's type/component constants.
