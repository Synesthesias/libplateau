using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML;

// TODO 実装中
public class Texture : Appearance
{
    internal Texture(IntPtr handle) : base(handle)
    {
    }

    public string Url =>
        DLLUtil.GetNativeString(Handle,
            NativeMethods.plateau_texture_get_url);
}