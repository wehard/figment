using System.Runtime.CompilerServices;

namespace Figment
{
    public static class NativeFunctions
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_GetPosition(ulong entityId, out Vec3 position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_SetPosition(ulong entityId, ref Vec3 position);
    }
}