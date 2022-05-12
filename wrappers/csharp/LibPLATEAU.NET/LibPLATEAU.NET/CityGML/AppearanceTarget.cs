
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// 見た目に関する情報のターゲットとなる CityGMLオブジェクトの基底クラスです。
    /// テーマ名と <see cref="TextureTargetDefinition"/>, <see cref="MaterialTargetDefinition"/> が紐付きます。
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
        
        public int MaterialThemesCount(bool front)
        {
            APIResult result = NativeMethods.plateau_appearance_target_get_all_material_themes_count(
                Handle, out int count, front);
            DLLUtil.CheckDllError(result);
            return count;
        }
        
        // TODO 未テスト MaterialTargetDefinition を利用したGMLファイルの例が見当たらないため
        public string[] MaterialThemes(bool front)
        {
            string[] ret = DLLUtil.GetNativeStringArrayByValue(
                Handle,
                (IntPtr handle, out int arrayLength) =>
                {
                    var result = NativeMethods.plateau_appearance_target_get_all_material_themes_count(
                        handle, out arrayLength, front
                    );
                    return result;
                },
                ( handle, strSizes) =>
                {
                    var result = NativeMethods.plateau_appearance_target_get_all_material_themes_str_sizes(
                        handle, strSizes, front);
                    return result;
                },
                ( handle, strPtrArrayPtr) =>
                {
                    var result = NativeMethods.plateau_appearance_target_get_all_material_themes(
                        handle, strPtrArrayPtr, front);
                    return result;
                }
            );
            return ret;
        }

        public TextureTargetDefinition GetTextureTargetDefinition(string themeName, bool front)
        {
            var result = NativeMethods.plateau_appearance_target_get_texture_target_definition_for_theme(
                Handle, out IntPtr texTargetHandle, themeName, front);
            if (result == APIResult.ErrorValueNotFound)
            {
                throw new KeyNotFoundException($"themeName: {themeName} is not found.");
            }
            DLLUtil.CheckDllError(result);
            return new TextureTargetDefinition(texTargetHandle);
        }

        public MaterialTargetDefinition GetMaterialTargetDefinition(string themeName, bool front)
        {
            var result = NativeMethods.plateau_appearance_target_get_material_target_definition(
                Handle, out IntPtr matTargetHandle, themeName, front);
            if (result == APIResult.ErrorValueNotFound)
            {
                throw new KeyNotFoundException($"themeName: {themeName} is not found.");
            }
            DLLUtil.CheckDllError(result);
            return new MaterialTargetDefinition(matTargetHandle);
        }
    }
}