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

        public string[] GetKeys()
        {
            int[] keySizes = GetKeySizes();
            int cnt = Count;
            string[] ret = Enumerable.Repeat("", cnt).ToArray();
            
            int sizeOfStringPointers = Marshal.SizeOf(typeof(IntPtr)) * cnt;
            IntPtr ptrOfStringPtr = Marshal.AllocCoTaskMem(sizeOfStringPointers);
            IntPtr[] stringPointers = new IntPtr[cnt];
            for (int i = 0; i < cnt; i++)
            {
                IntPtr stringPtr = Marshal.AllocCoTaskMem(keySizes[i]);
                stringPointers[i] = stringPtr;
            }
            Marshal.Copy(stringPointers, 0, ptrOfStringPtr, cnt);
            
            NativeMethods.plateau_attributes_map_get_keys(this.handle, ptrOfStringPtr);
            unsafe
            {
                for (int i = 0; i < cnt; i++)
                {
                    var stringPtr = ((IntPtr*)ptrOfStringPtr)[i];
                    ret[i] = Marshal.PtrToStringAnsi(stringPtr, keySizes[i]);
                }

                for (int i = 0; i < cnt; i++)
                {
                    var stringPtr = ((IntPtr*)ptrOfStringPtr)[i];
                    Marshal.FreeCoTaskMem(stringPtr);
                }
            }

            Marshal.FreeCoTaskMem(ptrOfStringPtr);

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