using System;
using System.Diagnostics;
using System.Runtime.InteropServices;
using LibPLATEAU.NET.Util;

namespace LibPLATEAU.NET.CityGML
{
    public class DllLogger
    {
        private IntPtr handle;
        
        public DllLogger(IntPtr handle)
        {
            this.handle = handle;
        }

        /// <summary>
        /// DLLから受け取ったログメッセージを C#での標準出力に転送するよう設定します。
        /// </summary>
        public void SetCallbacksToStdOut()
        {
            SetLogCallbacks(
                messagePtr => Console.Error.WriteLine(PtrToStr(messagePtr)),
                messagePtr => Console.WriteLine(PtrToStr(messagePtr)),
                messagePtr => Console.WriteLine(PtrToStr(messagePtr))
            );
        }

        public string PtrToStr(IntPtr strPtr)
        {
            return Marshal.PtrToStringAnsi(strPtr);
        }

        /// <summary>
        /// DLL内でのログをコールバックによって受け取ることができるようにします。
        /// </summary>
        public void SetLogCallbacks(
            LogCallbackFuncType errorCallback,
            LogCallbackFuncType warnCallback,
            LogCallbackFuncType infoCallback)
        {
            APIResult result = NativeMethods.plateau_dll_logger_set_callbacks(
                this.handle,
                Marshal.GetFunctionPointerForDelegate(errorCallback),
                Marshal.GetFunctionPointerForDelegate(warnCallback),
                Marshal.GetFunctionPointerForDelegate(infoCallback)
            );
            DLLUtil.CheckDllError(result);
        }
    }
}