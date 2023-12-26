using System;

namespace Figment
{
    public interface IComponent
    {
        void OnStart();
        void OnUpdate(float timeStep);
    }
    
    public abstract class Component : IComponent
    {
        public Entity Entity { get; internal set; }
        
        protected Component()
        {
            Console.WriteLine("Component constructor called.");
        }

        public static void Print()
        {
            Console.WriteLine("Called from C#");
        }

        public virtual void OnStart() {}
        public virtual void OnUpdate(float timeStep) {}
    }

    public class Info : Component
    {
        public string Name { get; }
    }

    public class Transform : Component
    {
        public Vec3 Position
        {
            get
            {
                NativeFunctions.Transform_GetPosition(Entity.Id, out var position);
                return position;
            }
            set => NativeFunctions.Transform_SetPosition(Entity.Id, ref value);
        }
        
        public Vec3 Rotation;
        public Vec3 Scale;
        
        public override void OnStart()
        {
            Console.WriteLine("Transform.OnStart");
        }

        public override void OnUpdate(float timeStep)
        {
            Console.WriteLine($"Transform.OnUpdate({timeStep}), {ToString()}");
        }

        public override string ToString()
        {
            return $"Position = {Position}, Rotation = {Rotation}, Scale = {Scale}";
        }
    }
}
