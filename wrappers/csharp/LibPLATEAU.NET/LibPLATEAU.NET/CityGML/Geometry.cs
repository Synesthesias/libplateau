﻿using System;
using System.Collections;
using System.Collections.Generic;
using System.Text;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// 建築物の形状と見た目の情報を保持します。
    /// <see cref="CityObject"/> が<see cref="Geometry"/>を保持します。
    /// <see cref="Geometry"/> は <see cref="Polygon"/> , 子<see cref="Geometry"/> を保持します。
    /// </summary>
    public class Geometry : AppearanceTarget
    {
        private Geometry?[]? cachedChildGeometries;
        private Polygon?[]? cachedPolygons;
        public Geometry(IntPtr handle) : base(handle)
        {
        }

        public GeometryType Type => DLLUtil.GetNativeValue<GeometryType>(Handle, NativeMethods.plateau_geometry_get_type);

        
        /// <summary> 子の <see cref="Geometry"/> の数を返します。 </summary>
        public int ChildGeometryCount => DLLUtil.GetNativeValue<int>(Handle, NativeMethods.plateau_geometry_get_geometries_count);
        
        
        /// <summary> インデックス指定で子の <see cref="Geometry"/> を1つ返します。 </summary>
        public Geometry GetChildGeometry(int index)
        {
            var geom = DLLUtil.ArrayCache(ref this.cachedChildGeometries, index, ChildGeometryCount, () =>
            {
                IntPtr childHandle = DLLUtil.GetNativeValue<IntPtr>(Handle, index,
                    NativeMethods.plateau_geometry_get_child_geometry);
                return new Geometry(childHandle);
            });
            return geom;
        }


        /// <summary> 子の <see cref="Geometry"/> をforeachやLinqで回したい時に利用できます。 </summary>
        public IEnumerable<Geometry> ChildGeometries
        {
            get
            {
                int cnt = ChildGeometryCount;
                for (int i = 0; i < cnt; i++)
                {
                    yield return GetChildGeometry(i);
                }
            }
        }

        /// <summary> <see cref="Polygon"/> の数を返します。 </summary>
        public int PolygonCount => DLLUtil.GetNativeValue<int>(Handle, NativeMethods.plateau_geometry_get_polygons_count);
        
        
        /// <summary> インデックス指定で <see cref="Polygon"/> を1つ返します。 </summary>
        public Polygon GetPolygon(int index)
        {
            var poly = DLLUtil.ArrayCache(ref this.cachedPolygons, index, PolygonCount, () =>
            {
                IntPtr polyHandle = DLLUtil.GetNativeValue<IntPtr>(Handle, index,
                    NativeMethods.plateau_geometry_get_polygon);
                return new Polygon(polyHandle);
            });
            return poly;
        }
        

        /// <summary>
        /// <see cref="Polygon"/> をforeachやLinqで回したい時に利用できます。
        /// </summary>
        public IEnumerable<Polygon> Polygons
        {
            get
            {
                int cnt = PolygonCount;
                for (int i = 0; i < cnt; i++)
                {
                    yield return GetPolygon(i);
                }
            }
        }
        
        
        /// <summary>
        /// LOD (Level Of Detail) を取得します。
        /// LOD は 0 がもっとも簡略化された形状であり、数字が上がるほど形状が詳細であることを意味します。
        /// </summary>
        public int LOD
        {
            get
            {
                int lod = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_geometry_get_lod);
                return lod;
            }
        }
        
        
        /// <summary> デバッグ用に自身に関する情報を文字列で返します。 </summary>
        public override string ToString()
        {
            return $"[ Geometry : (id: {ID}) , {ChildGeometryCount} child geometries , {PolygonCount} polygons , {LineStringCount} line strings , (attributesMap: {AttributesMap}) ]";
        }
        
        
        /// <summary>
        /// LineString の数を返します。
        /// TODO LineString の取得は未実装です。 GMLファイルが LineString を含むケースが今のところ見当たらないため
        /// </summary>
        public int LineStringCount
        {
            get
            {
                int count = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_geometry_get_line_string_count);
                return count;
            }
        }

        /// <summary>
        /// SRSName を取得します。
        /// TODO テスト未実装　GeometryでSRSNameが出てくる例が見当たらないため
        /// </summary>
        public string SRSName
        {
            get
            {
                string srsName = DLLUtil.GetNativeString(Handle,
                    NativeMethods.plateau_geometry_get_srs_name_str_length,
                    NativeMethods.plateau_geometry_get_srs_name);
                return srsName;
            }
        }
        

        /// <summary>
        /// デバッグ用に、自身と子の <see cref="Geometry"/> の情報を再帰的に表示します。
        /// </summary>
        public string DebugGeometryHierarchy()
        {
            var sb = new StringBuilder();
            DebugGeometryHierarchyRecursive(this, sb,  0);
            return sb.ToString();
        }

        
        private void DebugGeometryHierarchyRecursive(Geometry geom, StringBuilder sb, int recursiveDepth)
        {
            sb.Append("\n");
            // インデント
            for (int i = 0; i < recursiveDepth; i++)
            {
                sb.Append("--");
            }
            // 文字列化
            sb.Append(this);
            foreach (var c in ChildGeometries)
            {
                DebugGeometryHierarchyRecursive(c, sb, recursiveDepth + 1);
            }
        }

        
        
    }
}