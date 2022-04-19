using System;
using System.Diagnostics.Metrics;
using System.Linq;
using System.Reflection;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using NuGet.Frameworks;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class AttributesDictionaryTests
    {
        private AttributesDictionary attrDict;
        public AttributesDictionaryTests()
        {
            var cityModel = TestGMLLoader.LoadTestGMLFile();
            var cityObject = cityModel.RootCityObjects[0];
            this.attrDict = cityObject.AttributesDictionary;
        }

        [TestMethod]
        public void Test_GetKeys()
        {
            var keys = this.attrDict.GetKeys();
            foreach (var k in keys)
            {
                Console.WriteLine($"{k}\n");
            }
            Assert.IsTrue(keys.Contains("多摩水系多摩川、浅川、大栗川洪水浸水想定区域（想定最大規模）"));
        }

        [TestMethod]
        public void Test_GetKeySizes()
        {
            var sizes = this.attrDict.GetKeySizes();
            bool isAllSizePositive = true;
            Console.Write("Sizes : ");
            foreach (var s in sizes)
            {
                Console.Write($"{s} , ");
                isAllSizePositive &= s > 0;
            }
            Console.Write("\n");
            Assert.IsTrue(isAllSizePositive);
        }

        [TestMethod]
        public void Test_Count()
        {
            Console.WriteLine($"Count = {this.attrDict.Count}");
            Assert.IsTrue(this.attrDict.Count > 0);
        }

        [TestMethod]
        public void Test_GetAttributeValueAsString()
        {
            var val1 = this.attrDict.GetValue("建物ID");
            Assert.AreEqual("13111-bldg-147301", val1.AsString);
            var val2 = this.attrDict.GetValue("bldg:measuredheight");
            
            Assert.AreEqual("2.8（テスト）", val2.AsString);
        }

        [TestMethod]
        public void Test_GetAttributeType()
        {
            var val1 = this.attrDict.GetValue("建物ID");
            var val2 = this.attrDict.GetValue("bldg:measuredheight");
            var val3 = this.attrDict.GetValue("多摩水系多摩川、浅川、大栗川洪水浸水想定区域（想定最大規模）");
            Assert.AreEqual(AttributeType.String, val1.Type);
            Assert.AreEqual(AttributeType.Double, val2.Type);
            Assert.AreEqual(AttributeType.AttributeSet, val3.Type);
        }

        [TestMethod]
        public void Test_ChildAttributesMap()
        {
            var parent = this.attrDict.GetValue("多摩水系多摩川、浅川、大栗川洪水浸水想定区域（想定最大規模）");
            var children = parent.ChildAttrs;
            // 参考用にキーの一覧を表示します。
            Console.WriteLine($"parent: {this.attrDict}");
            Console.WriteLine($"children: {children}");

            var cval = children.GetValue("浸水深");
            Console.WriteLine($"child value => {cval.AsString}");
            // Assert.Equals(cval.StringValue)
        }
        
    }
}