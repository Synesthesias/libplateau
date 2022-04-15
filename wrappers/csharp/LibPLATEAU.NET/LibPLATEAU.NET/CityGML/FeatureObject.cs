using System;
using System.Linq;
using System.Runtime.InteropServices;

namespace LibPLATEAU.NET
{
    /// <summary>
    /// CityGMLにおける全ての地物オブジェクトのベースクラスです。
    /// </summary>
    public class FeatureObject : Object
    {
        internal FeatureObject(IntPtr handle) : base(handle)
        {
        }

        /// <summary>
        /// 建築物の範囲を double[6] で返します。
        /// </summary>
        /// <returns> 戻り値 double[6] の配列の中身は座標 { lower_x, lower_y, lower_z, upper_x, upper_y, upper_z } です。</returns>
        public double[] GetEnvelope()
        {
            const int envelopeArrayLength = 6;
            int size = Marshal.SizeOf(typeof(double)) * envelopeArrayLength;
            IntPtr ptr = Marshal.AllocCoTaskMem(size);
            double[] ret = Enumerable.Repeat(0.0, envelopeArrayLength).ToArray();
            try
            {
                // ptr に Envelope情報( double[6] ) を格納します。
                NativeMethods.plateau_feature_object_get_envelope(Handle, ptr);
                // ptr の内容を C# の double[6] にコピーします。
                Marshal.Copy(ptr, ret, 0, envelopeArrayLength);
            }
            finally
            {
                Marshal.FreeCoTaskMem(ptr);
            }

            return ret;
        }

        /// <summary>
        /// 建物の範囲を設定します。
        /// </summary>
        public void SetEnvelope(
            double lowerX, double lowerY, double lowerZ,
            double upperX, double upperY, double upperZ)
        {
            NativeMethods.plateau_feature_object_set_envelope(
                Handle,
                lowerX, lowerY, lowerZ, upperX, upperY, upperZ
            );
        }
    }
}