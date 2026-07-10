#include "display7seg.h"

static const uint8_t digit_encoding[10] = {0b00111111, 0b00000110, 0b01011011, 0b01001111, 0b01100110, 0b01101101, 0b01111101, 0b00000111, 0b01111111, 0b01101111 };
static const uint8_t D0_segment_map[8] = {14, 15, 9, 10, 11, 13, 12, 8}; // digit à gauche
static const uint8_t D1_segment_map[8] = {17, 19, 7, 6, 4, 16, 18, 5}; // digit au centre
static const uint8_t D2_segment_map[8] = {20, 21, 3, 1, 0, 22, 23, 2}; // digit à droite

void display_init()
{
    setBrightness(DISPLAY_BRIGHTNESS_V,'v');
    setBrightness(DISPLAY_BRIGHTNESS_A,'a');
    setBrightness(DISPLAY_BRIGHTNESS_W,'w');
}
static void applyDigit(uint32_t *mask, uint8_t digit, const uint8_t segment_map[8], bool decimalPoint){
    uint8_t segments = digit_encoding[digit];
    for (int i = 0; i < 7; i++)
    {
        if (segments & (1UL << i)) *mask |=  (1UL << segment_map[i]);
        else *mask &= ~(1UL << segment_map[i]);
    }
    if (decimalPoint) *mask |=  (1UL << segment_map[7]);
    else *mask &= ~(1UL << segment_map[7]);
}
void setDisplayValue(float value, char chipID){
    int scaled;
    bool lessThanTen = value < 10.0f;
    if (lessThanTen) scaled = (int)(value * 100.0f + 0.5f);
    else scaled = (int)(value * 10.0f + 0.5f);
    uint8_t d0   = (scaled / 100) % 10;
    uint8_t d1  = (scaled / 10)  % 10;
    uint8_t d2 = scaled % 10;
    uint32_t mask = 0;
    applyDigit(&mask, d0, D0_segment_map, lessThanTen);
    applyDigit(&mask, d1, D1_segment_map, !lessThanTen);
    applyDigit(&mask, d2, D2_segment_map, false);
    ESP_ERROR_CHECK(display(mask, chipID));
}
void displaySCTag(char chipID)
{
    uint32_t mask = 0;
    uint8_t segmentsD1 = 0b11101101; 
    for (int i = 0; i < 8; i++)
        if (segmentsD1 & (1UL << i)) mask |=  (1UL << D1_segment_map[i]);
    uint8_t segmentsD2 = 0b10111001; 
    for (int i = 0; i < 8; i++)
        if (segmentsD2 & (1UL << i)) mask |=  (1UL << D2_segment_map[i]);
    ESP_ERROR_CHECK(display(mask, chipID));
}
void displayErrTag(char chipID)
{
    uint32_t mask = 0;
    uint8_t segmentsD0 = 0b01111001; 
    for (int i = 0; i < 8; i++)
        if (segmentsD0 & (1UL << i)) mask |=  (1UL << D0_segment_map[i]);
    uint8_t segmentsD1 = 0b01010000; 
    for (int i = 0; i < 8; i++)
        if (segmentsD1 & (1UL << i)) mask |=  (1UL << D1_segment_map[i]);
    uint8_t segmentsD2 = 0b11010000; 
    for (int i = 0; i < 8; i++)
        if (segmentsD2 & (1UL << i)) mask |=  (1UL << D2_segment_map[i]);
    ESP_ERROR_CHECK(display(mask, chipID));
}