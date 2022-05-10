namespace LibPLATEAU.NET.CityGML;

// TODO AppearanceTargetDefinition を継承する
public class TextureTargetDefinition
{
    private readonly IntPtr handle;

    public TextureTargetDefinition(IntPtr handle)
    {
        this.handle = handle;
    }
}