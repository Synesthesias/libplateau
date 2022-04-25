﻿using System;
using System.Runtime.InteropServices;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// CityGMLにおける全てのオブジェクトのベースクラスです。
    /// ユニークIDと0個以上の属性ペアを持ち、属性ペアはすべて <see cref="CityGML.AttributesMap"/> に格納されています。
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
                
                IntPtr strPtr = DLLUtil.GetNativeValue<IntPtr>(this.handle,
                    NativeMethods.plateau_object_get_id);
                this.id = Marshal.PtrToStringAnsi(strPtr) ?? "";
                return this.id;
            }
        }

        /// <summary> 属性の辞書を取得します。 </summary>
        /// <returns> <see cref="CityGML.AttributesMap"/> 型で返します。</returns>
        public AttributesMap AttributesMap
        {
            get
            {
                if (this.attributesMap == null)
                {
                    IntPtr mapPtr = DLLUtil.GetNativeValue<IntPtr>(Handle,
                        NativeMethods.plateau_object_get_attributes_map);
                    var map = new AttributesMap(mapPtr);
                    this.attributesMap = map;
                }
                return this.attributesMap;
            }
        }
        
    }
}