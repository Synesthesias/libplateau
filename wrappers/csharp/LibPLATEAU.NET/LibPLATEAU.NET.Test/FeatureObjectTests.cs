using System;
using System.Text;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test {
    
    [TestClass]
    public class FeatureObjectTests {
        private FeatureObject featureObject;

        public FeatureObjectTests() {
            var cityModel = TestGMLLoader.LoadTestGMLFile();
            this.featureObject = cityModel.RootCityObjects[0];
        }
        
        [TestMethod]
        public void Test_GetEnvelop() {
            double[] envelope = featureObject.GetEnvelope();
            
            // 参考用に envelope の中身を出力します。
            StringBuilder sb = new();
            foreach (double d in envelope) {
                sb.Append($"{d} ,  ");
            }
            Console.WriteLine(sb.ToString());

            bool doContainNaN = false;
            foreach (double d in envelope) {
                doContainNaN |= Double.IsNaN(d);
            }
            Assert.AreEqual(false, doContainNaN);
        }
    }
}