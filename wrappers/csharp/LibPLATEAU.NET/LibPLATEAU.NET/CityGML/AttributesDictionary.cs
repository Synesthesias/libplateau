using System;
using System.Linq;
using System.Runtime.InteropServices;

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

        /// <summary>
        /// 属性のキーをすべて取得します。
        /// </summary>
        public string[] GetKeys()
        {
            int[] keySizes = GetKeySizes();
            int cnt = Count;

            IntPtr ptrOfStringArray = DLLUtil.AllocPtrArray(cnt, keySizes);
            
            NativeMethods.plateau_attributes_map_get_keys(this.handle, ptrOfStringArray);
            string[] ret = DLLUtil.PtrToStringArray(ptrOfStringArray, cnt, keySizes);
            
            DLLUtil.FreePtrArray(ptrOfStringArray, cnt);

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