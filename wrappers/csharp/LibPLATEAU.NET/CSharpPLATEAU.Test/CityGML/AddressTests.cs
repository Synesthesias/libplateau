using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{
    [TestClass]
    public class AddressTests
    {
        private Address address;
        // 前準備
        public AddressTests()
        {
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var cityObject = cityModel.RootCityObjects[0];
            this.address = cityObject.Address;
        }

        [TestMethod]
        public void Country_Returns_GML_Country()
        {
            const string gmlCountry = "日本";
            string actualCountry = this.address.Country;
            Console.WriteLine(actualCountry);
            Assert.AreEqual(gmlCountry, actualCountry);
        }

        [TestMethod]
        public void Locality_Returns_GML_Locality()
        {
            const string gmlLocality = "東京都大田区羽田空港二丁目";
            string actualLocality = this.address.Locality;
            Console.WriteLine(actualLocality);
            Assert.AreEqual(gmlLocality, actualLocality);
        }

        [TestMethod]
        public void PostalCode_Returns_GML_PostalCode()
        {
            const string gmlPostalCode = "123-4567";
            string actualPostalcode = this.address.PostalCode;
            Console.WriteLine(actualPostalcode);
            Assert.AreEqual(gmlPostalCode, actualPostalcode);
        }

        [TestMethod]
        public void ThoroughFareName_Returns_GML_ThoroughFareName()
        {
            const string gmlThoroughFareName = "Test Street テスト丁目";
            string actualThoroughFareName = this.address.ThoroughFareName;
            Console.WriteLine(actualThoroughFareName);
            Assert.AreEqual(gmlThoroughFareName, actualThoroughFareName);
        }

        [TestMethod]
        public void ThoroughFareNumber_Returns_GML_ThoroughFareNumber()
        {
            const string gmlThoroughFareNumber = "Test番地";
            string actualThoroughFareNumber = this.address.ThoroughFareNumber;
            Console.WriteLine(actualThoroughFareNumber);
            Assert.AreEqual(gmlThoroughFareNumber, actualThoroughFareNumber);
        }

    }
}