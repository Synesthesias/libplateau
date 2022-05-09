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
        /// NativeMethods で頻出するメソッドの型を delegate で登録しておくことで、delegate で呼び出せるようにします。
        /// ただし、すべてのメソッドがこのような型をしているわけではないので、対応していないメソッドもあります。
        /// </summary>
        internal delegate APIResult GetterDelegate<T>(IntPtr handle, out T ret);
        internal delegate APIResult GetterDelegateInt<T>(IntPtr handle, out T ret, int i);
        internal delegate APIResult IntGetDelegate(IntPtr handle, out int ret);
        internal delegate APIResult StrPtrGetDelegate(IntPtr handle, out IntPtr ret);
        
        // 下の3つのメソッドは、 DLL側で一時的に生成した「文字列の配列」の完全なコピーが欲しいという状況で利用できます。
        // 今のところ、代わりに DLL側で保持している文字列への「ポインタの配列」で済んでいるので利用していませんが、
        // 今後は完全なコピーが欲しい状況もあるかもしれないので残しておきます。
        
        
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

        /// <summary>
        /// DLLから文字列のポインタを受け取り、文字列を読んで返します。
        /// DLLの文字列を読み取る手順として
        /// 1. 文字列の長さを調べるネイティブ関数を実行
        /// 2. 文字列のポインタを取得するネイティブ関数を実行
        /// 3. ポインタから長さ分を読む
        /// の3つをまとめたメソッドになります。
        /// したがって、上記 1 と 2 で利用するネイティブ関数を引数で指定する必要があります。
        /// </summary>
        /// <param name="handle"> <see cref="NativeMethods"/> のメソッドに渡すハンドルです。 </param>
        /// <param name="strLengthGetter"> 文字列のバイト数を取得するための関数を指定します。 </param>
        /// <param name="strPtrGetter"> 文字列のポインタを取得するための関数を指定します。 </param>
        /// <returns></returns>
        internal static string GetNativeString(
            IntPtr handle,
            IntGetDelegate strLengthGetter,
            StrPtrGetDelegate strPtrGetter
        )
        {
            APIResult result = strLengthGetter(handle, out int strLength);
            CheckDllError(result);
            result = strPtrGetter(handle, out IntPtr strPtr);
            CheckDllError(result);
            return Marshal.PtrToStringAnsi(strPtr, strLength - 1); // -1 は null終端文字の分です。
        }

        /// <summary>
        /// ネイティブ関数から値を受け取り、エラーチェックしてから値を返します。
        /// <see cref="NativeMethods"/> を呼ぶたびに手動で <see cref="CheckDllError"/> を呼ぶのと同義ですが、それだと冗長なのでこのメソッドにまとめました。
        /// </summary>
        /// <param name="handle">ネイティブ関数に渡すハンドルです。</param>
        /// <param name="getterMethod"><see cref="NativeMethods"/> のメソッドを指定します。</param>
        /// <typeparam name="T">戻り値の型です。</typeparam>
        /// <returns>ネイティブ関数から受け取った値を返します。</returns>
        internal static T GetNativeValue<T>(IntPtr handle, GetterDelegate<T> getterMethod)
        {
            APIResult result = getterMethod(handle, out T ret);
            CheckDllError(result);
            return ret;
        }

        /// <summary>
        /// <see cref="GetNativeValue{T}(System.IntPtr,LibPLATEAU.NET.Util.DLLUtil.GetterDelegate{T})"/>
        /// の亜種で、<paramref name="getterMethod"/> のint引数が1つ増えた版です。
        /// </summary>
        internal static T GetNativeValue<T>(IntPtr handle, int i, GetterDelegateInt<T> getterMethod)
        {
            APIResult result = getterMethod(handle, out T ret, i);
            CheckDllError(result);
            return ret;
        }
        
        
        
        /// <summary>
        /// T型の配列であるキャッシュから値を読んで返します。
        /// キャッシュに値がなければ、値を生成してキャッシュに記憶してから返します。
        /// </summary>
        /// 
        /// <param name="cache">
        /// キャッシュです。
        /// <paramref name="cache"/> そのものが null であれば、キャッシュを T[arrayLength] で初期化します。
        /// キャッシュ本体がnullではないが、キャッシュの要素 <paramref name="cache"/>[<paramref name="index"/>] がnullであるときは、
        /// <paramref name="generator"/> で値を生成しキャッシュに記憶して返します。
        /// </param>
        ///
        /// <param name="index">T型配列から値を読みたいインデックスです。</param>
        /// <param name="arrayLength">T型配列の要素数です。</param>
        /// <param name="generator">キャッシュに値がないとき、値を生成するための関数を指定します。</param>
        public static T ArrayCache<T>(ref T?[]? cache, int index, int arrayLength, Func<T> generator)
        {
            if (cache == null)
            {
                // キャッシュの初期化
                cache = new T[arrayLength];
                for (int i = 0; i < arrayLength; i++)
                {
                    cache[i] = default(T);
                }
            }

            T? item = cache[index];
            // キャッシュがヒットしたとき
            if (item != null)
            {
                return item;
            }
            // キャッシュにないとき
            item = generator();
            cache[index] = item;
            return item;
        }

    }
}