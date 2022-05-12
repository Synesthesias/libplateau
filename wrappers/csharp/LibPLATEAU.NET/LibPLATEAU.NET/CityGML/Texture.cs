using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML;

// TODO 実装中
// TODO Appearanceを継承する
public class Texture : Object
{
    internal Texture(IntPtr handle) : base(handle)
    {
    }

    public string Url =>
        DLLUtil.GetNativeString(Handle,
            NativeMethods.plateau_texture_get_url);
}