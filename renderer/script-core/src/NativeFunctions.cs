using System.Runtime.CompilerServices;

namespace Figment
{
    public static class NativeFunctions
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_GetPosition(ulong entityID, out Vec3 position);
        [MethodImpl(MethodImplOptions.InternalCall)]
        internal static extern void Transform_SetPosition(ulong entityID, ref Vec3 position);
    }
}