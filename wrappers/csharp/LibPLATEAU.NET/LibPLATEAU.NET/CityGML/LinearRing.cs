using System;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    public class LinearRing : Object
    {
        internal LinearRing(IntPtr handle) : base(handle)
        {
        }

        public int VerticesCount
        {
            get
            {
                int count = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_linear_ring_get_vertices_count);
                return count;
            }
        }
    }
}