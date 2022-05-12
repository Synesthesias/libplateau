
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// 見た目に関する情報を持つオブジェクトです。
    /// </summary>
    public class AppearanceTarget : Object
    {
        internal AppearanceTarget(IntPtr handle) : base(handle)
        {
        }
        

        public int TextureThemesCount(bool front)
        {
            APIResult result = NativeMethods.plateau_appearance_target_get_all_texture_themes_count(
                Handle, out int count, front);
            DLLUtil.CheckDllError(result);
            return count;
        }

        public string[] TextureThemes(bool front)
        {
            string[] ret = DLLUtil.GetNativeStringArrayByValue(
                Handle,
                (IntPtr handle, out int arrayLength) =>
                {
                    var result = NativeMethods.plateau_appearance_target_get_all_texture_themes_count(
                        handle, out arrayLength, front
                    );
                    return result;
                },
                ( handle, strSizes) =>
                {
                    var result = NativeMethods.plateau_appearance_target_get_all_texture_themes_str_sizes(
                        handle, strSizes, front);
                    return result;
                },
                ( handle, strPtrArrayPtr) =>
                {
                    var result = NativeMethods.plateau_appearance_target_get_all_texture_themes(
                        handle, strPtrArrayPtr, front);
                    return result;
                }
            );
            return ret;
        }

        public TextureTargetDefinition GetTextureTargetDefinition(string themeName, bool front)
        {
            var result = NativeMethods.plateau_appearance_target_get_texture_target_definition_for_theme(
                Handle, themeName, out IntPtr texTargetHandle, front);
            if (result == APIResult.ErrorValueNotFound)
            {
                throw new KeyNotFoundException($"themeName: {themeName} is not found.");
            }
            DLLUtil.CheckDllError(result);
            // TODO 結果をキャッシュする
            return new TextureTargetDefinition(texTargetHandle);
        }
    }
}