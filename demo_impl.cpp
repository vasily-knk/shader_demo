#include "demo.h"

struct demo_impl
    : demo
{
    demo_impl();

    void render() override{};

public:
    bool is_ready() const;

private:
    bool init();

private:
    bool is_ready_;
    IDirect3D9Ptr d3d_;
    IDirect3DDevice9Ptr device_;
};

demo_impl::demo_impl()
    : is_ready_(false)
{
    is_ready_ = init();
}

bool init()
{
    return true;
}

demo_ptr create_demo()
{
    auto ptr = make_shared<demo_impl>();
    if (!ptr->is_ready())
        return demo_ptr();;

    return ptr;
}