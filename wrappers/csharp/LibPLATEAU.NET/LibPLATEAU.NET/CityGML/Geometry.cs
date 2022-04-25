using System;
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
                APIResult result = NativeMethods.plateau_geometry_get_child_geometry(Handle, out IntPtr childHandle, index);
                DLLUtil.CheckDllError(result);
                return new Geometry(childHandle);
            });
            return geom;
        }

        public Polygon GetPolygon(int index)
        {
            var poly = DLLUtil.ArrayCache(ref this.cachedPolygons, index, PolygonCount, () =>
            {
                APIResult result = NativeMethods.plateau_geometry_get_polygon(Handle, out IntPtr polyHandle, index);
                DLLUtil.CheckDllError(result);
                return new Polygon(polyHandle);
            });
            return poly;
        }
        

        public override string ToString()
        {
            return $"[ Geometry : (id: {ID}) , (childGeometryCount: {ChildGeometryCount}), (polygonCount: {PolygonCount}) (attributesMap: {AttributesMap}) ]";
        }
    }
}