using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML;

// TODO AppearanceTargetDefinition を継承する
// TODO 実装途中
// TODO C++のlibcitygmlのコメントを翻訳して載せておきたい
public class TextureTargetDefinition
{
    private readonly IntPtr handle;

    public TextureTargetDefinition(IntPtr handle)
    {
        this.handle = handle;
    }

    public int TextureCoordinatesCount
    {
        get
        {
            int count = DLLUtil.GetNativeValue<int>(this.handle,
                NativeMethods.plateau_texture_target_definition_get_texture_coordinates_count);
            return count;
        }
    }
}