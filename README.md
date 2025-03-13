# Iot_esp32cam (Projeto TCC de Pós em IoT Lato Senso)
Segurança Inteligente para Residências: Desenvolvimento de um Sistema IoT para Vigilância e Controle de Segurança em uma Residência com ESP32-CAM PIR Motion Detection &amp; Streaming Server.

ESP32-CAM PIR Motion Detection & Streaming Server

Este projeto utiliza o ESP32-CAM para capturar imagens e realizar streaming em tempo real. Integrado com um sensor PIR (Passive Infrared), o sistema detecta movimentos e, ao identificá-los, captura e exibe uma imagem. O servidor web incorporado permite controle remoto do ESP32-CAM, visualização do status do sensor PIR, captura de imagens e streaming ao vivo da câmera.
Funcionalidades

    Detecção de Movimento: Ao detectar movimento, o sensor PIR aciona a captura automática de uma imagem pela câmera.
    Página de Status: Exibe o status do sensor PIR (se o movimento foi detectado) e mostra a última imagem capturada.
    Captura de Imagem: Permite capturar uma nova imagem diretamente via navegador.
    Streaming ao Vivo: Exibe um feed contínuo de imagens JPEG da câmera em tempo real.

Requisitos
Hardware

    ESP32-CAM
    Sensor PIR (conectado ao pino 13 do ESP32)

Software

    Arduino IDE ou PlatformIO
    Bibliotecas para ESP32:
        esp_camera.h
        WiFi.h
        esp_http_server.h

Descrição do Código

    Conexão Wi-Fi: O ESP32 se conecta a uma rede Wi-Fi usando as credenciais fornecidas no código.
    Configuração da Câmera: Inicializa a câmera ESP32 com a configuração necessária para capturar imagens.
    Detecção de Movimento: O sensor PIR detecta movimento, atualizando a variável motionDetected e exibindo uma mensagem no console.
    Captura de Imagem: Uma imagem é capturada quando o movimento é detectado.
    Servidor Web: Um servidor HTTP é iniciado na porta 80, aguardando requisições de clientes.
    Respostas do Servidor: O servidor responde a diferentes requisições HTTP:
        /capture: Captura uma imagem e exibe uma página HTML com a imagem.
        /stream: Envia o feed de imagens JPEG capturadas como um fluxo de dados contínuo.
        /status: Retorna o status do sensor PIR (se o movimento foi detectado ou não).
        404 Not Found: Para requisições não reconhecidas, retorna uma mensagem de erro.
    Interface Web: A interface web permite visualizar o status do movimento, capturar imagens e assistir ao streaming ao vivo da câmera.

Como Usar
1. Configurar o ESP32

    Abra o código no Arduino IDE ou PlatformIO.
    Defina seu SSID e senha Wi-Fi nas variáveis ssid e password no código.

2. Configuração da Câmera

    O código está configurado para um modelo comum de ESP32-CAM. Caso esteja usando um modelo diferente, ajuste os pinos de conexão conforme a sua câmera.

3. Carregar o Código

    Conecte o ESP32 ao computador e selecione a placa correta no Arduino IDE.
    Carregue o código para o ESP32.

4. Conectar-se à Rede Wi-Fi

    Após carregar o código, o ESP32 se conectará à rede Wi-Fi e exibirá o endereço IP no monitor serial. Use esse IP para acessar o servidor web via navegador.

5. Usar a Interface Web

    Acessar o IP: Após a inicialização, o ESP32 exibirá o endereço IP no monitor serial. Acesse o IP em um navegador (exemplo: http://192.168.x.x).

    Status: Visualize o status do sensor PIR (se movimento foi detectado ou não) e a última imagem capturada.

    Captura de Imagem: Permite capturar uma nova imagem diretamente na interface.

    Streaming: Exibe o feed de vídeo ao vivo da câmera.

6. Sensor PIR

    Se o sensor PIR detectar movimento, o sistema capturará uma imagem automaticamente e atualizará o status na página.

Considerações

    O streaming em tempo real pode ser afetado pela qualidade da rede Wi-Fi e pelas limitações de processamento do ESP32.
    Caso utilize uma configuração de hardware diferente, ajuste os pinos da câmera conforme necessário.
