#include "esp_camera.h"
#include <WiFi.h>

#define PIR_PIN 13 // Pino do sensor PIR

const char* ssid = "SEU_SSID"; // Substitua pelo seu SSID
const char* password = "SUA_SENHA"; // Substitua pela sua senha

bool motionDetected = false; // Variável para armazenar o estado do movimento

void setup() {
  Serial.begin(115200);
  pinMode(PIR_PIN, INPUT);
  
  // Configuração da câmera
  camera_config_t config;
  config.ledc_channel = LEDC_CHANNEL;
  config.ledc_timer = LEDC_TIMER;
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

  // Inicializa a câmera
  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Erro ao inicializar a câmera");
    return;
  }

  // Conexão Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando ao WiFi...");
  }
  Serial.println("Conectado ao WiFi");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());

  // Inicia o servidor web
  startServer();
}

void loop() {
  if (digitalRead(PIR_PIN) == HIGH) {
    Serial.println("Movimento detectado!");
    motionDetected = true; // Atualiza o estado do movimento
    captureImage();
    delay(5000); // Aguarda 5 segundos antes de verificar novamente
  } else {
    motionDetected = false; // Reseta o estado do movimento
  }
}

void startServer() {
  WiFiServer server(80);
  server.begin();
  Serial.println("Servidor iniciado");

  while (true) {
    WiFiClient client = server.available();
    if (client) {
      String request = client.readStringUntil('\r');
      Serial.println(request);
      client.flush();

      if (request.indexOf("/status") != -1) {
        sendHTML(client);
      } else if (request.indexOf("/capture") != -1) {
        captureImage();
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: text/html");
        client.println();
        client.println("<html><body><h1>Imagem Capturada!</h1>");
        client.println("<img src=\"/stream\" style=\"width:100%;\"/></body></html>");
      } else if (request.indexOf("/stream") != -1) {
        client.println("HTTP/1.1 200 OK");
        client.println("Content-Type: image/jpeg");
        client.println();
        captureImage(client);
      } else {
        client.println("HTTP/1.1 404 Not Found");
        client.println("Content-Type: text/html");
        client.println();
        client.println("<html><body><h1>Página não encontrada</h1></body></html>");
      }
      client.stop();
    }
  }
}

void sendHTML(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html lang='pt-BR'>");
  client.println("<head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  client.println("<title>Detecção de Movimento</title>");
  client.println("<style>");
  client.println("body { font-family: Arial, sans-serif; text-align: center; background-color: #f4f4f4; padding: 20px; }");
  client.println("h1 { color: #333; }");
  client.println("#status { font-size: 1.5em; margin: 20px 0; }");
  client.println("#timer { font-size: 1.2em; margin: 10px 0; }");
  client.println("img, video { margin: 10px 0; max-width: 100%; height: auto; }");
  client.println("</style>");
  client.println("</head>");
  client.println("<body>");
  client.println("<h1>Detecção de Movimento</h1>");
  client.println("<div id='status'>Aguardando detecção...</div>");
  client.println("<div id='timer'>Timer: <span id='time'>00:00</span></div>");
  client.println("<div id='timestamp'></div>");
  client.println("<img id='cameraImage' src='' alt='Imagem da Câmera'>");
  client.println("<video id='cameraVideo' width='320' height='240' controls autoplay></video>");
  client.println("<script>");
  client.println("let timerInterval;");
  client.println("let seconds = 0;");
  client.println("function updateTimer() { seconds++; const minutes = Math.floor(seconds / 60); const displaySeconds = seconds % 60; document.getElementById('time').innerText = `${String(minutes).padStart(2, '0')}:${String(displaySeconds).padStart(2, '0')}`; }");
 client.println("function updateStatus(movimentoDetectado) {");

  client.println("    const agora = new Date();");

  client.println("    const horario = agora.toLocaleString();");

  client.println("    document.getElementById('timestamp').innerText = `Última Atualização: ${horario}`;");

  client.println("    if (movimentoDetectado) {");

  client.println("        document.getElementById('status').innerText = 'Movimento Detectado';");

  client.println("        if (!timerInterval) {");

  client.println("            timerInterval = setInterval(updateTimer, 1000);");

  client.println("        }");

  client.println("    } else {");

  client.println("        document.getElementById('status').innerText = 'Nenhum Movimento';");

  client.println("        clearInterval(timerInterval);");

  client.println("        timerInterval = null;");

  client.println("        seconds = 0;");

  client.println("        document.getElementById('time').innerText = '00:00';");

  client.println("    }");

  client.println("    // Atualiza a imagem da câmera");

  client.println("    document.getElementById('cameraImage').src = `http://<IP_DA_CAMERA>/imagem`; // Substitua <IP_DA_CAMERA> pelo IP real");

  client.println("    // Atualiza o vídeo da câmera");

  client.println("    document.getElementById('cameraVideo').src = `http://<IP_DA_CAMERA>/video`; // Substitua <IP_DA_CAMERA> pelo IP real");

  client.println("}");

  client.println("// Simulação de detecção de movimento");

  client.println("setInterval(() => {");

  client.println("    const movimentoDetectado = Math.random() < 0.5; // Simulação aleatória");

  client.println("    updateStatus(movimentoDetectado);");

  client.println("}, 3000); // Atualiza a cada 3 segundos");

  client.println("</script>");

  client.println("</body>");

  client.println("</html>");

}


void captureImage() {

  camera_fb_t *fb = esp_camera_fb_get();

  if (!fb) {

    Serial.println("Erro ao capturar imagem");

    return;

  }

  Serial.println("Imagem capturada!");

  esp_camera_fb_return(fb);

}


void captureImage(WiFiClient client) {

  camera_fb_t *fb = esp_camera_fb_get();

  if (!fb) {

    Serial.println("Erro ao capturar imagem");

    return;

  }

  client.write(fb->buf, fb->len); // Envia a imagem para o cliente

  esp_camera_fb_return(fb);

}