@fragment
fn fs_main(@location(0) Color: vec4<f32>, @location(1) Normal: vec3<f32>) -> @location(0) vec4<f32>
{
    var light: vec3<f32> = vec3<f32>(1.0, 1.0, 0.0);
    var col: vec3<f32> = vec3<f32>(Color.r , Color.g, Color.b) * dot(Normal, light);
    return vec4<f32>(col, 1.0);
}