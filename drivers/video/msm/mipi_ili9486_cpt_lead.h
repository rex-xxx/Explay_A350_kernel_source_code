#ifndef MIPI_ILI9486_CPT_LEAD_H
#define MIPI_ILI9486_CPT_LEAD_H

//#define MIPI_TRULY_FAKE_PANEL /* FAKE PANEL for test */
void mipi_ili9486_cpt_lead_sel_mode(int mode);

int mipi_ili9486_cpt_lead_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel);

#endif  /* MIPI_ILI9486_CPT_LEAD_H */
