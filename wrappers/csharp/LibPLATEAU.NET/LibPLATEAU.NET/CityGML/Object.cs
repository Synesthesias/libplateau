using System;
using System.Runtime.InteropServices;

namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// CityGMLにおける全てのオブジェクトのベースクラスです。
    /// ユニークIDと0個以上の属性ペアを持ち、属性ペアはすべて AttributesMap に格納されています。
    /// </summary>
    public class Object
    {
        private IntPtr handle;
        private AttributesMap? attributesMap;
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
        public AttributesMap AttributesMap
        {
            get
            {
                if (this.attributesMap == null)
                {
                    var mapPtr = NativeMethods.plateau_object_get_attributes_map(Handle);
                    var map = new AttributesMap(mapPtr);
                    this.attributesMap = map;
                }
                return this.attributesMap;
            }
        }
        
    }
}