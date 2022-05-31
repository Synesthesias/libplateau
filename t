[1mdiff --git a/wrappers/csharp/LibPLATEAU.NET/LibPLATEAU.NET/Util/DLLUtil.cs b/wrappers/csharp/LibPLATEAU.NET/LibPLATEAU.NET/Util/DLLUtil.cs[m
[1mindex a61a5e2..9d04fe4 100644[m
[1m--- a/wrappers/csharp/LibPLATEAU.NET/LibPLATEAU.NET/Util/DLLUtil.cs[m
[1m+++ b/wrappers/csharp/LibPLATEAU.NET/LibPLATEAU.NET/Util/DLLUtil.cs[m
[36m@@ -1,5 +1,6 @@[m
 ﻿using System;[m
 using System.Collections.Generic;[m
[32m+[m[32musing System.Linq;[m
 using System.Runtime.InteropServices;[m
 using System.Text;[m
 using LibPLATEAU.NET.CityGML;[m
[36m@@ -279,11 +280,13 @@[m [mnamespace LibPLATEAU.NET.Util[m
 [m
         public static byte[] StrToUtf8Bytes(string str)[m
         {[m
[31m-            if (string.IsNullOrEmpty(str))[m
[32m+[m[32m            if (str == null)[m
             {[m
                 return new byte[] { 0 }; // null終端文字のみ[m
             }[m
[31m-            return Encoding.UTF8.GetBytes(str);[m
[32m+[m[32m            var bytes = new List<byte>(Encoding.UTF8.GetBytes(str));[m
[32m+[m[32m            bytes.Add(0); // null終端文字[m
[32m+[m[32m            return bytes.ToArray();[m
         }[m
 [m
     }[m
