using System;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    public class Polygon : AppearanceTarget
    {
        internal Polygon(IntPtr handle) : base(handle)
        {
        }

        public int VertexCount
        {
            get
            {
                int vertCount = DLLUtil.GetNativeValue<int>(Handle, NativeMethods.plateau_polygon_get_vertices_count);
                return vertCount;
            }
        }
    }
}