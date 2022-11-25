using System;
using System.Collections.Generic;
using System.Linq;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;

namespace PLATEAU.Test.CityGML
{
    [TestClass]
    public class AttributesMapTests
    {
        private static CityModel cityModel;
        private static AttributesMap attrMap;
        
        [ClassInitialize]
        public static void ClassInitialize(TestContext _)
        {
            cityModel = TestUtil.LoadTestGMLFile(TestUtil.GmlFileCase.Simple);
            var cityObject = cityModel.RootCityObjects
                .SelectMany(co => co.CityObjectDescendantsDFS)
                .First(co => co.ID == "BLD_ae7f1207-dd09-45bc-8881-40533f3700bb");
            attrMap = cityObject.AttributesMap;
        }

        [ClassCleanup]
        public static void ClassCleanup()
        {
            cityModel.Dispose();
        }
        
        [TestMethod]
        public void Keys_Length_Equals_Map_Count()
        {
            int keysLength = attrMap.Keys.Count();
            int mapCount = attrMap.Count;
            Assert.AreEqual(mapCount, keysLength);
        }

        [TestMethod]
        public void Keys_Contains_GML_AttrName()
        {
            const string oneOfAttrNameInGmlFile = "多摩水系多摩川、浅川、大栗川洪水浸水想定区域（想定最大規模）";
            var keys = attrMap.Keys;
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
            Console.WriteLine($"Count = {attrMap.Count}");
            Assert.IsTrue(attrMap.Count > 0);
        }

        [TestMethod]
        public void TryGetValue_When_NotFound_Returns_False_And_Value_Null()
        {
            bool result = attrMap.TryGetValue("DummyNotFound", out AttributeValue value);
            Assert.AreEqual(false, result);
            Assert.IsNull(value);
            
        }

        [TestMethod]
        public void TryGetValue_When_Found_Returns_True_And_Value_Valid()
        {
            const string key = "建物ID";
            const string valueInGmlFile = "13111-bldg-98";
            bool result = attrMap.TryGetValue(key, out AttributeValue value);
            Assert.AreEqual(true, result);
            Assert.IsNotNull(value);
            string actualStr = value.AsString;
            Assert.AreEqual(valueInGmlFile, actualStr);
        }
        
        [TestMethod]
        public void ContainsKey_Returns_False_On_NotFound()
        {
            bool result = attrMap.ContainsKey("DummyNotFound");
            Assert.AreEqual(false, result);
            
        }

        [TestMethod]
        public void GetValueOrNull_Returns_Value_If_Found()
        {
            var val = attrMap.GetValueOrNull("建物ID");
            Assert.AreEqual("13111-bldg-98", val.AsString);
        }

        [TestMethod]
        public void GetValueOrNull_Returns_Null_If_Not_Found()
        {
            var val = attrMap.GetValueOrNull("DummyNotFound");
            Assert.IsNull(val);
        }

        [TestMethod]
        public void ContainsKey_Returns_True_On_Found()
        {
            bool result = attrMap.ContainsKey("建物ID");
            Assert.AreEqual(true, result);
        }

        [TestMethod]
        public void Values_Count_Equals_Map_Count()
        {
            int valuesCount = attrMap.Values.Count();
            int mapCount = attrMap.Count;
            Assert.AreEqual(mapCount, valuesCount);
        }

        [TestMethod]
        public void Values_Contains_GML_Value()
        {
            const string oneOfAttrValueInGmlFile = "13111-bldg-98";
            bool doContainValue = attrMap.Values.Select(v => v.AsString).Contains(oneOfAttrValueInGmlFile);
            Assert.AreEqual(true, doContainValue);
        }

        [TestMethod]
        public void Map_Can_Be_Iterated_By_Foreach()
        {
            int iterateCount = 0;
            foreach (var p in attrMap)
            {
                Console.WriteLine($"{p.Key}, {p.Value.AsString}");
                iterateCount++;
            }
            Assert.AreEqual(attrMap.Count, iterateCount);
        }

        [TestMethod]
        [ExpectedException(typeof(KeyNotFoundException))]
        public void Throws_KeyNotFoundException_When_Key_Not_Found()
        {
            var _ = attrMap["DummyNotFound"];
        }

        [TestMethod]
        public void Do_CachedKeys_Working()
        {
            // 1回目
            foreach (var attr in attrMap)
            {
                if (attr.Key == null)
                {
                    Assert.IsTrue(false);
                }
            }
            // 2回目
            foreach (var attr in attrMap)
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
            foreach (var pair in attrMap)
            {
                if (string.IsNullOrEmpty(pair.Key))
                {
                    emptyCount++;
                }
                Console.WriteLine($"'{pair.Key}', '{pair.Value.AsString}'");
            }
            Assert.AreEqual(0, emptyCount);
        }

        [TestMethod]
        public void ToString_Returns_Not_Empty()
        {
            string str = attrMap.ToString();
            Console.WriteLine(str);
            Assert.IsFalse(string.IsNullOrEmpty(str));
        }
    }
}
