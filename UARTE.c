#include "UARTE.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

char Buffer[1024];
int i = 0;

void uart_ini()
{
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
    };
    // Configure UART parameters
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_0, &uart_config));

    // Set UART pins
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_0, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));
    // Setup UART buffered IO with event queue
    QueueHandle_t uart_queue;
    // Install UART driver using an event queue here
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_0, BUF_SIZE * 2, \
                                            BUF_SIZE * 2, 1024, &uart_queue, 0));
}

void UART_putchar(uint8_t data)
{
    uart_write_bytes(UART_NUM_0, (const char *) &data, 1);
}

void UART_puts(char* text)
{
    while(*text != 0)
    {
        UART_putchar(*text);
        text++;
    }
}


char UART_getchar()
{
    char data[2] = {0}, len;
    do
    {
        len = uart_read_bytes(UART_NUM_0, (void *)data, 2, 20 / portTICK_PERIOD_MS);        
    }while(len == 0);

    return data[0];
}

void UART_gets()
{
    char data;
    
    for(i = 0; ; i++)
    {
        data = UART_getchar();

        if(data >= 32)
        {
            UART_putchar(data);
            Buffer[i] = data;

            //esp_spp_write(UART_NUM_0, i, &data);
        }
        else
        {

            switch (data)
            {
                case 8:
                    i-=2;
                    UART_puts("\b \b");
                    break;
                case 13:
                    Buffer[i] = '\0';
                    //esp_spp_write(UART_NUM_0, i, Buffer);
                    return;
                    break;
                
                default:
                    i -= 2;
                    break;
            }
        } 
        
    }
}

void clrscr(void)
{
	UART_puts("\033[2J"); //Comando para limpiar el puerto
    gotoxy(0, 0);
}

void gotoxy(uint8_t x, uint8_t y)
{
	char str[] = "\033["; //Concatena las coordenadas X y Y con la secuencia de escape
	char num1[20];
	itoa(y, num1, 10); //Se convierte x a cadena
	char num2[20];
	itoa(x, num2, 10); //Se convierte y a cadena
    UART_puts(str);
	UART_puts(num1); //Se concatena todo...
	UART_puts(";");
	UART_puts(num2);
	UART_puts("f"); //Se imprime en el UART
}

void gotox(uint8_t x)
{
    char num[10];
    itoa(x, num, 10);
    UART_puts("\033[");
    UART_puts(num);
    UART_putchar('G');
}