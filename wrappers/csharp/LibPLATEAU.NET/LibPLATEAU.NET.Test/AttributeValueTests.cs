using System;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class AttributeValueTests
    {
        private AttributesDictionary attrDict;
        
        public AttributeValueTests()
        {
            var cityModel = TestGMLLoader.LoadTestGMLFile();
            var cityObject = cityModel.RootCityObjects[0];
            this.attrDict = cityObject.AttributesDictionary;
        }
        
        [TestMethod]
        public void Test_GetAttributeValueAsString()
        {
            var val1 = this.attrDict["建物ID"];
            Assert.AreEqual("13111-bldg-147301", val1.AsString);
            var val2 = this.attrDict["bldg:measuredheight"];
            
            Assert.AreEqual("2.8（テスト）", val2.AsString);
        }
        
        [TestMethod]
        public void Test_GetAttributeType()
        {
            var val1 = this.attrDict["建物ID"];
            var val2 = this.attrDict["bldg:measuredheight"];
            var val3 = this.attrDict["多摩水系多摩川、浅川、大栗川洪水浸水想定区域（想定最大規模）"];
            Assert.AreEqual(AttributeType.String, val1.Type);
            Assert.AreEqual(AttributeType.Double, val2.Type);
            Assert.AreEqual(AttributeType.AttributeSet, val3.Type);
        }
        
        [TestMethod]
        public void Test_ChildAttributesMap()
        {
            var parent = this.attrDict["多摩水系多摩川、浅川、大栗川洪水浸水想定区域（想定最大規模）"];
            var children = parent.AsAttrSet;
            // 参考用にキーの一覧を表示します。
            Console.WriteLine($"parent: {this.attrDict}");
            Console.WriteLine($"children: {children}");
        
            var cval = children["規模"];
            Console.WriteLine($"child value => {cval.AsString}");
            Assert.AreEqual(cval.AsString, "L2");
        }
        
        [TestMethod]
        public void Test_AsDouble()
        {
            Assert.AreEqual(123.456, this.attrDict["doubleAttributeテスト"].AsDouble);
        }
        
        [TestMethod]
        public void Test_AsInt()
        {
            Assert.AreEqual(123, this.attrDict["intAttributeテスト"].AsInt);
        }
    }
}