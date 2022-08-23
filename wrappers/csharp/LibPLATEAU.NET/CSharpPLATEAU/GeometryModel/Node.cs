using System;

namespace PLATEAU.GeometryModel
{
    public class Node
    {
        private IntPtr handle;

        public Node(IntPtr handle)
        {
            this.handle = handle;
        }
    }
}