using Microsoft.Win32.SafeHandles;
using System.Runtime.InteropServices;

namespace LibPLATEAU.NET
{
    //public sealed class ObjWriterHandle : SafeHandleZeroOrMinusOneIsInvalid
    //{
    //    public ObjWriterHandle(bool ownsHandle) : base(ownsHandle) { }

    //    protected override bool ReleaseHandle()
    //    {
    //        if (!IsClosed)
    //        {
    //            NativeMethods.plateau_delete_obj_writer(this);
    //        }
    //        return true;
    //    }
    //}

    /// <summary>
    /// The <see cref="ObjWriter"/> class exports .obj file from .gml.
    /// </summary>
    public class ObjWriter : IDisposable
    {
        private readonly IntPtr handle;

        public PlateauVector3d ReferencePoint
        {
            get => NativeMethods.plateau_obj_writer_get_reference_point(this.handle);
            set => NativeMethods.plateau_obj_writer_set_reference_point(this.handle, value);
        }

        /// <summary>
        /// Initializes a new instance of the <see cref="ObjWriter"/> class.
        /// </summary>
        public ObjWriter()
        {
            handle = NativeMethods.plateau_create_obj_writer();
        }

        /// <summary>
        /// Gets the safe handle.
        /// </summary>
        public IntPtr Handle
        {
            get
            {
                return handle;
            }
        }

        public void Dispose()
        {
            NativeMethods.plateau_delete_obj_writer(this.handle);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// Converts .gml into .obj.
        /// </summary>
        /// <param name="objPath"></param>
        /// <param name="gmlPath"></param>
        public void Write(string objPath, string gmlPath)
        {
            NativeMethods.plateau_obj_writer_write(this.handle, objPath, gmlPath);
        }
    }
}
