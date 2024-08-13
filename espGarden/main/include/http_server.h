#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#ifndef MIN
#define MIN(x, y) ((x) < (y) ? (x) : (y))
#endif
const int RX_TX_BUFF_LEN = 8; // Message standard will be 8 bytes



/* Handler function for receiving commands */
esp_err_t rx_handler(httpd_req_t *req) {
    char rx_buf[RX_TX_BUFF_LEN]; // buffer for receiving data
    
    // Receive the data
    int rx = httpd_req_recv(req, rx_buf, MIN(req->content_len, sizeof(rx_buf) - 1));

    if (rx <= 0) {
        if (rx == HTTPD_SOCK_ERR_TIMEOUT) {
            httpd_resp_send_408(req);
        }
        return ESP_FAIL;
    }

    rx_buf[rx] = 0;
    ESP_LOGI(HTTP_LOG_TAG, "Received: %s", rx_buf);
    std::string resp = "rec";
    
    // Process the command here (e.g., water the garden for an hour = A:60)
    xSemaphoreTake(system_state.state_mutex, portMAX_DELAY);
    if (!add_watering_command_to_queue(std::string(rx_buf))) {
        resp = "err";
    };
    xSemaphoreGive(system_state.state_mutex);

    // Send a receive packet
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_send(req, resp.c_str(), HTTPD_RESP_USE_STRLEN);
    ESP_LOGI(HTTP_LOG_TAG, "Sent Receipt Confimation: %s", resp.c_str());

    return ESP_OK;
}

/* Handler function for sending state data */
esp_err_t tx_handler(httpd_req_t *req) {
    // Get the system state json
    std::string json_state = stateToJson();

    // Send the data
    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");
    httpd_resp_set_type(req, "application/json");
    httpd_resp_send(req, json_state.c_str(), HTTPD_RESP_USE_STRLEN);
    ESP_LOGI(HTTP_LOG_TAG, "Sent: %s", json_state.c_str());
    return ESP_OK;
}

/* Function to start the web server */
httpd_handle_t start_webserver(void) {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    httpd_handle_t server = NULL;

    // One URI for receiving, one for sending
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_uri_t rx_uri = {
            .uri = "/esp_rx",
            .method = HTTP_POST,
            .handler = rx_handler,
            .user_ctx = NULL
        };

        httpd_uri_t tx_uri = {
            .uri = "/esp_tx",
            .method = HTTP_GET,
            .handler = tx_handler,
            .user_ctx = NULL
        };

        httpd_register_uri_handler(server, &rx_uri);
        httpd_register_uri_handler(server, &tx_uri);
    }

    return server;
}

/* Function to stop the web server */
void stop_webserver(httpd_handle_t &server) {
    if (server) {
        httpd_stop(server);
    }
}

#endif // HTTP_SERVER_H
