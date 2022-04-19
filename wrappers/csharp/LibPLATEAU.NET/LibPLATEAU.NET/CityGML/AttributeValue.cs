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
                int valueStrSize = NativeMethods.plateau_attribute_value_get_str_length(
                    this.handle);
                StringBuilder sb = new(valueStrSize);
                NativeMethods.plateau_attribute_value_get_string(
                    this.handle, sb);
                return sb.ToString();
            }
        }

        public AttributeType Type => NativeMethods.plateau_attribute_value_get_type(this.handle);

        
        /// <summary>
        /// AttributeValue のタイプが AttributeSet であることを前提に、
        /// 子の AttributesDictionary を返します。
        /// </summary>
        public AttributesDictionary ChildAttrs
        {
            get
            {
                IntPtr ptr = NativeMethods.plateau_attribute_as_attribute_set(this.handle);
                return new AttributesDictionary(ptr);
            }
        }
    }
}