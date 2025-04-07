using System;
using System.Runtime.InteropServices;
using PLATEAU.Dataset;
using PLATEAU.Interop;

namespace PLATEAU.Native
{
    public class NativeVectorGridCode : NativeVectorDisposableBase<GridCode>
    {
        private NativeVectorGridCode(IntPtr ptr) : base(ptr)
        {
        }

        public static NativeVectorGridCode Create()
        {
            var result = NativeMethods.plateau_create_vector_mesh_code(out var ptr);
            DLLUtil.CheckDllError(result);
            return new NativeVectorGridCode(ptr);
        }

        public override GridCode At(int index)
        {
            ThrowIfDisposed();
            var gridCodePtr = DLLUtil.GetNativeValue<IntPtr>(Handle, index,
                NativeMethods.plateau_vector_grid_code_get_value);
            var gridCode = new GridCode(gridCodePtr, false);
            return gridCode.Copy(); // 寿命管理のためコピーを渡します。元データはvector廃棄時に消します。
        }

        public override int Length
        {
            get
            {
                ThrowIfDisposed();
                int count = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_vector_grid_code_count);
                return count;
            }
        }

        public void Add(MeshCode meshCode)
        {
            var result = NativeMethods.plateau_vector_mesh_code_push_back_value(
                Handle, meshCode);
            DLLUtil.CheckDllError(result);
        }


        protected override void DisposeNative()
        {
            var result = NativeMethods.plateau_delete_vector_mesh_code(Handle);
            DLLUtil.CheckDllError(result);
        }

        private static class NativeMethods
        {
            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_create_vector_mesh_code(
                out IntPtr outVectorPtr);

            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_delete_vector_mesh_code(
                [In] IntPtr vectorPtr);

            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_vector_grid_code_get_value(
                [In] IntPtr vectorPtr,
                out IntPtr outGridCodePtr,
                int index);
        
            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_vector_grid_code_count(
                [In] IntPtr handle,
                out int outCount);

            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult grid_code_push_back_value(
                [In] IntPtr handle,
                [In] IntPtr gridCodePtr);
        }
    }
}
