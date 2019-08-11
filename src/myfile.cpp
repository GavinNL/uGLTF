#include <gltfpp/gltfpp.h>
#include <fstream>

int func()
{
    #define TEST_MODEL "/home/gavin/Projects/gltfpp/share/gltfpp/CesiumMan.glb"

    uGLTF::GLTFModel M;

    std::ifstream in(TEST_MODEL);

    M.load(in);

    return 42;

}

