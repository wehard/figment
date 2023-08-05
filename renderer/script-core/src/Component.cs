using System;

namespace Figment
{
    public interface IComponent
    {
        void OnStart();
        void OnUpdate();
    }
    
    public abstract class Component : IComponent
    {
        protected Component()
        {
            Console.WriteLine("Component constructor called.");
        }

        public static void Print()
        {
            Console.WriteLine("Called from C#");
        }

        public virtual void OnStart() {}
        public virtual void OnUpdate() {}
    }

    public class Transform : Component
    {
        public Vec3 Position;
        public Vec3 Rotation;
        public Vec3 Scale;
        
        public override void OnStart()
        {
            Console.WriteLine("Transform.OnStart");
        }

        public override void OnUpdate()
        {
            Console.WriteLine($"Transform.OnUpdate, {ToString()}");
        }

        public override string ToString()
        {
            return $"Position = {Position}, Rotation = {Rotation}, Scale = {Scale}";
        }
    }
}
