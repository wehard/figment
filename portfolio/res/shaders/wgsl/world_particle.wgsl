struct CameraData {
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
};

@binding(0) @group(0) var<uniform> cameraData: CameraData;

struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(0) color: vec4<f32>,
    @location(1) normal: vec3<f32>,
    @location(2) @interpolate(flat) id: i32
};

@vertex
fn vs_main(@location(0) pos: vec3f, @location(1) col: vec4f) -> VertexOutput {
    var output: VertexOutput;
    output.pos = cameraData.proj * cameraData.view * vec4<f32>(pos, 1.0);
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
fn fs_main(@location(0) col: vec4f, @location(1) normal: vec3<f32>, @location(2) @interpolate(flat) id: i32) -> FragmentOutput {
    var output: FragmentOutput;

    var cameraDir = normalize(cameraData.view[3].xyz);
    var lightDir = normalize(vec3<f32>(0.0, 1.0, -2.0));
    var x = dot(normal, cameraDir) * 2.0;

    output.color = col;
    output.id = id;
    return output;
}
