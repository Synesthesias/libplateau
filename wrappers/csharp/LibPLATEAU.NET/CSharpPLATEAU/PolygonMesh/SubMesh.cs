﻿using System;
using System.Diagnostics;
using System.Runtime.CompilerServices;
using System.Text;
using PLATEAU.Interop;
using PLATEAU.Util;

namespace PLATEAU.PolygonMesh
{
    /// <summary>
    /// SubMesh は、 <see cref="Mesh"/> の一部 (Indices リストの中のとある範囲)がとあるテクスチャであることを表現します。
    ///
    /// 詳しくは <see cref="Model"/> クラスのコメントをご覧ください。
    /// </summary>
    public class SubMesh
    {
        private IntPtr handle;
        private bool isValid = true;
        public SubMesh(IntPtr handle)
        {
            this.handle = handle;
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
                int startIndex = DLLUtil.GetNativeValue<int>(this.handle,
                    NativeMethods.plateau_sub_mesh_get_start_index);
                return startIndex;
            }
        }

        public int EndIndex
        {
            get
            {
                ThrowIfInvalid();
                int endIndex = DLLUtil.GetNativeValue<int>(this.handle,
                    NativeMethods.plateau_sub_mesh_get_end_index);
                return endIndex;
            }
        }

        public string TexturePath
        {
            get
            {
                ThrowIfInvalid();
                string path = DLLUtil.GetNativeString(this.handle,
                    NativeMethods.plateau_sub_mesh_get_texture_path);
                return path;
            }
        }

        /// <summary>
        /// 取扱注意:
        /// 通常は <see cref="Model"/> が廃棄されるときに C++側で <see cref="SubMesh"/> も廃棄されるので、
        /// このメソッドを呼ぶ必要はありません。
        /// Model に属さず、C#側で明示的に Create した SubMesh のみ Dispose してください。
        /// それ以外のタイミングで呼ぶとメモリ違反でUnityが落ちます。
        /// </summary>
        public void Dispose()
        {
            ThrowIfInvalid();
            var result = NativeMethods.plateau_delete_sub_mesh(this.handle);
            DLLUtil.CheckDllError(result);
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
    }
}
