#include "displayTask.h"

void resetPanel()
{
    // hardware reset
    gpio_set_level(RST, LOW);
    vTaskDelay(10/portTICK_PERIOD_MS);
    gpio_set_level(RST, HIGH);
    vTaskDelay(10/portTICK_PERIOD_MS);
}

void displayImmage(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], bool p_displayFast)
{   
    g_display.setFullWindow();
    g_display.firstPage();

    if(p_displayFast)
    {
        // display.setCursor(0, 0);
        for(int row = 0; row < DISPLAY_WIDTH; row += 1)
        {
            for(int col = 0; col < DISPLAY_HEIGHT; col += 1)
            {
                g_display.drawPixel(col, row, g_matrixToDisplay[row][col]);
            }
        }

        g_display.display(true);
    }
    else
    {
        do 
        {
            // display.setCursor(0, 0);
            for(int row = 0; row < DISPLAY_WIDTH; row+= 1)
            {
                for(int col = 0; col < DISPLAY_HEIGHT; col += 1)
                {
                    g_display.drawPixel(col, row, g_matrixToDisplay[row][col]);
                }
            }
        } while(g_display.nextPage());
    }  
}

void displayBlackPixels(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], bool p_displayFast)
{   
    // g_display.setFullWindow();
    // g_display.firstPage();

    if(p_displayFast)
    {
        // display.setCursor(0, 0);
        for(int row = 0; row < DISPLAY_WIDTH; row += 1)
        {
            for(int col = 0; col < DISPLAY_HEIGHT; col += 1)
            {
                if(g_matrixToDisplay[row][col] == BLACK_PIXEL)
                {
                    g_display.drawPixel(col, row, g_matrixToDisplay[row][col]);
                }
            }
        }

        g_display.display(true);
    }
    else
    {
        do 
        {
            // display.setCursor(0, 0);
            for(int row = 0; row < DISPLAY_WIDTH; row += 1)
            {
                for(int col = 0; col < DISPLAY_HEIGHT; col += 1)
                {
                    if(g_matrixToDisplay[row][col] == BLACK_PIXEL)
                    {
                        g_display.drawPixel(col, row, g_matrixToDisplay[row][col]);
                    }
                }
            }
        }while(g_display.nextPage());
    }  
}

int getNumberOfDigits(int p_number)
{
    int numberOfDigits = 0;
    while(p_number/10 > 0)
    {
        numberOfDigits ++;
        p_number /= 10;
    }
    return numberOfDigits + 1;
} 

int intPow(int p_number, int p_exponent)
{
    int result = p_number;
    for(int i = 0; i < p_exponent - 1; i++)
    {
        result *= p_number;
    }

    return result;
}

int getOneOffsetValue(int p_number, int p_digitsInNumber)
{
    int offset = 0;
    if(p_number % 10 == 1)
    {
        offset += WHOLE_NUMBER_ADJUSTMENT;
    }
    if((p_number / intPow(10, p_digitsInNumber-1)) == 1)
    {
        offset -= WHOLE_NUMBER_ADJUSTMENT;
    }

    return offset;
}

void pullWithinOXBounds(int &p_xScreenPos)
{
    if(p_xScreenPos < 0) p_xScreenPos = 0;
    if(p_xScreenPos > DISPLAY_WIDTH) p_xScreenPos = DISPLAY_WIDTH;
}

void pullWithinOYBounds(int &p_yScreenPos)
{
    if(p_yScreenPos < 0) p_yScreenPos = 0;
    if(p_yScreenPos > DISPLAY_HEIGHT) p_yScreenPos = DISPLAY_HEIGHT;
}

bool isWithinOXBounds(int &p_xScreenPos)
{
    if(p_xScreenPos < 0) return false;
    if(p_xScreenPos > DISPLAY_WIDTH) return false;
    
    return true;
}

bool isWithinOYBounds(int &p_yScreenPos)
{
    if(p_yScreenPos < 0) return false;
    if(p_yScreenPos > DISPLAY_HEIGHT) return false;

    return true;
}

bool checkValueWithinBounds(int p_value, int p_lowerBound, int p_upperBound)
{
    if(p_value < p_lowerBound || p_value >= p_upperBound)
    {
        return false;
    }
    return true;
}

// void addSignCentered(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], const bool *p_sign_pointer, int p_width, int p_height, float p_scale)
// {   
    
//     int xStartPos = (DISPLAY_WIDTH - p_width * p_scale) / 2; // to center horisontally
//     int yStartPos = (DISPLAY_HEIGHT - p_height * p_scale) / 2;

//     // takes around 0.8 microseconds to read one digit from flash

//     for(int row = yStartPos; row < yStartPos + p_height* p_scale; row++)
//     {
//         for(int col = xStartPos; col < xStartPos + p_width * p_scale; col ++)
//         {      
//             uint8_t pixelToWrite;

//             pixelToWrite = pgm_read_byte(p_sign_pointer + int((row-yStartPos)/p_scale) * p_width) + int((col-xStartPos)/p_scale);

//             if(checkValueWithinBounds(row, 0, DISPLAY_HEIGHT)  
//                 && checkValueWithinBounds(col, 0, DISPLAY_WIDTH))
//             {
//                 p_immageBuffer[row][col] = pixelToWrite;
//             }
//         }
//     }
// }

void addSignCentered(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], const bool *p_sign_pointer, int p_width, int p_height, float p_scale)
{   
    
    int xStartPos = (DISPLAY_WIDTH - p_width * p_scale) / 2; // to center horisontally
    int yStartPos = (DISPLAY_HEIGHT - p_height * p_scale) / 2;

    // takes around 0.8 microseconds to read one digit from flash

    for(int row = yStartPos; row < yStartPos + p_height* p_scale; row++)
    {
        for(int col = xStartPos; col < xStartPos + p_width * p_scale; col ++)
        {      
            uint8_t pixelToWrite;
            
            int y_index = int((row-yStartPos)/p_scale);
            int x_index = int((col-xStartPos)/p_scale);
            int flat_index = int(y_index * p_width + x_index);
            pixelToWrite = pgm_read_byte(&(p_sign_pointer[flat_index]));
        
            if(checkValueWithinBounds(row, 0, DISPLAY_HEIGHT)  
                && checkValueWithinBounds(col, 0, DISPLAY_WIDTH))
            {
                p_immageBuffer[row][col] = pixelToWrite;
            }
        }
    }
}
// add scale function
// add function that displays number with one or two presision decimals
void addNumberCentered(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], int p_numberToWrite, int p_OxImmageOffset, int p_OyImmageOffset, float p_scale)
{   
    int digitsToWrite = getNumberOfDigits(p_numberToWrite);
    
    if(digitsToWrite > 2 && p_scale == 1)
    {
        p_scale = 1.0 / (0.45 * digitsToWrite);
    }

    int xStartPos = (DISPLAY_WIDTH - digitsToWrite * FONT_IMAGE_WIDTH * p_scale) / 2; // to center horisontally
    int yStartPos = (DISPLAY_HEIGHT - FONT_IMAGE_HEIGHT * p_scale) / 2;

    // takes around 0.8 microseconds to read one digit from flash
    for(int digitIdx = 0; digitIdx < digitsToWrite; digitIdx ++)
    {
        int digit = p_numberToWrite % 10;

        int startColIdx = xStartPos + FONT_IMAGE_WIDTH * p_scale * (digitsToWrite - digitIdx - 1);
        int stopColIdx = xStartPos + FONT_IMAGE_WIDTH * p_scale * (digitsToWrite - digitIdx - 1) + FONT_IMAGE_WIDTH * p_scale;

        for(int row = yStartPos; row < yStartPos + FONT_IMAGE_HEIGHT * p_scale; row++)
        {
            for(int col = startColIdx; col < stopColIdx; col ++)
            {      
                uint8_t pixelToWrite;

                pixelToWrite = pgm_read_byte(&g_digitVector[digit][int((row-yStartPos)/p_scale)][int((col-startColIdx)/p_scale)]);
                

                if(checkValueWithinBounds(row + p_OyImmageOffset, 0, DISPLAY_HEIGHT)  
                    && checkValueWithinBounds(col + p_OxImmageOffset, 0, DISPLAY_WIDTH))
                {
                    p_immageBuffer[row + p_OyImmageOffset][col + p_OxImmageOffset] = pixelToWrite;
                }
            }
        }
        p_numberToWrite /= 10;
    }
}

void displaySlantedDot(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], int p_x, int p_y, int p_width)
{
    int slant = 0;

    for(int i = 0; i < p_width; i++)
    {
        for(int j = 0; j < p_width; j++)
        {
            p_immageBuffer[p_x - i][p_y - j + slant] = BLACK_PIXEL;
        }
        if(i % DOT_SLANT_RATE_PX == 0)
        {
            slant += 1;
        }
    }
}

void addDoubleCentered(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], double p_numberToWrite, int p_decimalsToShow, int p_OxImmageOffset, int p_OyImmageOffset, float p_scale)
{   
    p_numberToWrite = (p_numberToWrite < 0) ? (p_numberToWrite * (-1)) : p_numberToWrite;

    int numberToShow = p_numberToWrite * pow(10, p_decimalsToShow);
    int digitsToWrite = getNumberOfDigits(numberToShow);
    
    if(digitsToWrite < p_decimalsToShow + 1) 
    {
        digitsToWrite = p_decimalsToShow + 1;
    }

    if(digitsToWrite > 2 && p_scale == 1)
    {
        p_scale = 1.0 / (0.45 * digitsToWrite);
    }

    int digitOffsetForDot = 0;

    int xStartPos = (DISPLAY_WIDTH - digitsToWrite * FONT_IMAGE_WIDTH * p_scale) / 2; // to center horisontally
    int yStartPos = (DISPLAY_HEIGHT - FONT_IMAGE_HEIGHT * p_scale) / 2;

    // takes around 0.8 microseconds to read one digit from flash
    for(int digitIdx = 0; digitIdx < digitsToWrite; digitIdx ++)
    {
        int digit = numberToShow % 10;

        int startColIdx = xStartPos + FONT_IMAGE_WIDTH * p_scale * (digitsToWrite - digitIdx - 1) - digitOffsetForDot;
        int stopColIdx = xStartPos + FONT_IMAGE_WIDTH * p_scale * (digitsToWrite - digitIdx - 1) + FONT_IMAGE_WIDTH * p_scale - digitOffsetForDot;

        for(int row = yStartPos; row < yStartPos + FONT_IMAGE_HEIGHT * p_scale; row++)
        {
            for(int col = startColIdx; col < stopColIdx; col ++)
            {      
                uint8_t pixelToWrite;

                pixelToWrite = pgm_read_byte(&g_digitVector[digit][int((row-yStartPos)/p_scale)][int((col-startColIdx)/p_scale)]);

                if(checkValueWithinBounds(row + p_OyImmageOffset, 0, DISPLAY_HEIGHT)  
                    && checkValueWithinBounds(col + p_OxImmageOffset, 0, DISPLAY_WIDTH))
                {
                    p_immageBuffer[row + p_OyImmageOffset][col + p_OxImmageOffset] = pixelToWrite;
                }
            }
        }

        if(digitIdx + 1 == p_decimalsToShow)
        {
            digitOffsetForDot = EMPTY_SPACE_FOR_DOT * p_scale;
        }

        if(digitIdx == p_decimalsToShow)
        {
            displaySlantedDot(p_immageBuffer, yStartPos + FONT_IMAGE_HEIGHT * p_scale, startColIdx + FONT_IMAGE_WIDTH * p_scale + DOT_STANDARD_OFFSET * p_scale, DOT_STANDARD_SIZE * p_scale);
        }
        
        numberToShow /= 10;
    }
}

// add scale function
// add function that displays number with one or two presision decimals
void addStampCentered(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], int p_stampToWrite, int p_OxImmageOffset, int p_OyImmageOffset, float p_scale)
{       
    int xStartPos = (DISPLAY_WIDTH - STAMP_WIDTH * p_scale) / 2; // to center horisontally
    int yStartPos = (DISPLAY_HEIGHT - STAMP_HEIGHT * p_scale) / 2;

    // takes around 0.8 microseconds to read one digit from flash

    for(int row = yStartPos; row < yStartPos + STAMP_HEIGHT * p_scale; row++)
    {
        int startColIdx = xStartPos + STAMP_WIDTH * p_scale;
        int stopColIdx = xStartPos + STAMP_WIDTH * p_scale + STAMP_WIDTH * p_scale;
        for(int col = startColIdx; col < stopColIdx; col ++)
        {      
            uint8_t pixelToWrite;

            pixelToWrite = pgm_read_byte(&g_stampVector[p_stampToWrite][int((row-yStartPos)/p_scale)][int((col-xStartPos)/p_scale)]);
            
            if(checkValueWithinBounds(row + p_OyImmageOffset, 0, DISPLAY_HEIGHT)  
                && checkValueWithinBounds(col + p_OxImmageOffset, 0, DISPLAY_WIDTH))
            {
                p_immageBuffer[row + p_OyImmageOffset][col + p_OxImmageOffset] = pixelToWrite;
            }
        }
    }
    p_stampToWrite /= 10;
}

void clearImmage(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT])
{
    for(int row = 0; row < DISPLAY_HEIGHT; row++)
    {
        for(int col = 0; col < DISPLAY_WIDTH; col ++)
        {
            p_immageBuffer[row][col] = 1;
        }
    }
}