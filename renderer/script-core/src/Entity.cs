using System;
using System.Collections.Generic;

namespace Figment
{
    public class Entity
    {
        private List<Component> m_Components = new List<Component>();

        public Entity()
        {
            Console.WriteLine("Entity::Entity");
        }

        public T AddComponent<T>() where T : Component, new()
        {
            var component = new T();
            m_Components.Add(component);
            return component;
        }

        public T GetComponent<T>() where T : Component
        {
            Type componentType = typeof(T);

            foreach (var component in m_Components)
            {
                if (componentType.IsAssignableFrom(component.GetType()))
                {
                    return (T)component;
                }
            }

            return null;
        }
    }
}