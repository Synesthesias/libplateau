using System;
using System.Collections.Generic;
using System.Linq;
using LibPLATEAU.NET.CityGML;

namespace LibPLATEAU.NET.Util
{
    public static class SearchUtil
    {
        /// <summary>
        /// 子孫の <see cref="CityObject"/> をすべて再帰的にイテレートします。自分自身を含みます。
        /// DFS（深さ優先探索）を行います。
        /// </summary>
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