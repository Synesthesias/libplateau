using System;
using System.Collections.Generic;
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
        /// <para name="returnStrMaxSize"> 取得する結果である文字列を格納するのに十分なbyte数を指定します。</para>
        /// <para name="result">
        /// result は正常終了の場合 Success, 異常終了の場合それ以外になります。
        /// 具体的には、結果を格納するのに returnStrMaxSize で足りない場合は ErrorLackOfBufferSize に、
        /// 属性が存在しないまたは値が空文字列である場合は ErrorValueNotFound になります。
        /// </para>
        /// </summary>
        public string GetAttribute(string name, int returnStrMaxSize, out APIResult result)
        {
            StringBuilder sb = new(returnStrMaxSize);

            result = NativeMethods.plateau_object_get_attribute(this.handle, name, sb, sb.Capacity);
            return sb.ToString();
        }


        /// <summary>
        /// 属性データを設定します。
        /// </summary>
        /// <para name="name"> 属性名です。 </para>
        /// <para name="value"> 属性の値です。 </para>
        /// <para name="doOverride"> 同名の属性がすでに設定済みである場合に、上書きするかどうかです。 </para>
        public void SetAttribute(string name, string value, bool doOverride = true,
            AttributeType type = AttributeType.String)
        {
            NativeMethods.plateau_object_set_attribute(this.handle, name, value, type, doOverride);
        }

        /// <summary>
        /// 属性の一覧を Dictionary 形式で返します。
        /// </summary>
        /// <para name="maxBufferSize"> ライブラリから取得する文字列が入るのに十分なバイト数を指定します。</para>
        /// <para name="result">
        /// APIの実行結果が入ります。
        /// 通常成功時は Success になります。
        /// separator が key, value と被った場合は ErrorInvalidArgument になります。
        /// maxBufferSize で与えられたバイト数では結果を格納するのに足りなかった場合は ErrorLackOfBufferSize になります。
        /// </para>
        /// <para name="separator">
        /// separator は、ライブラリと情報をやりとりするときに使う便宜上の区切り文字です。
        /// ライブラリに属性一覧を問い合わせ、その結果は
        /// key1 value1 key2 value2 ... の順番で間に区切り文字が入った文字列で返ります。
        /// そのため separator は 各key にも valueにも含まれない文字列にする必要があります。
        /// separatorのデフォルト値は、少し長いですが他とかぶることはそうそうないだろうと思われる文字列にしました。
        /// </para>
        public Dictionary<string, string> GetAttributes(int maxBufferSize, out APIResult result,
            string separator = "\n[(|+_separator_+|)]\n")
        {
            StringBuilder sb = new(maxBufferSize);
            var ret = new Dictionary<string, string>();
            // このメソッドを呼ぶと、StringBuilderに属性のkeyとvalueの一覧が入ります。
            result = NativeMethods.plateau_object_get_keys_values(this.handle, sb, maxBufferSize, separator);
            if (result != APIResult.Success) return ret;
            string receivedStr = sb.ToString();
            string[] tokens = receivedStr.Split(separator);
            if (tokens.Length == 1)
            {
                // 属性がないとき
                return ret;
            }

            if (tokens.Length % 2 != 0)
            {
                // 数が合わないとき
                Console.Error.WriteLine(
                    $"Error in {nameof(GetAttributes)} tokens must have even number of elements, but actual number is {tokens.Length}.");
                result = APIResult.ErrorInvalidData;
                return ret;
            }

            // 属性をDictionaryに追加
            for (int i = 0; i < tokens.Length; i += 2)
            {
                string key = tokens[i];
                string val = tokens[i + 1];
                ret.Add(key, val);
            }

            return ret;
        }
    }
}