using System;

namespace LibPLATEAU.NET
{
    /// <summary>
    /// CityGMLにおける全ての地物オブジェクトのベースクラスです。
    /// </summary>
    public class FeatureObject : Object
    {
        internal FeatureObject(IntPtr handle) : base(handle) { }
    }
}
