var figmentLib={
    $dependencies: {},
    JS_EntityCreate: function() {
        Module._entity_create();
    },
    UserWasmModuleFoo: function() {
        const r = UserWasmModule.foo();
        console.log(`UserWasmModuleFoo: ${r}`);
    }
};

autoAddDeps(figmentLib, '$dependencies');
mergeInto(LibraryManager.library, figmentLib);
