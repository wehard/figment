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

struct FragmentOutput {
    @location(0) color: vec4<f32>,
    @location(1) id: u32
};

@fragment
fn fs_main() -> FragmentOutput {
        var output : FragmentOutput;
        output.color = renderData.color;
        output.id = 69;
        return output;
}
