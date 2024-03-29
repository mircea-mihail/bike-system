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
                g_display.drawPixel(row, col, g_matrixToDisplay[row][col]);
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
                    g_display.drawPixel(row, col, g_matrixToDisplay[row][col]);
                }
            }
        }while(g_display.nextPage());
    }  
}

void displayBlackPixels(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], bool p_displayFast)
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
                if(g_matrixToDisplay[row][col] == BLACK)
                {
                    g_display.drawPixel(row, col, g_matrixToDisplay[row][col]);
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
                    if(g_matrixToDisplay[row][col] == BLACK)
                    {
                        g_display.drawPixel(row, col, g_matrixToDisplay[row][col]);
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

void addNumberCentered(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT], int p_numberToWrite)
{   
    int digitsToWrite = getNumberOfDigits(p_numberToWrite);
    
    int xStartPos = (DISPLAY_WIDTH - digitsToWrite * FONT_IMAGE_WIDTH) / 2; // to center horisontally
    int yStartPos = (DISPLAY_HEIGHT - FONT_IMAGE_HEIGHT) / 2;

    // offset the whole number for recentering after "one" adjustment
    int wholeNumberOffset = getOneOffsetValue(p_numberToWrite, digitsToWrite);

    // clear previous immage
    for(int digitIdx = 0; digitIdx < digitsToWrite; digitIdx ++)
    {
        int digit = p_numberToWrite % 10;

        for(int row = yStartPos; row < yStartPos + FONT_IMAGE_HEIGHT; row++)
        {
            for(int col = xStartPos + FONT_IMAGE_WIDTH *(digitsToWrite - digitIdx - 1); col < xStartPos + FONT_IMAGE_WIDTH * (digitsToWrite - digitIdx - 1) + FONT_IMAGE_WIDTH; col ++)
            {
                p_immageBuffer[row][col] = WHITE; 
            }
        }
    }

    // takes around 0.8 microseconds to read one digit from flash
    for(int digitIdx = 0; digitIdx < digitsToWrite; digitIdx ++)
    {
        int digit = p_numberToWrite % 10;

        for(int row = yStartPos; row < yStartPos + FONT_IMAGE_HEIGHT; row++)
        {
            for(int col = xStartPos + FONT_IMAGE_WIDTH *(digitsToWrite - digitIdx - 1); col < xStartPos + FONT_IMAGE_WIDTH * (digitsToWrite - digitIdx - 1) + FONT_IMAGE_WIDTH; col ++)
            {
                int digitOffset = 0;           // used to mitigate the one being too far from the other digits

                if(digit == 1 && digitIdx == digitsToWrite - 1)
                {
                    digitOffset += DIGIT_ONE_SHIFT;
                }
                if(digit == 1 && digitIdx == 0)
                {
                    digitOffset -= DIGIT_ONE_SHIFT;
                }
                
                uint8_t pixelToWrite = pgm_read_byte(&g_digitVector[digit][row-yStartPos][col-xStartPos]);
                if(pixelToWrite == BLACK)
                {
                    p_immageBuffer[row][col + digitOffset + wholeNumberOffset] = pixelToWrite;

                }                
            }
        }
        p_numberToWrite /= 10;
    }
}

void clearDisplay(uint8_t p_immageBuffer[DISPLAY_WIDTH][DISPLAY_HEIGHT])
{
    for(int row = 0; row < DISPLAY_HEIGHT; row++)
    {
        for(int col = 0; col < DISPLAY_WIDTH; col ++)
        {
            p_immageBuffer[row][col] = 1;
        }
    }
}