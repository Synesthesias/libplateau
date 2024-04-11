using System;
using System.Drawing;
using System.Runtime.InteropServices;
using PLATEAU.Interop;
using PLATEAU.Native;
using PLATEAU.PolygonMesh;

namespace PLATEAU.Texture
{
    public class HeightmapGenerator
    {
        public void GenerateFromMesh(Mesh InMesh, int TextureWidth, int TextureHeight, out PlateauVector3d Min, out PlateauVector3d Max, out float[,] HeightData )
        {
            IntPtr HeightmapDataPtr;
            int DataSize = 0;
            var apiResult =
                NativeMethods.heightmap_generator_generate_from_mesh(InMesh.Handle, TextureWidth, TextureHeight, out Min, out Max , out HeightmapDataPtr, out DataSize);

            HeightData = new float[TextureHeight, TextureWidth];

            DLLUtil.CheckDllError(apiResult);
        }

        static public void SavePngFile(string FileName, int width, int height, byte[] data)
        {
            IntPtr HeightmapDataPtr = IntPtr.Zero;
            Marshal.Copy(data, 0, HeightmapDataPtr ,sizeof(UInt16) * width * height);
            var apiResult =
                NativeMethods.heightmap_save_png_file(FileName, width, height, HeightmapDataPtr);
        }

        static public void ReadPngFile(string FileName, int width, int height, out UInt16[] data)
        {
            IntPtr HeightmapDataPtr = IntPtr.Zero;
            int DataSize = 0;
            var apiResult =
                NativeMethods.heightmap_read_png_file(FileName, width, height, out HeightmapDataPtr, out DataSize);

            byte[] outData = new byte[DataSize];
            Marshal.Copy(HeightmapDataPtr, outData, 0, sizeof(UInt16) * DataSize);

            data = new UInt16[height * width];
            int byteIndex = 0;
            for(int i = 0; i < data.Length; i++)
            {
                data[i] = BitConverter.ToUInt16(outData, byteIndex);
                byteIndex += 2;
            }
        }

        private static class NativeMethods
        {
            [DllImport(DLLUtil.DllName, CharSet = CharSet.Ansi)]
            internal static extern APIResult heightmap_generator_generate_from_mesh(
                [In] IntPtr srcMeshPtr,
                [In] int TextureWidth,
                [In] int TextureHeight,
                out PlateauVector3d Min,
                out PlateauVector3d Max,
                out IntPtr HeightmapDataPtr,
                out int dataSize
            );

            [DllImport(DLLUtil.DllName, CharSet = CharSet.Ansi)]
            internal static extern APIResult heightmap_generator_generate_from_mesh2(
            [In] IntPtr srcMeshPtr,
            [In] int TextureWidth,
            [In] int TextureHeight,
            out PlateauVector3d Min,
            out PlateauVector3d Max,
            out IntPtr HeightmapDataPtr,
            out int dataSize
            );


            [DllImport(DLLUtil.DllName, CharSet = CharSet.Ansi)]
            internal static extern APIResult heightmap_save_png_file(
             [In] string FileName,
             [In] int Width,
             [In] int Height,
             [In] IntPtr Data
            );

            [DllImport(DLLUtil.DllName, CharSet = CharSet.Ansi)]
            internal static extern APIResult heightmap_read_png_file(
             [In] string FileName,
             [In] int Width,
             [In] int Height,
             out IntPtr OutData,
             out int dataSize
            );

            [DllImport(DLLUtil.DllName, CharSet = CharSet.Ansi)]
            internal static extern APIResult heightmap_save_raw_file(
             [In] string FileName,
             [In] int Width,
             [In] int Height,
             [In] IntPtr Data
            );

            [DllImport(DLLUtil.DllName, CharSet = CharSet.Ansi)]
            internal static extern APIResult heightmap_read_raw_file(
             [In] string FileName,
             [In] int Width,
             [In] int Height,
             out IntPtr OutData,
             out int dataSize
            );

        }
    }


}
