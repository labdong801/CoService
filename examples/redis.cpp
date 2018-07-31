//
// Created by lml on 2018/6/27.
//

#include <iostream>
#include <redis3m/redis3m.hpp>

int main() {

//    redis3m::connection_pool::create()

    redis3m::simple_pool::ptr_t pool = redis3m::simple_pool::create("localhost");

    redis3m::connection::ptr_t c = pool->get();
    c->run(redis3m::command("SET")("foo")("helloworld"));
    pool->put(c);

    c = pool->get();
    std::cout << c->run(redis3m::command("GET")("foo")).str();
//    pool->put(c);

    return 0;

}


