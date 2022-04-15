using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using Object = LibPLATEAU.NET.CityGML.Object;

namespace LibPLATEAU.NET.Test
{
    [TestClass]
    public class ObjectTests
    {
        private Object plateauObject;

        public ObjectTests()
        {
            var cityModel = TestGMLLoader.LoadTestGMLFile();
            plateauObject = cityModel.RootCityObjects[0];
        }

        // /// <summary>
        // /// ?e?X?g??e: ?????? set ?? get ????????????B???{????????????B
        // /// </summary>
        // [DataTestMethod]
        // [DataRow("TestAttrName", "TestAttrValue")]
        // [DataRow("???{??????", "???{?????l")]
        // public void Test_GetAttribute_Returns_Same_As_Set(string attrName, string attrVal)
        // {
        //     plateauObject.SetAttribute(attrName, attrVal);
        //     string getVal = plateauObject.GetAttribute(attrName, 199, out APIResult result);
        //     Assert.AreEqual(attrVal, getVal);
        //     Assert.AreEqual(APIResult.Success, result);
        // }
        //
        // /// <summary>
        // /// ?e?X?g??e: ?????????m??T?C?Y???????????? result ?? ErrorLackOfBufferSize ???????B
        // /// </summary>
        // [TestMethod]
        // public void Test_GetAttribute_Results_Minus_2_If_Size_Small()
        // {
        //     plateauObject.SetAttribute("foobarAttr", "LongAttrValue");
        //     string val = plateauObject.GetAttribute("foobarAttr", 1, out APIResult result);
        //     Assert.AreEqual("", val);
        //     Assert.AreEqual(APIResult.ErrorLackOfBufferSize, result);
        // }
        //
        // /// <summary>
        // /// ?e?X?g??e: GetAttribute ?????l?????????????? result ?? ErrorValueNotFound ???????B
        // /// </summary>
        // [TestMethod]
        // public void Test_GetAttribute_Not_Found_Then_Result_Minus_1()
        // {
        //     string val = plateauObject.GetAttribute("DummyFakeTestNotFound", 199, out APIResult result);
        //     Assert.AreEqual("", val);
        //     Assert.AreEqual(APIResult.ErrorValueNotFound, result);
        // }
        //
        //
        // /// <summary>
        // /// ?e?X?g??e : SetAttribute ?? overwrite ?? false?????A?????????????????????B
        // /// true ??????????????B
        // /// </summary>
        // [TestMethod]
        // public void Test_SetAttribute_Overwrite()
        // {
        //     plateauObject.SetAttribute("TestOverride", "1");
        //     plateauObject.SetAttribute("TestOverride", "2", false);
        //     Assert.AreEqual("1", plateauObject.GetAttribute("TestOverride", 99, out APIResult _));
        //     plateauObject.SetAttribute("TestOverride", "2");
        //     Assert.AreEqual("2", plateauObject.GetAttribute("TestOverride", 99, out APIResult _));
        // }
        //
        // /// <summary>
        // /// ?e?X?g??e: GetAttributes ?? Dictionary ?`?????????èÔ???????B
        // /// </summary>
        // [TestMethod]
        // public void Test_GetAttributeList()
        // {
        //     plateauObject.SetAttribute("TestAttr", "TestValue");
        //     var attrs = plateauObject.GetAttributes(9999, out APIResult result);
        //
        //     // ?Q?l?p??S??????o???????B
        //     foreach (var attr in attrs)
        //     {
        //         Console.WriteLine($"{attr.Key}, {attr.Value}");
        //     }
        //
        //     Assert.AreEqual("TestValue", attrs["TestAttr"]);
        //     Assert.AreEqual(APIResult.Success, result);
        // }
        //
        // /// <summary>
        // /// ?e?X?g??e: GetAttributes?? separator ?? key, value ?????????????
        // /// result ?? ErrorInvalidArgument ???????B
        // /// </summary>
        // [TestMethod]
        // public void Test_GetAttributes_Result_Error_When_Separator_Invalid()
        // {
        //     plateauObject.SetAttribute("Test", "Test\nSeparator");
        //     plateauObject.GetAttributes(999, out APIResult result, "\n");
        //     Assert.AreEqual(APIResult.ErrorInvalidArgument, result);
        // }
        //
        // /// <summary>
        // /// ?e?X?g??e: GetAttributes ????p???????m??T?C?Y????????????? result ?? ErrorLackOfBufferSize ???????B
        // /// </summary>
        // [TestMethod]
        // public void Test_GetAttributes_LackOfBuffer()
        // {
        //     plateauObject.SetAttribute("TestAttr", "TestValue");
        //     plateauObject.GetAttributes(1, out APIResult result);
        //     Assert.AreEqual(APIResult.ErrorLackOfBufferSize, result);
        // }
    }
}