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
			plateauObject.SetAttribute(attrName, attrVal, AttributeType.String);
			string getResult = plateauObject.GetAttribute(attrName, 199, out APIResult result);
			Assert.AreEqual(attrVal, getResult);
			Assert.AreEqual(APIResult.Success, result);
		}

		/// <summary>
		/// �󂯎��l�̍ő�T�C�Y������Ȃ��ꍇ�� result �� ErrorLackOfBufferSize �ɂȂ�܂��B
		/// </summary>
		[TestMethod]
		public void Test_GetAttribute_Results_Minus_2_If_Size_Small() {
			plateauObject.SetAttribute("foobarAttr", "LongAttrValue");
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
		/// GetAttributes �� Dictionary �`���ő������擾�ł��邱�Ƃ��m�F���܂��B
		/// </summary>
		[TestMethod]
		public void Test_GetAttributeList() {
			plateauObject.SetAttribute("TestAttr", "TestValue");
			var attrs = plateauObject.GetAttributes(9999, out APIResult result);
			
			// �Q�l�p�ɑS�������o�͂��܂��B
			foreach (var attr in attrs) {
				Console.WriteLine($"{attr.Key}, {attr.Value}");
			}
			
			Assert.AreEqual("TestValue", attrs["TestAttr"]);
			Assert.AreEqual(APIResult.Success, result);
		}

		/// <summary>
		/// GetAttributes�� separator �� key, value �Ƃ��Ԃ����Ƃ���
		/// result �� ErrorInvalidArgument �ɂȂ�܂��B
		/// </summary>
		[TestMethod]
		public void Test_GetAttributes_Result_Error_When_Separator_Invalid() {
			plateauObject.SetAttribute("Test", "Test\nSeparator");
			plateauObject.GetAttributes(999, out APIResult result, "\n");
			Assert.AreEqual(APIResult.ErrorInvalidArgument, result);
		}

		/// <summary>
		/// GetAttributeList�Ńo�b�t�@�[������Ȃ��Ƃ��� result �� ErrorLackOfBufferSize �ɂȂ�܂��B
		/// </summary>
		[TestMethod]
		public void Test_GetAttributes_LackOfBuffer() {
			plateauObject.SetAttribute("TestAttr", "TestValue");
			plateauObject.GetAttributes(1, out APIResult result);
			Assert.AreEqual(APIResult.ErrorLackOfBufferSize, result);
		}

	}
}
