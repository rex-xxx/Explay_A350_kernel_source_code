#include <typedefs.h>
#include <linuxver.h>
#include <osl.h>

#include <proto/ethernet.h>
#include <dngl_stats.h>
//#include <dhd.h>
#include <dhd_dbg.h>

#include <linux/fcntl.h>
#include <linux/fs.h>

struct dhd_info;
extern int _dhd_set_mac_address(struct dhd_info *dhd, int ifidx, struct ether_addr *addr);

#ifdef READ_MACADDR
int dhd_read_macaddr(struct dhd_info *dhd, struct ether_addr *mac)
{
    struct file *fp      = NULL;
    struct file *fpnv      = NULL;
    char macbuffer[18]   = {0};
    mm_segment_t oldfs   = {0};
	char randommac[3]    = {0};
	char buf[18]         = {0};
	char* filepath       = "/data/.mac.info";
    char* nvfilepath       = "/persist/softmac";
	int ret = 0;


	fpnv = filp_open(nvfilepath, O_RDONLY, 0);
	if (IS_ERR(fpnv)) {
start_readmac:
		fpnv = NULL;
		fp = filp_open(filepath, O_RDONLY, 0);
		if (IS_ERR(fp)) {
			fp = filp_open(filepath, O_RDWR | O_CREAT, 0666);
			if(IS_ERR(fp)) {
				DHD_ERROR(("[WIFI] %s: File open error\n", filepath));
				return -1;
			}

			oldfs = get_fs();
			set_fs(get_ds());

			get_random_bytes(randommac, 3);
			
			sprintf(macbuffer,"%02X:%02X:%02X:%02X:%02X:%02X\n",
				0x00,0x90,0x4C,randommac[0],randommac[1],randommac[2]);
			DHD_TRACE(("[WIFI] The Random Generated MAC address : %s\n", macbuffer));

			if(fp->f_mode & FMODE_WRITE) {			
				ret = fp->f_op->write(fp, (const char *)macbuffer, sizeof(macbuffer), &fp->f_pos);
				if(ret < 0)
					DHD_ERROR(("[WIFI] Mac address [%s] Failed to write into File: %s\n", macbuffer, filepath));
				else
					DHD_TRACE(("[WIFI] Mac address [%s] written into File: %s\n", macbuffer, filepath));
			}
			set_fs(oldfs);
		}
		ret = kernel_read(fp, 0, buf, 18);
		buf[17] = '\0';
	}
	else {
		ret = kernel_read(fpnv, 0, buf, 18);
		buf[17] ='\0';
		DHD_INFO(("Read MAC :%s\n" , buf));
		if(strncmp(buf , "00:00:00:00:00:00" , 17) == 0) {
			filp_close(fpnv, NULL);
			goto start_readmac;
		}
	}

	DHD_TRACE(("kernel_read return %d\n", ret));

	if(ret) {
		unsigned int softmac[6];
		int i;
		sscanf(buf,"%02x:%02x:%02x:%02x:%02x:%02x",
			   &softmac[0], &softmac[1], &softmac[2], 
			    &softmac[3], &softmac[4], &softmac[5]);
		for (i = 0; i < 6; i++)
			mac->octet[i] = softmac[i] & 0xff;
	} else
		DHD_ERROR(("dhd_bus_start: Reading from the '%s' returns 0 bytes\n", filepath));

	if (fp)
		filp_close(fp, NULL);
	if (fpnv)
		filp_close(fpnv, NULL);    	

	if (0 == _dhd_set_mac_address(dhd, 0, mac))
		DHD_TRACE(("MAC address is overwritten\n"));
	else
		DHD_ERROR(("_dhd_set_mac_address() failed\n"));

    return 0;
}
#endif

