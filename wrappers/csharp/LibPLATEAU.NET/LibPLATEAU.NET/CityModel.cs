using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace LibPLATEAU.NET
{
    public class CityModel : IDisposable
    {
        private IntPtr handle;

        public CityModel(IntPtr handle)
        {
            this.handle = handle;
        }

        /// <summary>
        /// セーフハンドルを取得します。
        /// </summary>
        public IntPtr Handle => this.handle;

        public void Dispose()
        {
            // TODO: shared_ptr解放
        }
    }
}
