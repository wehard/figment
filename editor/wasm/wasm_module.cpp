#include "wasm_module.h"

extern "C" int foo()
{
    return 42;
}

extern "C" int on_start(t_entity *entity)
{
    entity->id = 42;
}