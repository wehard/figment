Figment = {};

Figment.EnableInput = function() {
    Module._enable_input();
}

Figment.DisableInput = function() {
    Module._disable_input();
}

Figment.EntityGet = function(handle) {
    return Module._entity_get(handle);
}

Figment.EntityCreate = function() {
    return new Entity(Module._entity_create());
}

Figment.EntityDestroy = function(entity) {
    Module._entity_destroy(entity);
}

Figment.EntityAddComponent = function(entity, component) {
    Module._entity_add_component(entity, component);
}

Figment.EntitySetName = function(entity, name) {
    const ptr = stringToNewUTF8(name);
    Module._entity_set_name(entity, ptr);
    Module._free(ptr);
}

Figment.EntitySetPosition = function(entity, x, y, z) {
    Module._entity_set_position(entity, x, y, z);
}

Figment.EntitySetRotation = function(entity, x, y, z) {
    Module._entity_set_rotation(entity, x, y, z);
}

Figment.EntitySetScale = function(entity, x, y, z) {
    Module._entity_set_scale(entity, x, y, z);
}

class Entity
{
    constructor(entity)
    {
        this.entity = entity;
    }

    destroy()
    {
        Figment.EntityDestroy(this.entity);
    }

    getHandle()
    {
        return this.entity;
    }

    addComponent(component)
    {
        Figment.EntityAddComponent(this.entity, component);
    }

    setName(name)
    {
        Figment.EntitySetName(this.entity, name);
    }

    setPosition(x, y, z)
    {
        Figment.EntitySetPosition(this.entity, x, y, z);
    }

    setRotation(x, y, z)
    {
        Figment.EntitySetRotation(this.entity, x, y, z);
    }

    setScale(x, y, z)
    {
        Figment.EntitySetScale(this.entity, x, y, z);
    }
}
