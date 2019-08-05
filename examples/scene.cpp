#include <gltfpp/gltfpp.h>
#include <fstream>

using Transform = int;

bool canDraw( gltfpp::Node * N)
{
    bool thisNodeDraws=false;

    // if this node contains a mesh
    // then it can be drawn.
    if( N->hasMesh() )
        thisNodeDraws = true;

    for(size_t i=0; i < N->childCount(); i++)
    {
        thisNodeDraws |= canDraw( N->getChild(i) );
    }

    return thisNodeDraws;
}

void drawNode( gltfpp::Node * N, Transform const & parentTransform, int indent=0)
{
    // transformStack.back() is the transform of the parent node.

    Transform localTransform = 1.0 /* N->getTransform */;

    Transform globalTransform = parentTransform * localTransform;

    /*

      drawMesh( N->getMesh(), globalTransform );

     */

    std::cout << std::string(indent, ' ') << N->hasTransforms() << "-drawing Node: " << N->name  << "Has Mesh?: " << N->hasMesh() << "   Has Skeleton?: " << N->hasSkin()   << "   Has Camera?: " << N->hasCamera()  << std::endl;

    for(size_t i=0; i < N->childCount(); i++)
    {
        drawNode( N->getChild(i), globalTransform, indent+2 );
    }
}

void drawScene( gltfpp::GLTFModel & M, int sceneIndex, float t=0.0f)
{
    auto & S = M.scenes[sceneIndex];


    // the root node of the scene
    auto rootNode = S.getRootNode(0);

    drawNode(rootNode, 1);
}

int main(int argc, char **argv)
{
    #define TEST_MODEL "/home/gavin/Projects/gltfpp/share/gltfpp/CesiumMan.glb"

    gltfpp::GLTFModel M;

    std::ifstream in(TEST_MODEL);

    M.load(in);

    drawScene(M, 0);

    return 42;
}

