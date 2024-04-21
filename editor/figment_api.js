Figment = {};

Figment.EntityCreate = function() {
    return Module._entity_create();
}

Figment.EntityDestroy = function(entity) {
    Module._entity_destroy(entity);
}

Figment.EntityAddComponent = function(entity, component) {
    Module._entity_add_component(entity, component);
}

Figment.EntitySetPosition = function(entity, x, y, z) {
    Module._entity_set_position(entity, x, y, z);
}
