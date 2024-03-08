struct TextureData {
    width: u32,
    height: u32,
};

@group(0) @binding(0) var<uniform> textureData: TextureData;
@group(0) @binding(1) var sourceTexture: texture_storage_2d<rgba8unorm, read>;
@group(0) @binding(2) var destTexture: texture_storage_2d<rgba8unorm, write>;

@compute @workgroup_size(1, 1, 1)
fn simulate(@builtin(global_invocation_id) id: vec3<u32>) {
    let pos = id.xy;
    let srcColor = textureLoad(sourceTexture, pos);
    if (all(srcColor == vec4<f32>(1.0, 0.0, 0.0, 1.0)))
    {
        textureStore(destTexture, pos, vec4<f32>(f32(id.x) / 255, f32(id.y) / 255, 0.0, 1.0));
    }
    else
    {
        textureStore(destTexture, pos, srcColor);
    }
}
