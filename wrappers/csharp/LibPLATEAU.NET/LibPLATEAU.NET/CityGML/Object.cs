using System;
using System.Runtime.InteropServices;

namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// CityGMLにおける全てのオブジェクトのベースクラスです。
    /// ユニークIDと0個以上の属性ペアを持ち、属性ペアはすべて AttributesDictionary に格納されています。
    /// </summary>
    public class Object
    {
        private IntPtr handle;
        private AttributesDictionary? attributesDictionary;
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
        /// 属性の辞書を取得します。
        /// </summary>
        public AttributesDictionary AttributesDictionary
        {
            get
            {
                if (this.attributesDictionary == null)
                {
                    var dictPtr = NativeMethods.plateau_object_get_attributes_map(Handle);
                    var dict = new AttributesDictionary(dictPtr);
                    this.attributesDictionary = dict;
                }
                return this.attributesDictionary;
            }
        }
        
    }
}