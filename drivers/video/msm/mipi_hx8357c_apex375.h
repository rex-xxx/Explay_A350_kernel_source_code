#ifndef MIPI_HX8357C_APEX375_H
#define MIPI_HX8357C_APEX375_H

void mipi_hx8357c_apex375_sel_mode(int mode);

int mipi_hx8357c_apex375_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel);

#endif  /* MIPI_HX8357C_APEX375_H */
