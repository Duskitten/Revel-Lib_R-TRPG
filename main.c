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
char fulltext[1037] = "\0";
char holder_text[1037] = "\0";
char temp_text[1037] = "\0";
char core_area[1037] = "                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            \n                                                            ";
char* dialogue_lines[] = {
    //intro text
    " ",
    "#######      ######   ######  #######",
    "##           ##    #  ##    # ##     ",
    "##  ###  ##  ######   ######  ##  ###",
    "##    #      ##    #  ##      ##    #",
    "#######      ##    #  ##      #######",
    ">> What do you do?",
    //textlines
    ">> You wake up on a boat.",
    ">> The sky above you is a soft gray of a foggy morning.",
    ">> You stand up and walk over to the edge of the boat.",
    ">> You sit and feel the boat gently rock back",
    "and fourth calmly."
};

int dialogue_hopper[]= {
    11, //How many to count over
    1,2,3,4,5,0,7,8,0,0,6, //counted over
    1, 
    9,
    1,
    10,11,

};

int dialogue_options[] = {

};

int save_data[] = {
    0, //Has begun game.
};

char scroller[1037] = "\0";

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

int initial_compile = 1;
int current_set = 0;
int accepting_input = 0;
int textpoint = 0;

void compile_text_lines(){
    if(initial_compile){
        initial_compile = 0;
        textpoint = 0;
        memset(holder_text,0,1037);
        memset(temp_text,0,1037);
        memset(fulltext,0,1037);
        for(int x = 0; x < (int)dialogue_hopper[current_set]; x++){
            strcat(holder_text, dialogue_lines[ dialogue_hopper[(current_set+1) + x ]]);
            strcat(holder_text, "\n");
        }
    }

    if (textpoint < strlen(holder_text)){
        char charString[1037];
        if(holder_text[textpoint] == 32){
            for(int x = textpoint; x < strlen(holder_text); x++){
                char string2[2];
                string2[0] = holder_text[x];
                string2[1] = '\0';
                strcat(temp_text,string2);
                if(holder_text[x] != 32){
                    textpoint = x + 1;
                    break;
                }
            }
         }
         else {
            char string2[2];
            string2[0] = holder_text[textpoint];
            string2[1] = '\0';
            strcat(temp_text,string2);
            textpoint += 1;
         }
         
    } else {
        accepting_input = 1;
    }
    strcpy(fulltext, temp_text);
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
    Texture* tex = load_texture("Assets/Font/monofur/monofurmono_regular.png",GU_TRUE);
    Font2D* text = create_font2d(tex,0xFFFFFFFF,(ScePspIVector2){16,8},(ScePspFVector2){8,16},(ScePspFVector2){0,0},"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890-=_+.,<>?/\\'\"!@#$%^&*()[]{}|:; ");

    //Setup Controllers
    SceCtrlData pad;
    sceCtrlSetSamplingCycle(0);
    sceCtrlSetSamplingMode(PSP_CTRL_MODE_ANALOG);
    
    //int textpoint = 0;
    
    
    size_t total_time = 0;
    
    clear_screen();

    compile_text_lines();
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
        
        if(total_time % 1 == 0){
            total_time = 0;
            compile_text_lines();
            draw_text_at_pos((ScePspIVector2){0,0},fulltext);
        }


        //** End Draw Everything Here **//
        //Controller Processing
        if (pad.Buttons != 0 && accepting_input)
        {
            if (pad.Buttons & PSP_CTRL_SQUARE){
            }
            if (pad.Buttons & PSP_CTRL_TRIANGLE){
            }
            if (pad.Buttons & PSP_CTRL_CIRCLE){
            }
            if (pad.Buttons & PSP_CTRL_CROSS){
                accepting_input = 0;
                current_set = 12;
                initial_compile = 1;
                clear_screen();
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
        total_time += 1;
        endFrame();
    }

cleanup:

    // Terminate Graphics
    termGraphics();

    // Exit Game
    sceKernelExitGame();
    return 0;
}