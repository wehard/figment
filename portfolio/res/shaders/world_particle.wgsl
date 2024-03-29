struct CameraData {
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
};

@binding(0) @group(0) var<uniform> cameraData: CameraData;

struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(0) vpos: vec3<f32>,
    @location(1) color: vec4<f32>,
    @location(2) normal: vec3<f32>,
    @location(3) @interpolate(flat) id: i32
};

@vertex
fn vs_main(@location(0) pos: vec3f, @location(1) col: vec4f) -> VertexOutput {
    var output: VertexOutput;
    output.pos = cameraData.proj * cameraData.view * vec4<f32>(pos, 1.0);
    output.vpos = pos;
    output.color = col;
    output.normal = normalize(pos);
    output.id = 42;
    return output;
}

struct FragmentOutput {
    @location(0) color: vec4<f32>,
    @location(1) id: i32
};

@fragment
fn fs_main(@location(0) vpos: vec3<f32>, @location(1) col: vec4f, @location(2) normal: vec3<f32>, @location(3) @interpolate(flat) id: i32) -> FragmentOutput {
    var output: FragmentOutput;
    output.color = col * vpos.z;
    output.id = id;
    return output;
}
