using System;
using System.Runtime.CompilerServices;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// CityGMLにおける都市オブジェクトです。
    /// </summary>
    public class CityObject : FeatureObject
    {
        private CityObjectType type = 0;
        private Address? cachedAddress;
        private CityObject?[]? cachedChildCityObjects;
        private Geometry?[]? cachedGeometries;

        internal CityObject(IntPtr handle) : base(handle)
        {
        }

        public CityObjectType Type
        {
            get
            {
                if (this.type != 0)
                {
                    return this.type;
                }

                APIResult result = NativeMethods.plateau_city_object_get_type(this.Handle, out CityObjectType objType);
                DLLUtil.CheckDllError(result);
                this.type = objType;
                return this.type;
            }
        }

        /// <summary>
        /// ジオメトリの数を返します。
        /// </summary>
        public int GeometryCount
        {
            get
            {
                APIResult result = NativeMethods.plateau_city_object_get_geometries_count(this.Handle, out int count);
                DLLUtil.CheckDllError(result);
                return count;
            }
        }

        public int ImplicitGeometryCount
        {
            get
            {
                APIResult result =
                    NativeMethods.plateau_city_object_get_implicit_geometry_count(this.Handle, out int count);
                DLLUtil.CheckDllError(result);
                return count;
            }
        }

        public Address Address
        {
            get
            {
                if (this.cachedAddress != null) return this.cachedAddress;
                var addressHandle = DLLUtil.GetNativeValue<IntPtr>(
                    Handle,
                    NativeMethods.plateau_city_object_get_address
                );
                this.cachedAddress = new Address(addressHandle);
                return this.cachedAddress;
            }
        }

        public int ChildCityObjectCount
        {
            get
            {
                int count = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_city_object_get_child_city_object_count);
                return count;
            }
        }
        

        public CityObject GetChildCityObject(int index)
        {
            var ret = DLLUtil.ArrayCache(ref this.cachedChildCityObjects, index, ChildCityObjectCount, () =>
            {
                APIResult result = NativeMethods.plateau_city_object_get_child_city_object(this.Handle, out IntPtr childHandle, index);
                DLLUtil.CheckDllError(result);
                return new CityObject(childHandle);
            });

            return ret;
        }

        

        public Geometry GetGeometry(int index)
        {
            var geom = DLLUtil.ArrayCache(ref this.cachedGeometries, index, GeometryCount, () =>
            {
                APIResult result = NativeMethods.plateau_city_object_get_geometry(Handle, out IntPtr geomHandle, index);
                DLLUtil.CheckDllError(result);
                return new Geometry(geomHandle);
            });
            return geom;
        }
    }
}