// Include Graphics Libraries
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include <pspdebug.h>
#include <pspctrl.h>

//Include Revel-Lib
#include "Revel-Lib/revel-lib.h"

// Define PSP Width / Height
#define PSP_BUF_WIDTH (512)
#define PSP_SCR_WIDTH (480)
#define PSP_SCR_HEIGHT (272)

// PSP Module Info
PSP_MODULE_INFO("Revel Lib Project Template", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

// sceGuobal variables
int running = 1;
static unsigned int __attribute__((aligned(16))) list[262144];

// Initialize Graphics
void initGraphics() {
    void* fbp0 = getStaticVramBuffer(PSP_BUF_WIDTH,PSP_SCR_HEIGHT,GU_PSM_8888);
	void* fbp1 = getStaticVramBuffer(PSP_BUF_WIDTH,PSP_SCR_HEIGHT,GU_PSM_8888);
	void* zbp = getStaticVramBuffer(PSP_BUF_WIDTH,PSP_SCR_HEIGHT,GU_PSM_4444);

	sceGuInit();

	sceGuStart(GU_DIRECT,list);
	sceGuDrawBuffer(GU_PSM_8888,fbp0,PSP_BUF_WIDTH);
	sceGuDispBuffer(PSP_SCR_WIDTH,PSP_SCR_HEIGHT,fbp1,PSP_BUF_WIDTH);
	sceGuDepthBuffer(zbp,PSP_BUF_WIDTH);
	sceGuOffset(2048 - (PSP_SCR_WIDTH/2),2048 - (PSP_SCR_HEIGHT/2));
	sceGuViewport(2048,2048,PSP_SCR_WIDTH,PSP_SCR_HEIGHT);
	sceGuDepthRange(65535,0);
	sceGuScissor(0,0,PSP_SCR_WIDTH,PSP_SCR_HEIGHT);
	sceGuEnable(GU_SCISSOR_TEST);
	sceGuDepthFunc(GU_GEQUAL);
	sceGuEnable(GU_DEPTH_TEST);
	sceGuFrontFace(GU_CW);
	sceGuShadeModel(GU_SMOOTH);
	sceGuEnable(GU_CULL_FACE);
	sceGuEnable(GU_TEXTURE_2D);
	sceGuEnable(GU_CLIP_PLANES);
	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(GU_TRUE);
}

// Start Frame
void startFrame() {
    sceGuStart(GU_DIRECT, list);
}

// End Frame
void endFrame() {
    sceGuFinish();
    sceGuSync(0, 0);
    sceDisplayWaitVblankStart();
	sceGuSwapBuffers();
}

// End Graphics
void termGraphics() {
    sceGuTerm();
}

int exit_callback(int arg1, int arg2, void* common){
	sceKernelExitGame();
	return 0;
}
 
int CallbackThread(SceSize args, void* argp) {
	int cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);
	sceKernelSleepThreadCB();
 
	return 0;
}
 
int SetupCallbacks(void) {
	int thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if (thid >= 0) {
		sceKernelStartThread(thid, 0, 0);
	}
	return thid;
}
char holder_text[1037] = "";
char core_area[1037] = "                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            ";
char* dialogue_lines[2] = {
    //intro text
    " ",
    "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
};
char scroller[1037] = "";

void clear_screen(){
    strcpy(scroller, core_area);
}

void draw_text_at_pos(ScePspIVector2 position, char* text){
    int startpoint = 0;
    int x_offset = 0;
    if(position.x < 60 && position.y < 17){
        for(int x = 0; x < strlen(text); x++){
            startpoint = (position.y * 61) + position.x;
            if(startpoint + x_offset > 1037){
                break;
            }
            if(text[x] == '\n'){
                position.y += 1;
                x_offset = 0;
                continue;
            }

            if(startpoint + x_offset < 1037 && position.x + x_offset < 60){
                scroller[startpoint+x_offset] = text[x];
                x_offset += 1;
            } else{
                
                position.y += 1;
                position.x = 0;
                x_offset = 0;
                x = x-1;
            }
            

        }
    }
}

int main() {
    // Boilerplate
    SetupCallbacks();

    // Initialize Graphics
    initGraphics();
    pspDebugScreenInit();
    
    // Initialize Matrices
    sceGumMatrixMode(GU_PROJECTION);
    sceGumLoadIdentity();
    sceGumOrtho(-16.0f / 9.0f, 16.0f / 9.0f, -1.0f, 1.0f, -10.0f, 10.0f);

    sceGumMatrixMode(GU_VIEW);
    sceGumLoadIdentity();

    sceGumMatrixMode(GU_MODEL);
    sceGumLoadIdentity();
    pspDebugScreenSetXY(0, 0);
    Texture* tex = load_texture("Assets/Font/Revel_PixelFont.png",GU_TRUE);
    Font2D* text = create_font2d(tex,0xFFFFFFFF,(ScePspIVector2){16,8},(ScePspFVector2){8,16},(ScePspFVector2){0,0},"abcdefghijklmnopqrstuvwxyz1234567890.,:;!?|'\"><# ");

    //Setup Controllers
    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    
    //int textpoint = 0;
    
    size_t total_time = 0;
    //strcpy(holder_text,dialogue_lines[0]);
    clear_screen();
    draw_text_at_pos((ScePspIVector2){0,0},"the text portion is done! wew!");
    
    //draw_text_at_pos((ScePspIVector2){5,3},dialogue_lines[1]);
    while(running){

        startFrame();
        pspDebugScreenSetXY(0, 0);
        // Blending
        sceGuBlendFunc(GU_ADD, GU_SRC_ALPHA, GU_ONE_MINUS_SRC_ALPHA, 0, 0);
        sceGuEnable(GU_BLEND);

        //Clear background to Bjack
        sceGuClearColor(0xFF111111);
        sceGuClearDepth(0);
        sceGuClear(GU_COLOR_BUFFER_BIT | GU_STENCIL_BUFFER_BIT | GU_DEPTH_BUFFER_BIT);

        sceCtrlReadBufferPositive(&pad, 1);
        //** Draw Everything Here **//
        draw_font2d(text, scroller, (ScePspFVector2){0,0});
        
        // if(total_time % (2000/1000) == 0){
        //     total_time = 0;
            //pspDebugScreenPrintf("%d", textpoint);
            // if (textpoint < strlen(holder_text)){
            //     char total_char[60] = "";
            //     for(int x = textpoint; x < strlen(holder_text); x++){
                    
            //          if( holder_text[x] == 32) {
            //             strcat(total_char, " ");
            //          } else {
            //             char charString[2];
            //             charString[0] = holder_text[x];
            //             charString[1] = '\0';
            //             strcat(total_char, charString);
                        
            //          }
                     
                     
                     

            //          if( holder_text[x] != 32 && holder_text[x] != '\n'){
            //              textpoint = x+1;
            //              strcat(scroller,total_char);
            //              break;
            //         }
            //     }

            //     // char charString[2];
            //     // charString[0] = chosentext[0][textpoint];
            //     // charString[1] = '\0';
            //     // strcat(scroller[0],charString);
            //     // textpoint += 1;
            // }
        //}


        //** End Draw Everything Here **//
        //Controller Processing
        if (pad.Buttons != 0)
        {
            if (pad.Buttons & PSP_CTRL_SQUARE){
            }
            if (pad.Buttons & PSP_CTRL_TRIANGLE){
            }
            if (pad.Buttons & PSP_CTRL_CIRCLE){
            }
            if (pad.Buttons & PSP_CTRL_CROSS){
            }
            if (pad.Buttons & PSP_CTRL_UP){
            }
            if (pad.Buttons & PSP_CTRL_DOWN){
            }
            if (pad.Buttons & PSP_CTRL_LEFT){
            }
            if (pad.Buttons & PSP_CTRL_RIGHT){
            }
            if (pad.Buttons & PSP_CTRL_SELECT){
            }
            if (pad.Buttons & PSP_CTRL_START){
            }
        }
        //total_time += 1;
        endFrame();
    }

cleanup:

    // Terminate Graphics
    termGraphics();

    // Exit Game
    sceKernelExitGame();
    return 0;
}