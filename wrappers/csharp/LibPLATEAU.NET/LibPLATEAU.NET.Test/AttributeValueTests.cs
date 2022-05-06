using System;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class AttributeValueTests
    {
        private AttributesMap attrMap;
        
        /// <summary> テストの前準備です。 </summary>
        public AttributeValueTests()
        {
            var cityModel = TestGMLLoader.LoadTestGMLFile(TestGMLLoader.GmlFileCase.Simple);
            var cityObject = cityModel.RootCityObjects[0];
            this.attrMap = cityObject.AttributesMap;
        }
        
        
        [DataTestMethod]
        [DataRow("建物ID", "13111-bldg-147301")]
        [DataRow("bldg:measuredheight", "2.8（テスト）")]
        public void AsString_Returns_GML_Value(string key, string valueInGmlFile)
        {
            var actualVal = this.attrMap[key].AsString;
            Assert.AreEqual(valueInGmlFile, actualVal);
        }
        
        [DataTestMethod]
        [DataRow("建物ID", AttributeType.String)]
        [DataRow("bldg:measuredheight", AttributeType.Double)]
        [DataRow("多摩水系多摩川、浅川、大栗川洪水浸水想定区域（想定最大規模）", AttributeType.AttributeSet)]
        public void Type_Returns_GML_Type(string key, AttributeType typeInGmlFile)
        {
            AttributeType actualType = this.attrMap[key].Type;
            Assert.AreEqual(typeInGmlFile, actualType);
        }
        
        [TestMethod]
        public void AsAttrSet_Returns_Child_Attrs_In_GML()
        {
            const string parentKey = "多摩水系多摩川、浅川、大栗川洪水浸水想定区域（想定最大規模）";
            const string childKey = "規模";
            const string childValInGmlFile = "L2";
            var parentVal = this.attrMap[parentKey];
            var children = parentVal.AsAttrSet;
            // 参考用にキーの一覧を表示します。
            Console.WriteLine($"parent: {this.attrMap}");
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
            double actualVal = this.attrMap[key].AsDouble;
            Assert.AreEqual(valueInGmlFile, actualVal);
        }
        
        [TestMethod]
        public void AsInt_Returns_GML_Value()
        {
            const string key = "intAttributeテスト";
            const int valueInGmlFile = 123;
            int actualVal = this.attrMap[key].AsInt;
            Assert.AreEqual(valueInGmlFile, actualVal);
        }
    }
}