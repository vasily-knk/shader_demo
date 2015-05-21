#pragma once

struct demo
{
    virtual ~demo() {};
    virtual void render() = 0;
};

typedef shared_ptr<demo> demo_ptr;
demo_ptr create_demo();