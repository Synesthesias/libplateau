using Microsoft.Win32.SafeHandles;
using System.Runtime.InteropServices;

namespace LibPLATEAU.NET
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
            handle = NativeMethods.plateau_create_obj_writer();
        }

        ~ObjWriter()
        {
            Dispose();
        }

        /// <summary>
        /// セーフハンドルを取得します。
        /// </summary>
        public IntPtr Handle => handle;
        
        public void Dispose()
        {
            if (Interlocked.Exchange(ref disposed, 1) == 0)
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
        /// MergeMeshフラグを設定します。
        /// </summary>
        public void SetMergeMeshFlg(bool value)
        {
            NativeMethods.plateau_obj_writer_set_merge_mesh_flg(this.handle, value);
        }


        /// <summary>
        /// 座標軸を設定します。
        /// </summary>
        public void SetDestAxes(AxesConversion value)
        {
            NativeMethods.plateau_obj_writer_set_dest_axes(this.handle, value);
        }


        /// <summary>
        /// <see cref="ReferencePoint"/>を<paramref name="cityModel"/>の<see cref="Envelope"/>の中心に設定します。
        /// </summary>
        /// <param name="cityModel"></param>
        public void SetValidReferencePoint(CityModel cityModel)
        {
            NativeMethods.plateau_obj_writer_set_valid_reference_point(this.handle, cityModel.Handle);
        }
    }
}
