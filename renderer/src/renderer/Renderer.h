

class IRenderer
{

public:
    enum class API
    {
        NONE,
        OPENGL,
        WEBGPU
    };

public:
    virtual ~IRenderer() = default;

    virtual void Init() = 0;
    virtual void Clear() = 0;

    static API GetAPI() { return s_API; }

private:
    static API s_API;
};

class Renderer : public IRenderer
{
private:
public:
    Renderer();
    ~Renderer();
    void Init() override;
};
