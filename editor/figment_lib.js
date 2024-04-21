var figmentApi={
    $dependencies: {},
    JS_EntityCreate: function() {
        Module._entity_create();
    },
};

autoAddDeps(figmentApi, '$dependencies');
mergeInto(LibraryManager.library, figmentApi);
