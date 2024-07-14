#pragma once

template<typename T>
class Graph
{
public:
    Graph() = default;
    ~Graph() = default;

    void AddNode(T node)
    {
        m_Nodes.push_back(node);
    }

    void AddEdge(T *from, T *to)
    {
        from->Neighbors.push_back(to);
    }

    void Clear()
    {
        m_Nodes.clear();
    }

    std::vector<T> &GetNodes()
    {
        return m_Nodes;
    }
private:
    std::vector<T> m_Nodes;
};
