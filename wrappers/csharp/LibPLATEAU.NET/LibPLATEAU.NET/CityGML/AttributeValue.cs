using System;
using System.Security.Cryptography.X509Certificates;
using System.Text;

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
                if (result != APIResult.Success) throw new Exception(result.ToString());
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
                if (result != APIResult.Success) throw new Exception(result.ToString());
                return type;
            }
        }


        /// <summary>
        /// AttributeValue のタイプが AttributeSet であることを前提に、
        /// 子の AttributesDictionary を返します。
        /// </summary>
        public AttributesDictionary ChildAttrs
        {
            get
            {
                APIResult result = NativeMethods.plateau_attribute_as_attribute_set(this.handle, out IntPtr ptr);
                if (result != APIResult.Success) throw new Exception(result.ToString());
                return new AttributesDictionary(ptr);
            }
        }
    }
}