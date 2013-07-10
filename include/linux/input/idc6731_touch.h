#ifndef IDC6731_TS_H
#define IDC6731_TS_H

struct idc6731_platform_data {
	void (*reset)(void);
	int (*init)(void);
	void (*wake)(void);
	void (*sleep)(void);
	int	irq_gpio;		/* irq number	*/
	int reset_gpio;
	int (*_pfCraeteVirKey)(void);
};

#endif
