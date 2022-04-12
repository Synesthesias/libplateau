using System;
using System.Reflection;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using NuGet.Frameworks;

namespace LibPLATEAU.NET.Test {
	[TestClass]
	public class ObjectTests {
		private Object plateauObject;
		public ObjectTests() {
			var cityModel = TestGMLLoader.LoadTestGMLFile();
			plateauObject = cityModel.RootCityObjects[0];
		}

		/// <summary>
		/// 属性の set と get は同じになります。日本語にも対応します。
		/// </summary>
		[DataTestMethod]
		[DataRow("TestAttrName", "TestAttrValue")]
		[DataRow("日本語属性名", "日本語属性値")]
		public void Test_GetAttribute_Returns_Same_As_Set(string attrName, string attrVal) {
			plateauObject.SetAttribute(attrName, attrVal, true, AttributeType.String);
			string getResult = plateauObject.GetAttribute(attrName, 199, out APIResult result);
			Assert.AreEqual(attrVal, getResult);
			Assert.AreEqual(APIResult.Success, result);
		}

		/// <summary>
		/// 受け取る値の最大サイズが足りない場合は result が ErrorLackOfBufferSize になります。
		/// </summary>
		[TestMethod]
		public void Test_GetAttribute_Results_Minus_2_If_Size_Small() {
			plateauObject.SetAttribute("foobarAttr", "LongAttrValue", true, AttributeType.String);
			string val = plateauObject.GetAttribute("foobarAttr", 1, out APIResult result);
			Assert.AreEqual("", val);
			Assert.AreEqual(APIResult.ErrorLackOfBufferSize, result);
		}

		/// <summary>
		/// 受け取る値が存在しない場合は result が ErrorValueNotFound になります。
		/// </summary>
		[TestMethod]
		public void Test_GetAttribute_Not_Found_Then_Result_Minus_1() {
			string val = plateauObject.GetAttribute("DummyFakeTestNotFound", 199, out APIResult result);
			Assert.AreEqual("", val);
			Assert.AreEqual(APIResult.ErrorValueNotFound, result);
		}

		/// <summary>
		/// GetAttributeList で Dictionary 形式で属性を取得できることを確認します。
		/// </summary>
		[TestMethod]
		public void Test_GetAttributeList() {
			plateauObject.SetAttribute("TestAttr", "TestValue", true, AttributeType.String);
			var attrs = plateauObject.GetAttributeList(9999, out APIResult result);
			Assert.AreEqual("TestValue", attrs["TestAttr"]);
			Assert.AreEqual(APIResult.Success, result);
			// 参考用に全属性を出力します。
			foreach (var attr in attrs) {
				Console.WriteLine($"{attr.Key}, {attr.Value}");
			}
		}

		[TestMethod]
		public void Test_SetAttribute_Override() {
			plateauObject.SetAttribute("TestOverride", "1", true);
			plateauObject.SetAttribute("TestOverride", "2", false);
			Assert.AreEqual("1", plateauObject.GetAttribute("TestOverride", 99, out APIResult _));
			plateauObject.SetAttribute("TestOverride", "2", true);
			Assert.AreEqual( "2", plateauObject.GetAttribute("TestOverride", 99, out APIResult _));
		}

		/// <summary>
		/// GetAttributeListでバッファーが足りないときは result が ErrorLackOfBufferSize になります。
		/// </summary>
		[TestMethod]
		public void Test_GetAttributeListTest_LackOfBuffer() {
			plateauObject.SetAttribute("TestAttr", "TestValue", true);
			plateauObject.GetAttributeList(1, out APIResult result);
			Assert.AreEqual(APIResult.ErrorLackOfBufferSize, result);
		}

	}
}
