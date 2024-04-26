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

    addComponent(component)
    {
        Figment.EntityAddComponent(this.entity, component);
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
