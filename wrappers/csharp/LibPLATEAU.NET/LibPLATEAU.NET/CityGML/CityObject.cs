using System;

namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// CityGMLにおける都市オブジェクトです。
    /// </summary>
    public class CityObject : FeatureObject
    {
        private CityObjectType type = 0;

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

                this.type = NativeMethods.plateau_city_object_get_type(this.Handle);
                return this.type;
            }
        }

        /// <summary>
        /// ジオメトリの数を返します。
        /// ライブラリ内で例外が起きた場合は -1 が返ります。
        /// </summary>
        public int GeometriesCount => NativeMethods.plateau_city_object_get_geometries_count(this.Handle);
    }
}