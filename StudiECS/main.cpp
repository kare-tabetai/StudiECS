#include "HanaTest.h"
#include "TypeIDTest.h"
#include "TypeInfoTest.h"
#include "UtilTest.h"
#include "World.h"

int main()
{
    World world;
    Entity entity = world.CreateEntity<int, float, void, int*, double&>();

    TypeInfoTest();
    TepeIDTest();
    UtilTest();
    return 0;
}