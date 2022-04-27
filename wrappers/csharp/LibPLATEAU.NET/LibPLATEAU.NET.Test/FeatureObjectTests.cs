using System;
using System.Text;
using LibPLATEAU.NET.CityGML;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class FeatureObjectTests
    {
        private FeatureObject featureObject;

        public FeatureObjectTests()
        {
            var cityModel = TestGMLLoader.LoadTestGMLFile();
            this.featureObject = cityModel.RootCityObjects[0];
        }

        [TestMethod]
        public void GetEnvelop_Returns_Same_As_Set()
        {
            double[] value = { 1.1, 2.2, 3.3, 4.4, 5.5, 6.6 };
            this.featureObject.SetEnvelope(
                value[0], value[1], value[2],
                value[3], value[4], value[5]
            );

            double[] getVal = this.featureObject.Envelope;

            // 参考用に envelope の中身を出力します。
            StringBuilder sb = new();
            foreach (double d in getVal)
            {
                sb.Append($"{d} ,  ");
            }

            Console.WriteLine(sb.ToString());

            // GetしてSetと同じ値が得られるかチェックします。
            bool isSameVal = true;
            for (int i = 0; i < 6; i++)
            {
                isSameVal &= Math.Abs(getVal[i] - value[i]) < 0.0000001;
            }

            Assert.AreEqual(true, isSameVal);
        }
    }
}