using System;
using PLATEAU.Interop;

namespace PLATEAU.PolygonMesh
{
    /// <summary>
    /// メッシュ情報です。
    /// Unity や Unreal Engine でメッシュを生成するために必要な情報が含まれるよう意図されています。
    /// 具体的には 頂点リスト、Indicesリスト、UV、サブメッシュ（含テクスチャ）があります。
    ///
    /// 詳しくは <see cref="Model"/> クラスのコメントをご覧ください。
    /// </summary>
    public class Mesh : PInvokeDisposable
    {
        public Mesh(IntPtr handle) : base(handle)
        {
            
        }

        public static Mesh Create(string meshID)
        {
            var result = NativeMethods.plateau_create_mesh(out var newMeshPtr, meshID);
            DLLUtil.CheckDllError(result);
            return new Mesh(newMeshPtr);
        }

        public int VerticesCount
        {
            get
            {
                int verticesCount = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_mesh_get_vertices_count);
                return verticesCount;
            }
        }

        public int IndicesCount
        {
            get
            {
                int indicesCount = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_mesh_get_indices_count);
                return indicesCount;
            }
        }

        public PlateauVector3d GetVertexAt(int index)
        {
            var vert = DLLUtil.GetNativeValue<PlateauVector3d>(Handle, index,
                NativeMethods.plateau_mesh_get_vertex_at_index);
            return vert;
        }

        public int GetIndiceAt(int index)
        {
            int vertexId = DLLUtil.GetNativeValue<int>(Handle, index,
                NativeMethods.plateau_mesh_get_indice_at_index);
            return vertexId;
        }

        public PlateauVector2f[] GetUv1()
        {
            var uv1 = new PlateauVector2f[VerticesCount];
            var result = NativeMethods.plateau_mesh_get_uv1(Handle, uv1);
            DLLUtil.CheckDllError(result);
            return uv1;
        }
        
        public PlateauVector2f[] GetUv2()
        {
            var uv2 = new PlateauVector2f[VerticesCount];
            var result = NativeMethods.plateau_mesh_get_uv2(Handle, uv2);
            DLLUtil.CheckDllError(result);
            return uv2;
        }
        
        public PlateauVector2f[] GetUv3()
        {
            var uv3 = new PlateauVector2f[VerticesCount];
            var result = NativeMethods.plateau_mesh_get_uv3(Handle, uv3);
            DLLUtil.CheckDllError(result);
            return uv3;
        }

        public int SubMeshCount
        {
            get
            {
                int numSubMesh = DLLUtil.GetNativeValue<int>(Handle,
                    NativeMethods.plateau_mesh_get_sub_mesh_count);
                return numSubMesh;
            }
        }
        
        public SubMesh GetSubMeshAt(int index)
        {
            var subMeshPtr = DLLUtil.GetNativeValue<IntPtr>(Handle, index,
                NativeMethods.plateau_mesh_get_sub_mesh_at_index);
            return new SubMesh(subMeshPtr);
        }

        protected override void DisposeNative()
        {
            var result = NativeMethods.plateau_delete_mesh(Handle);
            DLLUtil.CheckDllError(result);
        }
    }
}
