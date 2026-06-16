#include "Hardware/Implementations/OledDisplay.hpp"
#include "Error/Logger.hpp"
#include "Config/UiConfig.hpp"

OledDisplay::OledDisplay() 
    : _display(U8G2_R2, /* reset=*/ U8X8_PIN_NONE), _ready(false), _lastErrorTime(0) {
    for (int i = 0; i < 4; i++) {
        _lastLines[i] = "";
    }
}

bool OledDisplay::initialize() {
    _display.begin();
    _ready = true;
    
    LOG_INFO_CTX("OLED display initialized", "OledDisplay");
    return true;
}

void OledDisplay::clear() {
    if (!_ready) return;
    _display.clear();
}

void OledDisplay::printThreeLines(const String& line1, const String& line2, const String& line3) {
    printFourLines(line1, line2, line3, "");
}

void OledDisplay::printFourLines(const String& line1, const String& line2, const String& line3, const String& line4) {
    printFourLinesWithRecording(line1, line2, line3, line4, false);
}

void OledDisplay::printFourLinesWithRecording(const String& line1, const String& line2, const String& line3, const String& line4, bool isRecording) {
    if (!_ready) return;
    
    updateLastLines(line1, line2, line3, line4);
    
    _display.firstPage();
    do {
        // Draw recording indicator if needed
        drawRecordingIndicator(isRecording);
        
        // Line 1
        _display.setCursor(calcOffsetToCenterText(line1), 12);
        _display.print(line1);
        
        // Line 2
        _display.setCursor(calcOffsetToCenterText(line2), 24);
        _display.print(line2);
        
        // Line 3
        _display.setCursor(calcOffsetToCenterText(line3), 36);
        _display.print(line3);
        
        // Line 4
        _display.setCursor(calcOffsetToCenterText(line4), 48);
        _display.print(line4);
        
    } while (_display.nextPage());
}

void OledDisplay::showIntroAnimation(const String& text, int iterations) {
    if (!_ready) return;
    
    int x = HardwareConfig::Display::SCREEN_WIDTH;
    
    for (int i = 0; i < iterations; i++) {
        _display.firstPage();
        do {
            _display.setCursor(x, 32);
            _display.print(text);
        } while (_display.nextPage());
        
        x -= UiConfig::Animation::SCROLL_SPEED;
        if (x < -(int)text.length() * 6) {  // Approximate character width
            x = HardwareConfig::Display::SCREEN_WIDTH;
        }
        
        delay(UiConfig::Animation::BLINK_PERIOD_MS / 10);
    }
}

void OledDisplay::setAfterIntro() {
    if (!_ready) return;
    
    _display.setFont(u8g2_font_ncenB10_tr);
    LOG_DEBUG_CTX("Display set to normal mode", "OledDisplay");
}

bool OledDisplay::isReady() const {
    return _ready;
}

uint16_t OledDisplay::getWidth() const {
    return HardwareConfig::Display::SCREEN_WIDTH;
}

uint16_t OledDisplay::getHeight() const {
    return HardwareConfig::Display::SCREEN_HEIGHT;
}

void OledDisplay::printToScreen(const String& firstLine, const String& secondLine, const String& thirdLine) {
    printThreeLines(firstLine, secondLine, thirdLine);
}

void OledDisplay::printToScreen(const String& firstLine, const String& secondLine, const String& thirdLine, const String& fourthLine, bool isRecording) {
    printFourLinesWithRecording(firstLine, secondLine, thirdLine, fourthLine, isRecording);
}

void OledDisplay::printToScreen(const String& firstLine, const String& secondLine, const String& thirdLine, const String& fourthLine, bool isRecording, bool blinkState) {
    if (!_ready) return;
    
    updateLastLines(firstLine, secondLine, thirdLine, fourthLine);
    
    _display.firstPage();
    do {
        // Draw recording indicator with blink state
        drawRecordingIndicator(isRecording, blinkState);
        
        // Line 1
        _display.setCursor(calcOffsetToCenterText(firstLine), 12);
        _display.print(firstLine);
        
        // Line 2
        _display.setCursor(calcOffsetToCenterText(secondLine), 24);
        _display.print(secondLine);
        
        // Line 3
        _display.setCursor(calcOffsetToCenterText(thirdLine), 36);
        _display.print(thirdLine);
        
        // Line 4
        _display.setCursor(calcOffsetToCenterText(fourthLine), 48);
        _display.print(fourthLine);
        
    } while (_display.nextPage());
}

int16_t OledDisplay::calcOffsetToCenterText(const String& textLine) {
    int textWidth = textLine.length() * 6;  // Approximate character width
    return (HardwareConfig::Display::SCREEN_WIDTH - textWidth) / 2;
}

void OledDisplay::drawRecordingIndicator(bool isRecording, bool blinkState) {
    if (!isRecording) return;
    
    // Don't draw if blinking and in off state
    if (blinkState) return;
    
    // Draw recording circle in top-right corner
    _display.drawCircle(HardwareConfig::Display::SCREEN_WIDTH - 10, 10, 5);
    _display.drawDisc(HardwareConfig::Display::SCREEN_WIDTH - 10, 10, 3);
}

void OledDisplay::logError(const String& message) {
    LOG_ERROR_CTX(message, "OledDisplay");
    _lastErrorTime = millis();
}

void OledDisplay::updateLastLines(const String& line1, const String& line2, const String& line3, const String& line4) {
    _lastLines[0] = line1;
    _lastLines[1] = line2;
    _lastLines[2] = line3;
    _lastLines[3] = line4;
}

void OledDisplay::show() {
    // For OLED display, the content is already shown during the drawing operations
    // This method is kept for interface compatibility
    if (_ready) {
        _display.display();
    }
}
