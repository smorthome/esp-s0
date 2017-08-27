#include <c_types.h>
#include <os_type.h>
#include <espmissingincludes.h>
#include <osapi.h>
#include <gpio.h>

static volatile os_timer_t timer;
static volatile uint32 pulses[3] = {0, 0, 0};


void timerfn(void* arg){
	if (GPIO_REG_READ(GPIO_OUT_ADDRESS) & BIT2) {
		gpio_output_set(0, BIT2, BIT2, 0);
	}
	else{
		gpio_output_set(BIT2, 0, BIT2, 0);
	}

	os_printf("Counters: %d %d %d\n", pulses[0], pulses[1], pulses[2]);
}

LOCAL void interrupt_handler(void* arg){
	int* p = (int*) arg;

	uint32 gpio_status = GPIO_REG_READ(GPIO_STATUS_ADDRESS);
	if(gpio_status & BIT(12)){
		gpio_pin_intr_state_set(GPIO_ID_PIN(12), GPIO_PIN_INTR_DISABLE);
		p[0]++;
		GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(12));
		gpio_pin_intr_state_set(GPIO_ID_PIN(12), GPIO_PIN_INTR_NEGEDGE);
	}
	else if(gpio_status & BIT(13)){
		gpio_pin_intr_state_set(GPIO_ID_PIN(13), GPIO_PIN_INTR_DISABLE);
		p[1]++;
		GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(13));
		gpio_pin_intr_state_set(GPIO_ID_PIN(13), GPIO_PIN_INTR_NEGEDGE);
	}
	if(gpio_status & BIT(14)){
		gpio_pin_intr_state_set(GPIO_ID_PIN(14), GPIO_PIN_INTR_DISABLE);
		p[2]++;
		GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, gpio_status & BIT(14));
		gpio_pin_intr_state_set(GPIO_ID_PIN(14), GPIO_PIN_INTR_NEGEDGE);
	}
}

void ICACHE_FLASH_ATTR user_init(){
	uart_div_modify(0, UART_CLK_FREQ / 9600);
	gpio_init();
	//os_printf("this a test");

	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTCK_U, FUNC_GPIO13);
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTMS_U, FUNC_GPIO14);

	PIN_PULLUP_EN(PERIPHS_IO_MUX_MTDI_U);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_MTCK_U);
	PIN_PULLUP_EN(PERIPHS_IO_MUX_MTMS_U);

	gpio_output_set(0, 0, 0, BIT12 | BIT13 | BIT14);

	ETS_GPIO_INTR_DISABLE();
	ETS_GPIO_INTR_ATTACH(interrupt_handler, &pulses);

	//gpio_register_set(GPIO_PIN_ADDR(0), GPIO_PIN_INT_TYPE_SET(GPIO_PIN_INTR_DISABLE) | GPIO_PIN_PAD_DRIVER_SET(GPIO_PAD_DRIVER_DISABLE) | GPIO_PIN_SOURCE_SET(GPIO_AS_PIN_SOURCE));

	//clear mask
	GPIO_REG_WRITE(GPIO_STATUS_W1TC_ADDRESS, BIT(0));

	gpio_pin_intr_state_set(GPIO_ID_PIN(12), GPIO_PIN_INTR_NEGEDGE);
	gpio_pin_intr_state_set(GPIO_ID_PIN(13), GPIO_PIN_INTR_NEGEDGE);
	gpio_pin_intr_state_set(GPIO_ID_PIN(14), GPIO_PIN_INTR_NEGEDGE);
	ETS_GPIO_INTR_ENABLE();

	os_timer_setfn(&timer, (os_timer_func_t*)timerfn, NULL);
	os_timer_arm(&timer, 1000, 1);
}
