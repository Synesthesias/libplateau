﻿using System;
using PLATEAU.Interop;

namespace PLATEAU.PolygonMesh
{
    /// <summary>
    /// <see cref="Model"/> 以下の階層構造を構成するノードです。
    /// Node は 0個以上の 子Node を持つため階層構造になります。
    ///
    /// 詳しくは <see cref="Model"/> クラスのコメントをご覧ください。
    ///
    /// Name はゲームエンジン側ではゲームオブジェクトの名前として解釈されることが想定されます。
    /// Mesh はそのゲームオブジェクトの持つメッシュとして解釈されることが想定されます。
    /// </summary>
    public class Node
    {
        public IntPtr Handle { get; }
        private bool isValid = true;

        public static Node Create(string id)
        {
            var result = NativeMethods.plateau_create_node(out var outNodePtr, id);
            DLLUtil.CheckDllError(result);
            return new Node(outNodePtr);
        }

        public Node(IntPtr handle)
        {
            ThrowIfInvalid();
            Handle = handle;
        }

        /// <summary>
        /// ノードの名称を返します。
        /// ゲームエンジン側ではゲームオブジェクトの名称となります。
        /// </summary>
        public string Name
        {
            get
            {
                ThrowIfInvalid();
                string name = DLLUtil.GetNativeString(Handle,
                    NativeMethods.plateau_node_get_name);
                return name;
            }
        }

        /// <summary>
        /// 子ノードの数を返します。
        /// </summary>
        public int ChildCount
        {
            get
            {
                ThrowIfInvalid();
                int childCount = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_node_get_child_count);
                return childCount;
            }
        }

        /// <summary>
        /// <paramref name="index"/> 番目の子ノードを返します。
        /// </summary>
        public Node GetChildAt(int index)
        {
            ThrowIfInvalid();
            var childNodePtr = DLLUtil.GetNativeValue<IntPtr>(
                this.Handle, index,
                NativeMethods.plateau_node_get_child_at_index);
            return new Node(childNodePtr);
        }

        /// <summary>
        /// ノードが保有する <see cref="Mesh"/> を返します。
        /// なければ null を返します。
        /// </summary>
        public Mesh Mesh
        {
            get
            {
                ThrowIfInvalid();
                var result = NativeMethods.plateau_node_get_mesh(
                    this.Handle, out IntPtr meshPtr);
                if (result == APIResult.ErrorValueNotFound)
                {
                    return null;
                }
                DLLUtil.CheckDllError(result);
                return new Mesh(meshPtr);
            }
        }

        /// <summary>
        /// <see cref="Mesh"/> を <see cref="Node"/>にセットします。
        /// 取扱注意:
        /// C++の move によって <see cref="Mesh"/> を移動するので、
        /// 実行後は元の <see cref="Mesh"/> は利用不可になります。
        /// </summary>
        public void SetMeshByCppMove(Mesh mesh)
        {
            var result = NativeMethods.plateau_node_set_mesh_by_std_move(
                Handle, mesh.Handle);
            DLLUtil.CheckDllError(result);
            mesh.MarkInvalid();
        }

        /// <summary>
        /// 子ノードを追加します。
        /// 取扱注意:
        /// C++の move によって <see cref="Node"/> を移動するので、
        /// 実行後は元の <see cref="Node"/> は利用不可になります。
        /// </summary>
        public void AddChildNodeByCppMove(Node node)
        {
            var result = NativeMethods.plateau_node_add_child_node_by_std_move(
                Handle, node.Handle);
            DLLUtil.CheckDllError(result);
            node.MarkInvalid();
        }

        public void MarkInvalid()
        {
            this.isValid = false;
        }

         /// <summary>
         /// 取扱注意:
         /// 通常、<see cref="Model"/> が廃棄されたときに <see cref="Node"/> も破棄されるので
         /// <see cref="Dispose"/> を呼ぶ必要はありません。
         /// <see cref="Model"/> に属さず、C#側から <see cref="Create"/> したもののみ <see cref="Dispose"/> してください。
         /// </summary>
        public void Dispose()
        {
            var result = NativeMethods.plateau_delete_node(Handle);
            DLLUtil.CheckDllError(result);
            this.isValid = false;
        }

        private void ThrowIfInvalid()
        {
            if (!this.isValid) throw new Exception("The instance is invalid.");
        }
    }
}
