using System;
using System.Collections.ObjectModel;
using System.Threading;

namespace LibPLATEAU.NET
{
    public sealed class CityModel : IDisposable
    {
        private IntPtr handle;
        private int disposed;
        private CityObject[] rootCityObjects;

        /// <summary>
        /// セーフハンドルを取得します。
        /// </summary>
        public IntPtr Handle => this.handle;

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
            if (Interlocked.Exchange(ref disposed, 1) == 0)
            {
                NativeMethods.plateau_delete_city_model(this.handle);
            }
            GC.SuppressFinalize(this);
        }
    }
}
