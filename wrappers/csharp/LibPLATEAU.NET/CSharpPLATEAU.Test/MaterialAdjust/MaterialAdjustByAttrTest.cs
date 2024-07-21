using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.MaterialAdjust;
using PLATEAU.PolygonMesh;

namespace PLATEAU.Test.MaterialAdjust;

[TestClass]
public class MaterialAdjustByAttrTest
{
    [TestMethod]
    public void OnModelIsEmpty()
    {
        var model = Model.Create();
        var adjuster = MaterialAdjusterByAttr.Create();
        Assert.AreEqual(true, adjuster.RegisterAttribute("gmlID", "attribute"));
        Assert.AreEqual(true, adjuster.RegisterAttribute("gmlID2", "attribute2"));
        Assert.AreEqual(true, adjuster.RegisterMaterialPattern("attribute", 1));
        Assert.AreEqual(true, adjuster.RegisterMaterialPattern("attribute2", 2));
        Assert.AreEqual(false, adjuster.RegisterAttribute("gmlID", "aaa"), "重複キーを登録したらfalseが返ります。");
        Assert.AreEqual(false, adjuster.RegisterMaterialPattern("attribute", 999), "重複キーを登録したらfalseが返ります。");
        
        adjuster.Exec(model);
        
        Assert.AreEqual(0, model.RootNodesCount);
    }
}
