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
		/// ������ set �� get �͓����ɂȂ�܂��B���{��ɂ��Ή����܂��B
		/// </summary>
		[DataTestMethod]
		[DataRow("TestAttrName", "TestAttrValue")]
		[DataRow("���{�ꑮ����", "���{�ꑮ���l")]
		public void Test_GetAttribute_Returns_Same_As_Set(string attrName, string attrVal) {
			plateauObject.SetAttribute(attrName, attrVal, AttributeType.String, true);
			string getResult = plateauObject.GetAttribute(attrName, 199, out int result);
			Assert.AreEqual(attrVal, getResult);
			Assert.AreEqual(0, result);
		}

		/// <summary>
		/// �󂯎��l�̍ő�T�C�Y������Ȃ��ꍇ�� result �� -2 �ɂȂ�܂��B
		/// </summary>
		[TestMethod]
		public void Test_GetAttribute_Results_Minus_2_If_Size_Small() {
			plateauObject.SetAttribute("foobarAttr", "LongAttrValue", AttributeType.String, true);
			string val = plateauObject.GetAttribute("foobarAttr", 1, out int result);
			Assert.AreEqual("", val);
			Assert.AreEqual(-2, result);
		}

		/// <summary>
		/// �󂯎��l�����݂��Ȃ��ꍇ�� result �� -1 �ɂȂ�܂��B
		/// </summary>
		[TestMethod]
		public void Test_Attributes_Not_Found_Then_Result_Minus_1() {
			string val = plateauObject.GetAttribute("DummyFakeTestNotFound", 199, out int result);
			Assert.AreEqual("", val);
			Assert.AreEqual(-1, result);
		}

		/// <summary>
		/// GetAttributeList �� Dictionary �`���ő������擾�ł��邱�Ƃ��m�F���܂��B
		/// </summary>
		[TestMethod]
		public void Test_GetAttributeList() {
			plateauObject.SetAttribute("TestAttr", "TestValue", AttributeType.String, true);
			var attrs = plateauObject.GetAttributeList();
			Assert.AreEqual("TestValue", attrs["TestAttr"]);
			// �Q�l�p�ɑS�������o�͂��܂��B
			foreach (var attr in attrs) {
				Console.WriteLine($"{attr.Key}, {attr.Value}");
			}
		}

	}
}
