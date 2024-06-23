#include "HanaTest.h"
#include "TypeIDTest.h"
#include "TypeInfoTest.h"
#include "UtilTest.h"
#include "ChunkTest.h"
#include "World.h"

int main()
{
    World world;
    Entity entity = world.CreateEntity<int, float, void, int*, double&>();

    TypeInfoTest();
    TepeIDTest();
    UtilTest();
    ChunkTest();
    return 0;
}