using System;
using System.Text;
using LibPLATEAU.NET.Util;


namespace LibPLATEAU.NET.CityGML
{
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
        public AttributesDictionary ChildAttrSet
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