using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML;
/// <summary>
/// テクスチャ座標と <see cref="LinearRing"/> の頂点を紐付けます。
/// </summary>
public class TextureCoordinates : Object
{
    internal TextureCoordinates(IntPtr handle) : base(handle)
    {
    }

    public int Vec2CoordsCount
    {
        get
        {
            int count = DLLUtil.GetNativeValue<int>(Handle,
                NativeMethods.plateau_texture_coordinates_count);
            return count;
        }
    }


    public PlateauVector2f GetVec2Coord(int index)
    {
        var ret = DLLUtil.GetNativeValue<PlateauVector2f>(Handle, index,
            NativeMethods.plateau_texture_coordinates_get_coordinate);
        return ret;
    }

    public string TargetLinearRingId =>
        DLLUtil.GetNativeString(Handle,
            NativeMethods.plateau_texture_coordinates_get_target_linear_ring_id);
    
    public bool IsRingTarget(LinearRing ring)
    {
        APIResult result = NativeMethods.plateau_texture_coordinates_is_ring_target(
            Handle, out bool isTarget, ring.Handle);
        DLLUtil.CheckDllError(result);
        return isTarget;
    }
}