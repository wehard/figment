struct CameraData {
    view: mat4x4<f32>,
    proj: mat4x4<f32>,
};

struct ParticlesData {
    model: mat4x4<f32>,
    size: f32,
};

struct MeshInstanceData {
    @location(1) pos: vec3<f32>,
    @location(2) color: vec4<f32>,
};

@binding(0) @group(0) var<uniform> cameraData: CameraData;
@binding(1) @group(0) var<uniform> data: ParticlesData;

struct VertexOutput {
    @builtin(position) clipPos: vec4<f32>,
    @location(1) vertPos: vec3f,
    @location(2) color: vec4<f32>,
};

@vertex
fn vs_main(@location(0) pos: vec3f, instance: MeshInstanceData) -> VertexOutput {
    var output: VertexOutput;
    let mv = cameraData.view * data.model;
    let clipPos = cameraData.proj * (mv * vec4(instance.pos, 1.0) + vec4(pos.x * data.size, pos.y * data.size, 0.0, 0.0));

    let viewDir = vec3(mv[0][2], mv[1][2], mv[2][2]);
    let attenuation = dot(normalize(instance.pos), viewDir);

    output.clipPos = clipPos;
    output.vertPos = pos;
    output.color = instance.color * attenuation;

    return output;
}

struct FragmentOutput {
    @location(0) color: vec4<f32>,
};

@fragment
fn fs_main(@location(1) vertPos: vec3f, @location(2) color: vec4<f32>) -> FragmentOutput {
    var output: FragmentOutput;
    if (length(vertPos) > 0.5) {
        discard;
    }
    output.color = color;
    return output;
}
