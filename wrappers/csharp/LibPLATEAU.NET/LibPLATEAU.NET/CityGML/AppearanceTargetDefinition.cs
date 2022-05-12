using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML;

// TODO 実装中
public class AppearanceTargetDefinition : Object
{
    internal AppearanceTargetDefinition(IntPtr handle) : base(handle)
    {
    }

    public string TargetID
    {
        get
        {
            // AppearanceTargetDefinition の具体的な型は
            // TextureTargetDefinition か MaterialTargetDefinition のどちらかです。
            // どちらかによって呼ぶべき NativeMethod が変わるので場合分けします。
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