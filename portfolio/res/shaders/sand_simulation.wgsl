struct TextureData {
    width: u32,
    height: u32,
};

@group(0) @binding(0) var<uniform> textureData: TextureData;
@group(0) @binding(1) var sourceTexture: texture_storage_2d<rgba8unorm, read>;
@group(0) @binding(2) var destTexture: texture_storage_2d<rgba8unorm, write>;

@compute @workgroup_size(1, 1, 1)
fn simulate(@builtin(global_invocation_id) id: vec3<u32>) {
    let srcPos = id.xy;
    let dstPos = vec2<u32>(id.x, id.y - 1);
    let srcColor = textureLoad(sourceTexture, srcPos);
    let dstColor = textureLoad(sourceTexture, dstPos);
    if (id.y > 0 && id.y < textureData.height && all(srcColor == vec4<f32>(1.0, 0.0, 0.0, 1.0)))
    {
        textureStore(destTexture, srcPos, dstColor);
        textureStore(destTexture, dstPos, srcColor);
        return;
    }

    textureStore(destTexture, srcPos, srcColor);
}
