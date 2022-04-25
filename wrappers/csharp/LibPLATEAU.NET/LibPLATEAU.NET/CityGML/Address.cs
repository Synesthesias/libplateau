using System;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    public class Address
    {
        private IntPtr handle;

        public Address(IntPtr handle)
        {
            this.handle = handle;
        }

        public string Country =>
            DLLUtil.GetNativeString(
                this.handle,
                NativeMethods.plateau_address_get_country_str_length,
                NativeMethods.plateau_address_get_country
            );

        public string Locality =>
            DLLUtil.GetNativeString(
                this.handle,
                NativeMethods.plateau_address_get_locality_str_length,
                NativeMethods.plateau_address_get_locality
            );

        public string PostalCode =>
            DLLUtil.GetNativeString(
                this.handle,
                NativeMethods.plateau_address_get_postal_code_str_length,
                NativeMethods.plateau_address_get_postal_code
            );

        public string ThoroughFareName =>
            DLLUtil.GetNativeString(
                this.handle,
                NativeMethods.plateau_address_get_thoroughfare_name_str_length,
                NativeMethods.plateau_address_get_thoroughfare_name
            );

        public string ThoroughFareNumber =>
            DLLUtil.GetNativeString(
                this.handle,
                NativeMethods.plateau_address_get_thoroughfare_number_str_length,
                NativeMethods.plateau_address_get_thoroughfare_number
            );
    }
}