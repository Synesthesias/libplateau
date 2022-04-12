using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Reflection.Emit;
using System.Runtime.InteropServices;
using System.Text;

namespace LibPLATEAU.NET
{
    /// <summary>
    /// CityGMLにおける全てのオブジェクトのベースクラスです。
    /// ユニークIDと属性データを持ちます。
    /// </summary>
    public class Object
    {
        private IntPtr handle;
        private string id = "";

        internal Object(IntPtr handle)
        {
            this.handle = handle;
        }

        /// <summary>
        /// セーフハンドルを取得します。
        /// </summary>
        public IntPtr Handle => this.handle;

        /// <summary>
        /// オブジェクトのユニークIDを取得します。
        /// </summary>
        public string ID
        {
            get
            {
                if (this.id != "")
                {
                    return this.id;
                }
                this.id = Marshal.PtrToStringAnsi(NativeMethods.plateau_object_get_id(this.handle)) ?? "";
                return this.id;
            }
        }

        /// <summary>
        /// 属性データを取得し、stringで返します。
        /// <para name="returnStrMaxSize"> 取得する結果の、文字列としての最大サイズ(byte数)を指定します。</para>
        /// <para name="result">
        /// result は正常終了の場合 0, 異常終了の場合それ以外になります。
        /// 具体的には、結果を格納するのに最大サイズで足りない場合は -2 に、
        /// 属性が存在しないまたは値が空文字列である場合は -1 になります。
        /// </para>
        /// </summary>
        public string GetAttribute(string name, int returnStrMaxSize, out APIResult result) {
            StringBuilder sb = new (returnStrMaxSize);
            
            result = NativeMethods.plateau_object_get_attribute(this.handle, name, sb, sb.Capacity);
            return sb.ToString();
        }


        /// <summary>
        /// 属性データを設定します。
        /// </summary>
        public void SetAttribute(string name, string value, bool doOverride, AttributeType type = AttributeType.String) {
            NativeMethods.plateau_object_set_attribute(this.handle, name, value, type, doOverride);
        }

        /// <summary>
        /// 属性の一覧を Dictionary 形式で返します。
        /// </summary>
        public Dictionary<string, string> GetAttributeList(int maxBufferSize, out APIResult result) {
            StringBuilder sb = new(maxBufferSize);
            var ret = new Dictionary<string, string>();
            // このメソッドを呼ぶと、StringBuilderに属性のkeyとvalueの一覧が入ります。
            // key1, value1, key2, value2, ... の順番で改行区切りの文字列になります。
            NativeMethods.plateau_object_get_keys_values(this.handle, sb, maxBufferSize);
            string receivedStr = sb.ToString();
            string[] tokens = receivedStr.Split("\n");
            if (tokens.Length == 1) {
                // 属性がないとき
                result = APIResult.Success;
                return ret;
            }
            if (tokens.Length % 2 != 0) {
                // 数が合わないとき
                Console.Error.WriteLine($"Error in {nameof(GetAttributeList)} tokens must have even number of elements, but actual number is {tokens.Length}.");
                result = APIResult.ErrorInvalidData;
                return ret;
            }
            // 属性をDictionaryに追加
            for (int i = 0; i < tokens.Length; i += 2) {
                string key = tokens[i];
                string val = tokens[i + 1];
                ret.Add(key, val);
            }
            result = APIResult.Success;
            return ret;
        }
    }
}
