using System;
using System.Linq;
using System.Runtime.InteropServices;

namespace LibPLATEAU.NET
{
    /// <summary>
    /// CityGMLにおける全ての地物オブジェクトのベースクラスです。
    /// </summary>
    public class FeatureObject : Object {

        internal FeatureObject(IntPtr handle) : base(handle) { }

        /// <summary>
        /// 建築物の範囲を double[6] で返します。
        /// </summary>
        /// <returns> 戻り値は要素数 6 の double配列で、配列の中身は座標 { lower_x, lower_y, lower_z, upper_x, upper_y, upper_z } の形式です。</returns>
        public double[] GetEnvelope() {
            const int envelopeArrayLength = 6;
            int size = Marshal.SizeOf(typeof(double)) * envelopeArrayLength;
            IntPtr ptr = Marshal.AllocCoTaskMem(size);
            
            // ptr に Envelope情報( double[6] ) を格納します。
            NativeMethods.plateau_feature_object_get_envelop(Handle, ptr);
            
            // ptr の内容を C# の double[6] にコピーし、ptrを解放して値を返します。
            double[] envelope = new double[envelopeArrayLength];
            Marshal.Copy(ptr, envelope, 0, envelopeArrayLength);
            Marshal.FreeCoTaskMem(ptr);
            return envelope;
        }
    }
}
