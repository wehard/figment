struct CameraData {
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
};

@binding(0) @group(0) var<uniform> cameraData: CameraData;

struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(0) color: vec4<f32>,
    @location(1) uv: vec2<f32>,
    @location(2) @interpolate(flat) id: i32
};

@vertex
fn vs_main(@location(0) in_world_pos: vec3f, @location(1) in_local_pos: vec3f, @location(2) in_color: vec4f, @location(3) in_id: i32) -> VertexOutput {
    var output: VertexOutput;
    output.pos = cameraData.proj * cameraData.view * vec4<f32>(in_world_pos, 1.0);
    output.uv = in_local_pos.xy;
    output.color = in_color;
    output.id = in_id;
    return output;
}

struct FragmentOutput {
    @location(0) color: vec4<f32>,
    @location(1) id: i32
};

@fragment
fn fs_main(@location(0) color: vec4<f32>, @location(1) uv: vec2<f32>, @location(2)  @interpolate(flat) id: i32) -> FragmentOutput {
    var output: FragmentOutput;
    if(abs(length(uv)) > 0.5 || abs(length(uv)) < 0.4) {
        discard;
    }

    output.color = color;
    output.id = id;
    return output;
}
