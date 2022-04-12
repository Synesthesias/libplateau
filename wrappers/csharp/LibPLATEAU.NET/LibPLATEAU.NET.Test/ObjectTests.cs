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
		/// ������ set �� get �͓����ɂȂ�܂��B���{��ɂ��Ή����܂��B
		/// </summary>
		[DataTestMethod]
		[DataRow("TestAttrName", "TestAttrValue")]
		[DataRow("���{�ꑮ����", "���{�ꑮ���l")]
		public void Test_GetAttribute_Returns_Same_As_Set(string attrName, string attrVal) {
			plateauObject.SetAttribute(attrName, attrVal, true, AttributeType.String);
			string getResult = plateauObject.GetAttribute(attrName, 199, out APIResult result);
			Assert.AreEqual(attrVal, getResult);
			Assert.AreEqual(APIResult.Success, result);
		}

		/// <summary>
		/// �󂯎��l�̍ő�T�C�Y������Ȃ��ꍇ�� result �� ErrorLackOfBufferSize �ɂȂ�܂��B
		/// </summary>
		[TestMethod]
		public void Test_GetAttribute_Results_Minus_2_If_Size_Small() {
			plateauObject.SetAttribute("foobarAttr", "LongAttrValue", true, AttributeType.String);
			string val = plateauObject.GetAttribute("foobarAttr", 1, out APIResult result);
			Assert.AreEqual("", val);
			Assert.AreEqual(APIResult.ErrorLackOfBufferSize, result);
		}

		/// <summary>
		/// �󂯎��l�����݂��Ȃ��ꍇ�� result �� ErrorValueNotFound �ɂȂ�܂��B
		/// </summary>
		[TestMethod]
		public void Test_GetAttribute_Not_Found_Then_Result_Minus_1() {
			string val = plateauObject.GetAttribute("DummyFakeTestNotFound", 199, out APIResult result);
			Assert.AreEqual("", val);
			Assert.AreEqual(APIResult.ErrorValueNotFound, result);
		}

		/// <summary>
		/// GetAttributeList �� Dictionary �`���ő������擾�ł��邱�Ƃ��m�F���܂��B
		/// </summary>
		[TestMethod]
		public void Test_GetAttributeList() {
			plateauObject.SetAttribute("TestAttr", "TestValue", true, AttributeType.String);
			var attrs = plateauObject.GetAttributeList(9999, out APIResult result);
			Assert.AreEqual("TestValue", attrs["TestAttr"]);
			Assert.AreEqual(APIResult.Success, result);
			// �Q�l�p�ɑS�������o�͂��܂��B
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
		/// GetAttributeList�Ńo�b�t�@�[������Ȃ��Ƃ��� result �� ErrorLackOfBufferSize �ɂȂ�܂��B
		/// </summary>
		[TestMethod]
		public void Test_GetAttributeListTest_LackOfBuffer() {
			plateauObject.SetAttribute("TestAttr", "TestValue", true);
			plateauObject.GetAttributeList(1, out APIResult result);
			Assert.AreEqual(APIResult.ErrorLackOfBufferSize, result);
		}

	}
}
