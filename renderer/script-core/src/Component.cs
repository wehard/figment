using System;

namespace Figment
{
    public class Component
    {
        public Component()
        {
            Console.WriteLine("Hello, World!");
        }

        public static void Print()
        {
            Console.WriteLine("Called from C#");
        }

        public virtual void OnUpdate()
        {
            Console.WriteLine("Component.OnUpdate");
        }
    }

    public class Transform : Component
    {
        public override void OnUpdate()
        {
            Console.WriteLine("Transform.OnUpdate");
        }
    }
}
