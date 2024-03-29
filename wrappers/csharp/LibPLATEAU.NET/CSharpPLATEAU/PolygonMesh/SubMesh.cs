﻿using System;
using System.Runtime.InteropServices;
using PLATEAU.Interop;
using PLATEAU.Util;
using PLATEAU.CityGML;

namespace PLATEAU.PolygonMesh
{
    /// <summary>
    /// SubMesh は、 <see cref="Mesh"/> の一部 (Indices リストの中のとある範囲)がとあるテクスチャであることを表現します。
    ///
    /// 詳しくは <see cref="Model"/> クラスのコメントをご覧ください。
    /// </summary>
    public class SubMesh
    {
        public IntPtr Handle { get; }
        private bool isValid = true;
        public SubMesh(IntPtr handle)
        {
            this.Handle = handle;
        }

        public static SubMesh Create(int startIndex, int endIndex, string texturePath)
        {
            var result = NativeMethods.plateau_create_sub_mesh(
                out var outSubMeshPtr, startIndex, endIndex, texturePath);
            DLLUtil.CheckDllError(result);
            return new SubMesh(outSubMeshPtr);
        }

        public int StartIndex
        {
            get
            {
                ThrowIfInvalid();
                int startIndex = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_sub_mesh_get_start_index);
                return startIndex;
            }
        }

        public int EndIndex
        {
            get
            {
                ThrowIfInvalid();
                int endIndex = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_sub_mesh_get_end_index);
                return endIndex;
            }
        }

        public string TexturePath
        {
            get
            {
                ThrowIfInvalid();
                string path = DLLUtil.GetNativeString(Handle,
                    NativeMethods.plateau_sub_mesh_get_texture_path);
                return path;
            }
        }

        public PLATEAU.CityGML.Material Material
        {
            get
            {
                ThrowIfInvalid();
                var matHandle = DLLUtil.GetNativeValue<IntPtr>(Handle,
                    NativeMethods.plateau_sub_mesh_get_material);
                if (matHandle != IntPtr.Zero)
                    return new Material(matHandle);
                return null;
            }
        }

        /// <summary>
        /// <see cref="GameMaterialID"/>は、<see cref="SubMesh"/>にゲームエンジンのマテリアルを持たせたい場合に、
        /// <see cref="TexturePath"/>や<see cref="Material"/>の代わりに利用するIDです。
        /// 特に分割結合でゲームエンジンのマテリアルを維持するために利用します。
        /// IDがどのマテリアルを指すかはゲームエンジンが決めます。
        /// </summary>
        public int GameMaterialID
        {
            get
            {
                ThrowIfInvalid();
                int gameMaterialID =
                    DLLUtil.GetNativeValue<int>(Handle, NativeMethods.plateau_sub_mesh_get_game_material_id);
                return gameMaterialID;
            }
            set
            {
                ThrowIfInvalid();
                var result = NativeMethods.plateau_sub_mesh_set_game_material_id(Handle, value);
                DLLUtil.CheckDllError(result);
            }
        }

        /// <summary>
        /// 取扱注意:
        /// 通常は <see cref="Mesh"/> が廃棄されるときに C++側で <see cref="SubMesh"/> も廃棄されるので、
        /// このメソッドを呼ぶ必要はありません。
        /// <see cref="Mesh"/> に属さず、C#側で明示的に Create した <see cref="SubMesh"/> のみ <see cref="Dispose"/> してください。
        /// それ以外のタイミングで呼ぶとメモリ違反でUnityが落ちます。
        /// </summary>
        public void Dispose()
        {
            ThrowIfInvalid();
            DLLUtil.ExecNativeVoidFunc(Handle, NativeMethods.plateau_delete_sub_mesh);
            this.isValid = false;
        }

        public void DebugPrint(StringBuilderWithIndent sb)
        {
            sb.AppendLine($"SubMesh: indexRange = ({StartIndex}, {EndIndex}), texturePath = {TexturePath}");
        }

        private void ThrowIfInvalid()
        {
            if (!this.isValid)
            {
                throw new Exception($"{nameof(SubMesh)} is invalid.");
            }
        }

        private static class NativeMethods
        {
            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_sub_mesh_get_start_index(
                [In] IntPtr subMeshPtr,
                out int startIndex);

            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_sub_mesh_get_end_index(
                [In] IntPtr subMeshPtr,
                out int endIndex);

            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_sub_mesh_get_texture_path(
                [In] IntPtr subMeshPtr,
                out IntPtr strPtr,
                out int strLength);

            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_sub_mesh_get_material(
                [In] IntPtr subMeshPtr,
                out IntPtr matPtr);
            
            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_sub_mesh_set_game_material_id(
                [In] IntPtr subMeshPtr,
                int gameMaterialID);
            
            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_sub_mesh_get_game_material_id(
                [In] IntPtr subMeshPtr,
                out int outGameMaterialID);
            

            [DllImport(DLLUtil.DllName, CharSet = CharSet.Ansi)]
            internal static extern APIResult plateau_create_sub_mesh(
                out IntPtr outSubMeshPtr,
                int startIndex,
                int endIndex,
                string texturePath);

            [DllImport(DLLUtil.DllName)]
            internal static extern APIResult plateau_delete_sub_mesh(
                [In] IntPtr subMeshPtr);
        }
    }
}
