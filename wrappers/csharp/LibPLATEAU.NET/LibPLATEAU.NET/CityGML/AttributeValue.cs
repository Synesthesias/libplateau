using System;
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

        public string StringValue
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
    }
}