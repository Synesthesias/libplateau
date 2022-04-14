using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Security.Cryptography;
using System.Text;

namespace LibPLATEAU.NET
{
    // C++ における map は　C# における Dictionary に相当するので、
    // C#側では名前を AttributesMap から AttributesDictionary に変えます。
    
    public class AttributesDictionary
    {
        private IntPtr handle;

        internal AttributesDictionary(IntPtr handle)
        {
            this.handle = handle;
        }

        public unsafe string[] GetKeys()
        {
            int[] keySizes = GetKeySizes();
            int cnt = Count;
            string[] ret = Enumerable.Repeat("", cnt).ToArray();
            // var outSBs = new StringBuilder[cnt];
            // for (int i = 0; i < cnt; i++)
            // {
            //     outSBs[i] = new StringBuilder(keySizes[i]);
            // }
            
            // IntPtr[] stringPointers = new IntPtr[cnt];
            // for (int i = 0; i < cnt; i++)
            // {
            //     stringPointers[i] = Marshal.AllocCoTaskMem(keySizes[i]);
            // }
            //
            // IntPtr ptrOfStringPointers = &stringPointers;
            // NativeMethods.plateau_attributes_map_get_keys(this.handle, ref outSBs);
            
            // Shift-JISを使うために必要
            Encoding.RegisterProvider(CodePagesEncodingProvider.Instance);

            byte** ptrOfStringPtr = stackalloc byte*[cnt];
            for (int i = 0; i < cnt; i++)
            {
                byte* stringPtr = stackalloc byte[keySizes[i]];
                ptrOfStringPtr[i] = stringPtr;
            }
            NativeMethods.plateau_attributes_map_get_keys(this.handle, ptrOfStringPtr);
            for (int i = 0; i < cnt; i++)
            {
                byte[] stringBytes = new byte[keySizes[i]];
                Marshal.Copy((IntPtr)ptrOfStringPtr[i], stringBytes, 0, keySizes[i]);
                ret[i] = Encoding.GetEncoding("Shift_JIS").GetString(stringBytes);
            }

            // foreach (IntPtr ptr in stringPointers)
            // {
            //     Marshal.FreeCoTaskMem(ptr);
            // }

            return ret;
        }

        /// <summary>
        /// 属性の各キーの文字列としてのバイト数を配列で返します。
        /// 例外が起きたときは各要素が -1 の配列を返します。
        /// </summary>
        public int[] GetKeySizes()
        {
            int cnt = Count;
            int intArraySize = Marshal.SizeOf(typeof(int)) * cnt;
            IntPtr intArrayPtr = Marshal.AllocCoTaskMem(intArraySize);
            int[] ret = Enumerable.Repeat(-1, cnt).ToArray();
            try
            {
                // intArrayPtr に欲しい情報を格納します。
                NativeMethods.plateau_attributes_map_get_key_sizes(this.handle, intArrayPtr);
                // intArrayPtr の内容を C#の配列のコピーします。
                Marshal.Copy(intArrayPtr, ret, 0, cnt);
            }
            finally
            {
                Marshal.FreeCoTaskMem(intArrayPtr);
            }

            return ret;
        }

        public int Count => NativeMethods.plateau_attributes_map_get_key_count(this.handle);
    }
}