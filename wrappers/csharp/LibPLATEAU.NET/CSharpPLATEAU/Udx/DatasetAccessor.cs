﻿using System;
using System.Diagnostics;
using PLATEAU.Interop;

namespace PLATEAU.Udx
{
    public class DatasetAccessor : PInvokeDisposable
    {
        public DatasetAccessor(IntPtr selfAccessorPtr) :
            base(selfAccessorPtr)
        {
        }

        public static DatasetAccessor Create(IntPtr innerAccessorPtr)
        {
            var result = NativeMethods.plateau_create_dataset_accessor_p_invoke(
                innerAccessorPtr, out var selfAccessorPtr);
            DLLUtil.CheckDllError(result);
            return new DatasetAccessor(selfAccessorPtr);
        }

        public GmlFileInfo[] GetGmlFiles(Extent extent, PredefinedCityModelPackage package)
        {
            if ((extent.Max - extent.Min).SqrMagnitudeLatLon >= 1)
            {
                // 緯度経度の範囲が広すぎると地域メッシュコード生成が終わらなくなるので防止します。
                // ただし高さは問いません。
                throw new ArgumentException("Extent is too large.");
            }
            var resultG = NativeMethods.plateau_dataset_accessor_p_invoke_get_gml_files(
                Handle, extent, package);
            DLLUtil.CheckDllError(resultG);
            int count = DLLUtil.GetNativeValue<int>(Handle,
                NativeMethods.plateau_dataset_accessor_p_invoke_result_of_get_gml_files_count);
            var ret = new GmlFileInfo[count];
            for (int i = 0; i < count; i++)
            {
                    var gmlFileInfoPtr = DLLUtil.GetNativeValue<IntPtr>(Handle, i,
                        NativeMethods.plateau_dataset_accessor_p_invoke_result_of_get_gml_files);
                    ret[i] = new GmlFileInfo(gmlFileInfoPtr);
            }
            return ret;
        }

        protected override void DisposeNative()
        {
            var result = NativeMethods.plateau_delete_dataset_accessor_p_invoke(Handle);
            DLLUtil.CheckDllError(result);
        }
    }
}
