﻿using System;
using System.Runtime.InteropServices;
using System.Text;

namespace LibPLATEAU.NET.CityGML
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

        public override string ToString()
        {
            return $"({this.X}, {this.Y}, {this.Z})";
        }
    }

    /// <summary>
    /// GMLファイルのパース時の設定です。
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct CitygmlParserParams
    {
        public int Optimize;
        /// <summary>
        /// <see cref="Tessellate"/> を false に設定すると、 <see cref="Polygon"/> が頂点を保持する代わりに <see cref="LinearRing"/> を保持することがあります。
        /// </summary>
        [MarshalAs(UnmanagedType.U1)]
        public bool Tessellate;

        public CitygmlParserParams(int optimize = 1, bool tessellate = true)
        {
            this.Optimize = optimize;
            this.Tessellate = tessellate;
        }
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
    
    public enum GeometryType : ulong
    {
        GT_Unknown          = 1ul << 0,
        GT_Roof             = 1ul << 1,
        GT_Wall             = 1ul << 2,
        GT_Ground           = 1ul << 3,
        GT_Closure          = 1ul << 4,
        GT_Floor            = 1ul << 5,
        GT_InteriorWall     = 1ul << 6,
        GT_Ceiling          = 1ul << 7,
        GT_OuterCeiling     = 1ul << 8,
        GT_OuterFloor       = 1ul << 9,
        GT_Tin              = 1ul << 10,
    }

    /// <summary>
    /// 属性の値の想定形式です。
    /// 形式が String, Double, Integer, Data, Uri, Measure である場合、内部的にはデータは string です。
    /// AttributeSet である場合、内部的にはデータは <see cref="AttributesMap"/> への参照です。
    /// </summary>
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
        private const string DllName = "plateau_c";

        [DllImport(DllName)]
        internal static extern IntPtr plateau_load_citygml(
            [In] string gmlPath,
            [In] CitygmlParserParams parserParams);

        [DllImport(DllName)]
        internal static extern IntPtr plateau_create_obj_writer();

        [DllImport(DllName)]
        internal static extern void plateau_delete_obj_writer([In] IntPtr objWriter);

        [DllImport(DllName)]
        internal static extern void plateau_obj_writer_write(
            [In] IntPtr objWriter,
            [In] string objPath,
            [In] IntPtr cityModel,
            [In] string gmlPath);

        [DllImport(DllName)]
        internal static extern void plateau_obj_writer_set_merge_mesh_flg(
            [In] IntPtr objWriter,
            bool value);

        [DllImport(DllName)]
        internal static extern APIResult plateau_obj_writer_get_merge_mesh_flg(
            [In] IntPtr objWriter,
            out bool outMeshFlg);

        [DllImport(DllName)]
        internal static extern void plateau_obj_writer_set_dest_axes(
            [In] IntPtr objWriter,
            AxesConversion value);

        [DllImport(DllName)]
        internal static extern APIResult plateau_obj_writer_get_dest_axes(
            [In] IntPtr objWriter,
            out AxesConversion axesConversion);

        [DllImport(DllName)]
        internal static extern PlateauVector3d plateau_obj_writer_get_reference_point(
            [In] IntPtr objWriter);

        [DllImport(DllName)]
        internal static extern void plateau_obj_writer_set_reference_point(
            [In] IntPtr objWriter,
            [In] PlateauVector3d referencePoint);

        [DllImport(DllName)]
        internal static extern void plateau_obj_writer_set_valid_reference_point(
            [In] IntPtr objWriter,
            [In] IntPtr cityModel);

        [DllImport(DllName)]
        internal static extern int plateau_delete_city_model(
            [In] IntPtr cityModel);

        [DllImport(DllName)]
        internal static extern APIResult plateau_city_model_get_root_city_objects(
            [In] IntPtr cityModel,
            [In, Out] IntPtr[] cityObjects,
            int count);

        [DllImport(DllName)]
        internal static extern int plateau_city_model_get_root_city_object_count(
            [In] IntPtr cityModel);


        // ***************
        //  city_object_c.cpp
        // ***************

        [DllImport(DllName)]
        internal static extern APIResult plateau_city_object_get_type(
            [In] IntPtr cityObjectHandle,
            out CityObjectType outCityObjType);

        [DllImport(DllName)]
        internal static extern APIResult plateau_city_object_get_geometries_count(
            [In] IntPtr cityObjectHandle,
            out int outCount);

        [DllImport(DllName)]
        internal static extern APIResult plateau_city_object_get_address(
            [In] IntPtr cityObjectHandle,
            out IntPtr addressHandle);

        [DllImport(DllName)]
        internal static extern APIResult plateau_city_object_get_implicit_geometry_count(
            [In] IntPtr cityObjectHandle,
            out int outCount);

        [DllImport(DllName)]
        internal static extern APIResult plateau_city_object_get_child_city_object_count(
            [In] IntPtr cityObjectHandle,
            out int outCount);

        [DllImport(DllName)]
        internal static extern APIResult plateau_city_object_get_child_city_object(
            [In] IntPtr cityObjectHandle,
            out IntPtr outChildHandle,
            int index);

        [DllImport(DllName)]
        internal static extern APIResult plateau_city_object_get_geometry(
            [In] IntPtr cityObjectHandle,
            out IntPtr outGeometryHandle,
            int index);

        // ***************
        //  Object_c.cpp
        // ***************

        [DllImport(DllName)]
        internal static extern APIResult plateau_object_get_id(
            [In] IntPtr objHandle,
            out IntPtr outStrPtr);

        [DllImport(DllName)]
        internal static extern APIResult plateau_object_get_id_str_length(
            [In] IntPtr objHandle,
            out int outLength);
        
        
        [DllImport(DllName)]
        internal static extern APIResult plateau_object_get_attributes_map(
            [In] IntPtr objHandle,
            out IntPtr attributesMapPtr);


        // ***************
        //  featureobject_c.cpp
        // ***************

        [DllImport(DllName)]
        internal static extern APIResult plateau_feature_object_get_envelope(
            [In] IntPtr featureObject,
            [Out] double[] outEnvelope 
        );

        [DllImport(DllName)]
        internal static extern APIResult plateau_feature_object_set_envelope(
            [In] IntPtr featureObject,
            double lowerX, double lowerY, double lowerZ,
            double upperX, double upperY, double upperZ
        );
        
        
        // ***************
        //  attributesmap_c.cpp
        // ***************
        [DllImport(DllName)]
        internal static extern APIResult plateau_attributes_map_get_key_count(
            [In] IntPtr attributesMap,
            out int count);

        [DllImport(DllName)]
        internal static extern APIResult plateau_attributes_map_get_key_sizes(
            [In] IntPtr attributesMap,
            [Out] int[] outSizeIntArray);

        
        [DllImport(DllName, CharSet = CharSet.Ansi)]
        internal static extern APIResult plateau_attributes_map_get_keys(
            [In] IntPtr attributesMap,
            [In, Out] IntPtr[] keyHandles);
        
        [DllImport(DllName, CharSet = CharSet.Ansi)]
        internal static extern APIResult plateau_attributes_map_get_attribute_value(
            [In] IntPtr attributesMap,
            [In] string key,
            [Out] out IntPtr attrValuePtr);
        
        [DllImport(DllName, CharSet = CharSet.Ansi)]
        internal static extern APIResult plateau_attributes_map_do_contains_key(
            [In] IntPtr attributesMap,
            [In] string key,
            out bool doContainsKey);

        
        // ***************
        //  attributevalue_c.cpp
        // ***************
        
        [DllImport(DllName, CharSet = CharSet.Ansi)]
        internal static extern APIResult plateau_attribute_value_get_string(
            [In] IntPtr attributeValue,
            StringBuilder outValue);

        [DllImport(DllName, CharSet = CharSet.Ansi)]
        internal static extern APIResult plateau_attribute_value_get_str_length(
            [In] IntPtr attributeValue,
            out int strLength);

        [DllImport(DllName, CharSet = CharSet.Ansi)]
        internal static extern APIResult plateau_attribute_value_get_type(
            [In] IntPtr attributeValue,
            out AttributeType attrType);

        [DllImport(DllName)]
        internal static extern APIResult plateau_attribute_as_attribute_set(
            [In] IntPtr attributeValue,
            [Out] out IntPtr attrSetPtr);
        
        
        // ***************
        //  address_c.cpp
        // ***************
        [DllImport(DllName, CharSet = CharSet.Ansi)]
        internal static extern APIResult plateau_address_get_country(
            [In] IntPtr addressHandle,
            out IntPtr outCountryNamePtr);

        [DllImport(DllName)]
        internal static extern APIResult plateau_address_get_country_str_length(
            [In] IntPtr addressHandle,
            out int strLength);

        [DllImport(DllName)]
        internal static extern APIResult plateau_address_get_locality(
            [In] IntPtr addressHandle,
            out IntPtr outStrPtr);

        [DllImport(DllName)]
        internal static extern APIResult plateau_address_get_locality_str_length(
            [In] IntPtr addressHandle,
            out int strLength);

        [DllImport(DllName)]
        internal static extern APIResult plateau_address_get_postal_code(
            [In] IntPtr addressHandle,
            out IntPtr outStrPtr);

        [DllImport(DllName)]
        internal static extern APIResult plateau_address_get_postal_code_str_length(
            [In] IntPtr addressHandle,
            out int strLength);

        [DllImport(DllName)]
        internal static extern APIResult plateau_address_get_thoroughfare_name(
            [In] IntPtr addressHandle,
            out IntPtr outStrPtr);

        [DllImport(DllName)]
        internal static extern APIResult plateau_address_get_thoroughfare_name_str_length(
            [In] IntPtr addressHandle,
            out int strLength);

        [DllImport(DllName)]
        internal static extern APIResult plateau_address_get_thoroughfare_number(
            [In] IntPtr addressHandle,
            out IntPtr outStrPtr);

        [DllImport(DllName)]
        internal static extern APIResult plateau_address_get_thoroughfare_number_str_length(
            [In] IntPtr addressHandle,
            out int strLength);
        
        // ***************
        //  geometry_c.cpp
        // ***************
        [DllImport(DllName)]
        internal static extern APIResult plateau_geometry_get_type(
            [In] IntPtr geometryHandle,
            out GeometryType type);

        [DllImport(DllName)]
        internal static extern APIResult plateau_geometry_get_geometries_count(
            [In] IntPtr geometryHandle,
            out int outCount);

        [DllImport(DllName)]
        internal static extern APIResult plateau_geometry_get_polygons_count(
            [In] IntPtr geometryHandle,
            out int outCount);

        [DllImport(DllName)]
        internal static extern APIResult plateau_geometry_get_child_geometry(
            [In] IntPtr geometryHandle,
            out IntPtr childGeomHandle,
            int index);

        [DllImport(DllName)]
        internal static extern APIResult plateau_geometry_get_polygon(
            [In] IntPtr geometryHandle,
            out IntPtr polygonHandle,
            int index
        );

        [DllImport(DllName)]
        internal static extern APIResult plateau_geometry_get_lod(
            [In] IntPtr geometryHandle,
            out int outLod);

        [DllImport(DllName)]
        internal static extern APIResult plateau_geometry_get_srs_name(
            [In] IntPtr geometryHandle,
            out IntPtr outNameStrPtr);

        [DllImport(DllName)]
        internal static extern APIResult plateau_geometry_get_srs_name_str_length(
            [In] IntPtr geometryHandle,
            out int outLength);

        [DllImport(DllName)]
        internal static extern APIResult plateau_geometry_get_line_string_count(
            [In] IntPtr handle,
            out int outCount);
        
        // ***************
        //  polygon_c.cpp
        // ***************
        [DllImport(DllName)]
        internal static extern APIResult plateau_polygon_get_vertex_count(
            [In] IntPtr polygonHandle,
            out int outVertCount);

        [DllImport(DllName)]
        internal static extern APIResult plateau_polygon_get_vertex(
            [In] IntPtr handle,
            out PlateauVector3d outVertex,
            int index);

        [DllImport(DllName)]
        internal static extern APIResult plateau_polygon_get_indices_count(
            [In] IntPtr handle,
            out int outCount);

        [DllImport(DllName)]
        internal static extern APIResult plateau_polygon_get_index_of_indices(
            [In] IntPtr handle,
            out int outIndex,
            int indexOfIndicesList);

        [DllImport(DllName)]
        internal static extern APIResult plateau_polygon_get_exterior_ring(
            [In] IntPtr handle,
            out IntPtr ringHandle);
        
        // ***************
        //  linearring_c.cpp
        // ***************
        [DllImport(DllName)]
        internal static extern APIResult plateau_linear_ring_get_vertex_count(
            [In] IntPtr handle,
            out int outCount);

        [DllImport(DllName)]
        internal static extern APIResult plateau_linear_ring_get_vertex(
            [In] IntPtr handle,
            out PlateauVector3d outVert3d,
            int index);

        [DllImport(DllName)]
        internal static extern APIResult plateau_polygon_get_interior_ring_count(
            [In] IntPtr handle,
            out int outCount);

        [DllImport(DllName)]
        internal static extern APIResult plateau_polygon_get_interior_ring(
            [In] IntPtr handle,
            out IntPtr outRingHandle,
            int index);
    }
}