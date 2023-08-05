using System;
using System.Collections.Generic;

namespace Figment
{
    public class Entity
    {
        private readonly List<Component> m_Components = new List<Component>();

        public Entity()
        {
            Console.WriteLine("Entity::Entity");
            AddComponent<Transform>();
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
            var component = new T();
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