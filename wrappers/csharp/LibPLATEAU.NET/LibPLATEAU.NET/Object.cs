using System;
using System.Reflection.Emit;
using System.Runtime.InteropServices;
using System.Text;

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

        /// <summary>
        /// 属性データを取得します。
        /// 属性値の最大文字数を引数で指定します。
        /// </summary>
        public string GetAttribute(string name, int returnStrMaxSize) {
            StringBuilder sb = new (returnStrMaxSize);
            
            NativeMethods.plateau_object_get_attribute(this.handle, name, sb, sb.Capacity);
            return sb.ToString();
        }


        /// <summary>
        /// 属性データを設定します。
        /// </summary>
        public void SetAttribute(string name, string value, AttributeType type, bool doOverride) {
            NativeMethods.plateau_object_set_attribute(this.handle, name, value, type, doOverride);
        }
    }
}
