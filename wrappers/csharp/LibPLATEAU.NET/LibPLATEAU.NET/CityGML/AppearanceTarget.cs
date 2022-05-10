
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
            int cnt = TextureThemesCount(front);
            var strSizes = new int[cnt];
            var result = NativeMethods.plateau_appearance_target_get_all_texture_themes_str_sizes(
                Handle, strSizes, front);
            DLLUtil.CheckDllError(result);
            var strPtrArrayPtr = DLLUtil.AllocPtrArray(cnt, strSizes);
            result = NativeMethods.plateau_appearance_target_get_all_texture_themes(
                Handle, strPtrArrayPtr, front);
            DLLUtil.CheckDllError(result);
            var ret = DLLUtil.PtrToStringArray(strPtrArrayPtr, cnt, strSizes);
            DLLUtil.FreePtrArray(strPtrArrayPtr, cnt);
            return ret;
        }
    }
}