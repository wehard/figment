struct CameraData {
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
};

struct MeshInstanceData {
    model: mat4x4<f32>,
};

@binding(0) @group(0) var<uniform> cameraData: CameraData;
@binding(1) @group(0) var<uniform> meshInstanceData: MeshInstanceData;

struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(0) @interpolate(flat) id: i32
};

@vertex
fn vs_main(@location(0) pos: vec3f) -> VertexOutput {
    var output: VertexOutput;
    output.pos = cameraData.proj * cameraData.view * meshInstanceData.model * vec4<f32>(pos, 1.0);
    output.id = 1;
    return output;
}

struct FragmentOutput {
    @location(0) color: vec4<f32>,
    @location(1) id: i32
};

@fragment
fn fs_main(@location(0)  @interpolate(flat) id: i32) -> FragmentOutput {
    var output: FragmentOutput;
    output.color = vec4<f32>(1.0, 1.0, 1.0, 1.0);
    output.id = id;
    return output;
}
