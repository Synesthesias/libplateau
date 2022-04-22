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
        
        /// <summary> テストの前準備です。 </summary>
        public AttributesDictionaryTests()
        {
            var cityModel = TestGMLLoader.LoadTestGMLFile();
            var cityObject = cityModel.RootCityObjects[0];
            this.attrDict = cityObject.AttributesDictionary;
        }
        
        
        [TestMethod]
        public void Keys_Length_Equals_Dictionary_Count()
        {
            int keysLength = this.attrDict.Keys.Count();
            int dictionaryCount = this.attrDict.Count;
            Assert.AreEqual(dictionaryCount, keysLength);
        }

        [TestMethod]
        public void Keys_Contains_GML_AttrName()
        {
            const string oneOfAttrNameInGmlFile = "多摩水系多摩川、浅川、大栗川洪水浸水想定区域（想定最大規模）";
            var keys = this.attrDict.Keys;
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
            Console.WriteLine($"Count = {this.attrDict.Count}");
            Assert.IsTrue(this.attrDict.Count > 0);
        }
        


        [TestMethod]
        public void TryGetValue_When_NotFound_Returns_False_And_Value_Null()
        {
            bool result = this.attrDict.TryGetValue("DummyNotFound", out AttributeValue value);
            Assert.AreEqual(false, result);
            Assert.IsNull(value);
            
        }

        [TestMethod]
        public void TryGetValue_When_Found_Returns_True_And_Value_Valid()
        {
            const string key = "建物ID";
            const string valueInGmlFile = "13111-bldg-147301";
            bool result = this.attrDict.TryGetValue(key, out AttributeValue value);
            string actualStr = value.AsString;
            Assert.AreEqual(true, result);
            Assert.AreEqual(valueInGmlFile, actualStr);
        }
        
        [TestMethod]
        public void ContainsKey_Returns_False_On_NotFound()
        {
            bool result = this.attrDict.ContainsKey("DummyNotFound");
            Assert.AreEqual(false, result);
            
        }

        [TestMethod]
        public void ContainsKey_Returns_True_On_Found()
        {
            bool result = this.attrDict.ContainsKey("建物ID");
            Assert.AreEqual(true, result);
        }
        
        
        [TestMethod]
        public void Values_Count_Equals_Dictionary_Count()
        {
            int valuesCount = this.attrDict.Values.Count();
            int dictionaryCount = this.attrDict.Count;
            Assert.AreEqual(dictionaryCount, valuesCount);
        }

        [TestMethod]
        public void Values_Contains_GML_Value()
        {
            const string oneOfAttrValueInGmlFile = "13111-bldg-147301";
            bool doContainValue = this.attrDict.Values.Select(v => v.AsString).Contains(oneOfAttrValueInGmlFile);
            Assert.AreEqual(true, doContainValue);
        }
        
        
        [TestMethod]
        public void Dictionary_Can_Be_Iterated_By_Foreach()
        {
            int iterateCount = 0;
            foreach (var p in this.attrDict)
            {
                Console.WriteLine($"{p.Key}, {p.Value.AsString}");
                iterateCount++;
            }
            Assert.AreEqual(this.attrDict.Count, iterateCount);
        }
        

        [TestMethod]
        [ExpectedException(typeof(KeyNotFoundException))]
        public void Throws_KeyNotFoundException_When_Key_Not_Found()
        {
            var _ = this.attrDict["DummyNotFound"];
        }

    }
}