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

		[DataTestMethod]
		[DataRow("TestAttrName", "TestAttrValue")]
		[DataRow("“ú–{Œê‘®«–¼", "“ú–{Œê‘®«’l")]
		public void Test_GetAttribute_Returns_Same_As_Set(string attrName, string attrVal) {
			plateauObject.SetAttribute(attrName, attrVal, AttributeType.String, true);
			string getResult = plateauObject.GetAttribute(attrName, 199, out int result);
			Assert.AreEqual(attrVal, getResult);
			Assert.AreEqual(0, result);
		}

		[TestMethod]
		public void Test_GetAttribute_Results_Non_Zero_If_Size_Small() {
			plateauObject.SetAttribute("foobarAttr", "LongAttrValue", AttributeType.String, true);
			plateauObject.GetAttribute("foobarAttr", 1, out int result);
			Assert.AreNotEqual(0, result);
		}

		// [TestMethod]
		// public void Test_Attributes_Not_Found() {
		// 	plateauObject.GetAttribute("DummyFakeTestNotFound", 199, out int result);
		// 	Assert.AreNotEqual(0, result);
		// }

	}
}
