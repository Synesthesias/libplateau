﻿using System;
using System.Runtime.InteropServices;
using System.Runtime.Intrinsics.X86;
using System.Security.Cryptography;

namespace LibPLATEAU.NET
{
    public static class DLLUtil
    {
        /// <summary>
        /// ポインタの配列を作ります。
        /// 配列内の各ポインタのメモリ領域を sizes[i] の大きさで確保します。
        /// ポインタ配列 のアドレスを IntPtr で返します。
        /// ここで確保したメモリは必ず FreePtrArray() で解放してください。
        /// そうでないとメモリリークになります。
        /// </summary>
        /// <param name="count">ポインタ配列の要素数です。</param>
        /// <param name="sizes">sizes[i] = i番目のポインタの確保バイト数 となるような int配列です。</param>
        public static IntPtr AllocPtrArray(int count, int[] sizes)
        {
            if (count > sizes.Length)
            {
                throw new ArgumentException("sizes.Length should not be smaller than count.");
            }

            IntPtr[] managedPtrArray = new IntPtr[count];
            for (int i = 0; i < count; i++)
            {
                IntPtr ptr = Marshal.AllocCoTaskMem(sizes[i]);
                managedPtrArray[i] = ptr;
            }
            
            int sizeOfPtrArray = Marshal.SizeOf(typeof(IntPtr)) * count;
            IntPtr unmanagedPtrArray = Marshal.AllocCoTaskMem(sizeOfPtrArray);
            Marshal.Copy(managedPtrArray, 0, unmanagedPtrArray, count);
            return unmanagedPtrArray;
        }


        /// <summary>
        /// AllocPtrArray で確保したメモリを解放します。
        /// ポインタ配列内の各ポインタを解放し、続けてポインタ配列自体を解放します。
        /// </summary>
        /// <param name="ptrOfPtrArray">解放したいポインタ配列を指定します。</param>
        /// <param name="count">ポインタ配列の要素数を指定します。</param>
        public static unsafe void FreePtrArray(IntPtr ptrOfPtrArray, int count)
        {
            for (int i = 0; i < count; i++)
            {
                var ptr = ((IntPtr*)ptrOfPtrArray)[i];
                Marshal.FreeCoTaskMem(ptr);
            }
            Marshal.FreeCoTaskMem(ptrOfPtrArray);
        }

        public static unsafe string[] PtrToStringArray(IntPtr ptrOfStringArray, int count, int[] sizes)
        {
            string[] ret = new string[count];
            for (int i = 0; i < count; i++)
            {
                var stringPtr = ((IntPtr*)ptrOfStringArray)[i];
                ret[i] = Marshal.PtrToStringAnsi(stringPtr, sizes[i]);
            }
            return ret;
        }

    }
}