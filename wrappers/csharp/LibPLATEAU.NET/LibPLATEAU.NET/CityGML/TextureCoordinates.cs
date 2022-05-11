using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML;
// TODO 実装途中
// TODO C++の実装のコメントを日本語訳して載せておきたい
public class TextureCoordinates : Object
{
    internal TextureCoordinates(IntPtr handle) : base(handle)
    {
    }

    public int Vec2CoordsCount
    {
        get
        {
            int count = DLLUtil.GetNativeValue<int>(this.Handle,
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
        
}