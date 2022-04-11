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
        /// 属性データを取得し、stringで返します。
        /// <para name="returnStrMaxSize"> 取得する結果の、文字列としての最大サイズ(byte数)を指定します。</para>
        /// <para name="result">
        /// result は正常終了の場合 0, 異常終了の場合それ以外になります。
        /// 具体的には、結果を格納するのに最大サイズで足りない場合は -2 に、
        /// 属性が存在しないまたは値が空文字列である場合は -1 になります。
        /// </para>
        /// </summary>
        public string GetAttribute(string name, int returnStrMaxSize, out int result) {
            StringBuilder sb = new (returnStrMaxSize);
            
            result = NativeMethods.plateau_object_get_attribute(this.handle, name, sb, sb.Capacity);
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
