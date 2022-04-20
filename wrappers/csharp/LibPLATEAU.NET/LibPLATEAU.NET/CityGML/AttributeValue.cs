using System;
using System.Text;
using LibPLATEAU.NET.Util;


namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// Plateau のオブジェクトが持つ属性 (key と value のペア)　のうち、 value の部分です。
    /// value は、内部的には string または 子の属性セットへの参照 のどちらかのデータを保持します。
    /// 
    /// value の値がどのような形式であるかを enum AttributeType で保持します。
    /// Type が String, Double, Integer, Data, Uri, Measure の場合、内部的にはデータは単に string であり、 AsString で取得できます。
    /// AsDouble, AsInt プロパティもありますが、それは単に AsString を数値にパースするものです。
    /// 
    /// Type が AttributeSet である場合、 この属性の子に属性セットがあることを意味します。
    /// この場合は AsString は意味を成さず、代わりに AsAttrSet で子の属性セットを取得できます。
    /// </summary>
    public class AttributeValue
    {
        private IntPtr handle;

        public AttributeValue(IntPtr handle)
        {
            this.handle = handle;
        }

        public string AsString
        {
            get
            {
                APIResult result = NativeMethods.plateau_attribute_value_get_str_length(
                    this.handle, out int valueStrSize);
                DLLUtil.CheckDllError(result);
                StringBuilder sb = new(valueStrSize);
                NativeMethods.plateau_attribute_value_get_string(
                    this.handle, sb);
                return sb.ToString();
            }
        }

        public double AsDouble => Double.Parse(AsString);
        public int AsInt => int.Parse(AsString);
        

        public AttributeType Type
        {
            get
            {
                APIResult result = NativeMethods.plateau_attribute_value_get_type(this.handle, out AttributeType type);
                DLLUtil.CheckDllError(result);
                return type;
            }
        }


        /// <summary>
        /// AttributeValue のタイプが AttributeSet であることを前提に、
        /// 子の AttributesDictionary を返します。
        /// </summary>
        public AttributesDictionary AsAttrSet
        {
            get
            {
                APIResult result = NativeMethods.plateau_attribute_as_attribute_set(this.handle, out IntPtr ptr);
                DLLUtil.CheckDllError(result);
                return new AttributesDictionary(ptr);
            }
        }
    }
}