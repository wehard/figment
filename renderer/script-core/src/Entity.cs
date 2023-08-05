using System;
using System.Collections.Generic;

namespace Figment
{
    public class Entity
    {
        public ulong Id { get; } = 0;
        private readonly List<Component> m_Components = new List<Component>();
        
        public Entity()
        {
            Console.WriteLine("Entity::Entity");
            AddComponent<Info>();
            var t = AddComponent<Transform>();
            t.Position = new Vec3(10, 0, 0);
        }

        public Entity(ulong id)
        {
            Id = id;
        }

        public void OnUpdate()
        {
            foreach (var c in m_Components)
            {
                c.OnUpdate();
            }
        }

        public T AddComponent<T>() where T : Component, new()
        {
            var component = new T() {Entity = this};
            m_Components.Add(component);
            return component;
        }

        public T GetComponent<T>() where T : Component
        {
            var componentType = typeof(T);

            foreach (var component in m_Components)
            {
                if (componentType.IsInstanceOfType(component))
                {
                    return (T)component;
                }
            }

            return null;
        }
    }
}