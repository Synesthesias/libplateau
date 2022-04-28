using System;
using System.Collections.Generic;
using System.Reflection.Metadata;
using System.Text;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    public class Geometry : AppearanceTarget
    {
        private Geometry?[]? cachedChildGeometries;
        private Polygon?[]? cachedPolygons;
        public Geometry(IntPtr handle) : base(handle)
        {
        }

        public GeometryType Type
        {
            get
            {
                var type = DLLUtil.GetNativeValue<GeometryType>(Handle,
                    NativeMethods.plateau_geometry_get_type);
                return type;
            }
        }

        public int ChildGeometryCount
        {
            get
            {
                int count = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_geometry_get_geometries_count);
                return count;
            }
        }

        public IReadOnlyList<Geometry> ChildGeometries
        {
            get
            {
                int cnt = ChildGeometryCount;
                var children = new Geometry[cnt];
                for (int i = 0; i < cnt; i++)
                {
                    children[i] = GetChildGeometry(i);
                }

                return children;
            }
        }

        public int PolygonCount
        {
            get
            {
                int count = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_geometry_get_polygons_count);
                return count;
            }
        }

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
        

        public override string ToString()
        {
            return $"[ Geometry : (id: {ID}) , {ChildGeometryCount} child geometries , {PolygonCount} polygons , {LineStringCount} line strings , (attributesMap: {AttributesMap}) ]";
        }

        public int LOD
        {
            get
            {
                int lod = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_geometry_get_lod);
                return lod;
            }
        }

        /// <summary>
        /// 自身と子の <see cref="Geometry"/> の情報を再帰的に表示します。
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

        public int LineStringCount
        {
            get
            {
                int count = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_geometry_get_line_string_count);
                return count;
            }
        }
        
        // LineString の取得は未実装です。 GMLファイルが LineString を含むケースが今のところ見当たらないため

        public string SRSName
        {
            // TODO テスト未実装　GeometryでSRSNameが出てくる例が見当たらないため
            get
            {
                string srsName = DLLUtil.GetNativeString(Handle,
                    NativeMethods.plateau_geometry_get_srs_name_str_length,
                    NativeMethods.plateau_geometry_get_srs_name);
                return srsName;
            }
        }
    }
}