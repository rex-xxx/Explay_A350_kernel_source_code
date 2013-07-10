#ifndef MIPI_HX8357C_CPT_H
#define MIPI_HX8357C_CPT_H

//#define MIPI_TRULY_FAKE_PANEL /* FAKE PANEL for test */
void mipi_hx8357c_cpt_sel_mode(int mode);

int mipi_hx8357c_cpt_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel);

#endif  /* MIPI_HX8357C_CPT_H */
