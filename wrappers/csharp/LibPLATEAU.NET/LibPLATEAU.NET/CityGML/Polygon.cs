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

        public PlateauVector3d GetVertex(int index)
        {
            var vert = DLLUtil.GetNativeValue<PlateauVector3d>(Handle, index,
                NativeMethods.plateau_polygon_get_vertex);
            return vert;
        }

        public int IndicesCount
        {
            get
            {
                int count = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_polygon_get_indices_count);
                return count;
            }
        }
    }
}