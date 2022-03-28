using System.Runtime.InteropServices;

namespace LibPLATEAU.NET
{
    [StructLayout(LayoutKind.Sequential)]
    public struct PlateauVector3d
    {
        public double X;
        public double Y;
        public double Z;

        public PlateauVector3d(double x, double y, double z)
        {
            this.X = x;
            this.Y = y;
            this.Z = z;
        }
    }

    internal static class NativeMethods
    {
        private const string kDllName = "plateau_c";

        [DllImport(kDllName)]
        internal static extern IntPtr plateau_create_obj_writer();

        [DllImport(kDllName)]
        internal static extern void plateau_delete_obj_writer([In] IntPtr objWriter);

        [DllImport(kDllName)]
        internal static extern void plateau_obj_writer_write(
            [In] IntPtr objWriter,
            [In] string objPath,
            [In] string gmlPath);

        [DllImport(kDllName)]
        internal static extern PlateauVector3d plateau_obj_writer_get_reference_point(
            [In] IntPtr objWriter);

        [DllImport(kDllName)]
        internal static extern void plateau_obj_writer_set_reference_point(
            [In] IntPtr objWriter,
            [In] PlateauVector3d referencePoint);
    }
}
