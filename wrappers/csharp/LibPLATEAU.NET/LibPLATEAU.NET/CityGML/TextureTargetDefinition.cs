using System.Reflection.Metadata;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML;

// TODO AppearanceTargetDefinition を継承する
// TODO 実装途中
// TODO C++のlibcitygmlのコメントを翻訳して載せておきたい
public class TextureTargetDefinition
{
    private readonly IntPtr handle;
    private TextureCoordinates?[]? cachedCoords;

    public TextureTargetDefinition(IntPtr handle)
    {
        this.handle = handle;
    }

    public int TexCoordinatesCount
    {
        get
        {
            int count = DLLUtil.GetNativeValue<int>(this.handle,
                NativeMethods.plateau_texture_target_definition_get_texture_coordinates_count);
            return count;
        }
    }

    public TextureCoordinates GetCoordinate(int index)
    {
        var ret = DLLUtil.ArrayCache(ref this.cachedCoords, index, TexCoordinatesCount, () =>
        {
            IntPtr coordPtr = DLLUtil.GetNativeValue<IntPtr>(this.handle, index,
                NativeMethods.plateau_texture_target_definition_get_texture_coordinates);
            return new TextureCoordinates(coordPtr);
        });
        return ret;
    }
}