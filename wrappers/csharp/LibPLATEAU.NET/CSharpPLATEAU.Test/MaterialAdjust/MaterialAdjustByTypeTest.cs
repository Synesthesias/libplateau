using Microsoft.VisualStudio.TestTools.UnitTesting;
using PLATEAU.CityGML;
using PLATEAU.Dataset;
using PLATEAU.MaterialAdjust;
using PLATEAU.PolygonMesh;

namespace PLATEAU.Test.MaterialAdjust;

[TestClass]
public class MaterialAdjustByTypeTest
{
    [TestMethod]
    public void OnModelIsEmpty()
    {
        var model = Model.Create();
        var adjuster = MaterialAdjusterByType.Create();
        Assert.AreEqual(true, adjuster.RegisterType("gmlID", CityObjectType.COT_Building));
        Assert.AreEqual(true, adjuster.RegisterType("gmlID2", CityObjectType.COT_Bridge));
        Assert.AreEqual(true, adjuster.RegisterMaterialPattern(CityObjectType.COT_Building, 1));
        Assert.AreEqual(true, adjuster.RegisterMaterialPattern(CityObjectType.COT_Bridge, 2));
        Assert.AreEqual(false, adjuster.RegisterType("gmlID", CityObjectType.COT_Door), "重複キーを登録したらfalseが返ります。");
        Assert.AreEqual(false, adjuster.RegisterMaterialPattern(CityObjectType.COT_Building, 999), "重複キーを登録したらfalseが返ります。");
        
        adjuster.Exec(model);
        
        Assert.AreEqual(0, model.RootNodesCount);
    }
}
