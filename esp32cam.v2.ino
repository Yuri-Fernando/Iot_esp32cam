#include "esp_camera.h"
#include <WiFi.h>
#include "esp_http_server.h"

#define PIR_PIN 13 // Pino do sensor PIR
const char* ssid = "SEU_SSID";
const char* password = "SUA_SENHA";

bool motionDetected = false;
httpd_handle_t stream_httpd = NULL;
httpd_handle_t capture_httpd = NULL;
String lastCapturedImage = "";

void startCameraServer();

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  
  // Configuração da câmera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = 5;
  config.pin_d1 = 18;
  config.pin_d2 = 19;
  config.pin_d3 = 21;
  config.pin_d4 = 36;
  config.pin_d5 = 39;
  config.pin_d6 = 34;
  config.pin_d7 = 35;
  config.pin_xclk = 0;
  config.pin_pclk = 22;
  config.pin_vsync = 25;
  config.pin_href = 23;
  config.pin_sscb_sda = 26;
  config.pin_sscb_scl = 27;
  config.pin_pwdn = 32;
  config.pin_reset = -1;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Erro ao inicializar a câmera");
    return;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  startCameraServer(); // Inicia o servidor de streaming e captura
}

void loop() {
  if (digitalRead(PIR_PIN) == HIGH) {
    Serial.println("Movimento detectado!");
    motionDetected = true;
    captureImage();
    delay(5000);
  } else {
    motionDetected = false;
  }
}

void captureImage() {
  camera_fb_t *fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("Erro ao capturar imagem");
    return;
  }
  lastCapturedImage = String("data:image/jpeg;base64,") + base64::encode(fb->buf, fb->len);
  esp_camera_fb_return(fb);
}

static esp_err_t index_handler(httpd_req_t *req) {
  String response = "<html><body>"
                    "<h1>ESP32-CAM</h1>"
                    "<button onclick=\"location.href='/status'\">Status</button>"
                    "<button onclick=\"location.href='/capture'\">Capturar Imagem</button>"
                    "<button onclick=\"location.href='/stream'\">Streaming</button>"
                    "</body></html>";
  httpd_resp_set_type(req, "text/html");
  httpd_resp_send(req, response.c_str(), response.length());
  return ESP_OK;
}

static esp_err_t status_handler(httpd_req_t *req) {
  String response = "<html><body>"
                    "<h1>Status do Sensor PIR</h1>"
                    "<p>Movimento: " + String(motionDetected ? "Detectado" : "Não Detectado") + "</p>"
                    "<img src='" + lastCapturedImage + "' width='320'/>"
                    "<script>setTimeout(()=>location.reload(), 2000);</script>"
                    "</body></html>";
  httpd_resp_set_type(req, "text/html");
  httpd_resp_send(req, response.c_str(), response.length());
  return ESP_OK;
}

static esp_err_t stream_handler(httpd_req_t *req) {
  camera_fb_t *fb = NULL;
  httpd_resp_set_type(req, "multipart/x-mixed-replace; boundary=frame");

  while (true) {
    fb = esp_camera_fb_get();
    if (!fb) {
      Serial.println("Falha ao capturar frame");
      continue;
    }
    char part_header[] = "--frame\r\nContent-Type: image/jpeg\r\nContent-Length: %d\r\n\r\n";
    char buffer[64];
    int len = snprintf(buffer, 64, part_header, fb->len);
    httpd_resp_send_chunk(req, buffer, len);
    httpd_resp_send_chunk(req, (const char *)fb->buf, fb->len);
    httpd_resp_send_chunk(req, "\r\n", 2);
    esp_camera_fb_return(fb);
  }
  return ESP_OK;
}

static esp_err_t capture_handler(httpd_req_t *req) {
  captureImage();
  httpd_resp_set_type(req, "image/jpeg");
  httpd_resp_send(req, lastCapturedImage.c_str(), lastCapturedImage.length());
  return ESP_OK;
}

void startCameraServer() {
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  httpd_uri_t index_uri = { .uri = "/", .method = HTTP_GET, .handler = index_handler, .user_ctx = NULL };
  httpd_uri_t stream_uri = { .uri = "/stream", .method = HTTP_GET, .handler = stream_handler, .user_ctx = NULL };
  httpd_uri_t capture_uri = { .uri = "/capture", .method = HTTP_GET, .handler = capture_handler, .user_ctx = NULL };
  httpd_uri_t status_uri = { .uri = "/status", .method = HTTP_GET, .handler = status_handler, .user_ctx = NULL };
  
  httpd_start(&stream_httpd, &config);
  httpd_register_uri_handler(stream_httpd, &index_uri);
  httpd_register_uri_handler(stream_httpd, &stream_uri);
  httpd_register_uri_handler(stream_httpd, &capture_uri);
  httpd_register_uri_handler(stream_httpd, &status_uri);
}

