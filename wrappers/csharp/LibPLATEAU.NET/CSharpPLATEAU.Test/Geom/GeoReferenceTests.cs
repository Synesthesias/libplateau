using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Geometries;
using PLATEAU.Interop;

namespace PLATEAU.Test.Geom
{
    [TestClass]
    public class GeoReferenceTests
    {
        [TestMethod]
        public void When_GeoCoordinate_Is_OriginOfZone_Then_Project_Returns_Zero()
        {
            // 座標系の原点は次のWebサイトで示されるとおりです。
            // https://www.gsi.go.jp/LAW/heimencho.html
            // ただし、60分が 1, 30分が 0.5 となるように60分割を10進数の小数に直す必要があります。
            CheckZoneIDOrigin(1, 33.0, 129.5);
            CheckZoneIDOrigin(2, 33.0, 131.0);
            CheckZoneIDOrigin(3, 36.0, 132.166666667);
            CheckZoneIDOrigin(4, 33.0, 133.5);
            CheckZoneIDOrigin(5, 36.0, 134.33333333333);
            CheckZoneIDOrigin(6, 36, 136);
            CheckZoneIDOrigin(7, 36, 137.16666666667);
            CheckZoneIDOrigin(8, 36.0, 138.5);
            CheckZoneIDOrigin(9, 36.0, 139.833333333333);
            CheckZoneIDOrigin(10, 40.0, 140.83333333333333);
            CheckZoneIDOrigin(11, 44.0, 140.25);
            CheckZoneIDOrigin(12, 44.0, 142.25);
            CheckZoneIDOrigin(13, 44.0, 144.25);
            CheckZoneIDOrigin(14, 26.0, 142.0);
            CheckZoneIDOrigin(15, 26.0, 127.5);
            CheckZoneIDOrigin(16, 26, 124.0);
            CheckZoneIDOrigin(17, 26.0, 131.0);
            CheckZoneIDOrigin(18, 20.0, 136.0);
            CheckZoneIDOrigin(19, 26.0, 154.0);
        }

        /// <summary>
        /// 次のウェブサイトで示される平面直角座標系の原点について、
        /// https://www.gsi.go.jp/sokuchikijun/jpc.html
        /// 原点の座標を与えて Project すると結果は 0 に近くなることをテストします。
        /// </summary>
        private static void CheckZoneIDOrigin(int zoneID, double latitude, double longitude)
        {
            using var geoReference = new GeoReference(
                new PlateauVector3d(0, 0, 0),
                1f, CoordinateSystem.WUN, zoneID
            );
            var xyz = geoReference.Project(new GeoCoordinate(latitude, longitude, 0.0));
            Assert.IsTrue(Math.Abs(xyz.X) < 0.0001);
            Assert.IsTrue(Math.Abs(xyz.Z) < 0.0001);
            Assert.IsTrue(Math.Abs(xyz.Y) < 0.0001);
        }

        [TestMethod]
        public void Unproject_Converts_Local_Position_To_GeoCoordinate()
        {
            //check zoneID
            CheckUnproject(new PlateauVector3d(-8221.96099653525, -41667.536332340445, 0.0), new GeoCoordinate(35.62439457074015, 139.74256342432295, 0.0), 
                new PlateauVector3d(0, 0, 0), 1f, CoordinateSystem.ENU, 9);
            CheckUnproject(new PlateauVector3d(0.0, 0.0, 0.0), new GeoCoordinate(33.0, 131.0, 0.0),
                new PlateauVector3d(0, 0, 0), 1f, CoordinateSystem.ENU, 2);

            //check ReferencePoint
            CheckUnproject(new PlateauVector3d(-8221.96099653525 - 100, -41667.536332340445 - 200, 0.0), new GeoCoordinate(35.62439457074015, 139.74256342432295, 0.0),
                new PlateauVector3d(100, 200, 0), 1f, CoordinateSystem.ENU, 9);

            //check CoordinateSystem
            CheckUnproject(new PlateauVector3d(8221.96099653525, 0.0, -41667.536332340445), new GeoCoordinate(35.62439457074015, 139.74256342432295, 0.0),
                new PlateauVector3d(0, 0, 0), 1f, CoordinateSystem.WUN, 9);
            CheckUnproject(new PlateauVector3d(-41667.536332340445, 8221.96099653525, 0.0), new GeoCoordinate(35.62439457074015, 139.74256342432295, 0.0),
                new PlateauVector3d(0, 0, 0), 1f, CoordinateSystem.NWU, 9);

            //check UnitScale
            CheckUnproject(new PlateauVector3d(-8221.96099653525 / 2, -41667.536332340445 / 2, 0.0), new GeoCoordinate(35.62439457074015, 139.74256342432295, 0.0),
                new PlateauVector3d(0, 0, 0), 2.0f, CoordinateSystem.ENU, 9);
        }

        private static void CheckUnproject(PlateauVector3d point, GeoCoordinate target_lat_lon, PlateauVector3d reference_point, float unit_scale, CoordinateSystem coordinate_system, int zone_id)
        {
            using var geoReference = new GeoReference(reference_point, unit_scale, coordinate_system, zone_id);

            var lat_lon = geoReference.Unproject(point);
            Assert.IsTrue(Math.Abs(lat_lon.Latitude - target_lat_lon.Latitude) < 0.00000001);//およそ1mm相当の誤差以内か
            Assert.IsTrue(Math.Abs(lat_lon.Longitude - target_lat_lon.Longitude) < 0.00000001);//およそ1mm相当の誤差以内か
        }

        [TestMethod]
        public void Unproject_Is_Inverse_Of_Project()
        {
            CheckProjectUnproject(new PlateauVector3d(0, 0, 0), 1f, CoordinateSystem.ENU, 9);
            CheckProjectUnproject(new PlateauVector3d(10, 10, 10), 2f, CoordinateSystem.WUN, 9);
            CheckProjectUnproject(new PlateauVector3d(100, 100, 100), 3f, CoordinateSystem.NWU, 9);
            CheckProjectUnproject(new PlateauVector3d(-100, -100, -100), 4f, CoordinateSystem.EUN, 9);
        }

        [TestMethod]
        public void Getter_Returns_Value()
        {
            var geoReference = new GeoReference(
                new PlateauVector3d(1, 2, 3), 4, CoordinateSystem.ENU, 5
            );
            Assert.AreEqual(1, geoReference.ReferencePoint.X);
            Assert.AreEqual(2, geoReference.ReferencePoint.Y);
            Assert.AreEqual(3, geoReference.ReferencePoint.Z);
            Assert.AreEqual(4, geoReference.UnitScale);
            Assert.AreEqual(5, geoReference.ZoneID);
            Assert.AreEqual(CoordinateSystem.ENU, geoReference.CoordinateSystem);
        }

        private static void CheckProjectUnproject(PlateauVector3d reference_point, float unit_scale, CoordinateSystem coordinate_system, int zone_id)
        {
            using var geoReference = new GeoReference(reference_point, unit_scale, coordinate_system, zone_id);
            var latitude = 35.62439457074015;
            var longitude = 139.74256342432295;
            var xyz = geoReference.Project(new GeoCoordinate(latitude, longitude, 0.0));
            var lat_lon = geoReference.Unproject(xyz);
            Console.WriteLine($"lat diff = {lat_lon.Latitude - latitude}, lon diff = {lat_lon.Longitude - longitude}");
            Assert.IsTrue(Math.Abs(lat_lon.Latitude - latitude) < 0.00000001);//およそ1mm相当の誤差以内か
            Assert.IsTrue(Math.Abs(lat_lon.Longitude - longitude) < 0.00000001);//およそ1mm相当の誤差以内か
        }
    }
}
