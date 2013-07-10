#ifndef MIPI_ILI9486_HSD_H
#define MIPI_ILI9486_HSD_H

//#define MIPI_TRULY_FAKE_PANEL /* FAKE PANEL for test */
void mipi_ili9486_hsd_sel_mode(int mode);

int mipi_ili9486_hsd_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel);

#endif  /* MIPI_ILI9486_HSD_H */
