#include "HanaTest.h"
#include "TypeIDTest.h"
#include "TypeInfoTest.h"
#include "UtilTest.h"
#include "ChunkTest.h"
#include "World.h"
#include "SparseSetTest.h"

int main()
{
    World world;
    //Entity entity = world.CreateEntity<int, float, void, int*, double&>();

    TypeInfoTest();
    TepeIDTest();
    UtilTest();
    //ChunkTest();
    SparseSetTest();
    return 0;
}