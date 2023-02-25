#include "ColorFunctions.h"


int colorScaler(int color, int scalar, int max){   
  int r = (color >> 16 & 0xFF) * scalar / max;
  int g = (color >> 8 & 0xFF) * scalar/ max;
  int b = (color & 0xFF) * scalar/ max;
  return (r << 16) | (g << 8) | b;
}

int colorMapper(int colorIndex){
  int r, g, b;
  float hue = (float(colorIndex) / N_COLORS)  * 360.0;
  float c = 255.0;
  float x = c * (1 - abs(fmod(hue / 60.0, 2) - 1));
  float m = 0;
  if(hue >= 0 && hue < 60){
    r = c;
    g = x;
    b = 0;
  }
  else if(hue >= 60 && hue < 120){
    r = x;
    g = c;
    b = 0;
  }
  else if(hue >= 120 && hue < 180){
    r = 0;
    g = c;
    b = x;
  }
  else if(hue >= 180 && hue < 240){
    r = 0;
    g = x;
    b = c;
  }
  else if(hue >= 240 && hue < 300){
    r = x;
    g = 0;
    b = c;
  }
  else if(hue >= 300 && hue < 360){
    r = c;
    g = 0;
    b = x;
  }
  return (r << 16) | (g << 8) | b;
}
