using System;
using System.Collections.Generic;
using System.Linq;
using LibPLATEAU.NET.CityGML;

namespace LibPLATEAU.NET.Util
{
    public delegate bool DelegateCityObjectCondition(CityObject co);

    public delegate bool DelegateGeometryCondition(Geometry geo);

    public delegate bool DelegatePolygonCondition(Polygon poly);
    public static class SearchUtil
    {
        // public static IEnumerable<CityObject> ChildCityObjectsWhere(this CityObject searchRoot, DelegateCityObjectCondition condition)
        // {
        //     var results = SearchCityObjectRecursive(searchRoot, condition);
        //     foreach (var r in results)
        //     {
        //         yield return r;
        //     }
        // }
        //
        // public static IEnumerable<CityObject> CityObjectsWhere(this CityModel cityModel,
        //     DelegateCityObjectCondition condition)
        // {
        //     // TODO ここは Linq に置き換えられそう
        //     foreach (var co in cityModel.RootCityObjects)
        //     {
        //         var results = SearchCityObjectRecursive(co, condition);
        //         foreach (var r in results)
        //         {
        //             yield return r;
        //         }
        //     }
        // }
        //
        //
        //
        // private static IEnumerable<CityObject> SearchCityObjectRecursive(CityObject target, DelegateCityObjectCondition condition)
        // {
        //     if (condition(target))
        //     {
        //         yield return target;
        //     }
        //
        //     foreach (var co in target.ChildCityObjects)
        //     {
        //         var childResult = SearchCityObjectRecursive(co, condition);
        //         foreach (var result in childResult)
        //         {
        //             yield return result;
        //         }
        //     }
        // }
        //
        // public static IEnumerable<Geometry> GeometriesWhere(this CityObject targetCO, DelegateGeometryCondition condition)
        // {
        //     // TODO ここは Linq に置き換えられそう
        //     foreach (var geo in targetCO.Geometries)
        //     {
        //         var results = GeometriesWhereRecursive(geo, condition);
        //         foreach (var r in results)
        //         {
        //             yield return r;
        //         }
        //     }
        // }
        //
        // public static IEnumerable<Geometry> ChildGeometriesWhere(this Geometry targetGeo, DelegateGeometryCondition condition)
        // {
        //     var results = GeometriesWhereRecursive(targetGeo, condition);
        //     foreach (var r in results)
        //     {
        //         yield return r;
        //     }
        // }
        //
        // private static IEnumerable<Geometry> GeometriesWhereRecursive(Geometry targetGeo,
        //     DelegateGeometryCondition condition)
        // {
        //     if (condition(targetGeo))
        //     {
        //         yield return targetGeo;
        //     }
        //
        //     foreach (var child in targetGeo.ChildGeometries)
        //     {
        //         var results = GeometriesWhereRecursive(child, condition);
        //         foreach (var r in results)
        //         {
        //             yield return r;
        //         }
        //     }
        // }

        public static IEnumerable<CityObject> IterateChildrenDfs(this CityObject co)
        {
            yield return co;
            foreach (var child in co.ChildCityObjects)
            {
                var results = IterateChildrenDfs(child);
                foreach (var r in results)
                {
                    yield return r;
                }
            }
        }

    }
}