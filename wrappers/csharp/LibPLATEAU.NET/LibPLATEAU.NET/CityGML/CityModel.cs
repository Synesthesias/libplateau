using System;
using System.Collections.ObjectModel;
using System.Threading;

namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// GMLファイルをパースして得られる街のモデルです。
    /// 0個以上の <see cref="CityObject"/> を保持します。
    /// </summary>
    public sealed class CityModel : IDisposable
    {
        private readonly IntPtr handle;
        private int disposed;
        private CityObject[] rootCityObjects;

        /// <summary>
        /// セーフハンドルを取得します。
        /// </summary>
        public IntPtr Handle => this.handle;

        /// <summary>
        /// <see cref="CityModel"/> のトップレベルにある <see cref="CityObject"/> の一覧を返します。
        /// </summary>
        public ReadOnlyCollection<CityObject> RootCityObjects
        {
            get
            {
                if (this.rootCityObjects != null)
                {
                    return Array.AsReadOnly(this.rootCityObjects);
                }

                var count = NativeMethods.plateau_city_model_get_root_city_object_count(this.handle);
                var cityObjectHandles = new IntPtr[count];
                NativeMethods.plateau_city_model_get_root_city_objects(this.handle, cityObjectHandles, count);
                this.rootCityObjects = new CityObject[count];
                for (var i = 0; i < count; ++i)
                {
                    this.rootCityObjects[i] = new CityObject(cityObjectHandles[i]);
                }

                return Array.AsReadOnly(this.rootCityObjects);
            }
        }

        internal CityModel(IntPtr handle)
        {
            this.handle = handle;
        }

        ~CityModel()
        {
            Dispose();
        }

        public void Dispose()
        {
            if (Interlocked.Exchange(ref this.disposed, 1) == 0)
            {
                NativeMethods.plateau_delete_city_model(this.handle);
            }
            GC.SuppressFinalize(this);
        }
    }
}
