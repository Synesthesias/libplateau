using System;
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

		/// <summary>
		/// 属性の set と get は同じになります。日本語にも対応します。
		/// </summary>
		[DataTestMethod]
		[DataRow("TestAttrName", "TestAttrValue")]
		[DataRow("日本語属性名", "日本語属性値")]
		public void Test_GetAttribute_Returns_Same_As_Set(string attrName, string attrVal) {
			plateauObject.SetAttribute(attrName, attrVal, AttributeType.String, true);
			string getResult = plateauObject.GetAttribute(attrName, 199, out int result);
			Assert.AreEqual(attrVal, getResult);
			Assert.AreEqual(0, result);
		}

		/// <summary>
		/// 受け取る値の最大サイズが足りない場合は result が -2 になります。
		/// </summary>
		[TestMethod]
		public void Test_GetAttribute_Results_Minus_2_If_Size_Small() {
			plateauObject.SetAttribute("foobarAttr", "LongAttrValue", AttributeType.String, true);
			string val = plateauObject.GetAttribute("foobarAttr", 1, out int result);
			Assert.AreEqual("", val);
			Assert.AreEqual(-2, result);
		}

		/// <summary>
		/// 受け取る値が存在しない場合は result が -1 になります。
		/// </summary>
		[TestMethod]
		public void Test_Attributes_Not_Found_Then_Result_Minus_1() {
			string val = plateauObject.GetAttribute("DummyFakeTestNotFound", 199, out int result);
			Assert.AreEqual("", val);
			Assert.AreEqual(-1, result);
		}

		/// <summary>
		/// GetAttributeList で Dictionary 形式で属性を取得できることを確認します。
		/// </summary>
		[TestMethod]
		public void Test_GetAttributeList() {
			plateauObject.SetAttribute("TestAttr", "TestValue", AttributeType.String, true);
			var attrs = plateauObject.GetAttributeList();
			Assert.AreEqual("TestValue", attrs["TestAttr"]);
			// 参考用に全属性を出力します。
			foreach (var attr in attrs) {
				Console.WriteLine($"{attr.Key}, {attr.Value}");
			}
		}

	}
}
