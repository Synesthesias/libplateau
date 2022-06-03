using System;
using System.Threading;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// The <see cref="ObjWriter"/> class exports .obj file from .gml.
    /// </summary>
    public class ObjWriter : IDisposable
    {
        private readonly IntPtr handle;
        private int disposed;

        /// <summary>
        /// 書き出されるメッシュの平面直角座標系での基準点を取得または設定します。
        /// </summary>
        public PlateauVector3d ReferencePoint
        {
            get => NativeMethods.plateau_obj_writer_get_reference_point(this.handle);
            set => NativeMethods.plateau_obj_writer_set_reference_point(this.handle, value);
        }

        /// <summary>
        /// <see cref="ObjWriter"/>クラスのインスタンスを初期化します。
        /// </summary>
        public ObjWriter()
        {
            APIResult result = NativeMethods.plateau_create_obj_writer(out IntPtr outPtr);
            DLLUtil.CheckDllError(result);
            this.handle = outPtr;
        }

        ~ObjWriter()
        {
            Dispose();
        }

        /// <summary>
        /// セーフハンドルを取得します。
        /// </summary>
        public IntPtr Handle => this.handle;

        public void Dispose()
        {
            if (Interlocked.Exchange(ref this.disposed, 1) == 0)
            {
                NativeMethods.plateau_delete_obj_writer(this.handle);
            }

            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// .gmlを.objに変換します。
        /// </summary>
        /// <param name="objPath"></param>
        /// <param name="cityModel"></param>
        /// <param name="gmlPath"></param>
        public void Write(string objPath, CityModel cityModel, string gmlPath)
        {
            NativeMethods.plateau_obj_writer_write(this.handle, objPath, cityModel.Handle, gmlPath);
        }

        /// <summary>
        /// MeshGranularity（objの結合単位）を設定します。
        /// </summary>
        public void SetMeshGranularity(MeshGranularity value)
        {
            NativeMethods.plateau_obj_writer_set_mesh_granularity(this.handle, value);
        }

        /// <summary>
        /// 現在のMeshGranularity（objの結合単位）を返します。
        /// </summary>
        public MeshGranularity GetMeshGranularity()
        {
            MeshGranularity meshGranularity = DLLUtil.GetNativeValue<MeshGranularity>(this.handle,
                NativeMethods.plateau_obj_writer_get_mesh_granularity);
            return meshGranularity;
        }

        /// <summary>
        /// 座標軸を設定します。
        /// </summary>
        public void SetDestAxes(AxesConversion value)
        {
            NativeMethods.plateau_obj_writer_set_dest_axes(this.handle, value);
        }

        /// <summary>
        /// 現在の座標軸設定を返します。
        /// </summary>
        public AxesConversion GetDestAxes()
        {
            AxesConversion axesConversion = DLLUtil.GetNativeValue<AxesConversion>(this.handle,
                NativeMethods.plateau_obj_writer_get_dest_axes);
            return axesConversion;
        }
        
        /// <summary>
        /// <see cref="ReferencePoint"/>を<paramref name="cityModel"/>の Envelope の中心に設定します。
        /// </summary>
        /// <param name="cityModel"></param>
        public void SetValidReferencePoint(CityModel cityModel)
        {
            NativeMethods.plateau_obj_writer_set_valid_reference_point(this.handle, cityModel.Handle);
        }
    }
}