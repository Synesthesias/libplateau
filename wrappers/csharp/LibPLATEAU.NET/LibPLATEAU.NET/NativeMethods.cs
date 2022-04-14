using System;
using System.Runtime.InteropServices;
using System.Text;

namespace LibPLATEAU.NET
{
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
    }

    [StructLayout(LayoutKind.Sequential)]
    public struct CitygmlParserParams
    {
        public int Optimize;
    }

    public enum AxesConversion
    {
        WNU,
        RUF
    }

    public enum APIResult
    {
        Success,
        ErrorUnknown,
        ErrorValueNotFound,
        ErrorLackOfBufferSize,
        ErrorInvalidData,
        ErrorInvalidArgument
    }


    public enum CityObjectType : ulong
    {
        COT_GenericCityObject = 1ul,
        COT_Building = 1ul << 1,
        COT_Room = 1ul << 2,
        COT_BuildingInstallation = 1ul << 3,
        COT_BuildingFurniture = 1ul << 4,
        COT_Door = 1ul << 5,
        COT_Window = 1ul << 6,
        COT_CityFurniture = 1ul << 7,
        COT_Track = 1ul << 8,
        COT_Road = 1ul << 9,
        COT_Railway = 1ul << 10,
        COT_Square = 1ul << 11,
        COT_PlantCover = 1ul << 12,
        COT_SolitaryVegetationObject = 1ul << 13,
        COT_WaterBody = 1ul << 14,
        COT_ReliefFeature = 1ul << 15,
        COT_ReliefComponent = 1ul << 35,
        COT_TINRelief = 1ul << 36,
        COT_MassPointRelief = 1ul << 37,
        COT_BreaklineRelief = 1ul << 38,
        COT_RasterRelief = 1ul << 39,
        COT_LandUse = 1ul << 16,
        COT_Tunnel = 1ul << 17,
        COT_Bridge = 1ul << 18,
        COT_BridgeConstructionElement = 1ul << 19,
        COT_BridgeInstallation = 1ul << 20,
        COT_BridgePart = 1ul << 21,
        COT_BuildingPart = 1ul << 22,

        COT_WallSurface = 1ul << 23,
        COT_RoofSurface = 1ul << 24,
        COT_GroundSurface = 1ul << 25,
        COT_ClosureSurface = 1ul << 26,
        COT_FloorSurface = 1ul << 27,
        COT_InteriorWallSurface = 1ul << 28,
        COT_CeilingSurface = 1ul << 29,
        COT_CityObjectGroup = 1ul << 30,
        COT_OuterCeilingSurface = 1ul << 31,
        COT_OuterFloorSurface = 1ul << 32,


        // covers all supertypes of tran::_TransportationObject that are not Track, Road, Railway or Square...
        // there are to many for to few bits to explicitly enumerate them. However Track, Road, Railway or Square should be used most of the time
        COT_TransportationObject = 1ul << 33,

        // ADD Building model 
        COT_IntBuildingInstallation = 1ul << 34,

        COT_All = 0xFFFFFFFFFFFFFFFFul
    };

    public enum AttributeType
    {
        String,
        Double,
        Integer,
        Data,
        Uri,
        Measure,
        AttributeSet
    }

    internal static class NativeMethods
    {
        private const string kDllName = "plateau_c";

        [DllImport(kDllName)]
        internal static extern IntPtr plateau_load_citygml(
            [In] string gmlPath,
            [In] CitygmlParserParams parserParams);

        [DllImport(kDllName)]
        internal static extern IntPtr plateau_create_obj_writer();

        [DllImport(kDllName)]
        internal static extern void plateau_delete_obj_writer([In] IntPtr objWriter);

        [DllImport(kDllName)]
        internal static extern void plateau_obj_writer_write(
            [In] IntPtr objWriter,
            [In] string objPath,
            [In] IntPtr cityModel,
            [In] string gmlPath);

        [DllImport(kDllName)]
        internal static extern void plateau_obj_writer_set_merge_mesh_flg(
            [In] IntPtr objWriter,
            bool value);

        [DllImport(kDllName)]
        [return: MarshalAs(UnmanagedType.U1)]
        internal static extern bool plateau_obj_writer_get_merge_mesh_flg(
            [In] IntPtr objWriter);

        [DllImport(kDllName)]
        internal static extern void plateau_obj_writer_set_dest_axes(
            [In] IntPtr objWriter,
            AxesConversion value);

        [DllImport(kDllName)]
        internal static extern AxesConversion plateau_obj_writer_get_dest_axes(
            [In] IntPtr objWriter);

        [DllImport(kDllName)]
        internal static extern PlateauVector3d plateau_obj_writer_get_reference_point(
            [In] IntPtr objWriter);

        [DllImport(kDllName)]
        internal static extern void plateau_obj_writer_set_reference_point(
            [In] IntPtr objWriter,
            [In] PlateauVector3d referencePoint);

        [DllImport(kDllName)]
        internal static extern void plateau_obj_writer_set_valid_reference_point(
            [In] IntPtr objWriter,
            [In] IntPtr cityModel);

        [DllImport(kDllName)]
        internal static extern int plateau_delete_city_model(
            [In] IntPtr cityModel);

        [DllImport(kDllName)]
        internal static extern void plateau_city_model_get_root_city_objects(
            [In] IntPtr cityModel,
            [In, Out] IntPtr[] cityObjects,
            int count);

        [DllImport(kDllName)]
        internal static extern int plateau_city_model_get_root_city_object_count(
            [In] IntPtr cityModel);


        // ***************
        //  city_object_c.cpp
        // ***************

        [DllImport(kDllName)]
        internal static extern CityObjectType plateau_city_object_get_type(
            [In] IntPtr cityObject);

        [DllImport(kDllName)]
        internal static extern int plateau_city_object_get_geometries_count(
            [In] IntPtr cityObject);


        // ***************
        //  Object_c.cpp
        // ***************

        [DllImport(kDllName)]
        internal static extern IntPtr plateau_object_get_id(
            [In] IntPtr obj);
        
        
        [DllImport(kDllName)]
        internal static extern IntPtr plateau_object_get_attributes_map(
            [In] IntPtr obj);

        // [DllImport(kDllName, CharSet = CharSet.Ansi)]
        // internal static extern APIResult plateau_object_get_attribute(
        //     [In] IntPtr plateauObject,
        //     string name,
        //     StringBuilder sb,
        //     int bufferSize);
        //
        // [DllImport(kDllName, CharSet = CharSet.Ansi)]
        // internal static extern void plateau_object_set_attribute(
        //     [In] IntPtr plateauObject,
        //     string name,
        //     string value,
        //     AttributeType type,
        //     bool overwrite);
        //
        // [DllImport(kDllName, CharSet = CharSet.Ansi)]
        // internal static extern APIResult plateau_object_get_keys_values(
        //     [In] IntPtr plateauObject,
        //     StringBuilder keysValues,
        //     int bufferSize,
        //     string separator);


        // ***************
        //  featureobject_c.cpp
        // ***************

        [DllImport(kDllName)]
        internal static extern void plateau_feature_object_get_envelope(
            [In] IntPtr featureObject,
            IntPtr outArray
        );

        [DllImport(kDllName)]
        internal static extern void plateau_feature_object_set_envelope(
            [In] IntPtr featureObject,
            double lowerX, double lowerY, double lowerZ,
            double upperX, double upperY, double upperZ
        );
        
        
        // ***************
        //  attributesmap_c.cpp
        // ***************
        [DllImport(kDllName)]
        internal static extern int plateau_attributes_map_get_key_count(
            [In] IntPtr attributesMap);

        [DllImport(kDllName)]
        internal static extern void plateau_attributes_map_get_key_sizes(
            [In] IntPtr attributesMap,
            [In, Out] IntPtr outSizeIntArray);

        [DllImport(kDllName, CharSet = CharSet.Ansi)]
        internal static extern void plateau_attributes_map_get_keys(
            [In] IntPtr attributesMap,
            [In, Out] ref StringBuilder[] outStringBuilders);
        
    }
}