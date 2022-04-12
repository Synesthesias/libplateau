﻿using System;
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
        public void SetAttribute(string name, string value, AttributeType type = AttributeType.String) {
            // overwrite が false のときは 上書きしないのが本来意図された挙動ではありますが、
            // cityGmlのバグで正しく動作しないので overwrite は常に true とします。
            // バグ原因は citygml/object.cpp 43行目で、 overwrite = false の場合の上書きしない条件 count() >= 1
            // と書くべきところが誤って count() > 1 になっているためです。
            NativeMethods.plateau_object_set_attribute(this.handle, name, value, type, true);
        }

        /// <summary>
        /// 属性の一覧を Dictionary 形式で返します。
        /// </summary>
        /// <para name="maxBufferSize"> ライブラリから取得する文字列が入るのに十分なバイト数を指定します。</para>
        /// <para name="result"> APIの実行結果が入ります。</para>
        /// <para name="separator">
        /// separator は、ライブラリと情報をやりとりするときに使う便宜上の区切り文字です。
        /// ライブラリに属性一覧を問い合わせ、その結果は
        /// key1 value1 key2 value2 ... の順番で間に区切り文字が入った文字列で返ります。
        /// そのため separator は 各key にも valueにも含まれない文字列にする必要があります。
        /// </para>
        public Dictionary<string, string> GetAttributeList(int maxBufferSize, out APIResult result, string separator="\n[(|+_separator_+|)]\n") {
            StringBuilder sb = new(maxBufferSize);
            var ret = new Dictionary<string, string>();
            // このメソッドを呼ぶと、StringBuilderに属性のkeyとvalueの一覧が入ります。
            // key1, value1, key2, value2, ... の順番で間に区切り文字が入った文字列になります。
            result = NativeMethods.plateau_object_get_keys_values(this.handle, sb, maxBufferSize, separator);
            if (result != APIResult.Success) return ret;
            string receivedStr = sb.ToString();
            string[] tokens = receivedStr.Split(separator);
            if (tokens.Length == 1) {
                // 属性がないとき
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
            return ret;
        }
    }
}
