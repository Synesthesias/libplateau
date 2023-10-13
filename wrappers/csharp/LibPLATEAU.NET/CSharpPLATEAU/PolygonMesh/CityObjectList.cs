using System;
using System.Linq;
using PLATEAU.Native;
using System.Runtime.InteropServices;
using PLATEAU.Interop;

namespace PLATEAU.PolygonMesh
{
    [StructLayout(LayoutKind.Sequential)]
    public struct CityObjectIndex
    {
        public int PrimaryIndex;
        public int AtomicIndex;

        public CityObjectIndex(int primaryIndex, int atomicIndex)
        {
            this.PrimaryIndex = primaryIndex;
            this.AtomicIndex = atomicIndex;
        }

        public static CityObjectIndex FromUV(PlateauVector2f uv)
        {
            return new CityObjectIndex
            {
                PrimaryIndex = (int)Math.Round(uv.X),
                AtomicIndex = (int)Math.Round(uv.Y)
            };
        }
    }


    public class CityObjectList : PInvokeDisposable
    {

        internal CityObjectList(IntPtr handle) : base(handle)
        {
        }

        public static CityObjectList Create()
        {
            var result = NativeMethods.plateau_create_city_object_list(out var outPtr);
            DLLUtil.CheckDllError(result);
            return new CityObjectList(outPtr);
        }

        public CityObjectIndex[] GetAllKeys()
        {
            var keys = NativeVectorCityObjectIndex.Create();
            var result = NativeMethods.plateau_city_object_list_get_all_keys(Handle, keys.Handle);
            DLLUtil.CheckDllError(result);
            return keys.ToArray();
        }

        public string GetPrimaryID(int index)
        {
            var result = NativeMethods.plateau_city_object_list_get_primary_id(Handle, out var strPtr, out int strLength, index);
            return result != APIResult.Success
                ? null
                : DLLUtil.ReadUtf8Str(strPtr, strLength);
        }

        public string GetAtomicID(CityObjectIndex index)
        {
            var result = NativeMethods.plateau_city_object_list_get_atomic_id(Handle, out var strPtr, out int strLength, index);
            return result != APIResult.Success
                ? null
                : DLLUtil.ReadUtf8Str(strPtr, strLength);
        }

        public bool TryGetAtomicID(CityObjectIndex index, out string outGmlID)
        {
            outGmlID = "";
            var result =
                NativeMethods.plateau_city_object_list_try_get_gml_id(Handle, index, out var outGmlIDStrPtr,
                    out int outStrLength);
            if (result == APIResult.ErrorValueNotFound) return false;
            DLLUtil.CheckDllError(result);
            outGmlID = DLLUtil.ReadUtf8Str(outGmlIDStrPtr, outStrLength);
            return true;
        }

        public void Add(CityObjectIndex cityObjIndex, string gmlID)
        {
            var result = NativeMethods.plateau_city_object_list_add(Handle, cityObjIndex, gmlID);
            DLLUtil.CheckDllError(result);
        }
        
        protected override void DisposeNative()
        {
            var result = NativeMethods.plateau_delete_city_object_list(Handle);
            DLLUtil.CheckDllError(result);
        }

        /// <summary>
        /// gml:idに対応する<see cref="CityObjectIndex"/>を取得します。
        /// 存在しない場合は(-1, -1)を返します。
        /// </summary>
        public CityObjectIndex GetCityObjectIndex(string gmlID)
        {
            var result = NativeMethods.plateau_city_object_list_get_city_object_index(Handle, out var index, gmlID);
            DLLUtil.CheckDllError(result);
            return index;
        }

        private static class NativeMethods
        {
            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_create_city_object_list(
                [Out] out IntPtr outCityObjListPtr
                );
            
            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_delete_city_object_list(
                [In] IntPtr handle);

            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_city_object_list_get_primary_id(
                [In] IntPtr handle,
                out IntPtr outStrPtr,
                out int strLength,
                [In] int index);

            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_city_object_list_get_atomic_id(
                [In] IntPtr handle,
                out IntPtr outStrPtr,
                out int strLength,
                [In] CityObjectIndex index);

            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_city_object_list_get_city_object_index(
                [In] IntPtr handle,
                out CityObjectIndex index,
                [In] string gmlID);

            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_city_object_list_get_all_keys(
                [In] IntPtr handle,
                [In, Out] IntPtr keys);

            [DllImport(DLLUtil.DllName, CharSet = CharSet.Ansi)]
            internal static extern APIResult plateau_city_object_list_add(
                [In] IntPtr cityObjListPtr,
                [In] CityObjectIndex cityObjIndex,
                [In] string gmlID
            );

            [DllImport(DLLUtil.DllName, CharSet = CharSet.Ansi)]
            internal static extern APIResult plateau_city_object_list_try_get_gml_id(
                [In] IntPtr cityObjListPtr,
                [In] CityObjectIndex cityObjIndex,
                [Out] out IntPtr outGmlIDStrPtr,
                [Out] out int strLength
            );
        }
    }
}
