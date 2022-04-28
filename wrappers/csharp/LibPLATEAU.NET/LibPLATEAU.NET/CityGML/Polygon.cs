using System;
using System.Collections.Generic;
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

        public int GetIndexOfIndices(int indexOfIndicesList)
        {
            int ret = DLLUtil.GetNativeValue<int>(Handle, indexOfIndicesList,
                NativeMethods.plateau_polygon_get_index_of_indices);
            return ret;
        }

        public IReadOnlyList<int> Indices
        {
            get
            {
                int cnt = IndicesCount;
                int[] indices = new int[cnt];
                for (int i = 0; i < cnt; i++)
                {
                    indices[i] = GetIndexOfIndices(i);
                }

                return indices;
            }
        }
    }
}