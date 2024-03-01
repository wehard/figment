struct CameraData {
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
};

struct MeshInstanceData {
    @location(1) model_matrix_0: vec4<f32>,
    @location(2) model_matrix_1: vec4<f32>,
    @location(3) model_matrix_2: vec4<f32>,
    @location(4) model_matrix_3: vec4<f32>,
};

@group(0) @binding(0) var<uniform> cameraData: CameraData;
@group(0) @binding(1) var texture: texture_2d<f32>;
@group(0) @binding(2) var texSampler: sampler;

struct VertexOutput {
    @builtin(position) pos: vec4<f32>,
    @location(0) @interpolate(flat) id: i32
};

@vertex
fn vs_main(@location(0) pos: vec3f, instance: MeshInstanceData) -> VertexOutput {
    var output: VertexOutput;
    var model = mat4x4<f32>(
        instance.model_matrix_0,
        instance.model_matrix_1,
        instance.model_matrix_2,
        instance.model_matrix_3
    );
    output.pos = cameraData.proj * cameraData.view * model * vec4<f32>(pos, 1.0);
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
    output.color = textureSampleLevel(texture, texSampler, vec2<f32>(0.0, 0.0), 0.0);;
    output.id = id;
    return output;
}
