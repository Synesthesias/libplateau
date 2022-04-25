﻿using System;
using System.Text;
using LibPLATEAU.NET.Util;


namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// <para>
    /// Plateau のオブジェクトが持つ属性 (key と value のペア)　のうち、 value の部分です。
    /// value は、内部的には string または 子の属性セットへの参照 のどちらかのデータを保持します。</para>
    ///
    /// <para>
    /// value の値がどのような形式であるかを <see cref="AttributeType"/> 形式で保持し、<see cref="Type"/> で取得できます。</para>
    ///
    /// <para>
    /// <see cref="Type" /> が String, Double, Integer, Data, Uri, Measure の場合、内部的にはデータは単に string であり、 <see cref="AsString"/> で取得できます。
    /// <see cref="AsDouble"/>, <see cref="AsInt"/> プロパティもありますが、それは単に <see cref="AsString"/> を数値にパースするものです。</para>
    ///
    /// <para>
    /// <see cref="Type"/> が <see cref="AttributeType.AttributeSet"/> である場合、 この属性の子に属性セットがあることを意味します。
    /// この場合は <see cref="AsString"/> は意味を成さず、代わりに <see cref="AsAttrSet" /> で子の属性セットを取得できます。</para>
    /// </summary>
    public class AttributeValue
    {
        private IntPtr handle;

        public AttributeValue(IntPtr handle)
        {
            this.handle = handle;
        }

        /// <summary>
        /// 属性値を string 形式で取得します。
        /// <see cref="Type"/> が String, Double, Integer, Data, Uri, Measure の場合に有効です。
        /// <see cref="Type"/> が <see cref="AttributeType.AttributeSet"/> の場合は意味を成しません。
        /// </summary>
        public string AsString
        {
            get
            {
                int valueStrSize = DLLUtil.GetNativeValue<int>(this.handle,
                    NativeMethods.plateau_attribute_value_get_str_length);
                StringBuilder sb = new(valueStrSize);
                NativeMethods.plateau_attribute_value_get_string(
                    this.handle, sb);
                return sb.ToString();
            }
        }
        

        /// <summary> 属性値を double にパースして返します。 </summary>
        public double AsDouble => Double.Parse(AsString);
        
        /// <summary> 属性値を int にパースして返します。 </summary>
        public int AsInt => int.Parse(AsString);
        

        /// <summary>
        /// 属性の値の想定形式です。<see cref="AttributeType"/> 型で返します。
        /// </summary>
        public AttributeType Type
        {
            get
            {
                AttributeType type = DLLUtil.GetNativeValue<AttributeType>(this.handle,
                    NativeMethods.plateau_attribute_value_get_type);
                return type;
            }
        }


        /// <summary>
        /// <see cref="Type"/> が <see cref="AttributeType.AttributeSet"/> であることを前提に、
        /// 子の <see cref="AttributesMap"/> を返します。
        /// <see cref="Type"/> がその他である場合は意味を成しません。
        /// </summary>
        public AttributesMap AsAttrSet
        {
            get
            {
                IntPtr attributesHandle = DLLUtil.GetNativeValue<IntPtr>(this.handle,
                    NativeMethods.plateau_attribute_as_attribute_set);
                return new AttributesMap(attributesHandle);
            }
        }
    }
}