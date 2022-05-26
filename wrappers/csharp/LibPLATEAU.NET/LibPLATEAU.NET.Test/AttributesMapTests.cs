using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.CompilerServices;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class AttributesMapTests
    {
        private AttributesMap attrMap;
        
        /// <summary> テストの前準備です。 </summary>
        public AttributesMapTests()
        {
            var cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var cityObject = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .First(co => co.ID == "BLD_ae7f1207-dd09-45bc-8881-40533f3700bb");
            this.attrMap = cityObject.AttributesMap;
        }
        
        [TestMethod]
        public void Keys_Length_Equals_Map_Count()
        {
            int keysLength = this.attrMap.Keys.Count();
            int mapCount = this.attrMap.Count;
            Assert.AreEqual(mapCount, keysLength);
        }

        [TestMethod]
        public void Keys_Contains_GML_AttrName()
        {
            const string oneOfAttrNameInGmlFile = "多摩水系多摩川、浅川、大栗川洪水浸水想定区域（想定最大規模）";
            var keys = this.attrMap.Keys;
            bool doContainKey = false;
            foreach (var k in keys)
            {
                Console.WriteLine($"{k}\n");
                if (k == oneOfAttrNameInGmlFile)
                {
                    doContainKey = true;
                }
            }
            Assert.IsTrue(doContainKey);
        }
        
        [TestMethod]
        public void Count_Returns_Positive_Value()
        {
            Console.WriteLine($"Count = {this.attrMap.Count}");
            Assert.IsTrue(this.attrMap.Count > 0);
        }

        [TestMethod]
        public void TryGetValue_When_NotFound_Returns_False_And_Value_Null()
        {
            bool result = this.attrMap.TryGetValue("DummyNotFound", out AttributeValue value);
            Assert.AreEqual(false, result);
            Assert.IsNull(value);
            
        }

        [TestMethod]
        public void TryGetValue_When_Found_Returns_True_And_Value_Valid()
        {
            const string key = "建物ID";
            const string valueInGmlFile = "13111-bldg-98";
            bool result = this.attrMap.TryGetValue(key, out AttributeValue value);
            string actualStr = value.AsString;
            Assert.AreEqual(true, result);
            Assert.AreEqual(valueInGmlFile, actualStr);
        }
        
        [TestMethod]
        public void ContainsKey_Returns_False_On_NotFound()
        {
            bool result = this.attrMap.ContainsKey("DummyNotFound");
            Assert.AreEqual(false, result);
            
        }

        [TestMethod]
        public void ContainsKey_Returns_True_On_Found()
        {
            bool result = this.attrMap.ContainsKey("建物ID");
            Assert.AreEqual(true, result);
        }

        [TestMethod]
        public void Values_Count_Equals_Map_Count()
        {
            int valuesCount = this.attrMap.Values.Count();
            int mapCount = this.attrMap.Count;
            Assert.AreEqual(mapCount, valuesCount);
        }

        [TestMethod]
        public void Values_Contains_GML_Value()
        {
            const string oneOfAttrValueInGmlFile = "13111-bldg-98";
            bool doContainValue = this.attrMap.Values.Select(v => v.AsString).Contains(oneOfAttrValueInGmlFile);
            Assert.AreEqual(true, doContainValue);
        }

        [TestMethod]
        public void Map_Can_Be_Iterated_By_Foreach()
        {
            int iterateCount = 0;
            foreach (var p in this.attrMap)
            {
                Console.WriteLine($"{p.Key}, {p.Value.AsString}");
                iterateCount++;
            }
            Assert.AreEqual(this.attrMap.Count, iterateCount);
        }

        [TestMethod]
        [ExpectedException(typeof(KeyNotFoundException))]
        public void Throws_KeyNotFoundException_When_Key_Not_Found()
        {
            var _ = this.attrMap["DummyNotFound"];
        }

        [TestMethod]
        public void Do_CachedKeys_Working()
        {
            // 1回目
            foreach (var attr in this.attrMap)
            {
                if (attr.Key == null)
                {
                    Assert.IsTrue(false);
                }
            }
            // 2回目
            foreach (var attr in this.attrMap)
            {
                if (attr.Key == null)
                {
                    Assert.IsTrue(false);
                }
            }
        }

        [TestMethod]
        public void Keys_Not_Contain_Empty()
        {
            int emptyCount = 0;
            foreach (var pair in this.attrMap)
            {
                if (string.IsNullOrEmpty(pair.Key))
                {
                    emptyCount++;
                }
                Console.WriteLine($"'{pair.Key}', '{pair.Value.AsString}'");
            }
            Assert.AreEqual(0, emptyCount);
        }

    }
}