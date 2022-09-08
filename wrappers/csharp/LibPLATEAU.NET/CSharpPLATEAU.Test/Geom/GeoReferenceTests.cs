using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Geom;
using PLATEAU.Interop;
using PLATEAU.IO;

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
        public void Temp_Test_For_Unproject()
        {
            using var geoReference = new GeoReference(
                new PlateauVector3d(0, 0, 0),
                1f, CoordinateSystem.WUN, 9
            );
            var lat_lon = geoReference.Unproject(new PlateauVector3d(-8221.9609, -41667.536, 0));
            Assert.IsTrue(Math.Abs(lat_lon.Latitude - 35.62439457074015) < 0.00000001);//およそ1mmの誤差以内か
            Assert.IsTrue(Math.Abs(lat_lon.Longitude - 139.74256342432295) < 0.00000001);//およそ1mmの誤差以内か
        }
    }
}
