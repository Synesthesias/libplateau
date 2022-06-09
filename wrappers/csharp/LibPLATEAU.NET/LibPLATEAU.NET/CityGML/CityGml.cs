
using System;
using System.IO;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    public static class CityGml
    {
        /// <summary>
        /// DLLの機能によって gmlファイルをパースし、CityModelを生成します。
        /// </summary>
        /// <param name="gmlPath">gmlファイルのパスです。</param>
        /// <param name="parserParams">変換の設定です。</param>
        /// <param name="logCallbacks">ログを受け取るコールバックです。標準出力で良い場合は <see cref="LogCallbacks"/>.<see cref="LogCallbacks.StdOut"/> を指定してください。</param>
        /// <param name="logLevel">ログの詳細度です。</param>
        public static CityModel Load(
            string gmlPath, CitygmlParserParams parserParams,
            LogCallbacks logCallbacks,
            DllLogLevel logLevel = DllLogLevel.Error
        )
        {
            APIResult result = NativeMethods.plateau_load_citygml(
                gmlPath, parserParams, out IntPtr cityModelHandle,
                logLevel, logCallbacks.LogErrorFuncPtr, logCallbacks.LogWarnFuncPtr, logCallbacks.LogInfoFuncPtr);
            if (result == APIResult.ErrorLoadingCityGml)
            {
                throw new FileLoadException(
                    $"Loading gml failed.\nPlease check codelist xml files are located in (gmlFolder)/../../codelists\nAND gml file is located at {gmlPath}\nand ");
            }
            DLLUtil.CheckDllError(result);
            return new CityModel(cityModelHandle);
        }
    }
}
