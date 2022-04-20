using System;
using System.Collections.Generic;
using System.Linq;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

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
            var keys = this.attrDict.Keys.ToArray();
            bool doContainKey = false;
            foreach (var k in keys)
            {
                Console.WriteLine($"{k}\n");
                if (k == "多摩水系多摩川、浅川、大栗川洪水浸水想定区域（想定最大規模）")
                {
                    doContainKey = true;
                }
            }
            Assert.IsTrue(doContainKey);
            Assert.AreEqual(this.attrDict.Count, keys.Length);
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


        // [TestMethod]
        // public void Test_TryGetValue()
        // {
        //     bool result = this.attrDict.TryGetValue("DummyNotFound", out AttributeValue value);
        //     Assert.AreEqual(false, result);
        //     Assert.IsNull(value);
        //     bool result2 = this.attrDict.TryGetValue("建物ID", out AttributeValue value2);
        //     Assert.AreEqual(true, result2);
        //     Assert.AreEqual("13111-bldg-147301", value2.AsString);
        // }
        //
        // [TestMethod]
        // public void Test_ContainsKey()
        // {
        //     bool result = this.attrDict.ContainsKey("DummyNotFound");
        //     Assert.AreEqual(false, result);
        //     bool result2 = this.attrDict.ContainsKey("建物ID");
        //     Assert.AreEqual(true, result2);
        // }
        //
        // [TestMethod]
        // public void Test_Values()
        // {
        //     var values = this.attrDict.Values.ToArray();
        //     foreach (var val in values)
        //     {
        //         Console.WriteLine(val.AsString);
        //     }
        //     Assert.AreEqual(true, values.Select(v => v.AsString).Contains("13111-bldg-147301"));
        //     Assert.AreEqual(this.attrDict.Count, values.Length);
        // }
        //
        // /// <summary>
        // /// テスト内容 : foreachを使って Dictionary の全要素を回すことができます。
        // /// </summary>
        // [TestMethod]
        // public void Test_Enumerable()
        // {
        //     int count = 0;
        //     foreach (var p in this.attrDict)
        //     {
        //         Console.WriteLine($"{p.Key}, {p.Value.AsString}");
        //         count++;
        //     }
        //     Assert.AreEqual(this.attrDict.Count, count);
        // }
        //
        // /// <summary>
        // /// テスト内容 : 存在しないキーを参照しようとしたとき、KeyNotFoundException を投げます。
        // /// </summary>
        // [TestMethod]
        // [ExpectedException(typeof(KeyNotFoundException))]
        // public void Test_MissingKey()
        // {
        //     var _ = this.attrDict["DummyNotFound"];
        // }
        //
        // [TestMethod]
        // public void Test_AsDouble()
        // {
        //     Assert.AreEqual(123.456, this.attrDict["doubleAttributeテスト"].AsDouble);
        // }
        //
        // [TestMethod]
        // public void Test_AsInt()
        // {
        //     Assert.AreEqual(123, this.attrDict["intAttributeテスト"].AsInt);
        // }
        
    }
}