#include<stdio.h>
#include<string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
static unsigned int *gp_fb;
static int g_fd;
static const char *FBDEVICE="/dev/fb0";
/* 经过内存映射之后，我们可以写 gp_fb指向的内存 来操控framebuffer */
void fb_draw_back(unsigned int width, unsigned int height, unsigned int color)
{
	unsigned int x, y;
	
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			*(gp_fb + y * width + x) = color;
		}
	}
}
int main(void)
{
    int ret = -1;
	
    struct fb_fix_screeninfo finfo;
    struct fb_var_screeninfo vinfo;
 
    memset(&finfo, 0, sizeof(finfo));
    memset(&vinfo, 0, sizeof(vinfo));
 
    /* 第1步：打开设备 */
    g_fd = open(FBDEVICE, O_RDWR);
    if (g_fd < 0)
    {
        perror("open");
        return -1;
    }
 
    /* 第2步：获取设备的硬件信息，如屏幕分辨率，颜色位深等信息 */
    ret = ioctl(g_fd, FBIOGET_FSCREENINFO, &finfo);
    if (ret < 0)
    {
        perror("ioctl");
        return -1;
    }
 
    ret = ioctl(g_fd, FBIOGET_VSCREENINFO, &vinfo);
    if (ret < 0)
    {
        perror("ioctl");
        return -1;
    }
 
    /* 第3步：进行mmap，申请fb */
    unsigned long len = vinfo.xres_virtual * vinfo.yres_virtual *     \
                                                    vinfo.bits_per_pixel / 8;
 
    gp_fb = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, g_fd, 0);
 
    if (NULL == gp_fb)
    {
        perror("mmap");
        return -1;
    }
    fb_draw_back(100,100,0X0F100);
}

 
 
 
 
 
