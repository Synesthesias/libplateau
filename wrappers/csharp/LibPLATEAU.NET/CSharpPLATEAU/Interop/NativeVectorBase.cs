﻿using System;
using System.Collections;
using System.Collections.Generic;

namespace PLATEAU.Interop
{
    /// <summary>
    /// <seealso cref="NativeVectorDisposableBase{T}"/>
    /// </summary>
    public abstract class NativeVectorBase<T> : INativeVector<T>
    {
        public IntPtr Handle { get; }
        protected NativeVectorBase(IntPtr handle)
        {
            Handle = handle;
        }

        public abstract T At(int index);
        public abstract int Length { get; }

        public IEnumerator<T> GetEnumerator()
        {
            int len = Length;
            for (int i = 0; i < len; i++)
            {
                yield return At(i);
            }
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            return GetEnumerator();
        }

    }
}
