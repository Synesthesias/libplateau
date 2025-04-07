using System;
using System.Runtime.InteropServices;
using PLATEAU.Interop;
using PLATEAU.Native;

namespace PLATEAU.Dataset
{
    /// <summary>
    /// メッシュコードと国土基本図の図郭のC++上の親クラスです。
    /// </summary>
    public class GridCode : PInvokeDisposable
    {
        internal GridCode(IntPtr handle, bool autoDispose = true) : base(handle, autoDispose)
        {
        }

        public static GridCode Parse(string code)
        {
            var result = NativeMethods.plateau_grid_code_parse(code, out var gridCodePtr);
            DLLUtil.CheckDllError(result);
            return new GridCode(gridCodePtr);
        }

        public GridCode Copy()
        {
            return Parse(StringCode);
        }
        
        public Extent Extent
        {
            get
            {
                ThrowIfInvalid();
                Extent value = new Extent();
                APIResult result = NativeMethods.plateau_grid_code_get_extent(Handle, ref value);
                DLLUtil.CheckDllError(result);
                return value;
            }
        }

        public string StringCode
        {
            get
            {
                ThrowIfInvalid();
                return DLLUtil.GetNativeStringByValue(
                    Handle,
                    NativeMethods.plateau_grid_code_get_string_code_size,
                    NativeMethods.plateau_grid_code_get_string_code
                );
            }
        }
        
        public bool IsValid
        {
            get
            {
                var result = NativeMethods.plateau_grid_code_is_valid(Handle, out bool resultIsValid);
                DLLUtil.CheckDllError(result);
                return resultIsValid;
            }
        }
        
        private void ThrowIfInvalid()
        {
            if (IsValid) return;
            throw new Exception("Invalid GridCode.");
        }

        protected override void DisposeNative()
        {
            var result = NativeMethods.plateau_grid_code_delete(Handle);
            DLLUtil.CheckDllError(result);
        }

        private static class NativeMethods
        {
            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_grid_code_parse(
                [In] string code,
                out IntPtr gridCodePtr
            );
            
            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_grid_code_get_extent(
                [In] IntPtr gridCode,
                [In, Out] ref Extent outExtent);

            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_grid_code_delete(
                [In] IntPtr gridCodePtr
            );
            
            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_grid_code_is_valid(
                [In] IntPtr gridCodePtr,
                [MarshalAs(UnmanagedType.U1)] out bool outIsValid);

            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_grid_code_get_string_code_size(
                [In] IntPtr gridCodePtr,
                out int strSize
            );
            
            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_grid_code_get_string_code(
                [In] IntPtr gridCodePtr,
                [In, Out] IntPtr outStringCode
            );
        }
    }
}
