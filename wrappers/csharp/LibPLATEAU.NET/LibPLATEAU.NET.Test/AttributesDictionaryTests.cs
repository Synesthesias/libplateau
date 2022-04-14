using System;
using System.Diagnostics.Metrics;
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

        // [TestMethod]
        // public void Test_GetKeys()
        // {
        //     var keys = this.attrDict.GetKeys();
        //     foreach (var k in keys)
        //     {
        //         Console.WriteLine($"{k}\n");
        //     }
        // }

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
    }
}