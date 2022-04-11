using System.Reflection;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace LibPLATEAU.NET.Test {
	[TestClass]
	public class ObjectTests {
		private Object plateauObject;
		public ObjectTests() {
			var cityModel = TestGMLLoader.LoadTestGMLFile();
			plateauObject = cityModel.RootCityObjects[0];
		}

		[DataTestMethod()]
		[DataRow("TestAttrName", "TestAttrValue")]
		[DataRow("a", "b")]
		// [DataRow("日本語属性名", "日本語属性値")]
		// [DataRow("特殊文字属性名🎵🙂", "特殊文字属性値🎵🙂")]
		public void Test_GetAttribute_Returns_Same_As_Set(string attrName, string attrVal) {
			plateauObject.SetAttribute(attrName, attrVal, AttributeType.String, true);
			string getResult = plateauObject.GetAttribute(attrName, 199);
			Assert.AreEqual(attrVal, getResult);
		}
		
		
	}
}
