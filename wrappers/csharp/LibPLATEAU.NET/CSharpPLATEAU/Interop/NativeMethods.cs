using System;
using System.Runtime.InteropServices;
using PLATEAU.CityGML;
using PLATEAU.Geometries;
using PLATEAU.PolygonMesh;
using PLATEAU.Dataset;

namespace PLATEAU.Interop
{
    /// <summary>
    /// メッシュの結合単位
    /// </summary>
    public enum MeshGranularity
    {
        /// <summary>
        /// 最小地物単位(LOD2, LOD3の各部品)
        /// </summary>
        PerAtomicFeatureObject,
        /// <summary>
        /// 主要地物単位(建築物、道路等)
        /// </summary>
        PerPrimaryFeatureObject,
        /// <summary>
        /// 都市モデル地域単位(GMLファイル内のすべてを結合)
        /// </summary>
        PerCityModelArea
    }

    public enum GltfFileFormat
    {
        GLB,
        GLTF
    }

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

        public override string ToString()
        {
            return $"({this.X}, {this.Y}, {this.Z})";
        }
        
        public static PlateauVector3d operator+(PlateauVector3d op1, PlateauVector3d op2)
        {
            return new PlateauVector3d(op1.X + op2.X, op1.Y + op2.Y, op1.Z + op2.Z);
        }

        public static PlateauVector3d operator-(PlateauVector3d op1, PlateauVector3d op2)
        {
            return new PlateauVector3d(op1.X - op2.X, op1.Y - op2.Y, op1.Z - op2.Z);
        }

        public static PlateauVector3d operator*(PlateauVector3d vec, double scalar)
        {
            return new PlateauVector3d(vec.X * scalar, vec.Y * scalar, vec.Z * scalar);
        }

        public static PlateauVector3d operator /(PlateauVector3d vec, double scalar)
        {
            return new PlateauVector3d(vec.X / scalar, vec.Y / scalar, vec.Z / scalar);
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct PlateauVector2f
    {
        public float X;
        public float Y;

        public PlateauVector2f(float x, float y)
        {
            this.X = x;
            this.Y = y;
        }

        public override string ToString()
        {
            return $"({this.X}, {this.Y})";
        }
    }

    /// <summary>
    ///  GMLファイルのパース時の設定です。
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct CitygmlParserParams
    {
        [MarshalAs(UnmanagedType.U1)]
        public bool Optimize;
        /// <summary>
        /// <see cref="Tessellate"/> を false に設定すると、 <see cref="Polygon"/> が頂点を保持する代わりに <see cref="LinearRing"/> を保持することがあります。
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public bool Tessellate;

        [MarshalAs(UnmanagedType.U1)]
        public bool IgnoreGeometries;

        public CitygmlParserParams(bool optimize, bool tessellate, bool ignoreGeometries)
        {
            this.Optimize = optimize;
            this.Tessellate = tessellate;
            this.IgnoreGeometries = ignoreGeometries;
        }
    }
    
    public enum MeshFileFormat{OBJ, GLTF, FBX}


    [StructLayout(LayoutKind.Sequential)]
    public struct MeshConvertOptionsData
    {
        public CoordinateSystem MeshAxes;
        public PlateauVector3d ReferencePoint;
        public MeshGranularity MeshGranularity;
        public uint MinLOD;
        public uint MaxLOD;
        [MarshalAs(UnmanagedType.U1)] public bool ExportLowerLOD;
        [MarshalAs(UnmanagedType.U1)] public bool ExportAppearance;
        public float UnitScale;
        public MeshFileFormat MeshFileFormat;
        public int CoordinateZoneID;
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct FbxWriteOptions
    {
        public FbxFileFormat FileFormat;

        public FbxWriteOptions(FbxFileFormat fileFormat)
        {
            this.FileFormat = fileFormat;
        }
    }

    public enum FbxFileFormat : UInt32
    {
        Binary, Ascii
    }

    /// <summary>
    /// 地理院地図のタイル座標です。
    /// タイル座標については国土地理院のWebサイトを参照してください。
    /// <see href="https://maps.gsi.go.jp/development/siyou.html#siyou-zm"/>
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct TileCoordinate
    {
        public int Column;
        public int Row;
        public int ZoomLevel;

        public TileCoordinate(int column, int row, int zoomLevel)
        {
            this.Column = column;
            this.Row = row;
            this.ZoomLevel = zoomLevel;
        }

        public override string ToString()
        {
            return $"TileCoordinate: (Column={this.Column}, Row={this.Row}, ZoomLevel={this.ZoomLevel})";
        }
    }

    /// <summary>
    /// GMLファイルから3Dメッシュを取り出すための設定です。
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct MeshExtractOptions
    {
        /// <summary> 直交座標系における座標で、3Dモデルの原点をどこに設定するかです。 </summary>
        public PlateauVector3d ReferencePoint;
        /// <summary> 座標軸の向きです。 </summary>
        public CoordinateSystem MeshAxes;
        /// <summary> メッシュ結合の粒度です。 </summary>
        public MeshGranularity MeshGranularity;
        /// <summary> 出力するLODの範囲上限です。 </summary>
        public uint MaxLOD;
        /// <summary> 出力するLODの範囲の下限です。 </summary>
        public uint MinLOD;
        /// <summary> テクスチャを含めるかどうかです。 </summary>
        [MarshalAs(UnmanagedType.U1)] public bool ExportAppearance;
        /// <summary> メッシュ結合の粒度が「都市モデル単位」の時のみ有効で、この設定では都市を格子状のグリッドに分割するので、その1辺あたりの分割数(縦の数 = 横の数)です。</summary>
        public int GridCountOfSide;
        /// <summary>  大きさ補正です。  </summary>
        public float UnitScale;
        /// <summary>
        /// 国土交通省が規定する、日本の平面直角座標系の基準点の番号です。
        /// 詳しくは次の国土地理院のサイトをご覧ください。
        /// <see href="https://www.gsi.go.jp/sokuchikijun/jpc.html"/>
        /// </summary>
        public int CoordinateZoneID;
        /// <summary>
        /// 範囲外の3Dモデルを出力から除外するための、2つの方法のうち1つを有効にするかどうかを bool で指定します。
        /// その方法とは、都市オブジェクトの最初の頂点の位置が範囲外のとき、そのオブジェクトはすべて範囲外とみなして出力から除外します。
        /// これはビル1棟程度の大きさのオブジェクトでは有効ですが、
        /// 10km×10kmの地形のような巨大なオブジェクトでは、実際には範囲内なのに最初の頂点が遠いために除外されるということがおきます。
        /// したがって、この値は建物では true, 地形では false となるべきです。
        /// </summary>
        [MarshalAs(UnmanagedType.U1)] public bool ExcludeCityObjectOutsideExtent;
        /// <summary>
        /// 範囲外の3Dモデルを出力から除外するための、2つの方法のうち1つを有効にするかどうかを bool で指定します。
        /// その方法とは、メッシュ操作によって、範囲外に存在するポリゴンを除外します。
        /// この方法であれば 10km×10km の地形など巨大なオブジェクトにも対応できます。
        /// </summary>
        [MarshalAs(UnmanagedType.U1)] public bool ExcludeTrianglesOutsideExtent;
        /// <summary>  対象範囲を緯度・経度・高さで指定します。 </summary>
         public Extent Extent;
        
        /// <summary> デフォルト値の設定を返します。 </summary>
        public static MeshExtractOptions DefaultValue()
        {
            var apiResult = NativeMethods.plateau_mesh_extract_options_default_value(out var defaultOptions);
            DLLUtil.CheckDllError(apiResult);
            return defaultOptions;
        }

         /// <summary>
        /// 設定の値が正常なら true, 異常な点があれば false を返します。
         /// <param name="failureMessage">異常な点があれば、それを説明する文字列が入ります。正常なら空文字列になります。</param>
        /// </summary>
        public bool Validate(out string failureMessage)
        {
            failureMessage = "";
            if (this.MinLOD > this.MaxLOD)
            {
                failureMessage = $"Validate failed : {nameof(this.MinLOD)} should not greater than {nameof(this.MaxLOD)}.";
                return false;
            }

            if (this.GridCountOfSide <= 0)
            {
                failureMessage = $"Validate failed : {nameof(this.GridCountOfSide)} should be positive number.";
                return false;
            }

            if (Math.Abs(this.UnitScale) < 0.00000001)
            {
                failureMessage = $"Validate failed : {nameof(this.UnitScale)} is too small.";
                return false;
            }

            return true;
        }
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct GeoCoordinate
    {
        public double Latitude;
        public double Longitude;
        public double Height;

        public GeoCoordinate(double lat, double lon, double height)
        {
            this.Latitude = lat;
            this.Longitude = lon;
            this.Height = height;
        }

        public override string ToString()
        {
            return $"GeoCoordinate: (Lat={this.Latitude}, Lon={this.Longitude}, Height={this.Height})";
        }

        public static GeoCoordinate Min(GeoCoordinate op1, GeoCoordinate op2)
        {
            return new GeoCoordinate(
                Math.Min(op1.Latitude, op2.Latitude),
                Math.Min(op1.Longitude, op2.Longitude),
                Math.Min(op1.Height, op2.Height)
            );
        }

        public static GeoCoordinate Max(GeoCoordinate op1, GeoCoordinate op2)
        {
            return new GeoCoordinate(
                Math.Max(op1.Latitude, op2.Latitude),
                Math.Max(op1.Longitude, op2.Longitude),
                Math.Max(op1.Height, op2.Height)
            );
        }
        
        public static GeoCoordinate operator +(GeoCoordinate op1, GeoCoordinate op2)
        {
            return new GeoCoordinate(
                op1.Latitude + op2.Latitude,
                op1.Longitude + op2.Longitude,
                op1.Height + op2.Height
            );
        }

        public static GeoCoordinate operator -(GeoCoordinate op1, GeoCoordinate op2)
        {
            return new GeoCoordinate(
                op1.Latitude - op2.Latitude,
                op1.Longitude - op2.Longitude,
                op1.Height - op2.Height
            );
        }

        public static GeoCoordinate operator *(GeoCoordinate geo, double scalar)
        {
            return new GeoCoordinate(
                geo.Latitude * scalar,
                geo.Longitude * scalar,
                geo.Height * scalar
            );
        }

        public static GeoCoordinate operator /(GeoCoordinate geo, double scalar)
        {
            if (Math.Abs(scalar) <= double.Epsilon) throw new DivideByZeroException();
            return geo * (1.0 / scalar);
        }

        /// <summary>
        /// 緯度、経度の値を2次元ベクトルとして見たときのベクトルの長さの2乗です。
        /// 高さは無視されます。
        /// </summary>
        public double SqrMagnitudeLatLon => this.Latitude * this.Latitude + this.Longitude * this.Longitude;
    }

    /// <summary>
    /// 最小・最大からなる範囲です。
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct Extent
    {
        public GeoCoordinate Min;
        public GeoCoordinate Max;

        public Extent(GeoCoordinate min, GeoCoordinate max)
        {
            this.Min = min;
            this.Max = max;
        }

        public GeoCoordinate Center => new GeoCoordinate(
            (this.Min.Latitude + this.Max.Latitude) * 0.5,
            (this.Min.Longitude + this.Max.Longitude) * 0.5,
            (this.Min.Height + this.Max.Height) * 0.5);

         /// <summary>
         /// 共通部分を返します。
         /// なければ (-99, -99, -99), (-99, -99, -99)を返します。
         /// </summary>
         public static Extent Intersection(Extent op1, Extent op2)
         {
             var max = GeoCoordinate.Max(op1.Max, op2.Max);
             var min = GeoCoordinate.Min(op1.Min, op2.Min);
             var intersectSize = op1.Size() + op2.Size() - (max - min);
             if (intersectSize.Latitude <= 0 || intersectSize.Latitude <= 0 || intersectSize.Height <= 0)
                 return new Extent(new GeoCoordinate(-99,-99,-99), new GeoCoordinate(-99, -99, -99));
             var minMax = GeoCoordinate.Min(op1.Max, op2.Max);
             var maxMin = GeoCoordinate.Max(op1.Min, op2.Min);
             return new Extent(maxMin, minMax);
         }

         public GeoCoordinate Size()
         {
             return this.Max - this.Min;
         }

         public static readonly Extent All =
             new Extent(
                 new GeoCoordinate(-90, -180, -9999),
                 new GeoCoordinate(90, 180, 9999));
         
        public override string ToString()
        {
            return $"Extent: (Min={this.Min}, Max={this.Max})";
        }
    }

    public enum APIResult
    {
        Success,
        ErrorUnknown,
        ErrorValueNotFound,
        ErrorLoadingCityGml,
        ErrorIndexOutOfBounds,
        ErrorFileSystem,
        ErrorInvalidArgument
    }

    public enum DllLogLevel
    {
        Error = 4,
        Warning = 3,
        Info = 2,
        Debug = 1,
        Trace = 0
    }

    public delegate void LogCallbackFuncType(IntPtr textPtr);

    internal static class NativeMethods
    {
        public const string DllName = "plateau";

        [DllImport(DllName, CharSet = CharSet.Ansi)]
        internal static extern APIResult plateau_load_citygml(
            [In] byte[] gmlPathUtf8,
            [In] CitygmlParserParams parserParams,
            out IntPtr cityModelHandle,
            DllLogLevel logLevel,
            IntPtr logErrorCallbackFuncPtr,
            IntPtr logWarnCallbackFuncPtr,
            IntPtr logInfoCallbackFuncPtr);

        [DllImport(DllName)]
        internal static extern APIResult plateau_create_mesh_converter(
            out IntPtr outHandle
        );

        [DllImport(DllName)]
        internal static extern void plateau_delete_mesh_converter([In] IntPtr meshConverter);

        [DllImport(DllName, CharSet = CharSet.Ansi)]
        internal static extern APIResult plateau_mesh_converter_convert(
            [In] IntPtr meshConverter,
            [In] string destinationDirectory,
            [In] string gmlPath,
            [In] IntPtr cityModel,
            [In] IntPtr logger);

        [DllImport(DllName)]
        internal static extern APIResult plateau_mesh_converter_get_last_exported_model_file_names_count(
            [In] IntPtr meshConverter,
            out int fileCount);

        [DllImport(DllName)]
        internal static extern APIResult plateau_mesh_converter_get_last_exported_model_file_names(
            [In] IntPtr meshConverter,
            [In, Out] IntPtr[] strPointers,
            [Out] int[] outStrSizes);

        [DllImport(DllName)]
        internal static extern APIResult plateau_mesh_converter_set_options(
            [In] IntPtr meshConverter,
            MeshConvertOptionsData value);

        [DllImport(DllName)]
        internal static extern APIResult plateau_mesh_converter_get_options(
            [In] IntPtr meshConverter,
            out MeshConvertOptionsData value);

        [DllImport(DllName)]
        internal static extern MeshConvertOptionsData plateau_create_mesh_convert_options();

        [DllImport(DllName)]
        internal static extern APIResult plateau_mesh_convert_options_set_valid_reference_point(
            [In, Out] ref MeshConvertOptionsData options,
            [In] IntPtr cityModel);

        [DllImport(DllName)]
        internal static extern APIResult plateau_delete_city_model(
            [In] IntPtr cityModel);

        [DllImport(DllName)]
        internal static extern APIResult plateau_city_model_get_root_city_objects(
            [In] IntPtr cityModel,
            [In, Out] IntPtr[] cityObjects,
            int count);

        [DllImport(DllName)]
        internal static extern APIResult plateau_city_model_get_root_city_object_count(
            [In] IntPtr cityModel,
            out int outCount);

        [DllImport(DllName)]
        internal static extern APIResult plateau_city_model_get_all_city_object_count_of_type(
            [In] IntPtr cityModel,
            out int count,
            CityObjectType type);

        [DllImport(DllName)]
        internal static extern APIResult plateau_city_model_get_all_city_objects_of_type(
            [In] IntPtr cityModel,
            [In, Out] IntPtr[] cityObjects,
            CityObjectType type,
            int count);

        [DllImport(DllName, CharSet = CharSet.Ansi)]
        internal static extern APIResult plateau_city_model_get_city_object_by_id(
            [In] IntPtr handle,
            out IntPtr cityObjectPtr,
            [In] string id);

        // ***************
        //  geometry_utils_c.cpp
        // ***************
        [DllImport(DllName)]
        internal static extern APIResult plateau_geometry_utils_get_center_point(
            [In] IntPtr cityModelPtr,
            out PlateauVector3d outCenterPoint,
            int coordinateZoneID);
        
        
        // ***************
        //  mesh_extract_options_c.cpp
        // ***************
        [DllImport(DllName)]
        internal static extern APIResult plateau_mesh_extract_options_default_value(
            out MeshExtractOptions outDefaultOptions);

        // ***************
        //  vector_tile_downloader_c.cpp
        // ***************
        
        [DllImport(DllName)]
        internal static extern APIResult plateau_tile_projection_project(
            [In] GeoCoordinate geoCoordinate,
            int zoomLevel,
            out TileCoordinate outTileCoordinate);

        [DllImport(DllName)]
        internal static extern APIResult plateau_tile_projection_unproject(
            [In] TileCoordinate tileCoordinate,
            out Extent outExtent);

        
        
        // ***************
        //  available_lod_searcher_c.cpp
        // ***************

        [DllImport(DllName, CharSet = CharSet.Ansi)]
        internal static extern APIResult plateau_lod_searcher_search_lods_in_file(
            [In] byte[] filePathUtf8,
            out uint outLodFlags);
        
    }
}
