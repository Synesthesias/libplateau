using System.Diagnostics;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML;

// TODO 実装中
// TODO where T : Texture を where T : Appearance に変える
public class AppearanceTargetDefinition<T> : Object where T : Texture
{
    internal AppearanceTargetDefinition(IntPtr handle) : base(handle)
    {
    }
    
    // AppearanceTargetDefinition の具体的な型は
    // TextureTargetDefinition か MaterialTargetDefinition のどちらかです。
    // どちらかによって呼ぶべき NativeMethod が変わるので場合分けします。
    
    public T Appearance {
        get
        {
            switch (this)
            {
                case TextureTargetDefinition:
                    IntPtr ptr = DLLUtil.GetNativeValue<IntPtr>(
                        Handle,
                        NativeMethods.plateau_appearance_target_definition_tex_get_appearance
                    );
                    return (T)new Texture(ptr);
                case MaterialTargetDefinition:
                    // 未実装
                    throw new NotImplementedException();
                default:
                    throw new NotImplementedException();
            }
        }
    }

    public string TargetID
    {
        get
        {
            switch (this)
            {
                case TextureTargetDefinition:
                    return DLLUtil.GetNativeString(Handle,
                        NativeMethods.plateau_appearance_target_definition_tex_get_target_id);
                case MaterialTargetDefinition:
                    // MaterialTargetDefinitionは未実装です。
                    throw new NotImplementedException();
                default:
                    throw new NotImplementedException();
            }
        }
    }
}