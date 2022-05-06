﻿using System;
using System.Collections.Generic;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// 多角形による形状表現です。
    /// 頂点座標のリストを保持します。
    /// <see cref="Polygon"/> が <see cref="LinearRing"/> を保持します。（ただし条件付きで一部のみです。）
    /// </summary>
    public class LinearRing : Object
    {
        internal LinearRing(IntPtr handle) : base(handle)
        {
        }

        /// <summary> 頂点数を取得します。 </summary>
        public int VerticesCount
        {
            get
            {
                int count = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_linear_ring_get_vertices_count);
                return count;
            }
        }

        /// <summary>
        /// <paramref name="index"/> 番目の頂点座標を取得します。
        /// </summary>
        public PlateauVector3d GetVertex(int index)
        {
            var vert3d = DLLUtil.GetNativeValue<PlateauVector3d>(Handle, index,
                NativeMethods.plateau_linear_ring_get_vertex);
            return vert3d;
        }
    }
}