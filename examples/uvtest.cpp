#include <iostream>
#include <uv.h>
#include <string.h>
#include <unistd.h>

uv_loop_s * loop;

uv_buf_t alloc_buffer(uv_handle_t *handle, size_t suggested_size) {
    return uv_buf_init((char*) malloc(suggested_size), suggested_size);
}

void echo_write(uv_write_t *req, int status) {
    if (status) {
        fprintf(stderr, "Write error %s\n",""/* uv_strerror(status)*/);
    }
    free(req);
}

void echo_read(uv_stream_t *client, ssize_t nread, uv_buf_t buf) {
    const char * http_response = "HTTP/1.0 200 OK\r\nServer: NetServer\r\nContent-Length: 11\r\nContent-type: text/plain\r\n\r\nHello world!\n";
    if (nread < 0) {
        //发生错误（客户端主动关闭连接等...）
        if (nread != UV_EOF)
            fprintf(stderr, "Read error %s\n", ""/*uv_err_name(nread)*/);
        uv_close((uv_handle_t*) client, NULL);
    } else if (nread > 0) {
        //接收到数据 uv_buf_t 结构体
        uv_write_t *req = (uv_write_t *) malloc(sizeof(uv_write_t));
        uv_buf_t wrbuf = uv_buf_init(const_cast<char *>(http_response), strlen(http_response));
        sleep(1);
        uv_write(req, client, &wrbuf, 1, echo_write);


        //测试用，一般不主动关闭
        uv_close((uv_handle_t*) client, NULL);
    }

    if (buf.base)
        free(buf.base);
}

void on_new_connection(uv_stream_t *server, int status) {
    if (status == -1) {
        // error!
        return;
    }

    uv_tcp_t *client = (uv_tcp_t*) malloc(sizeof(uv_tcp_t));
    uv_tcp_init(loop, client);

    if (uv_accept(server, (uv_stream_t*) client) == 0) {
        uv_read_start((uv_stream_t*) client, alloc_buffer, echo_read);

        struct sockaddr_in cli_addr;
        int clilen = sizeof(cli_addr);
        memset(&cli_addr, 0, clilen);
        uv_tcp_getpeername(client, reinterpret_cast<sockaddr *>(&cli_addr), &clilen);

        std::cout << "connect from << " << inet_ntoa(cli_addr.sin_addr) << ":" << cli_addr.sin_port << std::endl;

    }
    else {
        uv_close((uv_handle_t*) client, NULL);
    }
}

int main() {

    loop = uv_default_loop();

    uv_tcp_t server;
    uv_tcp_init(loop, &server);

    struct sockaddr_in bind_addr = uv_ip4_addr("0.0.0.0", 40625);
    uv_tcp_bind(&server, bind_addr);
    int r = uv_listen((uv_stream_t*) &server, 128, on_new_connection);
    if (r) {
        fprintf(stderr, "Listen error %s\n", uv_err_name(uv_last_error(loop)));
        return 1;
    }
    return uv_run(loop, UV_RUN_DEFAULT);

}


