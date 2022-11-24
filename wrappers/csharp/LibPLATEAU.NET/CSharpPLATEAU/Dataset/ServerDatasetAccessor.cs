using System;
using PLATEAU.Interop;

namespace PLATEAU.Dataset
{
    public class ServerDatasetAccessor
    {
        public IntPtr Handle { get; }
        public ServerDatasetAccessor(IntPtr ptr)
        {
            Handle = ptr;
        }

        public static ServerDatasetAccessor Create()
        {
            // TODO PtrOfNewInstance を適用できる場所は他にもあるので置き換える
            return new ServerDatasetAccessor(DLLUtil.PtrOfNewInstance(
                NativeMethods.plateau_create_server_dataset_accessor
            ));
        }

        public NativeVectorDatasetMetadataGroup GetMetadataGroup()
        {
            var ret = NativeVectorDatasetMetadataGroup.Create();
            var result = NativeMethods.plateau_server_dataset_accessor_get_dataset_metadata_group(
                Handle, ret.Handle);
            DLLUtil.CheckDllError(result);
            return ret;
        }

        public void Dispose()
        {
            // TODO ExecNativeVoidFunc を適用できる箇所は他にもあるので置き換える
            DLLUtil.ExecNativeVoidFunc(Handle, NativeMethods.plateau_delete_server_dataset_accessor);
        }
    }
}
