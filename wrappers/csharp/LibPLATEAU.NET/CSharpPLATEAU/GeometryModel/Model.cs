using System;

namespace PLATEAU.GeometryModel
{
    public class Model
    {
        private IntPtr handle;
        
        public Model(IntPtr handle)
        {
            this.handle = handle;
        }
    }
}