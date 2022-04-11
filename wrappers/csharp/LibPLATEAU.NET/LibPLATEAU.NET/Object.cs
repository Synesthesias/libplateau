using System.Runtime.InteropServices;

namespace LibPLATEAU.NET
{
    /// <summary>
    /// CityGMLにおける全てのオブジェクトのベースクラスです。
    /// ユニークIDと属性データを持ちます。
    /// </summary>
    public class Object
    {
        private IntPtr handle;
        private string id = "";

        internal Object(IntPtr handle)
        {
            this.handle = handle;
        }

        /// <summary>
        /// セーフハンドルを取得します。
        /// </summary>
        public IntPtr Handle => this.handle;

        /// <summary>
        /// オブジェクトのユニークIDを取得します。
        /// </summary>
        public string ID
        {
            get
            {
                if (this.id != "")
                {
                    return this.id;
                }
                this.id = Marshal.PtrToStringAnsi(NativeMethods.plateau_object_get_id(this.handle)) ?? "";
                return this.id;
            }
        }
    }
}
