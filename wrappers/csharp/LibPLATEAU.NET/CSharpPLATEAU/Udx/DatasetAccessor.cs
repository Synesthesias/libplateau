using System;
using PLATEAU.Interop;

namespace PLATEAU.Udx
{
    public class DatasetAccessor
    {
        public IntPtr Handle { get; private set; }
        public DatasetAccessor(IntPtr handle)
        {
            Handle = handle;
        }


        public NativeVectorGmlFile GetGmlFiles(Extent extent, PredefinedCityModelPackage package)
        {
            var gmlFiles = NativeVectorGmlFile.Create();
            var result = NativeMethods.plateau_i_dataset_accessor_get_gml_files(
                Handle, extent, package, gmlFiles.Handle);
            DLLUtil.CheckDllError(result);
            return gmlFiles;
        }

        
    }
}
