#ifndef MIPI_ILI9486_HSD_DJN375_H
#define MIPI_ILI9486_HSD_DJN375_H

//#define MIPI_TRULY_FAKE_PANEL /* FAKE PANEL for test */
void mipi_ili9486_hsd_djn375_sel_mode(int mode);

int mipi_ili9486_hsd_djn375_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel);

#endif  /* MIPI_ILI9486_HSD_DJN375_H */
