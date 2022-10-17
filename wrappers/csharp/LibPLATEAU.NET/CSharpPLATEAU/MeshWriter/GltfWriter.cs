using System;
using PLATEAU.Interop;
using System.Threading;
using PLATEAU.PolygonMesh;

namespace PLATEAU.MeshWriter
{
    public class GltfWriter : IDisposable
    {
        private readonly IntPtr handle;
        private bool isDisposed;

        public bool Write(string destination, Model model, GltfWriteOptions options)
        {
            string tex_path = options.texture_directory_path;
            MeshFileFormat format = options.mesh_file_format;
            var result = NativeMethods.plateau_gltf_writer_write(this.handle, out var flg, destination, model.Handle, tex_path, format);
            DLLUtil.CheckDllError(result);
            return flg;
        }

        public GltfWriter()
        {
            APIResult result = NativeMethods.plateau_create_gltf_writer(out IntPtr outPtr);
            DLLUtil.CheckDllError(result);
            this.handle = outPtr;
        }

        ~GltfWriter()
        {
            Dispose();
        }

        public void Dispose()
        {
            if (this.isDisposed) return;
            var result = NativeMethods.plateau_delete_gltf_writer(this.handle);
            DLLUtil.CheckDllError(result);
            GC.SuppressFinalize(this);
            this.isDisposed = true;
        }
    }
}
