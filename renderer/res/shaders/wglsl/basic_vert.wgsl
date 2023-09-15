struct TransformData {
    model: mat4x4<f32>,
    view: mat4x4<f32>,
    proj: mat4x4<f32>
};
@binding(0) @group(0) var<uniform> transformUBO: TransformData;

struct Fragment {
    @builtin(position) Position : vec4<f32>,
    @location(0) Color : vec4<f32>,
    @location(1) Normal : vec3<f32>
};

@vertex
fn vs_main(@location(0) vertexPosition: vec3<f32>) -> Fragment {
    var output : Fragment;

    output.Position = transformUBO.proj * transformUBO.view * transformUBO.model * vec4<f32>(vertexPosition, 1.0);
    output.Color = vec4<f32>(1.0, 0.0, 0.0, 1.0);
    output.Normal = normalize(vertexPosition);

    return output;
}
