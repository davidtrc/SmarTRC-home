#ifndef _EPDPAINT_H    /* Guard against multiple inclusion */
    #define _EPDPAINT_H

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

// Display Orientation
#define ROTATE_0            0
#define ROTATE_90           1
#define ROTATE_180          2
#define ROTATE_270          3

#include "fonts.h"

void Paint_Init(unsigned char* image, int width, int height);
void Paint_Clear(int colored);
int  GetWidth(void);
void SetWidth(int width);
int  GetHeight(void);
void SetHeight(int height);
int  GetRotate(void);
void SetRotate(int rotate);
unsigned char* GetImage(void);
void DrawAbsolutePixel(int x, int y, int colored);
void DrawPixel(int x, int y, int colored);
void DrawCharAt(int x, int y, char ascii_char, sFONT* font, int colored);
void DrawStringAt(int x, int y, const char* text, sFONT* font, int colored);
void DrawLine(int x0, int y0, int x1, int y1, int colored);
void DrawHorizontalLine(int x, int y, int width, int colored);
void DrawVerticalLine(int x, int y, int height, int colored);
void DrawRectangle(int x0, int y0, int x1, int y1, int colored);
void DrawFilledRectangle(int x0, int y0, int x1, int y1, int colored);
void DrawCircle(int x, int y, int radius, int colored);
void DrawFilledCircle(int x, int y, int radius, int colored);

    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EPDPAINT_H */

/* *****************************************************************************
 End of File
 */
