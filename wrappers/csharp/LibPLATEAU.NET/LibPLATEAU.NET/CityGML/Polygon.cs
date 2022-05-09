using System;
using System.Collections.Generic;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// 建築物の形状におけるポリゴンです。
    /// <see cref="Geometry"/> が <see cref="Polygon"/> を保持します。
    /// <see cref="Polygon"/> は Vertices , Indices を保持します。
    /// ただし、GMLファイルのパース時に <see cref="CitygmlParserParams.Tessellate"/> を false に設定した時に限り、
    /// Vertices, Indices の代わりに <see cref="ExteriorRing"/> , <see cref="InteriorRings"/> を保持することがあります。
    /// </summary>
    public class Polygon : AppearanceTarget
    {
        private LinearRing? cachedExteriorRing;
        private LinearRing?[]? cachedInteriorRings;
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

        public IEnumerable<int> Indices
        {
            get
            {
                int cnt = IndicesCount;
                for (int i = 0; i < cnt; i++)
                {
                    yield return GetIndexOfIndices(i);
                }
            }
        }

        /// <summary>
        /// 建物の外周の形状です。
        /// GMLファイルのパース時の設定で <see cref="CitygmlParserParams.Tessellate"/> が true の場合、この情報は保持されません。
        /// false の場合、 Vertices, Indices の代わりに <see cref="ExteriorRing"/>, <see cref="InteriorRings"/>が保持される場合があります。
        /// </summary>
        public LinearRing ExteriorRing
        {
            get
            {
                if (this.cachedExteriorRing != null) return this.cachedExteriorRing;
                IntPtr ringHandle = DLLUtil.GetNativeValue<IntPtr>(Handle,
                    NativeMethods.plateau_polygon_get_exterior_ring);
                this.cachedExteriorRing = new LinearRing(ringHandle);
                return this.cachedExteriorRing;
            }
        }

        public int InteriorRingCount =>
            DLLUtil.GetNativeValue<int>(Handle,
                NativeMethods.plateau_polygon_get_interior_ring_count);

        public LinearRing GetInteriorRing(int index)
        {
            var ring = DLLUtil.ArrayCache(ref this.cachedInteriorRings, index, InteriorRingCount, () =>
            {
                IntPtr ringHandle = DLLUtil.GetNativeValue<IntPtr>(Handle, index,
                    NativeMethods.plateau_polygon_get_interior_ring);
                return new LinearRing(ringHandle);
            });
            return ring;
        }

        public IEnumerable<LinearRing> InteriorRings
        {
            get
            {
                int cnt = InteriorRingCount;
                for (int i = 0; i < cnt; i++)
                {
                    yield return GetInteriorRing(i);
                }
            }
        }
    }
}