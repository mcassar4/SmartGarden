#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif

#include <esp_err.h>
#include <esp_http_server.h>
#include <stdlib.h>
#include <stdbool.h>

// Define the session context structure
typedef struct {
    int request_count;       // Counter to track the number of requests
    char data_buffer[100];   // Buffer to store some data temporarily
    bool status_flag;        // A flag to indicate some status
} my_data_type;

esp_err_t set_cors_headers(httpd_req_t *req) {
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    httpd_resp_set_hdr(req, "Access-Control-Allow-Headers", "Content-Type");
    return ESP_OK;
}
 

/* Handler function for GET requests */
esp_err_t get_handler(httpd_req_t *req)
{
    set_cors_headers(req);    
    const char resp[] = "URI GET Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Handler function for POST requests */
esp_err_t post_handler(httpd_req_t *req)
{
    set_cors_headers(req);    
    char content[100];
    size_t recv_size = MIN(req->content_len, sizeof(content));
    int ret = httpd_req_recv(req, content, recv_size);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    const char resp[] = "URI POST Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Custom function to free context */
void free_ctx_func(void *ctx)
{
    free(ctx);
}

/* Handler function for a POST request that uses session context */
esp_err_t adder_post_handler(httpd_req_t *req)
{
    set_cors_headers(req);    

    if (!req->sess_ctx) {
        req->sess_ctx = malloc(sizeof(my_data_type));
        req->free_ctx = free_ctx_func;
    }

    my_data_type *ctx_data = (my_data_type*)req->sess_ctx;

    // Increment request count
    ctx_data->request_count++;

    // Store received data in context buffer
    size_t recv_size = MIN(req->content_len, sizeof(ctx_data->data_buffer) - 1);
    int ret = httpd_req_recv(req, ctx_data->data_buffer, recv_size);
    if (ret <= 0) {
        if (ret == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }
    ctx_data->data_buffer[recv_size] = '\0'; // Null-terminate the received data

    // Update the status flag (example logic)
    ctx_data->status_flag = true;

    /* Respond */
    const char resp[] = "Adder POST Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

/* Custom URI handler function */
esp_err_t my_uri_handler(httpd_req_t *req)
{
    set_cors_headers(req);    
    // Recv, Process and Send
    const char resp[] = "Custom URI Response";
    httpd_resp_send(req, resp, HTTPD_RESP_USE_STRLEN);

    // On success
    return ESP_OK;
}

/* Function to start the webserver */
httpd_handle_t start_webserver(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t uri_get = {
            .uri = "/uri/get",
            .method = HTTP_GET,
            .handler = get_handler,
            .user_ctx = NULL
        };

        httpd_uri_t uri_post = {
            .uri = "/uri/post",
            .method = HTTP_POST,
            .handler = post_handler,
            .user_ctx = NULL
        };
        httpd_uri_t uri_adder_post = {
            .uri = "/uri/post/adder",
            .method = HTTP_POST,
            .handler = adder_post_handler,
            .user_ctx = NULL
        };

        httpd_register_uri_handler(server, &uri_get);
        httpd_register_uri_handler(server, &uri_post);

        // Example of registering custom URI handler
        httpd_uri_t my_uri = {
            .uri = "/my_uri/path/xyz",
            .method = HTTP_GET,
            .handler = my_uri_handler,
            .user_ctx = NULL
        };

        if (httpd_register_uri_handler(server, &my_uri) != ESP_OK) {
            // Handle failure to register handler
        }
    }

    return server;
}

/* Function to stop the webserver */
void stop_webserver(httpd_handle_t server)
{
    if (server) {
        httpd_stop(server);
    }
}

#endif // HTTP_SERVER_H
