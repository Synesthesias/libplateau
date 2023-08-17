using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{
    [TestClass]
    public class AttributeValueTests
    {
        private static CityModel cityModel;
        private static AttributesMap attrMap;
        
        [ClassInitialize]
        public static void ClassInitialize(TestContext _)
        {
            cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var cityObject = cityModel.RootCityObjects[0];
            attrMap = cityObject.AttributesMap;
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            cityModel.Dispose();
        }

        [DataTestMethod]
        [DataRow("建物ID", "13111-bldg-147301")]
        [DataRow("bldg:measuredheight", "2.8")]
        public void AsString_Returns_GML_Value(string key, string valueInGmlFile)
        {
            var actualVal = attrMap[key].AsString;
            Assert.AreEqual(valueInGmlFile, actualVal);
        }
        
        [DataTestMethod]
        [DataRow("建物ID", AttributeType.String)]
        [DataRow("bldg:measuredheight", AttributeType.Double)]
        [DataRow("多摩水系多摩川、浅川、大栗川洪水浸水想定区域（想定最大規模）", AttributeType.AttributeSet)]
        public void Type_Returns_GML_Type(string key, AttributeType typeInGmlFile)
        {
            AttributeType actualType = attrMap[key].Type;
            Assert.AreEqual(typeInGmlFile, actualType);
        }
        
        [TestMethod]
        public void AsAttrSet_Returns_Child_Attrs_In_GML()
        {
            const string parentKey = "多摩水系多摩川、浅川、大栗川洪水浸水想定区域（想定最大規模）";
            const string childKey = "規模";
            const string childValInGmlFile = "L2";
            var parentVal = attrMap[parentKey];
            var children = parentVal.AsAttrSet;
            // 参考用にキーの一覧を表示します。
            Console.WriteLine($"parent: {attrMap}");
            Console.WriteLine($"children: {children}");
        
            var actualChildStr = children[childKey].AsString;
            Console.WriteLine($"child value => {actualChildStr}");
            Assert.AreEqual(childValInGmlFile, actualChildStr);
        }
        
        [TestMethod]
        public void AsDouble_Returns_GML_Value()
        {
            const string key = "doubleAttributeテスト";
            const double valueInGmlFile = 123.456;
            double actualVal = attrMap[key].AsDouble;
            Assert.AreEqual(valueInGmlFile, actualVal);
        }
        
        [TestMethod]
        public void AsInt_Returns_GML_Value()
        {
            const string key = "intAttributeテスト";
            const int valueInGmlFile = 123;
            int actualVal = attrMap[key].AsInt;
            Assert.AreEqual(valueInGmlFile, actualVal);
        }

        [TestMethod]
        public void AsBool_Returns_GML_Value()
        {
            const string parentKey = "uro:buildingDetails";
            const string childKey = "energy:isHeated";
            const bool valueInGmlFile = true;
            var parentVal = attrMap[parentKey];
            var children = parentVal.AsAttrSet;
            bool actualVal = children[childKey].AsBool;
            Assert.AreEqual(valueInGmlFile, actualVal);
        }
    }
}
