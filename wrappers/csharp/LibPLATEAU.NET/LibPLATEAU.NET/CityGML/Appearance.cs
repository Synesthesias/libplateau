﻿using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML;
// TODO 実装中
public class Appearance : Object
{
    internal Appearance(IntPtr handle) : base(handle)
    {
    }

    public string Type =>
        DLLUtil.GetNativeString(Handle,
            NativeMethods.plateau_appearance_get_type);

    public bool IsFront =>
        DLLUtil.GetNativeValue<bool>(Handle,
            NativeMethods.plateau_appearance_get_is_front);

}