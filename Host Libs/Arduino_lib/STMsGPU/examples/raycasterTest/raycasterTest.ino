#include <STMsGPU.h>

// ---------------------------------------------------------- //
/*
 * which pin arduino must check, 
 * but by default this functianality is disabled to save
 * RAM and ROM memory.
 * To enable it go to library STMsGPU.h and 
 * set define: 'REMOVE_HARDWARE_BSY' to 0
 */ 
//#define CHK_GPU_BSY_PIN 2

/* BE CAREFULL!! USED ONLY HARDWARE SERIAL PORT!!
*  If your board have only ONE hardware serial,
*  then you MUST use SoftWareSerial instead!
*  On STM32 boards used Serial1 on PA9 and PA10.
*/
//STMGPU gpu(CHK_GPU_BSY_PIN); // use hardware BSY check, pin used
STMGPU gpu; // use software BSY check, no pin used
// ---------------------------------------------------------- //

/* pins to attach analoge sticks
 * at lest need two stick whith 2 move direction.
 * One stick for Left Right, Up, Down directions,
 * and one for turn clockwise and backwise.
 */
#define PIN_DIR_X  0 // Analoge A0 pin
#define PIN_DIR_Y  1 // Analoge A1 pin
#define PIN_DIR_LR 2 // Analoge A2 pin

#define PIN_NUM_COUNT 3 // total pins to read

#define FPS_MIN 10  // \__ frame limit values
#define FPS_MAX 30  // /

#define FPS_CALC(a) (1000/a) // 30 frames per second (1000/30 == 33)

// this is need to load textures from *.tle file 
// located on SD card - correctly
#define MAX_TILES 5 //
#define RAM_BASE 0
#define TLE_START 0
#define TILE_SET_W 5 // this is width of tileSet in tiles ( one tile width == 8 pixels)

#define X_POS   (50)
#define Y_POS   (-150)
#define LR_POS  (1)

// these array need to store calibrated values X, Y, or LR
// (center positions of sticks)
int calValueXYLR[PIN_NUM_COUNT] = { 0 };

// pins number stored in array for easy
// accses in cicle
uint8_t pinsToReadXYLR[PIN_NUM_COUNT] = {
  PIN_DIR_X,
  PIN_DIR_Y,
  PIN_DIR_LR
};

// store directions in array, codes of directions are defined in STMsGPU.h
uint8_t dirArray[PIN_NUM_COUNT][2] = {
  { MOVE_LEFT, MOVE_RIGHT },
  { MOVE_UP, MOVE_DOWN },
  { MOVE_CLOCKWISE_R, MOVE_CLOCKWISE_L }
};

uint32_t lastMicros, thisMicros;
uint32_t delayLong = 18; // calibrated and tested value

// ---------------------------------------------------------- //

void setup() {
  // different speeds can be found in library STMsGPU.h
  // if call gpu.begin() without param speed will be BAUD_SPEED_57600
  gpu.begin(BAUD_SPEED_1M); // BAUD_SPEED_1M = 1,000,000 bod/s

  /* load MAX_TILES tiles to sGPU's RAM at RAM_BASE position in it's RAM,
  *  from tileFileName,
  *  located on SD card attached to STM32 sGPU
  *  TLE_START - nunber of tile in tileset from which tiles will be loaded
  *  file name must respond to 8.3 name system
  *  8 chars max for filename, 3 chars max for file extension
  *  sGPU add *.tle extension automatically
  */
  gpu.loadTileSet16x16("w00L00t", TILE_SET_W, RAM_BASE, TLE_START, MAX_TILES);
  //gpu.loadTileMap("w00L00m"); // future feature

  //gpu.setCamPosition(X_POS, Y_POS, LR_POS);
  // different mods can be found in library STMsGPU.h
  //gpu.setTextureMode(TEXTURE_MODE_1); // default value == TEXTURE_MODE_1 (16x16 tiles)
  //gpu.setWallCollision(false); // IDCLIP == false; by default value == false
  //gpu.setSkyFloor(COLOR_GREEN, COLOR_DARKCYAN); // set color for sky and floor

  gpu.setTextColor(COLOR_WHITE, COLOR_BLACK); // for debug

  // say to gpu: "render single frame"
  // force render, overwise user will see something only after moving sticks
  gpu.fillScreen(COLOR_DARKGREY); // make borders around render window
  gpu.renderFrame();

  // first call for calibration center values
  calibrateAnalogeSticks();
}

void loop() {
  uint8_t camPos = checkAnalogeSticks(); // get move directions

  if(camPos != 0) { // if user not move, gpu don`t render frame.
    gpu.moveCamera(camPos); // send direction moves
    gpu.renderFrame();

    // delay for frame limit, sGPU can glitch from DDoS like that
    // it also decrese input lag (i`m siriosly, this is not joke!)
    limitFPS(); // hmm... replace by non blocking func seems good idea...
  }
}

void limitFPS(void)
{
  // calculate how long we draw previous frame
  lastMicros = thisMicros;
  thisMicros = millis();
  uint32_t resultMicros = thisMicros - lastMicros;
    
  uint16_t fps = 1000/resultMicros;
 
  if(fps <= FPS_MIN) --delayLong;    //trying to get FPS_MIN fps
  if(fps >= FPS_MAX) ++delayLong;    //but not more FPS_MAX fps

  //gpu.setCursor(0, 200); // \_for debug only
  //gpu.print(renderTime); // / in us
  
  //everynting is stop... this is wrong...
  gpu.iDelay(delayLong*2); //slow down! it`s too faast!
}

void calibrateAnalogeSticks(void)
{
  // Before run, be shure what stiks are at center position!
  for (uint8_t i = 0; i < PIN_NUM_COUNT; i++) {
    calValueXYLR[i] = getPinValue(pinsToReadXYLR[i]);
    gpu.iDelay(50);
  }
}

inline uint8_t getPinValue(int pin)
{
  int newValuePin = analogRead(pin);

  // approximate newValuePin data, and encode to ASCII 
  // (just because it works and i left it as is)
  return (newValuePin * 9 / 1024) + 48;
}

uint8_t checkAnalogeSticks(void)
{
  // store new direction value
  uint8_t direction =0;
  int newValueXYLR = 0;

  for(uint8_t count =0; count < PIN_NUM_COUNT; count++) {
    // get number of pin to read, and store result
    newValueXYLR = getPinValue(pinsToReadXYLR[count]);
    
    // stick position changed?
    if(newValueXYLR != calValueXYLR[count]) {
      // is it more than calibrated value? (in both case apply direction)
      direction |= (newValueXYLR > calValueXYLR[count] ? dirArray[count][0] : dirArray[count][1]);
    }
  }
  // return combined directions in single byte
  return direction;
}
