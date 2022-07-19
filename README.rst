
.. Zephyr Course documentation master file, created by
   sphinx-quickstart on Fri Apr 29 14:43:22 2022.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

Bem vindo ao GitHub do Curso Introdução ao Zephyr
===================================================



=================================
Exemplos
=================================

Os exemplos aqui apresentados foram pensados de forma incremental, introduzindo
novas estruturas do Zephyr gradativamente.

* **first_project**: Hello World com estrutura básica de projeto.
* **pisca_led**: Pisca Led básico. 
  ``driver/gpio``
* **pisca_timer**: Introduz o uso do timer. 
  ``k_timer`` ``driver/gpio`` 
* **pisca_workqueue**:  Introduz o uso da System Work Queue.
  ``k_work`` ``k_timer`` ``driver/gpio``  
* **pisca_app_workqueue**: Altera para uso da Application Work Queue.
  ``k_work`` ``k_timer`` ``driver/gpio`` 
* **pisca_button**: Adiciona botão com interrupção. 
  ``driver/gpio`` ``gpio_callback`` ``k_work``
* **i2c_scanner**: Escaneia dispositivos no barramento i2c_scanner.
  ``driver/i2c``
* **sensor**: Leitura de temperatura e humidade de sensor HTS221.
  ``sensor``
* **sensor_timer**: Adiciona a API de Timer.
  ``timer`` ``sensor``
* **sensor_log**: Adiciona sistema de LOG.
  ``log`` ``sensor``
* **sensor_trigger**: Acelerômetro com Trigger
  ``sensor_trigger`` ``sensor`` ``log``
* **sensor_msgq**: Adiona amostragem periódica, uso de Message Queue com dump de amostras.
  ``k_msgq`` ``sensor`` ``k_work``  ``k_timer`` ``driver/gpio`` ``gpio_callback``
* **timer_msgq**: Simula sensor amostrando e consumo lentro através de timer.
  ``k_msgq`` ``k_work``  ``k_timer``
* **sensor_thread**: Amostragem periódica e thread adicional para leitura.
  ``thread`` ``k_msgq`` ``sensor`` ``k_work``  ``k_timer``
* **uart_poll**: Uart no modo pool
  ``uart``
* **uart_int**: Uart no modo interrupt
  ``uart`` ``k_msgq``
* **uart_async**: Uart no modo assincrono com DMA - Verificar suporte no hardware
  ``uart`` ``k_msgq`
* **pisca_led_kconfig**: Pisca Led com Kconfig 
  ``kconfig`` ``driver/gpio``
* **pisca_led_serial_recovery**: Pisca Led com MCUBoot 
  ``mcuboot`` ``driver/gpio``


=================================
Outros Recursos
=================================
* **cmd**: Exemplos de .cmd para setar variáveis do Zephyr no Windows
