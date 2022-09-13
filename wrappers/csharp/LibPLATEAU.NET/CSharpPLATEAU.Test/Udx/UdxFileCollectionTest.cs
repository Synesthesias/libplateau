using System;
using System.IO;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.Interop;
using PLATEAU.Udx;

namespace PLATEAU.Test.Udx
{
    [TestClass]
    public class UdxFileCollectionTest
    {
        [TestMethod]
        public void Can_Get_All_Mesh_Codes()
        {
            var collection = UdxFileCollection.Find("data");
            Assert.AreEqual(4, collection.MeshCodes.Count);
        }

        [TestMethod]
        public void Filtered_Collection_Contains_All_Files_In_Extent()
        {
            var extent = new Extent
            {
                Max = new GeoCoordinate(35.53725521, 139.77699279, 0),
                Min = new GeoCoordinate(35.53725511, 139.77699239, 0)
            };
            var collection = UdxFileCollection.Find("data");
            var filteredCollection = collection.Filter(extent);
            Assert.AreEqual(1, filteredCollection.MeshCodes.Count);
            Assert.AreEqual("53392642", filteredCollection.MeshCodes[0].ToString());
        }

        [TestMethod]
        public void Packages_Getter_Returns_All_Packages()
        {
            var collection = UdxFileCollection.Find("data");
            var expected = PredefinedCityModelPackage.Building;
            Assert.AreEqual(expected, collection.Packages);
        }

        [TestMethod]
        public void Get_Gml_Files_Returns_All_Files()
        {
            var collection = UdxFileCollection.Find("data");
            Assert.AreEqual(Path.GetFullPath("data\\udx\\bldg\\53391540_bldg_6697_op.gml"), Path.GetFullPath(collection.GetGmlFiles(PredefinedCityModelPackage.Building)[0]));
        }
    }
}
