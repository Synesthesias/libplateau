namespace LibPLATEAU.NET
{
    /// <summary>
    /// CityGMLにおける都市オブジェクトです。
    /// </summary>
    public class CityObject : FeatureObject
    {
        private CityObjectType type = 0;

        internal CityObject(IntPtr handle) : base(handle) {}

        public CityObjectType Type
        {
            get
            {
                if (this.type != 0)
                {
                    return this.type;
                }

                this.type = NativeMethods.plateau_city_object_get_type(this.Handle);
                return this.type;
            }
        }
    }
}
