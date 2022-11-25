using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{
    [TestClass]
    public class AddressTests
    {
        private static Address address;
        private static CityModel cityModel;

        [ClassInitialize]
        public static void ClassInitialize(TestContext _)
        {
            cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var cityObject = cityModel.RootCityObjects[0];
            address = cityObject.Address;
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            cityModel.Dispose();
        }

        [TestMethod]
        public void Country_Returns_GML_Country()
        {
            const string gmlCountry = "日本";
            string actualCountry = address.Country;
            Console.WriteLine(actualCountry);
            Assert.AreEqual(gmlCountry, actualCountry);
        }

        [TestMethod]
        public void Locality_Returns_GML_Locality()
        {
            const string gmlLocality = "東京都大田区羽田空港二丁目";
            string actualLocality = address.Locality;
            Console.WriteLine(actualLocality);
            Assert.AreEqual(gmlLocality, actualLocality);
        }

        [TestMethod]
        public void PostalCode_Returns_GML_PostalCode()
        {
            const string gmlPostalCode = "123-4567";
            string actualPostalcode = address.PostalCode;
            Console.WriteLine(actualPostalcode);
            Assert.AreEqual(gmlPostalCode, actualPostalcode);
        }

        [TestMethod]
        public void ThoroughFareName_Returns_GML_ThoroughFareName()
        {
            const string gmlThoroughFareName = "Test Street テスト丁目";
            string actualThoroughFareName = address.ThoroughFareName;
            Console.WriteLine(actualThoroughFareName);
            Assert.AreEqual(gmlThoroughFareName, actualThoroughFareName);
        }

        [TestMethod]
        public void ThoroughFareNumber_Returns_GML_ThoroughFareNumber()
        {
            const string gmlThoroughFareNumber = "Test番地";
            string actualThoroughFareNumber = address.ThoroughFareNumber;
            Console.WriteLine(actualThoroughFareNumber);
            Assert.AreEqual(gmlThoroughFareNumber, actualThoroughFareNumber);
        }

    }
}
