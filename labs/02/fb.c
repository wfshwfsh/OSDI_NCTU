#include "mailbox.h"
#include "reg.h"
#include "fb.h"
#include "util.h"
#include "uart.h"

#define FB_BUFFER_SIZE  (FB_PHY_WIDTH*FB_PHY_HEIGHT)
#define FB_PHY_WIDTH    1920
#define FB_PHY_HEIGHT   1080
#define FB_VIRT_WIDTH   1920
#define FB_VIRT_HEIGHT  1080
#define FB_OFFSET_X     0
#define FB_OFFSET_Y     0
#define FB_DEPTH        32

#define PIXEL_ORDER_BGR 0x0
#define PIXEL_ORDER_RGB 0x1
#define FB_PIXEL_ORDER  (PIXEL_ORDER_RGB)

static unsigned int *fb_base;
static int fb_size=0, fb_pitch;

void fb_init()
{
    int idx=1;
    
    memset(&mailbox, 0, sizeof(mailbox));
    
    //mailbox[0] = 7 * 4; // buffer size in bytes
    mailbox[idx++] = TAG_REQUEST_CODE;

    /* ----- TAGS BEGIN ----- */
    // 1. Allocate buffer
    mailbox[idx++] = 0x00040001;
    mailbox[idx++] = 4;
    mailbox[idx++] = REQUEST_CODE;
	mailbox[idx++] = FB_BUFFER_SIZE;
    //mailbox[idx++] = 0;
    //mailbox[idx++] = 0;
    

    // 2. Physical Buffer Width/Height
    mailbox[idx++] = 0x00048003;
    mailbox[idx++] = 8;
	mailbox[idx++] = REQUEST_CODE;
    mailbox[idx++] = FB_PHY_WIDTH;
    mailbox[idx++] = FB_PHY_HEIGHT;
    
    // 3. Virtual Buffer Width/Height
    mailbox[idx++] = 0x00048004;
    mailbox[idx++] = 8;
	mailbox[idx++] = REQUEST_CODE;
    mailbox[idx++] = FB_VIRT_WIDTH;
    mailbox[idx++] = FB_VIRT_HEIGHT;
    
    // 4. Virtual Buffer Offset
    mailbox[idx++] = 0x00048009;
    mailbox[idx++] = 8;
	mailbox[idx++] = REQUEST_CODE;
    mailbox[idx++] = FB_OFFSET_X;
    mailbox[idx++] = FB_OFFSET_Y;
    
    // 5. Depth
    mailbox[idx++] = 0x00048005;
    mailbox[idx++] = 4;
	mailbox[idx++] = REQUEST_CODE;
    mailbox[idx++] = FB_DEPTH;//BIT_PER_PIXEL;
    
    // 6. Pixel Order
    mailbox[idx++] = 0x00048006;
    mailbox[idx++] = 4;
	mailbox[idx++] = REQUEST_CODE;
    mailbox[idx++] = FB_PIXEL_ORDER;
    
    // 7. Get pitch
    mailbox[idx++] = 0x00040008;
    mailbox[idx++] = 4;
	mailbox[idx++] = REQUEST_CODE;
    mailbox[idx++] = 0;

    mailbox[idx++] = END_TAG;
    mailbox[0] = idx * 4; // buffer size in bytes
    /* ----- TAGS END ----- */
    
    mailbox_call(MAILBOX_CH__ARM2VC);
    my_printf("length: %d\n", idx);
    my_printf("Req result: %d\n", mailbox[1]);
    
    fb_base = mailbox[5];
    my_printf("fb_base: 0x%x\n", fb_base);
    
    fb_size = mailbox[6];
    my_printf("fb_size: %d\n", fb_size);
    
    fb_pitch = mailbox[32];
    my_printf("pitch: %d\n", fb_pitch);
}

#define LEN_SQUARE_X 20
#define LEN_SQUARE_Y 20
void fb_loadSplashImage()
{
    int x,y;
    int color_w=0x00000000, color_b=0xffffffff;
    int isWhite=1;
	
    for(y=0;y<FB_PHY_HEIGHT;y++)
    {
		if( (y%(LEN_SQUARE_Y)) == 0)
			isWhite = !isWhite;
		
        for(x=0;x<FB_PHY_WIDTH;x++)
        {
			if( (x%LEN_SQUARE_X) == 0)
				isWhite = !isWhite;

			//fill color
			if(isWhite)
				*fb_base++ = color_w;
			else
				*fb_base++ = color_b;
        }
    }
}
