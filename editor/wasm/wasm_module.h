#ifndef WASM_MODULE_H
#define WASM_MODULE_H

typedef struct s_entity {
    int id;
    char name[20];
} t_entity;

extern "C" int get_component();

#endif // WASM_MODULE_H