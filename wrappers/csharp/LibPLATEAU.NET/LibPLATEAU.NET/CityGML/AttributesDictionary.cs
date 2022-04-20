using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Net;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Text;
using System.Transactions;
using LibPLATEAU.NET.Util;

// key と value のペアに短縮名を付けます。
using AttrPair = System.Collections.Generic.KeyValuePair<string, LibPLATEAU.NET.CityGML.AttributeValue>;

namespace LibPLATEAU.NET.CityGML
{
    /// <summary>
    /// 属性の辞書です。
    /// IReadOnlyDictionary を実装します。
    /// string をキーとし、 AttributeValue が値になります。
    /// this[key] で AttributeValueが返ります。
    /// </summary>
    public class AttributesDictionary : IReadOnlyDictionary<string, AttributeValue>
    {
        private IntPtr handle;
        private string[]? cachedKeys;

        internal AttributesDictionary(IntPtr handle)
        {
            this.handle = handle;
        }
        
        /// <summary> 属性の数を返します。 </summary>
        public int Count => NativeMethods.plateau_attributes_map_get_key_count(this.handle);

        /// <summary>
        /// 属性のキーをすべて取得します。
        /// 結果はキャッシュされるので2回目以降は速いです。
        /// </summary>
        public IEnumerable<string> Keys
        {
            get
            {
                if (this.cachedKeys != null) return this.cachedKeys;
                int[] keySizes = GetKeySizes();
                int cnt = Count;

                IntPtr ptrOfStringArray = DLLUtil.AllocPtrArray(cnt, keySizes);

                NativeMethods.plateau_attributes_map_get_keys(this.handle, ptrOfStringArray);
                string[] ret = DLLUtil.PtrToStringArray(ptrOfStringArray, cnt, keySizes);

                DLLUtil.FreePtrArray(ptrOfStringArray, cnt);

                this.cachedKeys = ret;
                return ret;
            }
        }

        public IEnumerable<AttributeValue> Values
        {
            get
            {
                var values = Keys.Select(key => this[key]);
                return values;
            }
        }

        /// <summary>
        /// 属性のキーから値を返します。
        /// </summary>
        public AttributeValue this[string key]
        {
            get
            {
                IntPtr valueHandle = NativeMethods.plateau_attributes_map_get_attribute_value(
                    this.handle, key);
                return new AttributeValue(valueHandle);
            }
        }

        public bool ContainsKey(string key)
        {
            return NativeMethods.plateau_attributes_map_do_contains_key(this.handle, key);
        }

        /// <summary>
        /// 属性の中に key が存在すればその値を value に代入して true を返します。
        /// key が存在しなければ value に null を代入して false を返します。
        /// </summary>
        public bool TryGetValue(string key, out AttributeValue value)
        {
            if (ContainsKey(key))
            {
                value = this[key];
                return true;
            }
            value = null!;
            return false;
        }


        /// <summary>
        /// 属性の各キーの文字列としてのバイト数を配列で返します。
        /// 例外が起きたときは各要素が -1 の配列を返します。
        /// </summary>
        private int[] GetKeySizes()
        {
            int cnt = Count;
            int intArraySize = Marshal.SizeOf(typeof(int)) * cnt;
            IntPtr intArrayPtr = Marshal.AllocCoTaskMem(intArraySize);
            int[] ret = Enumerable.Repeat(-1, cnt).ToArray();
            try
            {
                // intArrayPtr に欲しい情報を格納します。
                NativeMethods.plateau_attributes_map_get_key_sizes(this.handle, intArrayPtr);
                // intArrayPtr の内容を C#の配列のコピーします。
                Marshal.Copy(intArrayPtr, ret, 0, cnt);
            }
            finally
            {
                Marshal.FreeCoTaskMem(intArrayPtr);
            }

            return ret;
        }

        
        
        public IEnumerator<AttrPair> GetEnumerator()
        {
            return new AttributesDictionaryEnumerator(this);
        }

        /// <summary>
        /// key と value と型 のセットをすべて文字にします。
        /// </summary>
        public override string ToString()
        {
            var sb = new StringBuilder();
            sb.Append("[ ");
            sb.Append(String.Join(", ", Keys.Select(key => $"{{ ({this[key].Type}) {key} , {this[key].AsString} }}")));
            sb.Append(" ]");
            return sb.ToString();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }
        
        
        
        /// <summary>
        /// インナークラスです。
        /// AttributesDictionary の Enumerator です。
        /// </summary>
        public class AttributesDictionaryEnumerator : IEnumerator<AttrPair>
        {
            private AttributesDictionary dict;
            private int index;


            public AttributesDictionaryEnumerator(AttributesDictionary dict)
            {
                this.dict = dict;
                Reset();
            }
            public bool MoveNext()
            {
                this.index++;
                return this.index < this.dict.Count;
            }

            public void Reset()
            {
                this.index = -1;
            }

            AttrPair IEnumerator<AttrPair>.Current => (AttrPair)Current;

            public object Current
            {
                get
                {
                    this.dict.cachedKeys ??= this.dict.Keys.ToArray();
                    string key = this.dict.cachedKeys[this.index];
                    return new KeyValuePair<string, AttributeValue>(key, this.dict[key]);
                }
            }


            public void Dispose()
            {
            }
        }
    }



    
}