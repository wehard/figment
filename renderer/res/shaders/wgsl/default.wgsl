struct RenderData {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
    color: vec4f
};

@binding(0) @group(0) var<uniform> renderData: RenderData;

@vertex
fn vs_main(@location(0) in_vertex_position: vec3f) -> @builtin(position) vec4f {
    return renderData.proj * renderData.view * renderData.model * vec4f(in_vertex_position, 1.0);
}

@fragment
fn fs_main() -> @location(0) vec4f {
    return renderData.color;
}
