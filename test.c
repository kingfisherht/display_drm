#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <stdlib.h>

#include <sys/mman.h>
#define FB_DEVICE_NAME "/dev/fb0"
#define RED_COLOR565 	0X0F100
#define GREEN_COLOR565	0X007E0
#define BLUE_COLOR565	0X0001F
#define White_COLOR565 	0xFFFFF
#define BLACK_COLOR			0x0
 
void delay(int a)
{
	int k;
 
	for(k=0;k<a;k++);
}
 
typedef struct fb_dev{
	int fd;					/*帧缓冲设备硬件描述符 */
	void *pfb; 				/*指向帧缓冲映射到用户空间的首地址 */
	int xres; 				/*一帧图像的宽度 */
	int yres;				/*一帧图像的高度 */
	int size;				/*一帧图像的大小 */
	int bits_per_pixel;		/*每个像素的大小 */
} fb_dev_t;
 
int fb_open(fb_dev_t *fbd, char *fbn)	//打开操作（由于帧缓冲是字符设备，应用程序需按文件打开一个帧缓冲设备。打开成功则可以对帧缓冲设备进行读、写等操作
{
	struct fb_var_screeninfo vinfo;		//（struct fb_info结构体的成员）当前缓冲区的可变参数，也就是当前的视屏信息
 
	if((fbd->fd = open(fbn, O_RDWR)) == -1)	//判断是否打开framebuffer
	{
		printf("Error: Cannot open framebuffer device.\n");
		exit(1);
	}
	ioctl(fbd->fd, FBIOGET_VSCREENINFO, &vinfo);	//ioctl()函数可读取/设置显示设备以及屏幕的参数。如分辨率、显示颜色、屏幕大小等。ioctl()函数是由底层的驱动程序完成的。
	fbd->xres = vinfo.xres;							//屏幕x轴
	fbd->yres = vinfo.yres;					 		//屏幕y轴
	fbd->bits_per_pixel = vinfo.bits_per_pixel; 	//每个像素点占用多少个字节。
	fbd->size = fbd->xres * fbd->yres * 2;//fbd->bits_per_pixel; //计算一帧图像的大小
	printf("%d * %d,%d bits_per_pixel,screensize = %d.\n",fbd->xres,fbd->yres,fbd->bits_per_pixel,fbd->size);
	fbd->pfb = mmap(NULL, fbd->size, PROT_READ | PROT_WRITE, MAP_SHARED, fbd->fd, 0); //mmap函数执行帧缓冲设备具体的内存映射。将文件内容映射到一段内存中，准确的说是虚拟内存，通过对内存的读取和修改。?
 
	if((int)fbd->pfb == -1)	//判断是否映射成功，失败打印错误信息。
	{
		printf("Error: Failed to map frambuffer device to memory!\n");
		exit(2);
	}
	return 0;
}
 
int fb_close(fb_dev_t *fbd)		//关闭设备文件
{
	munmap(fbd->pfb,fbd->size);	 //解除映射
	close(fbd->fd);
}
 
int fb_drawrect(fb_dev_t *fbd, int x0, int y0, int w, int h, int color) //画图像到屏幕上。
{
	int x,y;
 
	for(y=y0; y<y0+h; y++)
	{
		for(x=x0; x<x0+w; x++)
		{
			*((short *)(fbd->pfb) + y*fbd->xres +x) = color;	//计算一个像素的位置
		}
	}
	return 0;
}
 
 
int main(int argc, char **argv)
{
	fb_dev_t *fbd;
 
	fbd = (fb_dev_t *)malloc(sizeof(fb_dev_t));	//为设备文件分配内存
 
	fb_open(fbd, FB_DEVICE_NAME);	 // 打开帧缓冲设备文件（帧缓冲设备对应的设备文件为/dev/fb*）这里应该加一句，判断一下打开帧缓冲设备是否成功。
 
	if(fbd->bits_per_pixel == 16)	
	{
		/*while(fbd->bits_per_pixel == 16)*/
		
		printf("Red/Green/Blue/Black Screen!\n");		// 输出屏幕将要显示的颜色
		fb_drawrect(fbd, 0, 0, fbd->xres, fbd->yres/4, RED_COLOR565);	//屏幕颜色显示，将屏幕分割为4个
		fb_drawrect(fbd, 0, fbd->yres/4, fbd->xres, fbd->yres/4,White_COLOR565);
		fb_drawrect(fbd, 0, fbd->yres*2/4, fbd->xres, fbd->yres/4,BLUE_COLOR565);
		fb_drawrect(fbd, 0, fbd->yres*3/4, fbd->xres, fbd->yres/4,BLACK_COLOR);

	}else if(fbd->bits_per_pixel == 32){
		 printf("Red/Green/Blue/Black Screen!\n");		// 输出屏幕将要显示的颜色
		fb_drawrect(fbd, 0, 0, fbd->xres, fbd->yres/4, RED_COLOR565);	//屏幕颜色显示，将屏幕分割为4个
		fb_drawrect(fbd, 0, fbd->yres/4, fbd->xres, fbd->yres/4,White_COLOR565);
		fb_drawrect(fbd, 0, fbd->yres*2/4, fbd->xres, fbd->yres/4,BLUE_COLOR565);
		fb_drawrect(fbd, 0, fbd->yres*3/4, fbd->xres, fbd->yres/4,BLACK_COLOR);
	}
	else{
		printf("16 bits only!\n");
	}
	fb_close(fbd);
	return 0;
}


