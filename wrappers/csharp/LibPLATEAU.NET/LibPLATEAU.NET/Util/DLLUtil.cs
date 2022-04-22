using System;
using System.Runtime.InteropServices;
using LibPLATEAU.NET.CityGML;

namespace LibPLATEAU.NET.Util
{
    /// <summary>
    /// DLLとデータをやりとりするためのユーティリティクラスです。
    /// </summary>
    internal static class DLLUtil
    {
        /// <summary>
        /// ポインタの配列を作ります。
        /// 配列内の各ポインタのメモリ領域を <paramref name="sizes"/>[i] の大きさで確保します。
        /// ポインタ配列 のアドレスを <see cref="IntPtr"/> で返します。
        /// ここで確保したメモリは必ず <see cref="FreePtrArray"/> で解放してください。
        /// そうでないとメモリリークになります。
        /// </summary>
        /// <param name="count">ポインタ配列の要素数です。</param>
        /// <param name="sizes"><see cref="sizes"/>[i] = i番目のポインタの確保バイト数 となるような int配列です。</param>
        internal static IntPtr AllocPtrArray(int count, int[] sizes)
        {
            if (count > sizes.Length)
            {
                throw new ArgumentException("sizes.Length should not be smaller than count.");
            }
            
            IntPtr[] managedPtrArray = new IntPtr[count]; // ポインタの配列 (managed)
            for (int i = 0; i < count; i++)
            {
                IntPtr ptr = Marshal.AllocCoTaskMem(sizes[i]); // 配列内の各ポインタについてメモリ確保
                managedPtrArray[i] = ptr;
            }
            
            int sizeOfPtrArray = Marshal.SizeOf(typeof(IntPtr)) * count;
            IntPtr unmanagedPtrArray = Marshal.AllocCoTaskMem(sizeOfPtrArray); // ポインタの配列 (unmanaged)
            Marshal.Copy(managedPtrArray, 0, unmanagedPtrArray, count);
            return unmanagedPtrArray;
        }


        /// <summary>
        /// <see cref="AllocPtrArray"/> で確保したメモリを解放します。
        /// ポインタ配列内の各ポインタを解放し、続けてポインタ配列自体を解放します。
        /// </summary>
        /// <param name="ptrOfPtrArray">解放したいポインタ配列を指定します。</param>
        /// <param name="count">ポインタ配列の要素数を指定します。</param>
        internal static unsafe void FreePtrArray(IntPtr ptrOfPtrArray, int count)
        {
            for (int i = 0; i < count; i++)
            {
                var ptr = ((IntPtr*)ptrOfPtrArray)[i];
                Marshal.FreeCoTaskMem(ptr);
            }
            Marshal.FreeCoTaskMem(ptrOfPtrArray);
        }

        /// <summary>
        /// ポインタ(stringの配列のポインタ)から string の配列を読み込みます。
        /// ポインタ <paramref name="ptrOfStringArray"/> は <see cref="AllocPtrArray"/> で確保したものと同じであることを前提とし、
        /// 引数 <paramref name="count"/>, <paramref name="sizes"/> には <see cref="AllocPtrArray"/> で渡したものと同じ値を渡してください。 
        /// </summary>
        internal static unsafe string[] PtrToStringArray(IntPtr ptrOfStringArray, int count, int[] sizes)
        {
            string[] ret = new string[count];
            for (int i = 0; i < count; i++)
            {
                var stringPtr = ((IntPtr*)ptrOfStringArray)[i];
                ret[i] = Marshal.PtrToStringAnsi(stringPtr, sizes[i] - 1); // -1 は null終端文字を除くためです。
                // Console.WriteLine(BitConverter.ToString(PtrToBytes(stringPtr, sizes[i])));
            }
            return ret;
        }

        internal static byte[] PtrToBytes(IntPtr ptr, int length)
        {
            byte[] bytes = new byte[length];
            Marshal.Copy(ptr, bytes, 0, length);
            return bytes;
        }

        
        /// <summary>
        /// <paramref name="result"/> が <see cref="APIResult.Success"/> でなかった場合に例外を投げます。
        /// </summary>
        internal static void CheckDllError(APIResult result)
        {
            if (result != APIResult.Success)
            {
                throw new Exception($"Error in Lib Plateau DLL. APIResult = {result}");
            }
        }

    }
}