#include "epdpaint.h"

unsigned char* image;
int width;
int height;
int rotate;

void Paint_Init(unsigned char* ext_image, int ext_width, int ext_height){
    rotate = ROTATE_0;
    image = ext_image;
    width = ext_width % 8 ? ext_width + 8 - (ext_width % 8) : ext_width;
    height = ext_height;
}

/**
 *  @brief: clear the image
 */
void Paint_Clear(int colored) {
    int x = 0;
    int y = 0;
    
    for (x = 0; x < width; x++) {
        for (y = 0; y < height; y++) {
            DrawAbsolutePixel(x, y, colored);
        }
    }
}

/**
 *  @brief: this draws a pixel by absolute coordinates.
 *          this function won't be affected by the rotate parameter.
 */
void DrawAbsolutePixel(int x, int y, int colored) {
    if (x < 0 || x >= width || y < 0 || y >= height) {
        return;
    }
	if (colored) {
		image[(x + y * width) / 8] &= ~(0x80 >> (x % 8));
	} else {
		image[(x + y * width) / 8] |= 0x80 >> (x % 8);
	}
}

/**
 *  @brief: Getters and Setters
 */
unsigned char* GetImage(void) {
    return image;
}

int GetWidth(void) {
    return width;
}

void SetWidth(int ext_width) {
    width = ext_width % 8 ? ext_width + 8 - (ext_width % 8) : ext_width;
}

int GetHeight(void) {
    return height;
}

void SetHeight(int ext_height) {
    height = ext_height;
}

int GetRotate(void) {
    return rotate;
}

void SetRotate(int ext_rotate){
    rotate = ext_rotate;
}

/**
 *  @brief: this draws a pixel by the coordinates
 */
void DrawPixel(int x, int y, int colored) {
    int point_temp;
    if (rotate == ROTATE_0) {
        if(x < 0 || x >= width || y < 0 || y >= height) {
            return;
        }
        DrawAbsolutePixel(x, y, colored);
    } else if (rotate == ROTATE_90) {
        if(x < 0 || x >= height || y < 0 || y >= width) {
          return;
        }
        point_temp = x;
        x = width - y;
        y = point_temp;
        DrawAbsolutePixel(x, y, colored);
    } else if (rotate == ROTATE_180) {
        if(x < 0 || x >= width || y < 0 || y >= height) {
          return;
        }
        x = width - x;
        y = height - y;
        DrawAbsolutePixel(x, y, colored);
    } else if (rotate == ROTATE_270) {
        if(x < 0 || x >= height || y < 0 || y >= width) {
          return;
        }
        point_temp = x;
        x = y;
        y = height - point_temp;
        DrawAbsolutePixel(x, y, colored);
    }
}

/**
 *  @brief: this draws a charactor on the pattern buffer but not refresh
 */
void DrawCharAt(int x, int y, char ascii_char, sFONT* font, int colored) {
    int i, j;
    unsigned int char_offset = (ascii_char - ' ') * font->Height * (font->Width / 8 + (font->Width % 8 ? 1 : 0));
    const unsigned char* ptr = &font->table[char_offset];

    for (j = 0; j < font->Height; j++) {
        for (i = 0; i < font->Width; i++) {
            if ((*ptr) & (0x80 >> (i % 8))) {
                DrawPixel(x + i, y + j, colored);
            }
            if (i % 8 == 7) {
                ptr++;
            }
        }
        if (font->Width % 8 != 0) {
            ptr++;
        }
    }
}

/**
*  @brief: this displays a string on the pattern buffer but not refresh
*/
void DrawStringAt(int x, int y, const char* text, sFONT* font, int colored) {
    const char* p_text = text;
    unsigned int counter = 0;
    int refcolumn = x;
    
    /* Send the string character by character on EPD */
    while (*p_text != 0) {
        /* Display one character on EPD */
        DrawCharAt(refcolumn, y, *p_text, font, colored);
        /* Decrement the column position by 16 */
        refcolumn += font->Width;
        /* Point on the next character */
        p_text++;
        counter++;
    }
}

/**
*  @brief: this draws a line on the pattern buffer
*/
void DrawLine(int x0, int y0, int x1, int y1, int colored) {
    /* Bresenham algorithm */
    int dx = x1 - x0 >= 0 ? x1 - x0 : x0 - x1;
    int sx = x0 < x1 ? 1 : -1;
    int dy = y1 - y0 <= 0 ? y1 - y0 : y0 - y1;
    int sy = y0 < y1 ? 1 : -1;
    int err = dx + dy;

    while((x0 != x1) && (y0 != y1)) {
        DrawPixel(x0, y0 , colored);
        if (2 * err >= dy) {     
            err += dy;
            x0 += sx;
        }
        if (2 * err <= dx) {
            err += dx; 
            y0 += sy;
        }
    }
}

/**
*  @brief: this draws a horizontal line on the pattern buffer
*/
void DrawHorizontalLine(int x, int y, int line_width, int colored) {
    int i;
    for (i = x; i < x + line_width; i++) {
        DrawPixel(i, y, colored);
    }
}

/**
*  @brief: this draws a vertical line on the pattern buffer
*/
void DrawVerticalLine(int x, int y, int line_height, int colored) {
    int i;
    for (i = y; i < y + line_height; i++) {
        DrawPixel(x, i, colored);
    }
}

/**
*  @brief: this draws a rectangle
*/
void DrawRectangle(int x0, int y0, int x1, int y1, int colored) {
    int min_x, min_y, max_x, max_y;
    min_x = x1 > x0 ? x0 : x1;
    max_x = x1 > x0 ? x1 : x0;
    min_y = y1 > y0 ? y0 : y1;
    max_y = y1 > y0 ? y1 : y0;
    
    DrawHorizontalLine(min_x, min_y, max_x - min_x + 1, colored);
    DrawHorizontalLine(min_x, max_y, max_x - min_x + 1, colored);
    DrawVerticalLine(min_x, min_y, max_y - min_y + 1, colored);
    DrawVerticalLine(max_x, min_y, max_y - min_y + 1, colored);
}

/**
*  @brief: this draws a filled rectangle
*/
void DrawFilledRectangle(int x0, int y0, int x1, int y1, int colored) {
    int min_x, min_y, max_x, max_y;
    int i;
    min_x = x1 > x0 ? x0 : x1;
    max_x = x1 > x0 ? x1 : x0;
    min_y = y1 > y0 ? y0 : y1;
    max_y = y1 > y0 ? y1 : y0;
    
    for (i = min_x; i <= max_x; i++) {
      DrawVerticalLine(i, min_y, max_y - min_y + 1, colored);
    }
}

/**
*  @brief: this draws a circle
*/
void DrawCircle(int x, int y, int radius, int colored) {
    /* Bresenham algorithm */
    int x_pos = -radius;
    int y_pos = 0;
    int err = 2 - 2 * radius;
    int e2;

    do {
        DrawPixel(x - x_pos, y + y_pos, colored);
        DrawPixel(x + x_pos, y + y_pos, colored);
        DrawPixel(x + x_pos, y - y_pos, colored);
        DrawPixel(x - x_pos, y - y_pos, colored);
        e2 = err;
        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;
            if(-x_pos == y_pos && e2 <= x_pos) {
              e2 = 0;
            }
        }
        if (e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while (x_pos <= 0);
}

/**
*  @brief: this draws a filled circle
*/
void DrawFilledCircle(int x, int y, int radius, int colored) {
    /* Bresenham algorithm */
    int x_pos = -radius;
    int y_pos = 0;
    int err = 2 - 2 * radius;
    int e2;

    do {
        DrawPixel(x - x_pos, y + y_pos, colored);
        DrawPixel(x + x_pos, y + y_pos, colored);
        DrawPixel(x + x_pos, y - y_pos, colored);
        DrawPixel(x - x_pos, y - y_pos, colored);
        DrawHorizontalLine(x + x_pos, y + y_pos, 2 * (-x_pos) + 1, colored);
        DrawHorizontalLine(x + x_pos, y - y_pos, 2 * (-x_pos) + 1, colored);
        e2 = err;
        if (e2 <= y_pos) {
            err += ++y_pos * 2 + 1;
            if(-x_pos == y_pos && e2 <= x_pos) {
                e2 = 0;
            }
        }
        if(e2 > x_pos) {
            err += ++x_pos * 2 + 1;
        }
    } while(x_pos <= 0);
}
