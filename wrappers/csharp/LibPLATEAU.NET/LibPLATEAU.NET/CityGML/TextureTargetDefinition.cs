using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML;

// TODO C++のlibcitygmlのコメントを翻訳して載せておきたい
public class TextureTargetDefinition : AppearanceTargetDefinition
{
    private TextureCoordinates?[]? cachedCoords;

    public TextureTargetDefinition(IntPtr handle) : base(handle)
    {
    }

    public int TexCoordinatesCount
    {
        get
        {
            int count = DLLUtil.GetNativeValue<int>(Handle,
                NativeMethods.plateau_texture_target_definition_get_texture_coordinates_count);
            return count;
        }
    }

    public TextureCoordinates GetCoordinate(int index)
    {
        var ret = DLLUtil.ArrayCache(ref this.cachedCoords, index, TexCoordinatesCount, () =>
        {
            IntPtr coordPtr = DLLUtil.GetNativeValue<IntPtr>(Handle, index,
                NativeMethods.plateau_texture_target_definition_get_texture_coordinates);
            return new TextureCoordinates(coordPtr);
        });
        return ret;
    }
}