struct CameraData {
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
};

@binding(0) @group(0) var<uniform> cameraData: CameraData;

struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(1) color: vec4<f32>,
    @location(2) @interpolate(flat) id: i32
};

@vertex
fn vs_main(@location(0) in_vertex_position: vec3f, @location(1) in_color: vec4f, @location(2) in_id: i32) -> VertexOutput {
    var output: VertexOutput;
    output.pos = cameraData.proj * cameraData.view * vec4<f32>(in_vertex_position, 1.0);
    output.color = in_color;
    output.id = in_id;
    return output;
}

struct FragmentOutput {
    @location(0) color: vec4<f32>,
    @location(1) id: i32
};

@fragment
fn fs_main(input: VertexOutput) -> FragmentOutput {
    var output: FragmentOutput;
    output.color = input.color;
    output.id = input.id;
    return output;
}
