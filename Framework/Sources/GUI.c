#include <GUI.h>

#if defined(GUI_SUPPORT_ICONS)
    #include "riconsdata.h"     // Required for: raygui icons data
#endif

#include <stdio.h>              // Required for: FILE, fopen(), fclose(), fprintf(), feof(), fscanf(), vsprintf()
#include <string.h>             // Required for: strlen() on GuiTextBox()
#include <stdlib.h>             // Required for: atoi()

#ifdef __cplusplus
    #define RAYGUI_CLITERAL(name) name
#else
    #define RAYGUI_CLITERAL(name) (name)
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// Gui control property style color element
typedef enum { BORDER = 0, BASE, TEXT, OTHER } GuiPropertyElement;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static GuiControlState guiState = GUI_STATE_NORMAL;

static Font guiFont = { 0 };            // NOTE: Highly coupled to raylib
static bool guiLocked = false;
static float guiAlpha = 1.0f;

// Global gui style array (allocated on heap by default)
// NOTE: In raygui we manage a single int array with all the possible style properties.
// When a new style is loaded, it loads over the global style... but default gui style
// could always be recovered with GuiLoadStyleDefault()
static unsigned int guiStyle[NUM_CONTROLS*(NUM_PROPS_DEFAULT + NUM_PROPS_EXTENDED)] = { 0 };
static bool guiStyleLoaded = false;

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static vec3 ConvertHSVtoRGB(vec3 hsv);    // Convert color data from HSV to RGB
static vec3 ConvertRGBtoHSV(vec3 rgb);    // Convert color data from RGB to HSV

// Gui get text width using default font
static int GetTextWidth(const char *text)       // TODO: GetTextSize()
{
    vec2 size = { 0 };

    if ((text != NULL) && (text[0] != '\0')) size = MeasureTextEx(guiFont, text, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING));

    // TODO: Consider text icon width here???

    return (int)size.x;
}

// Get text bounds considering control bounds
static rect GetTextBounds(int control, rect bounds)
{
    rect textBounds = bounds;

    textBounds.x = bounds.x + GuiGetStyle(control, BORDER_WIDTH);
    textBounds.y = bounds.y + GuiGetStyle(control, BORDER_WIDTH);
    textBounds.width = bounds.width - 2*GuiGetStyle(control, BORDER_WIDTH);
    textBounds.height = bounds.height - 2*GuiGetStyle(control, BORDER_WIDTH);

    // Consider TEXT_PADDING properly, depends on control type and TEXT_ALIGNMENT
    switch (control)
    {
        case COMBOBOX: bounds.width -= (GuiGetStyle(control, COMBO_BUTTON_WIDTH) + GuiGetStyle(control, COMBO_BUTTON_PADDING)); break;
        case VALUEBOX: break;   // NOTE: ValueBox text value always centered, text padding applies to label
        default:
        {
            if (GuiGetStyle(control, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_RIGHT) textBounds.x -= GuiGetStyle(control, TEXT_PADDING);
            else textBounds.x += GuiGetStyle(control, TEXT_PADDING);
        } break;
    }

    // TODO: Special cases (no label): COMBOBOX, DROPDOWNBOX, LISTVIEW (scrollbar?)
    // More special cases (label side): CHECKBOX, SLIDER, VALUEBOX, SPINNER

    return textBounds;
}

// Get text icon if provided and move text cursor
static const char *GetTextIcon(const char *text, int *iconId)
{
#if defined(GUI_SUPPORT_ICONS)
    *iconId = -1;
    if (text[0] == '#')     // Maybe we have an icon!
    {
        char iconValue[4] = { 0 };

        int i = 1;
        for (i = 1; i < 4; i++)
        {
            if ((text[i] != '#') && (text[i] != '\0')) iconValue[i - 1] = text[i];
            else break;
        }

        iconValue[3] = '\0';
        *iconId = atoi(iconValue);

        // Move text pointer after icon
        // WARNING: If only icon provided, it could point to EOL character!
        if (*iconId >= 0) text += (i + 1);
    }
#endif

    return text;
}

// Gui draw text using default font
static void GuiDrawText(const char *text, rect bounds, int alignment, Color tint)
{
    #define TEXT_VALIGN_PIXEL_OFFSET(h)  ((int)h%2)     // Vertical alignment for pixel perfect

    if ((text != NULL) && (text[0] != '\0'))
    {
        int iconId = 0;
        text = GetTextIcon(text, &iconId);  // Check text for icon and move cursor

        // Get text position depending on alignment and iconId
        //---------------------------------------------------------------------------------
        #define ICON_TEXT_PADDING   4

        vec2 position = { bounds.x, bounds.y };

        // NOTE: We get text size after icon been processed
        int textWidth = GetTextWidth(text);
        int textHeight = GuiGetStyle(DEFAULT, TEXT_SIZE);

#if defined(GUI_SUPPORT_ICONS)
        if (iconId >= 0)
        {
            textWidth += RICON_SIZE;

            // WARNING: If only icon provided, text could be pointing to eof character!
            if ((text != NULL) && (text[0] != '\0')) textWidth += ICON_TEXT_PADDING;
        }
#endif
        // Check guiTextAlign global variables
        switch (alignment)
        {
            case GUI_TEXT_ALIGN_LEFT:
            {
                position.x = bounds.x;
                position.y = bounds.y + bounds.height/2 - textHeight/2 + TEXT_VALIGN_PIXEL_OFFSET(bounds.height);
            } break;
            case GUI_TEXT_ALIGN_CENTER:
            {
                position.x = bounds.x + bounds.width/2 - textWidth/2;
                position.y = bounds.y + bounds.height/2 - textHeight/2 + TEXT_VALIGN_PIXEL_OFFSET(bounds.height);
            } break;
            case GUI_TEXT_ALIGN_RIGHT:
            {
                position.x = bounds.x + bounds.width - textWidth;
                position.y = bounds.y + bounds.height/2 - textHeight/2 + TEXT_VALIGN_PIXEL_OFFSET(bounds.height);
            } break;
            default: break;
        }

        // NOTE: Make sure we get pixel-perfect coordinates,
        // In case of decimals we got weird text positioning
        position.x = (float)((int)position.x);
        position.y = (float)((int)position.y);
        //---------------------------------------------------------------------------------

        // Draw text (with icon if available)
        //---------------------------------------------------------------------------------
#if defined(GUI_SUPPORT_ICONS)
        if (iconId >= 0)
        {
            // NOTE: We consider icon height, probably different than text size
            GuiDrawIcon(iconId, RAYGUI_CLITERAL(vec2){ position.x, bounds.y + bounds.height/2 - RICON_SIZE/2 + TEXT_VALIGN_PIXEL_OFFSET(bounds.height) }, 1, tint);
            position.x += (RICON_SIZE + ICON_TEXT_PADDING);
        }
#endif
        DrawTextEx(guiFont, text, position, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING), tint);
        //---------------------------------------------------------------------------------
    }
}

// Split controls text into multiple strings
// Also check for multiple columns (required by GuiToggleGroup())
static const char **GuiTextSplit(const char *text, int *count, int *textRow);

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Enable gui global state
void GuiEnable(void) { guiState = GUI_STATE_NORMAL; }

// Disable gui global state
void GuiDisable(void) { guiState = GUI_STATE_DISABLED; }

// Lock gui global state
void GuiLock(void) { guiLocked = true; }

// Unlock gui global state
void GuiUnlock(void) { guiLocked = false; }

// Set gui controls alpha global state
void GuiFade(float alpha)
{
    if (alpha < 0.0f) alpha = 0.0f;
    else if (alpha > 1.0f) alpha = 1.0f;

    guiAlpha = alpha;
}

// Set gui state (global state)
void GuiSetState(int state) { guiState = (GuiControlState)state; }

// Get gui state (global state)
int GuiGetState(void) { return guiState; }

// Set custom gui font
// NOTE: Font loading/unloading is external to raygui
void GuiSetFont(Font font)
{
    if (font.texture.id > 0)
    {
        guiFont = font;
        GuiSetStyle(DEFAULT, TEXT_SIZE, font.baseSize);
    }
}

// Get custom gui font
Font GuiGetFont(void)
{
    return guiFont;
}

// Set control style property value
void GuiSetStyle(int control, int property, int value)
{
    if (!guiStyleLoaded) GuiLoadStyleDefault();
    guiStyle[control*(NUM_PROPS_DEFAULT + NUM_PROPS_EXTENDED) + property] = value;

    // Default properties are propagated to all controls
    if ((control == 0) && (property < NUM_PROPS_DEFAULT))
    {
        for (int i = 1; i < NUM_CONTROLS; i++) guiStyle[i*(NUM_PROPS_DEFAULT + NUM_PROPS_EXTENDED) + property] = value;
    }
}

// Get control style property value
int GuiGetStyle(int control, int property)
{
    if (!guiStyleLoaded) GuiLoadStyleDefault();
    return guiStyle[control*(NUM_PROPS_DEFAULT + NUM_PROPS_EXTENDED) + property];
}

// Window Box control
bool GuiWindowBox(rect bounds, const char *title)
{
    // NOTE: This define is also used by GuiMessageBox() and GuiTextInputBox()
    #define WINDOW_STATUSBAR_HEIGHT        22

    GuiControlState state = guiState;
    bool clicked = false;

    int statusBarHeight = WINDOW_STATUSBAR_HEIGHT + 2*GuiGetStyle(STATUSBAR, BORDER_WIDTH);
    statusBarHeight += (statusBarHeight%2);

    rect statusBar = { bounds.x, bounds.y, bounds.width, statusBarHeight };
    if (bounds.height < statusBarHeight*2) bounds.height = statusBarHeight*2;

    rect closeButtonRec = { statusBar.x + statusBar.width - GuiGetStyle(STATUSBAR, BORDER_WIDTH) - 20,
                                 statusBar.y + statusBarHeight/2 - 18/2, 18, 18 };

    // Update control
    //--------------------------------------------------------------------
    // NOTE: Logic is directly managed by button
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------

    // Draw window base
    DrawRectangleLinesEx(bounds, GuiGetStyle(DEFAULT, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(DEFAULT, BORDER + (state*3))), guiAlpha));
    DrawRectangleRec(RAYGUI_CLITERAL(rect){ bounds.x + GuiGetStyle(DEFAULT, BORDER_WIDTH), bounds.y + GuiGetStyle(DEFAULT, BORDER_WIDTH),
                                  bounds.width - GuiGetStyle(DEFAULT, BORDER_WIDTH)*2, bounds.height - GuiGetStyle(DEFAULT, BORDER_WIDTH)*2 },
                                  Fade(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), guiAlpha));

    // Draw window header as status bar
    GuiStatusBar(statusBar, title);

    // Draw window close button
    int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
    int tempTextAlignment = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 1);
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
#if defined(GUI_SUPPORT_ICONS)
    clicked = GuiButton(closeButtonRec, GuiIconText(RICON_CROSS_SMALL, NULL));
#else
    clicked = GuiButton(closeButtonRec, "x");
#endif
    GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlignment);
    //--------------------------------------------------------------------

    return clicked;
}

// Group Box control with text name
void GuiGroupBox(rect bounds, const char *text)
{
    #define GROUPBOX_LINE_THICK     1
    #define GROUPBOX_TEXT_PADDING  10

    GuiControlState state = guiState;

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangle(bounds.x, bounds.y, GROUPBOX_LINE_THICK, bounds.height, Fade(GetColor(GuiGetStyle(DEFAULT, (state == GUI_STATE_DISABLED)? BORDER_COLOR_DISABLED : LINE_COLOR)), guiAlpha));
    DrawRectangle(bounds.x, bounds.y + bounds.height - 1, bounds.width, GROUPBOX_LINE_THICK, Fade(GetColor(GuiGetStyle(DEFAULT, (state == GUI_STATE_DISABLED)? BORDER_COLOR_DISABLED : LINE_COLOR)), guiAlpha));
    DrawRectangle(bounds.x + bounds.width - 1, bounds.y, GROUPBOX_LINE_THICK, bounds.height, Fade(GetColor(GuiGetStyle(DEFAULT, (state == GUI_STATE_DISABLED)? BORDER_COLOR_DISABLED : LINE_COLOR)), guiAlpha));

    GuiLine(RAYGUI_CLITERAL(rect){ bounds.x, bounds.y, bounds.width, 1 }, text);
    //--------------------------------------------------------------------
}

// Line control
void GuiLine(rect bounds, const char *text)
{
    #define LINE_TEXT_PADDING  10

    GuiControlState state = guiState;

    Color color = Fade(GetColor(GuiGetStyle(DEFAULT, (state == GUI_STATE_DISABLED)? BORDER_COLOR_DISABLED : LINE_COLOR)), guiAlpha);

    // Draw control
    //--------------------------------------------------------------------
    if (text == NULL) DrawRectangle(bounds.x, bounds.y + bounds.height/2, bounds.width, 1, color);
    else
    {
        rect textBounds = { 0 };
        textBounds.width = GetTextWidth(text);      // TODO: Consider text icon
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + LINE_TEXT_PADDING;
        textBounds.y = bounds.y - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

        // Draw line with embedded text label: "--- text --------------"
        DrawRectangle(bounds.x, bounds.y, LINE_TEXT_PADDING - 2, 1, color);
        GuiLabel(textBounds, text);
        DrawRectangle(bounds.x + LINE_TEXT_PADDING + textBounds.width + 4, bounds.y, bounds.width - textBounds.width - LINE_TEXT_PADDING - 4, 1, color);
    }
    //--------------------------------------------------------------------
}

// Panel control
void GuiPanel(rect bounds)
{
    #define PANEL_BORDER_WIDTH   1

    GuiControlState state = guiState;

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleRec(bounds, Fade(GetColor(GuiGetStyle(DEFAULT, (state == GUI_STATE_DISABLED)? BASE_COLOR_DISABLED : BACKGROUND_COLOR)), guiAlpha));
    DrawRectangleLinesEx(bounds, PANEL_BORDER_WIDTH, Fade(GetColor(GuiGetStyle(DEFAULT, (state == GUI_STATE_DISABLED)? BORDER_COLOR_DISABLED: LINE_COLOR)), guiAlpha));
    //--------------------------------------------------------------------
}

// Scroll Panel control
rect GuiScrollPanel(rect bounds, rect content, vec2 *scroll)
{
    GuiControlState state = guiState;

    vec2 scrollPos = { 0.0f, 0.0f };
    if (scroll != NULL) scrollPos = *scroll;

    bool hasHorizontalScrollBar = (content.width > bounds.width - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH))? true : false;
    bool hasVerticalScrollBar = (content.height > bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH))? true : false;

    // Recheck to account for the other scrollbar being visible
    if (!hasHorizontalScrollBar) hasHorizontalScrollBar = (hasVerticalScrollBar && (content.width > (bounds.width - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH))))? true : false;
    if (!hasVerticalScrollBar) hasVerticalScrollBar = (hasHorizontalScrollBar && (content.height > (bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH))))? true : false;

    const int horizontalScrollBarWidth = hasHorizontalScrollBar? GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH) : 0;
    const int verticalScrollBarWidth =  hasVerticalScrollBar? GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH) : 0;
    const rect horizontalScrollBar = { (float)((GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE)? (float)bounds.x + verticalScrollBarWidth : (float)bounds.x) + GuiGetStyle(DEFAULT, BORDER_WIDTH), (float)bounds.y + bounds.height - horizontalScrollBarWidth - GuiGetStyle(DEFAULT, BORDER_WIDTH), (float)bounds.width - verticalScrollBarWidth - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH), (float)horizontalScrollBarWidth };
    const rect verticalScrollBar = { (float)((GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE)? (float)bounds.x + GuiGetStyle(DEFAULT, BORDER_WIDTH) : (float)bounds.x + bounds.width - verticalScrollBarWidth - GuiGetStyle(DEFAULT, BORDER_WIDTH)), (float)bounds.y + GuiGetStyle(DEFAULT, BORDER_WIDTH), (float)verticalScrollBarWidth, (float)bounds.height - horizontalScrollBarWidth - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) };

    // Calculate view area (area without the scrollbars)
    rect view = (GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE)?
                RAYGUI_CLITERAL(rect){ bounds.x + verticalScrollBarWidth + GuiGetStyle(DEFAULT, BORDER_WIDTH), bounds.y + GuiGetStyle(DEFAULT, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - verticalScrollBarWidth, bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - horizontalScrollBarWidth } :
                RAYGUI_CLITERAL(rect){ bounds.x + GuiGetStyle(DEFAULT, BORDER_WIDTH), bounds.y + GuiGetStyle(DEFAULT, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - verticalScrollBarWidth, bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - horizontalScrollBarWidth };

    // Clip view area to the actual content size
    if (view.width > content.width) view.width = content.width;
    if (view.height > content.height) view.height = content.height;

    // TODO: Review!
    const int horizontalMin = hasHorizontalScrollBar? ((GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE)? -verticalScrollBarWidth : 0) - GuiGetStyle(DEFAULT, BORDER_WIDTH) : ((GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE)? -verticalScrollBarWidth : 0) - GuiGetStyle(DEFAULT, BORDER_WIDTH);
    const int horizontalMax = hasHorizontalScrollBar? content.width - bounds.width + verticalScrollBarWidth + GuiGetStyle(DEFAULT, BORDER_WIDTH) - ((GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE)? verticalScrollBarWidth : 0) : -GuiGetStyle(DEFAULT, BORDER_WIDTH);
    const int verticalMin = hasVerticalScrollBar? -GuiGetStyle(DEFAULT, BORDER_WIDTH) : -GuiGetStyle(DEFAULT, BORDER_WIDTH);
    const int verticalMax = hasVerticalScrollBar? content.height - bounds.height + horizontalScrollBarWidth + GuiGetStyle(DEFAULT, BORDER_WIDTH) : -GuiGetStyle(DEFAULT, BORDER_WIDTH);

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        // Check button state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;

            if (hasHorizontalScrollBar)
            {
                if (IsKeyDown(KEY_RIGHT)) scrollPos.x -= GuiGetStyle(SCROLLBAR, SCROLL_SPEED);
                if (IsKeyDown(KEY_LEFT)) scrollPos.x += GuiGetStyle(SCROLLBAR, SCROLL_SPEED);
            }

            if (hasVerticalScrollBar)
            {
                if (IsKeyDown(KEY_DOWN)) scrollPos.y -= GuiGetStyle(SCROLLBAR, SCROLL_SPEED);
                if (IsKeyDown(KEY_UP)) scrollPos.y += GuiGetStyle(SCROLLBAR, SCROLL_SPEED);
            }

            scrollPos.y += GetMouseWheelMove()*20;
        }
    }

    // Normalize scroll values
    if (scrollPos.x > -horizontalMin) scrollPos.x = -horizontalMin;
    if (scrollPos.x < -horizontalMax) scrollPos.x = -horizontalMax;
    if (scrollPos.y > -verticalMin) scrollPos.y = -verticalMin;
    if (scrollPos.y < -verticalMax) scrollPos.y = -verticalMax;
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleRec(bounds, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));        // Draw background

    // Save size of the scrollbar slider
    const int slider = GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE);

    // Draw horizontal scrollbar if visible
    if (hasHorizontalScrollBar)
    {
        // Change scrollbar slider size to show the diff in size between the content width and the widget width
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, ((bounds.width - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - verticalScrollBarWidth)/content.width)*(bounds.width - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - verticalScrollBarWidth));
        scrollPos.x = -GuiScrollBar(horizontalScrollBar, -scrollPos.x, horizontalMin, horizontalMax);
    }

    // Draw vertical scrollbar if visible
    if (hasVerticalScrollBar)
    {
        // Change scrollbar slider size to show the diff in size between the content height and the widget height
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, ((bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - horizontalScrollBarWidth)/content.height)* (bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - horizontalScrollBarWidth));
        scrollPos.y = -GuiScrollBar(verticalScrollBar, -scrollPos.y, verticalMin, verticalMax);
    }

    // Draw detail corner rectangle if both scroll bars are visible
    if (hasHorizontalScrollBar && hasVerticalScrollBar)
    {
        // TODO: Consider scroll bars side
        DrawRectangle(horizontalScrollBar.x + horizontalScrollBar.width + 2,
                      verticalScrollBar.y + verticalScrollBar.height + 2,
                      horizontalScrollBarWidth - 4, verticalScrollBarWidth - 4,
                      Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT + (state*3))), guiAlpha));
    }

    // Set scrollbar slider size back to the way it was before
    GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, slider);

    // Draw scrollbar lines depending on current state
    DrawRectangleLinesEx(bounds, GuiGetStyle(DEFAULT, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, (float)BORDER + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    if (scroll != NULL) *scroll = scrollPos;

    return view;
}

// Label control
void GuiLabel(rect bounds, const char *text)
{
    GuiControlState state = guiState;

    // Update control
    //--------------------------------------------------------------------
    // ...
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawText(text, GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LABEL, (state == GUI_STATE_DISABLED)? TEXT_COLOR_DISABLED : TEXT_COLOR_NORMAL)), guiAlpha));
    //--------------------------------------------------------------------
}

// Button control, returns true when clicked
bool GuiButton(rect bounds, const char *text)
{
    GuiControlState state = guiState;
    bool pressed = false;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        // Check button state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) pressed = true;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleLinesEx(bounds, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha));
    DrawRectangle(bounds.x + GuiGetStyle(BUTTON, BORDER_WIDTH), bounds.y + GuiGetStyle(BUTTON, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(BUTTON, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha));

    GuiDrawText(text, GetTextBounds(BUTTON, bounds), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
    //------------------------------------------------------------------

    return pressed;
}

// Label button control
bool GuiLabelButton(rect bounds, const char *text)
{
    GuiControlState state = guiState;
    bool pressed = false;

    // NOTE: We force bounds.width to be all text
    int textWidth = MeasureTextEx(guiFont, text, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING)).x;
    if (bounds.width < textWidth) bounds.width = textWidth;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        // Check checkbox state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) pressed = true;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawText(text, GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    return pressed;
}

// Image button control, returns true when clicked
bool GuiImageButton(rect bounds, const char *text, Texture texture)
{
    return GuiImageButtonEx(bounds, text, texture, RAYGUI_CLITERAL(rect){ 0, 0, (float)texture.width, (float)texture.height });
}

// Image button control, returns true when clicked
bool GuiImageButtonEx(rect bounds, const char *text, Texture texture, rect texSource)
{
    GuiControlState state = guiState;
    bool clicked = false;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        // Check button state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) clicked = true;
            else state = GUI_STATE_FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleLinesEx(bounds, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha));
    DrawRectangle(bounds.x + GuiGetStyle(BUTTON, BORDER_WIDTH), bounds.y + GuiGetStyle(BUTTON, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(BUTTON, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha));

    if (text != NULL) GuiDrawText(text, GetTextBounds(BUTTON, bounds), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
    if (texture.id > 0) DrawTextureRec(texture, texSource, RAYGUI_CLITERAL(vec2){ bounds.x + bounds.width/2 - texSource.width/2, bounds.y + bounds.height/2 - texSource.height/2 }, Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
    //------------------------------------------------------------------

    return clicked;
}

// Toggle Button control, returns true when active
bool GuiToggle(rect bounds, const char *text, bool active)
{
    GuiControlState state = guiState;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        // Check toggle button state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                state = GUI_STATE_NORMAL;
                active = !active;
            }
            else state = GUI_STATE_FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (state == GUI_STATE_NORMAL)
    {
        DrawRectangleLinesEx(bounds, GuiGetStyle(TOGGLE, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TOGGLE, (active? BORDER_COLOR_PRESSED : (BORDER + state*3)))), guiAlpha));
        DrawRectangle(bounds.x + GuiGetStyle(TOGGLE, BORDER_WIDTH), bounds.y + GuiGetStyle(TOGGLE, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(TOGGLE, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(TOGGLE, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TOGGLE, (active? BASE_COLOR_PRESSED : (BASE + state*3)))), guiAlpha));

        GuiDrawText(text, GetTextBounds(TOGGLE, bounds), GuiGetStyle(TOGGLE, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(TOGGLE, (active? TEXT_COLOR_PRESSED : (TEXT + state*3)))), guiAlpha));
    }
    else
    {
        DrawRectangleLinesEx(bounds, GuiGetStyle(TOGGLE, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TOGGLE, BORDER + state*3)), guiAlpha));
        DrawRectangle(bounds.x + GuiGetStyle(TOGGLE, BORDER_WIDTH), bounds.y + GuiGetStyle(TOGGLE, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(TOGGLE, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(TOGGLE, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TOGGLE, BASE + state*3)), guiAlpha));

        GuiDrawText(text, GetTextBounds(TOGGLE, bounds), GuiGetStyle(TOGGLE, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(TOGGLE, TEXT + state*3)), guiAlpha));
    }
    //--------------------------------------------------------------------

    return active;
}

// Toggle Group control, returns toggled button index
int GuiToggleGroup(rect bounds, const char *text, int active)
{
    #if !defined(TOGGLEGROUP_MAX_ELEMENTS)
        #define TOGGLEGROUP_MAX_ELEMENTS    32
    #endif

    float initBoundsX = bounds.x;

    // Get substrings items from text (items pointers)
    int rows[TOGGLEGROUP_MAX_ELEMENTS] = { 0 };
    int itemsCount = 0;
    const char **items = GuiTextSplit(text, &itemsCount, rows);

    int prevRow = rows[0];

    for (int i = 0; i < itemsCount; i++)
    {
        if (prevRow != rows[i])
        {
            bounds.x = initBoundsX;
            bounds.y += (bounds.height + GuiGetStyle(TOGGLE, GROUP_PADDING));
            prevRow = rows[i];
        }

        if (i == active) GuiToggle(bounds, items[i], true);
        else if (GuiToggle(bounds, items[i], false) == true) active = i;

        bounds.x += (bounds.width + GuiGetStyle(TOGGLE, GROUP_PADDING));
    }

    return active;
}

// Check Box control, returns true when active
bool GuiCheckBox(rect bounds, const char *text, bool checked)
{
    GuiControlState state = guiState;

    rect textBounds = { 0 };

    if (text != NULL)
    {
        textBounds.width = GetTextWidth(text);
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width + GuiGetStyle(CHECKBOX, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;
        if (GuiGetStyle(CHECKBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_LEFT) textBounds.x = bounds.x - textBounds.width - GuiGetStyle(CHECKBOX, TEXT_PADDING);
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        rect totalBounds = {
            (GuiGetStyle(CHECKBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_LEFT)? textBounds.x : bounds.x,
            bounds.y,
            bounds.width + textBounds.width + GuiGetStyle(CHECKBOX, TEXT_PADDING),
            bounds.height,
        };

        // Check checkbox state
        if (CheckCollisionPointRec(mousePoint, totalBounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) checked = !checked;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleLinesEx(bounds, GuiGetStyle(CHECKBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(CHECKBOX, BORDER + (state*3))), guiAlpha));
    if (checked) DrawRectangle(bounds.x + GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING),
                               bounds.y + GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING),
                               bounds.width - 2*(GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING)),
                               bounds.height - 2*(GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING)),
                               Fade(GetColor(GuiGetStyle(CHECKBOX, TEXT + state*3)), guiAlpha));

    if (text != NULL) GuiDrawText(text, textBounds, (GuiGetStyle(CHECKBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_RIGHT)? GUI_TEXT_ALIGN_LEFT : GUI_TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    return checked;
}

// Combo Box control, returns selected item index
int GuiComboBox(rect bounds, const char *text, int active)
{
    GuiControlState state = guiState;

    bounds.width -= (GuiGetStyle(COMBOBOX, COMBO_BUTTON_WIDTH) + GuiGetStyle(COMBOBOX, COMBO_BUTTON_PADDING));

    rect selector = { (float)bounds.x + bounds.width + GuiGetStyle(COMBOBOX, COMBO_BUTTON_PADDING),
                           (float)bounds.y, (float)GuiGetStyle(COMBOBOX, COMBO_BUTTON_WIDTH), (float)bounds.height };

    // Get substrings items from text (items pointers, lengths and count)
    int itemsCount = 0;
    const char **items = GuiTextSplit(text, &itemsCount, NULL);

    if (active < 0) active = 0;
    else if (active > itemsCount - 1) active = itemsCount - 1;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked && (itemsCount > 1))
    {
        vec2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds) ||
            CheckCollisionPointRec(mousePoint, selector))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                active += 1;
                if (active >= itemsCount) active = 0;
            }

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    // Draw combo box main
    DrawRectangleLinesEx(bounds, GuiGetStyle(COMBOBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(COMBOBOX, BORDER + (state*3))), guiAlpha));
    DrawRectangle(bounds.x + GuiGetStyle(COMBOBOX, BORDER_WIDTH), bounds.y + GuiGetStyle(COMBOBOX, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(COMBOBOX, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(COMBOBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(COMBOBOX, BASE + (state*3))), guiAlpha));

    GuiDrawText(items[active], GetTextBounds(COMBOBOX, bounds), GuiGetStyle(COMBOBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(COMBOBOX, TEXT + (state*3))), guiAlpha));

    // Draw selector using a custom button
    // NOTE: BORDER_WIDTH and TEXT_ALIGNMENT forced values
    int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
    int tempTextAlign = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 1);
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

    GuiButton(selector, TextFormat("%i/%i", active + 1, itemsCount));

    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlign);
    GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);
    //--------------------------------------------------------------------

    return active;
}

// Dropdown Box control
// NOTE: Returns mouse click
bool GuiDropdownBox(rect bounds, const char *text, int *active, bool editMode)
{
    GuiControlState state = guiState;
    int itemSelected = *active;
    int itemFocused = -1;

    // Get substrings items from text (items pointers, lengths and count)
    int itemsCount = 0;
    const char **items = GuiTextSplit(text, &itemsCount, NULL);

    rect boundsOpen = bounds;
    boundsOpen.height = (itemsCount + 1)*(bounds.height + GuiGetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_PADDING));

    rect itemBounds = bounds;

    bool pressed = false;       // Check mouse button pressed

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked && (itemsCount > 1))
    {
        vec2 mousePoint = GetMousePosition();

        if (editMode)
        {
            state = GUI_STATE_PRESSED;

            // Check if mouse has been pressed or released outside limits
            if (!CheckCollisionPointRec(mousePoint, boundsOpen))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) pressed = true;
            }

            // Check if already selected item has been pressed again
            if (CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) pressed = true;

            // Check focused and selected item
            for (int i = 0; i < itemsCount; i++)
            {
                // Update item rectangle y position for next item
                itemBounds.y += (bounds.height + GuiGetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_PADDING));

                if (CheckCollisionPointRec(mousePoint, itemBounds))
                {
                    itemFocused = i;
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                    {
                        itemSelected = i;
                        pressed = true;     // Item selected, change to editMode = false
                    }
                    break;
                }
            }

            itemBounds = bounds;
        }
        else
        {
            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    pressed = true;
                    state = GUI_STATE_PRESSED;
                }
                else state = GUI_STATE_FOCUSED;
            }
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (editMode) GuiPanel(boundsOpen);

    DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BASE + state*3)), guiAlpha));
    DrawRectangleLinesEx(bounds, GuiGetStyle(DROPDOWNBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BORDER + state*3)), guiAlpha));
    GuiDrawText(items[itemSelected], GetTextBounds(DEFAULT, bounds), GuiGetStyle(DROPDOWNBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT + state*3)), guiAlpha));

    if (editMode)
    {
        // Draw visible items
        for (int i = 0; i < itemsCount; i++)
        {
            // Update item rectangle y position for next item
            itemBounds.y += (bounds.height + GuiGetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_PADDING));

            if (i == itemSelected)
            {
                DrawRectangleRec(itemBounds, Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BASE_COLOR_PRESSED)), guiAlpha));
                DrawRectangleLinesEx(itemBounds, GuiGetStyle(DROPDOWNBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BORDER_COLOR_PRESSED)), guiAlpha));
                GuiDrawText(items[i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(DROPDOWNBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT_COLOR_PRESSED)), guiAlpha));
            }
            else if (i == itemFocused)
            {
                DrawRectangleRec(itemBounds, Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BASE_COLOR_FOCUSED)), guiAlpha));
                DrawRectangleLinesEx(itemBounds, GuiGetStyle(DROPDOWNBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BORDER_COLOR_FOCUSED)), guiAlpha));
                GuiDrawText(items[i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(DROPDOWNBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT_COLOR_FOCUSED)), guiAlpha));
            }
            else GuiDrawText(items[i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(DROPDOWNBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT_COLOR_NORMAL)), guiAlpha));
        }
    }

    // TODO: Avoid this function, use icon instead or 'v'
    DrawTriangle(RAYGUI_CLITERAL(vec2){ bounds.x + bounds.width - GuiGetStyle(DROPDOWNBOX, ARROW_PADDING), bounds.y + bounds.height/2 - 2 },
                 RAYGUI_CLITERAL(vec2){ bounds.x + bounds.width - GuiGetStyle(DROPDOWNBOX, ARROW_PADDING) + 5, bounds.y + bounds.height/2 - 2 + 5 },
                 RAYGUI_CLITERAL(vec2){ bounds.x + bounds.width - GuiGetStyle(DROPDOWNBOX, ARROW_PADDING) + 10, bounds.y + bounds.height/2 - 2 },
                 Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT + (state*3))), guiAlpha));

    //GuiDrawText("v", RAYGUI_CLITERAL(rect){ bounds.x + bounds.width - GuiGetStyle(DROPDOWNBOX, ARROW_PADDING), bounds.y + bounds.height/2 - 2, 10, 10 },
    //            GUI_TEXT_ALIGN_CENTER, Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    *active = itemSelected;
    return pressed;
}

// Text Box control, updates input text
// NOTE 1: Requires static variables: framesCounter
// NOTE 2: Returns if KEY_ENTER pressed (useful for data validation)
bool GuiTextBox(rect bounds, char *text, int textSize, bool editMode)
{
    static int framesCounter = 0;           // Required for blinking cursor

    GuiControlState state = guiState;
    bool pressed = false;

    rect cursor = {
        bounds.x + GuiGetStyle(TEXTBOX, TEXT_PADDING) + GetTextWidth(text) + 2,
        bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE),
        1,
        GuiGetStyle(DEFAULT, TEXT_SIZE)*2
    };

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        if (editMode)
        {
            state = GUI_STATE_PRESSED;
            framesCounter++;

            int key = GetKeyPressed();
            int keyCount = strlen(text);

            // Only allow keys in range [32..125]
            if (keyCount < (textSize - 1))
            {
                int maxWidth = (bounds.width - (GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING)*2));

                if (GetTextWidth(text) < (maxWidth - GuiGetStyle(DEFAULT, TEXT_SIZE)))
                {
                    if (((key >= 32) && (key <= 125)) ||
                        ((key >= 128) && (key < 255)))
                    {
                        text[keyCount] = (char)key;
                        keyCount++;
                        text[keyCount] = '\0';
                    }
                }
            }

            // Delete text
            if (keyCount > 0)
            {
                if (IsKeyPressed(KEY_BACKSPACE))
                {
                    keyCount--;
                    text[keyCount] = '\0';
                    framesCounter = 0;
                    if (keyCount < 0) keyCount = 0;
                }
                else if (IsKeyDown(KEY_BACKSPACE))
                {
                    if ((framesCounter > TEXTEDIT_CURSOR_BLINK_FRAMES) && (framesCounter%2) == 0) keyCount--;
                    text[keyCount] = '\0';
                    if (keyCount < 0) keyCount = 0;
                }
            }

            if (IsKeyPressed(KEY_ENTER) || (!CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(0))) pressed = true;

            // Check text alignment to position cursor properly
            int textAlignment = GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT);
            if (textAlignment == GUI_TEXT_ALIGN_CENTER) cursor.x = bounds.x + GetTextWidth(text)/2 + bounds.width/2 + 1;
            else if (textAlignment == GUI_TEXT_ALIGN_RIGHT) cursor.x = bounds.x + bounds.width - GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING);
        }
        else
        {
            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                state = GUI_STATE_FOCUSED;
                if (IsMouseButtonPressed(0)) pressed = true;
            }
        }

        if (pressed) framesCounter = 0;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleLinesEx(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha));

    if (state == GUI_STATE_PRESSED)
    {
        DrawRectangle(bounds.x + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_PRESSED)), guiAlpha));

        // Draw blinking cursor
        if (editMode && ((framesCounter/20)%2 == 0)) DrawRectangleRec(cursor, Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED)), guiAlpha));
    }
    else if (state == GUI_STATE_DISABLED)
    {
        DrawRectangle(bounds.x + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)), guiAlpha));
    }

    GuiDrawText(text, GetTextBounds(TEXTBOX, bounds), GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(TEXTBOX, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    return pressed;
}

// Spinner control, returns selected value
bool GuiSpinner(rect bounds, const char *text, int *value, int minValue, int maxValue, bool editMode)
{
    GuiControlState state = guiState;

    bool pressed = false;
    int tempValue = *value;

    rect spinner = { bounds.x + GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH) + GuiGetStyle(SPINNER, SPIN_BUTTON_PADDING), bounds.y,
                          bounds.width - 2*(GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH) + GuiGetStyle(SPINNER, SPIN_BUTTON_PADDING)), bounds.height };
    rect leftButtonBound = { (float)bounds.x, (float)bounds.y, (float)GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.height };
    rect rightButtonBound = { (float)bounds.x + bounds.width - GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.y, (float)GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.height };

    rect textBounds = { 0 };
    if (text != NULL)
    {
        textBounds.width = GetTextWidth(text);
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width + GuiGetStyle(SPINNER, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;
        if (GuiGetStyle(SPINNER, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_LEFT) textBounds.x = bounds.x - textBounds.width - GuiGetStyle(SPINNER, TEXT_PADDING);
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        // Check spinner state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;
        }
    }

    if (!editMode)
    {
        if (tempValue < minValue) tempValue = minValue;
        if (tempValue > maxValue) tempValue = maxValue;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    // TODO: Set Spinner properties for ValueBox
    pressed = GuiValueBox(spinner, NULL, &tempValue, minValue, maxValue, editMode);

    // Draw value selector custom buttons
    // NOTE: BORDER_WIDTH and TEXT_ALIGNMENT forced values
    int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
    int tempTextAlign = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
    GuiSetStyle(BUTTON, BORDER_WIDTH, GuiGetStyle(SPINNER, BORDER_WIDTH));
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

#if defined(GUI_SUPPORT_ICONS)
    if (GuiButton(leftButtonBound, GuiIconText(RICON_ARROW_LEFT_FILL, NULL))) tempValue--;
    if (GuiButton(rightButtonBound, GuiIconText(RICON_ARROW_RIGHT_FILL, NULL))) tempValue++;
#else
    if (GuiButton(leftButtonBound, "<")) tempValue--;
    if (GuiButton(rightButtonBound, ">")) tempValue++;
#endif

    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlign);
    GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);

    // Draw text label if provided
    if (text != NULL) GuiDrawText(text, textBounds, (GuiGetStyle(SPINNER, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_RIGHT)? GUI_TEXT_ALIGN_LEFT : GUI_TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    *value = tempValue;
    return pressed;
}

// Value Box control, updates input text with numbers
// NOTE: Requires static variables: framesCounter
bool GuiValueBox(rect bounds, const char *text, int *value, int minValue, int maxValue, bool editMode)
{
    #if !defined(VALUEBOX_MAX_CHARS)
        #define VALUEBOX_MAX_CHARS  32
    #endif

    static int framesCounter = 0;           // Required for blinking cursor

    GuiControlState state = guiState;
    bool pressed = false;

    char textValue[VALUEBOX_MAX_CHARS + 1] = "\0";
    sprintf(textValue, "%i", *value);

    rect textBounds = { 0 };
    if (text != NULL)
    {
        textBounds.width = GetTextWidth(text);
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width + GuiGetStyle(VALUEBOX, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;
        if (GuiGetStyle(VALUEBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_LEFT) textBounds.x = bounds.x - textBounds.width - GuiGetStyle(VALUEBOX, TEXT_PADDING);
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        bool valueHasChanged = false;

        if (editMode)
        {
            state = GUI_STATE_PRESSED;

            framesCounter++;

            int keyCount = strlen(textValue);

            // Only allow keys in range [48..57]
            if (keyCount < VALUEBOX_MAX_CHARS)
            {
                int maxWidth = bounds.width;
                if (GetTextWidth(textValue) < maxWidth)
                {
                    int key = GetKeyPressed();
                    if ((key >= 48) && (key <= 57))
                    {
                        textValue[keyCount] = (char)key;
                        keyCount++;
                        valueHasChanged = true;
                    }
                }
            }

            // Delete text
            if (keyCount > 0)
            {
                if (IsKeyPressed(KEY_BACKSPACE))
                {
                    keyCount--;
                    textValue[keyCount] = '\0';
                    framesCounter = 0;
                    if (keyCount < 0) keyCount = 0;
                    valueHasChanged = true;
                }
                else if (IsKeyDown(KEY_BACKSPACE))
                {
                    if ((framesCounter > TEXTEDIT_CURSOR_BLINK_FRAMES) && (framesCounter%2) == 0) keyCount--;
                    textValue[keyCount] = '\0';
                    if (keyCount < 0) keyCount = 0;
                    valueHasChanged = true;
                }
            }

            if (valueHasChanged) *value = atoi(textValue);

            if (IsKeyPressed(KEY_ENTER) || (!CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(0))) pressed = true;
        }
        else
        {
            if (*value > maxValue) *value = maxValue;
            else if (*value < minValue) *value = minValue;

            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                state = GUI_STATE_FOCUSED;
                if (IsMouseButtonPressed(0)) pressed = true;
            }
        }

        if (pressed) framesCounter = 0;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleLinesEx(bounds, GuiGetStyle(VALUEBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(VALUEBOX, BORDER + (state*3))), guiAlpha));

    if (state == GUI_STATE_PRESSED)
    {
        DrawRectangle(bounds.x + GuiGetStyle(VALUEBOX, BORDER_WIDTH), bounds.y + GuiGetStyle(VALUEBOX, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(VALUEBOX, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(VALUEBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(VALUEBOX, BASE_COLOR_PRESSED)), guiAlpha));

        // Draw blinking cursor
        // NOTE: ValueBox internal text is always centered
        if (editMode && ((framesCounter/20)%2 == 0)) DrawRectangle(bounds.x + GetTextWidth(textValue)/2 + bounds.width/2 + 2, bounds.y + 2*GuiGetStyle(VALUEBOX, BORDER_WIDTH), 1, bounds.height - 4*GuiGetStyle(VALUEBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(VALUEBOX, BORDER_COLOR_PRESSED)), guiAlpha));
    }
    else if (state == GUI_STATE_DISABLED)
    {
        DrawRectangle(bounds.x + GuiGetStyle(VALUEBOX, BORDER_WIDTH), bounds.y + GuiGetStyle(VALUEBOX, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(VALUEBOX, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(VALUEBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(VALUEBOX, BASE_COLOR_DISABLED)), guiAlpha));
    }

    GuiDrawText(textValue, GetTextBounds(VALUEBOX, bounds), GUI_TEXT_ALIGN_CENTER, Fade(GetColor(GuiGetStyle(VALUEBOX, TEXT + (state*3))), guiAlpha));

    // Draw text label if provided
    if (text != NULL) GuiDrawText(text, textBounds, (GuiGetStyle(VALUEBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_RIGHT)? GUI_TEXT_ALIGN_LEFT : GUI_TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    return pressed;
}

// Text Box control with multiple lines
bool GuiTextBoxMulti(rect bounds, char *text, int textSize, bool editMode)
{
    static int framesCounter = 0;           // Required for blinking cursor

    GuiControlState state = guiState;
    bool pressed = false;

    rect textAreaBounds = {
        bounds.x + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING),
        bounds.y + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING),
        bounds.width - 2*GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING),
        bounds.height - 2*GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING)
    };

    bool textHasChange = false;
    int currentLine = 0;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        if (editMode)
        {
            state = GUI_STATE_PRESSED;
            framesCounter++;

            int keyCount = strlen(text);
            int maxWidth = textAreaBounds.width;
            int maxHeight = textAreaBounds.height;

            // Only allow keys in range [32..125]
            if (keyCount < (textSize - 1))
            {
                int key = GetKeyPressed();

                if (MeasureTextEx(guiFont, text, GuiGetStyle(DEFAULT, TEXT_SIZE), 1).y < (maxHeight - GuiGetStyle(DEFAULT, TEXT_SIZE)))
                {
                    if (IsKeyPressed(KEY_ENTER))
                    {
                        text[keyCount] = '\n';
                        keyCount++;
                    }
                    else if (((key >= 32) && (key <= 125)) || ((key >= 128) && (key < 255)))
                    {
                        text[keyCount] = (char)key;
                        keyCount++;
                        textHasChange = true;
                    }
                }
                else if (GetTextWidth(strrchr(text, '\n')) < (maxWidth - GuiGetStyle(DEFAULT, TEXT_SIZE)))
                {
                    if (((key >= 32) && (key <= 125)) || ((key >= 128) && (key < 255)))
                    {
                        text[keyCount] = (char)key;
                        keyCount++;
                        textHasChange = true;
                    }
                }
            }

            // Delete text
            if (keyCount > 0)
            {
                if (IsKeyPressed(KEY_BACKSPACE))
                {
                    keyCount--;
                    text[keyCount] = '\0';
                    framesCounter = 0;
                    if (keyCount < 0) keyCount = 0;
                    textHasChange = true;
                }
                else if (IsKeyDown(KEY_BACKSPACE))
                {
                    if ((framesCounter > TEXTEDIT_CURSOR_BLINK_FRAMES) && (framesCounter%2) == 0) keyCount--;
                    text[keyCount] = '\0';

                    if (keyCount < 0) keyCount = 0;
                    textHasChange = true;
                }
            }

            // Introduce automatic new line if necessary
            if (textHasChange)
            {
                textHasChange = false;

                char *lastLine = strrchr(text, '\n');
                int maxWidth = (bounds.width - (GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING)*2));

                if (lastLine != NULL)
                {
                    if (GetTextWidth(lastLine) > maxWidth)
                    {
                        int firstIndex = lastLine - text;

                        char *lastSpace = strrchr(lastLine, 32);

                        if (lastSpace != NULL)
                        {
                            int secondIndex = lastSpace - lastLine;
                            text[firstIndex + secondIndex] = '\n';
                        }
                        else
                        {
                            int len = (lastLine != NULL)? strlen(lastLine) : 0;
                            char lastChar = lastLine[len - 1];
                            lastLine[len - 1] = '\n';
                            lastLine[len] = lastChar;
                            lastLine[len + 1] = '\0';
                            keyCount++;
                        }
                    }
                }
                else
                {
                    if (GetTextWidth(text) > maxWidth)
                    {
                        char *lastSpace = strrchr(text, 32);

                        if (lastSpace != NULL)
                        {
                            int index = lastSpace - text;
                            text[index] = '\n';
                        }
                        else
                        {
                            int len = (lastLine != NULL)? strlen(lastLine) : 0;
                            char lastChar = lastLine[len - 1];
                            lastLine[len - 1] = '\n';
                            lastLine[len] = lastChar;
                            lastLine[len + 1] = '\0';
                            keyCount++;
                        }
                    }
                }
            }

            // Counting how many new lines
            for (int i = 0; i < keyCount; i++)
            {
                if (text[i] == '\n') currentLine++;
            }

            // Exit edit mode
            if (!CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(0)) pressed = true;
        }
        else
        {
            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                state = GUI_STATE_FOCUSED;
                if (IsMouseButtonPressed(0)) pressed = true;
            }
        }

        if (pressed) framesCounter = 0;     // Reset blinking cursor
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleLinesEx(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha));

    if (state == GUI_STATE_PRESSED)
    {
        DrawRectangle(bounds.x + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_PRESSED)), guiAlpha));

        // Draw blinking cursor
        if (editMode && ((framesCounter/20)%2 == 0))
        {
            char *line = NULL;
            if (currentLine > 0) line = strrchr(text, '\n');
            else line = text;

            rect cursor = {
                bounds.x + GuiGetStyle(TEXTBOX, BORDER_WIDTH) + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING) + GetTextWidth(line),
                bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH) + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING)/2 + ((GuiGetStyle(DEFAULT, TEXT_SIZE) + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING))*currentLine),
                1, GuiGetStyle(DEFAULT, TEXT_SIZE) + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING)
            };

            DrawRectangleRec(cursor, Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED)), guiAlpha));
        }
    }
    else if (state == GUI_STATE_DISABLED)
    {
        DrawRectangle(bounds.x + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)), guiAlpha));
    }

    DrawTextRec(guiFont, text, textAreaBounds, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING), true, Fade(GetColor(GuiGetStyle(TEXTBOX, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    return pressed;
}

// Slider control with pro parameters
// NOTE: Other GuiSlider*() controls use this one
float GuiSliderPro(rect bounds, const char *textLeft, const char *textRight, float value, float minValue, float maxValue, int sliderWidth)
{
    GuiControlState state = guiState;

    int sliderValue = (int)(((value - minValue)/(maxValue - minValue))*(bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH)));

    rect slider = { bounds.x, bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH) + GuiGetStyle(SLIDER, SLIDER_PADDING),
                         0, bounds.height - 2*GuiGetStyle(SLIDER, BORDER_WIDTH) - 2*GuiGetStyle(SLIDER, SLIDER_PADDING) };

    if (sliderWidth > 0)        // Slider
    {
        slider.x += (sliderValue - sliderWidth/2);
        slider.width = sliderWidth;
    }
    else if (sliderWidth == 0)  // SliderBar
    {
        slider.x += GuiGetStyle(SLIDER, BORDER_WIDTH);
        slider.width = sliderValue;
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                state = GUI_STATE_PRESSED;

                // Get equivalent value and slider position from mousePoint.x
                value = ((maxValue - minValue)*(mousePoint.x - (float)(bounds.x + sliderWidth/2)))/(float)(bounds.width - sliderWidth) + minValue;

                if (sliderWidth > 0) slider.x = mousePoint.x - slider.width/2;  // Slider
                else if (sliderWidth == 0) slider.width = sliderValue;          // SliderBar
            }
            else state = GUI_STATE_FOCUSED;
        }

        if (value > maxValue) value = maxValue;
        else if (value < minValue) value = minValue;
    }

    // Bar limits check
    if (sliderWidth > 0)        // Slider
    {
        if (slider.x <= (bounds.x + GuiGetStyle(SLIDER, BORDER_WIDTH))) slider.x = bounds.x + GuiGetStyle(SLIDER, BORDER_WIDTH);
        else if ((slider.x + slider.width) >= (bounds.x + bounds.width)) slider.x = bounds.x + bounds.width - slider.width - GuiGetStyle(SLIDER, BORDER_WIDTH);
    }
    else if (sliderWidth == 0)  // SliderBar
    {
        if (slider.width > bounds.width) slider.width = bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH);
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleLinesEx(bounds, GuiGetStyle(SLIDER, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(SLIDER, BORDER + (state*3))), guiAlpha));
    DrawRectangle(bounds.x + GuiGetStyle(SLIDER, BORDER_WIDTH), bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(SLIDER, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(SLIDER, (state != GUI_STATE_DISABLED)?  BASE_COLOR_NORMAL : BASE_COLOR_DISABLED)), guiAlpha));

    // Draw slider internal bar (depends on state)
    if ((state == GUI_STATE_NORMAL) || (state == GUI_STATE_PRESSED)) DrawRectangleRec(slider, Fade(GetColor(GuiGetStyle(SLIDER, BASE_COLOR_PRESSED)), guiAlpha));
    else if (state == GUI_STATE_FOCUSED) DrawRectangleRec(slider, Fade(GetColor(GuiGetStyle(SLIDER, TEXT_COLOR_FOCUSED)), guiAlpha));

    // Draw left/right text if provided
    if (textLeft != NULL)
    {
        rect textBounds = { 0 };
        textBounds.width = GetTextWidth(textLeft);  // TODO: Consider text icon
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x - textBounds.width - GuiGetStyle(SLIDER, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

        GuiDrawText(textLeft, textBounds, GUI_TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(SLIDER, TEXT + (state*3))), guiAlpha));
    }

    if (textRight != NULL)
    {
        rect textBounds = { 0 };
        textBounds.width = GetTextWidth(textRight);  // TODO: Consider text icon
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width + GuiGetStyle(SLIDER, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

        GuiDrawText(textRight, textBounds, GUI_TEXT_ALIGN_LEFT, Fade(GetColor(GuiGetStyle(SLIDER, TEXT + (state*3))), guiAlpha));
    }
    //--------------------------------------------------------------------

    return value;
}

// Slider control extended, returns selected value and has text
float GuiSlider(rect bounds, const char *textLeft, const char *textRight, float value, float minValue, float maxValue)
{
    return GuiSliderPro(bounds, textLeft, textRight, value, minValue, maxValue, GuiGetStyle(SLIDER, SLIDER_WIDTH));
}

// Slider Bar control extended, returns selected value
float GuiSliderBar(rect bounds, const char *textLeft, const char *textRight, float value, float minValue, float maxValue)
{
    return GuiSliderPro(bounds, textLeft, textRight, value, minValue, maxValue, 0);
}

// Progress Bar control extended, shows current progress value
float GuiProgressBar(rect bounds, const char *textLeft, const char *textRight, float value, float minValue, float maxValue)
{
    GuiControlState state = guiState;

    rect progress = { bounds.x + GuiGetStyle(PROGRESSBAR, BORDER_WIDTH),
                           bounds.y + GuiGetStyle(PROGRESSBAR, BORDER_WIDTH) + GuiGetStyle(PROGRESSBAR, PROGRESS_PADDING), 0,
                           bounds.height - 2*GuiGetStyle(PROGRESSBAR, BORDER_WIDTH) - 2*GuiGetStyle(PROGRESSBAR, PROGRESS_PADDING) };

    // Update control
    //--------------------------------------------------------------------
    if (state != GUI_STATE_DISABLED) progress.width = (int)(value/(maxValue - minValue)*(float)(bounds.width - 2*GuiGetStyle(PROGRESSBAR, BORDER_WIDTH)));
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleLinesEx(bounds, GuiGetStyle(PROGRESSBAR, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(PROGRESSBAR, BORDER + (state*3))), guiAlpha));

    // Draw slider internal progress bar (depends on state)
    if ((state == GUI_STATE_NORMAL) || (state == GUI_STATE_PRESSED)) DrawRectangleRec(progress, Fade(GetColor(GuiGetStyle(PROGRESSBAR, BASE_COLOR_PRESSED)), guiAlpha));
    else if (state == GUI_STATE_FOCUSED) DrawRectangleRec(progress, Fade(GetColor(GuiGetStyle(PROGRESSBAR, TEXT_COLOR_FOCUSED)), guiAlpha));

    // Draw left/right text if provided
    if (textLeft != NULL)
    {
        rect textBounds = { 0 };
        textBounds.width = GetTextWidth(textLeft);  // TODO: Consider text icon
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x - textBounds.width - GuiGetStyle(PROGRESSBAR, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

        GuiDrawText(textLeft, textBounds, GUI_TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(PROGRESSBAR, TEXT + (state*3))), guiAlpha));
    }

    if (textRight != NULL)
    {
        rect textBounds = { 0 };
        textBounds.width = GetTextWidth(textRight);  // TODO: Consider text icon
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width + GuiGetStyle(PROGRESSBAR, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

        GuiDrawText(textRight, textBounds, GUI_TEXT_ALIGN_LEFT, Fade(GetColor(GuiGetStyle(PROGRESSBAR, TEXT + (state*3))), guiAlpha));
    }
    //--------------------------------------------------------------------

    return value;
}

// Status Bar control
void GuiStatusBar(rect bounds, const char *text)
{
    GuiControlState state = guiState;

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleLinesEx(bounds, GuiGetStyle(STATUSBAR, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(STATUSBAR, (state != GUI_STATE_DISABLED)? BORDER_COLOR_NORMAL : BORDER_COLOR_DISABLED)), guiAlpha));
    DrawRectangleRec(RAYGUI_CLITERAL(rect){ bounds.x + GuiGetStyle(STATUSBAR, BORDER_WIDTH), bounds.y + GuiGetStyle(STATUSBAR, BORDER_WIDTH), bounds.width - GuiGetStyle(STATUSBAR, BORDER_WIDTH)*2, bounds.height - GuiGetStyle(STATUSBAR, BORDER_WIDTH)*2 }, Fade(GetColor(GuiGetStyle(STATUSBAR, (state != GUI_STATE_DISABLED)? BASE_COLOR_NORMAL : BASE_COLOR_DISABLED)), guiAlpha));

    GuiDrawText(text, GetTextBounds(STATUSBAR, bounds), GuiGetStyle(STATUSBAR, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(STATUSBAR, (state != GUI_STATE_DISABLED)? TEXT_COLOR_NORMAL : TEXT_COLOR_DISABLED)), guiAlpha));
    //--------------------------------------------------------------------
}

// Dummy rectangle control, intended for placeholding
void GuiDummyRec(rect bounds, const char *text)
{
    GuiControlState state = guiState;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        // Check button state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleRec(bounds, Fade(GetColor(GuiGetStyle(DEFAULT, (state != GUI_STATE_DISABLED)? BASE_COLOR_NORMAL : BASE_COLOR_DISABLED)), guiAlpha));

    GuiDrawText(text, GetTextBounds(DEFAULT, bounds), GUI_TEXT_ALIGN_CENTER, Fade(GetColor(GuiGetStyle(BUTTON, (state != GUI_STATE_DISABLED)? TEXT_COLOR_NORMAL : TEXT_COLOR_DISABLED)), guiAlpha));
    //------------------------------------------------------------------
}

// Scroll Bar control
// TODO: I feel GuiScrollBar could be simplified...
int GuiScrollBar(rect bounds, int value, int minValue, int maxValue)
{
    GuiControlState state = guiState;

    // Is the scrollbar horizontal or vertical?
    bool isVertical = (bounds.width > bounds.height)? false : true;

    // The size (width or height depending on scrollbar type) of the spinner buttons
    const int spinnerSize = GuiGetStyle(SCROLLBAR, ARROWS_VISIBLE)? (isVertical? bounds.width - 2*GuiGetStyle(SCROLLBAR, BORDER_WIDTH) : bounds.height - 2*GuiGetStyle(SCROLLBAR, BORDER_WIDTH)) : 0;

    // Arrow buttons [<] [>] [∧] [∨]
    rect arrowUpLeft = { 0 };
    rect arrowDownRight = { 0 };

    // Actual area of the scrollbar excluding the arrow buttons
    rect scrollbar = { 0 };

    // Slider bar that moves     --[///]-----
    rect slider = { 0 };

    // Normalize value
    if (value > maxValue) value = maxValue;
    if (value < minValue) value = minValue;

    const int range = maxValue - minValue;
    int sliderSize = GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE);

    // Calculate rectangles for all of the components
    arrowUpLeft = RAYGUI_CLITERAL(rect){ (float)bounds.x + GuiGetStyle(SCROLLBAR, BORDER_WIDTH), (float)bounds.y + GuiGetStyle(SCROLLBAR, BORDER_WIDTH), (float)spinnerSize, (float)spinnerSize };

    if (isVertical)
    {
        arrowDownRight = RAYGUI_CLITERAL(rect){ (float)bounds.x + GuiGetStyle(SCROLLBAR, BORDER_WIDTH), (float)bounds.y + bounds.height - spinnerSize - GuiGetStyle(SCROLLBAR, BORDER_WIDTH), (float)spinnerSize, (float)spinnerSize};
        scrollbar = RAYGUI_CLITERAL(rect){ bounds.x + GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_PADDING), arrowUpLeft.y + arrowUpLeft.height, bounds.width - 2*(GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_PADDING)), bounds.height - arrowUpLeft.height - arrowDownRight.height - 2*GuiGetStyle(SCROLLBAR, BORDER_WIDTH) };
        sliderSize = (sliderSize >= scrollbar.height)? (scrollbar.height - 2) : sliderSize;     // Make sure the slider won't get outside of the scrollbar
        slider = RAYGUI_CLITERAL(rect){ (float)bounds.x + GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING), (float)scrollbar.y + (int)(((float)(value - minValue)/range)*(scrollbar.height - sliderSize)), (float)bounds.width - 2*(GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING)), (float)sliderSize };
    }
    else
    {
        arrowDownRight = RAYGUI_CLITERAL(rect){ (float)bounds.x + bounds.width - spinnerSize - GuiGetStyle(SCROLLBAR, BORDER_WIDTH), (float)bounds.y + GuiGetStyle(SCROLLBAR, BORDER_WIDTH), (float)spinnerSize, (float)spinnerSize};
        scrollbar = RAYGUI_CLITERAL(rect){ arrowUpLeft.x + arrowUpLeft.width, bounds.y + GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_PADDING), bounds.width - arrowUpLeft.width - arrowDownRight.width - 2*GuiGetStyle(SCROLLBAR, BORDER_WIDTH), bounds.height - 2*(GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_PADDING))};
        sliderSize = (sliderSize >= scrollbar.width)? (scrollbar.width - 2) : sliderSize;       // Make sure the slider won't get outside of the scrollbar
        slider = RAYGUI_CLITERAL(rect){ (float)scrollbar.x + (int)(((float)(value - minValue)/range)*(scrollbar.width - sliderSize)), (float)bounds.y + GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING), (float)sliderSize, (float)bounds.height - 2*(GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING)) };
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            state = GUI_STATE_FOCUSED;

            // Handle mouse wheel
            int wheel = GetMouseWheelMove();
            if (wheel != 0) value += wheel;

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (CheckCollisionPointRec(mousePoint, arrowUpLeft)) value -= range/GuiGetStyle(SCROLLBAR, SCROLL_SPEED);
                else if (CheckCollisionPointRec(mousePoint, arrowDownRight)) value += range/GuiGetStyle(SCROLLBAR, SCROLL_SPEED);

                state = GUI_STATE_PRESSED;
            }
            else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                if (!isVertical)
                {
                    rect scrollArea = { arrowUpLeft.x + arrowUpLeft.width, arrowUpLeft.y, scrollbar.width, bounds.height - 2*GuiGetStyle(SCROLLBAR, BORDER_WIDTH)};
                    if (CheckCollisionPointRec(mousePoint, scrollArea)) value = ((float)(mousePoint.x - scrollArea.x - slider.width/2)*range)/(scrollArea.width - slider.width) + minValue;
                }
                else
                {
                    rect scrollArea = { arrowUpLeft.x, arrowUpLeft.y+arrowUpLeft.height, bounds.width - 2*GuiGetStyle(SCROLLBAR, BORDER_WIDTH),  scrollbar.height};
                    if (CheckCollisionPointRec(mousePoint, scrollArea)) value = ((float)(mousePoint.y - scrollArea.y - slider.height/2)*range)/(scrollArea.height - slider.height) + minValue;
                }
            }
        }

        // Normalize value
        if (value > maxValue) value = maxValue;
        if (value < minValue) value = minValue;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleRec(bounds, Fade(GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_DISABLED)), guiAlpha));   // Draw the background
    DrawRectangleRec(scrollbar, Fade(GetColor(GuiGetStyle(BUTTON, BASE_COLOR_NORMAL)), guiAlpha));     // Draw the scrollbar active area background

    DrawRectangleLinesEx(bounds, GuiGetStyle(SCROLLBAR, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER + state*3)), guiAlpha));

    DrawRectangleRec(slider, Fade(GetColor(GuiGetStyle(SLIDER, BORDER + state*3)), guiAlpha));         // Draw the slider bar

    // Draw arrows
    const int padding = (spinnerSize - GuiGetStyle(SCROLLBAR, ARROWS_SIZE))/2;
    const vec2 lineCoords[] =
    {
        // Coordinates for <     0,1,2
        { arrowUpLeft.x + padding, arrowUpLeft.y + spinnerSize/2 },
        { arrowUpLeft.x + spinnerSize - padding, arrowUpLeft.y + padding },
        { arrowUpLeft.x + spinnerSize - padding, arrowUpLeft.y + spinnerSize - padding },

        // Coordinates for >     3,4,5
        { arrowDownRight.x + padding, arrowDownRight.y + padding },
        { arrowDownRight.x + spinnerSize - padding, arrowDownRight.y + spinnerSize/2 },
        { arrowDownRight.x + padding, arrowDownRight.y + spinnerSize - padding },

        // Coordinates for ∧     6,7,8
        { arrowUpLeft.x + spinnerSize/2, arrowUpLeft.y + padding },
        { arrowUpLeft.x + padding, arrowUpLeft.y + spinnerSize - padding },
        { arrowUpLeft.x + spinnerSize - padding, arrowUpLeft.y + spinnerSize - padding },

        // Coordinates for ∨     9,10,11
        { arrowDownRight.x + padding, arrowDownRight.y + padding },
        { arrowDownRight.x + spinnerSize/2, arrowDownRight.y + spinnerSize - padding },
        { arrowDownRight.x + spinnerSize - padding, arrowDownRight.y + padding }
    };

    Color lineColor = Fade(GetColor(GuiGetStyle(BUTTON, TEXT + state*3)), guiAlpha);

    if (GuiGetStyle(SCROLLBAR, ARROWS_VISIBLE))
    {
        if (isVertical)
        {
            DrawTriangle(lineCoords[6], lineCoords[7], lineCoords[8], lineColor);
            DrawTriangle(lineCoords[9], lineCoords[10], lineCoords[11], lineColor);
        }
        else
        {
            DrawTriangle(lineCoords[2], lineCoords[1], lineCoords[0], lineColor);
            DrawTriangle(lineCoords[5], lineCoords[4], lineCoords[3], lineColor);
        }
    }
    //--------------------------------------------------------------------

    return value;
}

// List View control
int GuiListView(rect bounds, const char *text, int *scrollIndex, int active)
{
    int itemsCount = 0;
    const char **items = NULL;

    if (text != NULL) items = GuiTextSplit(text, &itemsCount, NULL);

    return GuiListViewEx(bounds, items, itemsCount, NULL, scrollIndex, active);
}

// List View control with extended parameters
int GuiListViewEx(rect bounds, const char **text, int count, int *focus, int *scrollIndex, int active)
{
    GuiControlState state = guiState;
    int itemFocused = (focus == NULL)? -1 : *focus;
    int itemSelected = active;

    // Check if we need a scroll bar
    bool useScrollBar = false;
    if ((GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING))*count > bounds.height) useScrollBar = true;

    // Define base item rectangle [0]
    rect itemBounds = { 0 };
    itemBounds.x = bounds.x + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING);
    itemBounds.y = bounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING) + GuiGetStyle(DEFAULT, BORDER_WIDTH);
    itemBounds.width = bounds.width - 2*GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING) - GuiGetStyle(DEFAULT, BORDER_WIDTH);
    itemBounds.height = GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
    if (useScrollBar) itemBounds.width -= GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);

    // Get items on the list
    int visibleItems = bounds.height/(GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING));
    if (visibleItems > count) visibleItems = count;

    int startIndex = (scrollIndex == NULL)? 0 : *scrollIndex;
    if ((startIndex < 0) || (startIndex > (count - visibleItems))) startIndex = 0;
    int endIndex = startIndex + visibleItems;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        // Check mouse inside list view
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            state = GUI_STATE_FOCUSED;

            // Check focused and selected item
            for (int i = 0; i < visibleItems; i++)
            {
                if (CheckCollisionPointRec(mousePoint, itemBounds))
                {
                    itemFocused = startIndex + i;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        if (itemSelected == (startIndex + i)) itemSelected = -1;
                        else itemSelected = startIndex + i;
                    }
                    break;
                }

                // Update item rectangle y position for next item
                itemBounds.y += (GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING));
            }

            if (useScrollBar)
            {
                int wheelMove = GetMouseWheelMove();
                startIndex -= wheelMove;

                if (startIndex < 0) startIndex = 0;
                else if (startIndex > (count - visibleItems)) startIndex = count - visibleItems;

                endIndex = startIndex + visibleItems;
                if (endIndex > count) endIndex = count;
            }
        }
        else itemFocused = -1;

        // Reset item rectangle y to [0]
        itemBounds.y = bounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING) + GuiGetStyle(DEFAULT, BORDER_WIDTH);
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleRec(bounds, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));     // Draw background
    DrawRectangleLinesEx(bounds, GuiGetStyle(DEFAULT, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER + state*3)), guiAlpha));

    // Draw visible items
    for (int i = 0; ((i < visibleItems) && (text != NULL)); i++)
    {
        if (state == GUI_STATE_DISABLED)
        {
            if ((startIndex + i) == itemSelected)
            {
                DrawRectangleRec(itemBounds, Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_DISABLED)), guiAlpha));
                DrawRectangleLinesEx(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_DISABLED)), guiAlpha));
            }

            GuiDrawText(text[startIndex + i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_DISABLED)), guiAlpha));
        }
        else
        {
            if ((startIndex + i) == itemSelected)
            {
                // Draw item selected
                DrawRectangleRec(itemBounds, Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_PRESSED)), guiAlpha));
                DrawRectangleLinesEx(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_PRESSED)), guiAlpha));
                GuiDrawText(text[startIndex + i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_PRESSED)), guiAlpha));
            }
            else if ((startIndex + i) == itemFocused)
            {
                // Draw item focused
                DrawRectangleRec(itemBounds, Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_FOCUSED)), guiAlpha));
                DrawRectangleLinesEx(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_FOCUSED)), guiAlpha));
                GuiDrawText(text[startIndex + i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_FOCUSED)), guiAlpha));
            }
            else
            {
                // Draw item normal
                GuiDrawText(text[startIndex + i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_NORMAL)), guiAlpha));
            }
        }

        // Update item rectangle y position for next item
        itemBounds.y += (GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING));
    }

    if (useScrollBar)
    {
        rect scrollBarBounds = {
            bounds.x + bounds.width - GuiGetStyle(LISTVIEW, BORDER_WIDTH) - GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH),
            bounds.y + GuiGetStyle(LISTVIEW, BORDER_WIDTH), (float)GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH),
            bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH)
        };

        // Calculate percentage of visible items and apply same percentage to scrollbar
        float percentVisible = (float)(endIndex - startIndex)/count;
        float sliderSize = bounds.height*percentVisible;

        int prevSliderSize = GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE);   // Save default slider size
        int prevScrollSpeed = GuiGetStyle(SCROLLBAR, SCROLL_SPEED); // Save default scroll speed
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, sliderSize);            // Change slider size
        GuiSetStyle(SCROLLBAR, SCROLL_SPEED, count - visibleItems); // Change scroll speed

        startIndex = GuiScrollBar(scrollBarBounds, startIndex, 0, count - visibleItems);

        GuiSetStyle(SCROLLBAR, SCROLL_SPEED, prevScrollSpeed); // Reset scroll speed to default
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, prevSliderSize); // Reset slider size to default
    }
    //--------------------------------------------------------------------

    if (focus != NULL) *focus = itemFocused;
    if (scrollIndex != NULL) *scrollIndex = startIndex;

    return itemSelected;
}

// Color Panel control
Color GuiColorPanelEx(rect bounds, Color color, float hue)
{
    GuiControlState state = guiState;
    vec2 pickerSelector = { 0 };

    vec3 vcolor = { (float)color.r/255.0f, (float)color.g/255.0f, (float)color.b/255.0f };
    vec3 hsv = ConvertRGBtoHSV(vcolor);

    pickerSelector.x = bounds.x + (float)hsv.y*bounds.width;            // HSV: Saturation
    pickerSelector.y = bounds.y + (1.0f - (float)hsv.z)*bounds.height;  // HSV: Value

    vec3 maxHue = { hue >= 0.0f ? hue : hsv.x, 1.0f, 1.0f };
    vec3 rgbHue = ConvertHSVtoRGB(maxHue);
    Color maxHueCol = { (unsigned char)(255.0f*rgbHue.x),
                      (unsigned char)(255.0f*rgbHue.y),
                      (unsigned char)(255.0f*rgbHue.z), 255 };

    const Color colWhite = { 255, 255, 255, 255 };
    const Color colBlack = { 0, 0, 0, 255 };

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                state = GUI_STATE_PRESSED;
                pickerSelector = mousePoint;

                // Calculate color from picker
                vec2 colorPick = { pickerSelector.x - bounds.x, pickerSelector.y - bounds.y };

                colorPick.x /= (float)bounds.width;     // Get normalized value on x
                colorPick.y /= (float)bounds.height;    // Get normalized value on y

                hsv.y = colorPick.x;
                hsv.z = 1.0f - colorPick.y;

                vec3 rgb = ConvertHSVtoRGB(hsv);

                // NOTE: Vector3ToColor() only available on raylib 1.8.1
                color = RAYGUI_CLITERAL(Color){ (unsigned char)(255.0f*rgb.x),
                                 (unsigned char)(255.0f*rgb.y),
                                 (unsigned char)(255.0f*rgb.z),
                                 (unsigned char)(255.0f*(float)color.a/255.0f) };

            }
            else state = GUI_STATE_FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (state != GUI_STATE_DISABLED)
    {
        DrawRectangleGradientEx(bounds, Fade(colWhite, guiAlpha), Fade(colWhite, guiAlpha), Fade(maxHueCol, guiAlpha), Fade(maxHueCol, guiAlpha));
        DrawRectangleGradientEx(bounds, Fade(colBlack, 0), Fade(colBlack, guiAlpha), Fade(colBlack, guiAlpha), Fade(colBlack, 0));

        // Draw color picker: selector
        DrawRectangle(pickerSelector.x - GuiGetStyle(COLORPICKER, COLOR_SELECTOR_SIZE)/2, pickerSelector.y - GuiGetStyle(COLORPICKER, COLOR_SELECTOR_SIZE)/2, GuiGetStyle(COLORPICKER, COLOR_SELECTOR_SIZE), GuiGetStyle(COLORPICKER, COLOR_SELECTOR_SIZE), Fade(colWhite, guiAlpha));
    }
    else
    {
        DrawRectangleGradientEx(bounds, Fade(Fade(GetColor(GuiGetStyle(COLORPICKER, BASE_COLOR_DISABLED)), 0.1f), guiAlpha), Fade(Fade(colBlack, 0.6f), guiAlpha), Fade(Fade(colBlack, 0.6f), guiAlpha), Fade(Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER_COLOR_DISABLED)), 0.6f), guiAlpha));
    }

    DrawRectangleLinesEx(bounds, 1, Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER + state*3)), guiAlpha));
    //--------------------------------------------------------------------

    return color;
}

Color GuiColorPanel(rect bounds, Color color)
{
    return GuiColorPanelEx(bounds, color, -1.0f);
}

// Color Bar Alpha control
// NOTE: Returns alpha value normalized [0..1]
float GuiColorBarAlpha(rect bounds, float alpha)
{
    #define COLORBARALPHA_CHECKED_SIZE   10

    GuiControlState state = guiState;
    rect selector = { (float)bounds.x + alpha*bounds.width - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (float)bounds.y - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (float)GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_HEIGHT), (float)bounds.height + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)*2 };

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds) ||
            CheckCollisionPointRec(mousePoint, selector))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                state = GUI_STATE_PRESSED;
                selector.x = mousePoint.x - selector.width/2;

                alpha = (mousePoint.x - bounds.x)/bounds.width;
                if (alpha <= 0.0f) alpha = 0.0f;
                if (alpha >= 1.0f) alpha = 1.0f;
                //selector.x = bounds.x + (int)(((alpha - 0)/(100 - 0))*(bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH))) - selector.width/2;
            }
            else state = GUI_STATE_FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------

    // Draw alpha bar: checked background
    if (state != GUI_STATE_DISABLED)
    {
        int checksX = bounds.width/COLORBARALPHA_CHECKED_SIZE;
        int checksY = bounds.height/COLORBARALPHA_CHECKED_SIZE;

        for (int x = 0; x < checksX; x++)
        {
            for (int y = 0; y < checksY; y++)
            {
                DrawRectangle(bounds.x + x*COLORBARALPHA_CHECKED_SIZE,
                              bounds.y + y*COLORBARALPHA_CHECKED_SIZE,
                              COLORBARALPHA_CHECKED_SIZE, COLORBARALPHA_CHECKED_SIZE,
                              ((x + y)%2)? Fade(Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER_COLOR_DISABLED)), 0.4f), guiAlpha) :
                                     Fade(Fade(GetColor(GuiGetStyle(COLORPICKER, BASE_COLOR_DISABLED)), 0.4f), guiAlpha));
            }
        }

        DrawRectangleGradientEx(bounds, RAYGUI_CLITERAL(Color){ 255, 255, 255, 0 }, RAYGUI_CLITERAL(Color){ 255, 255, 255, 0 }, Fade(RAYGUI_CLITERAL(Color){ 0, 0, 0, 255 }, guiAlpha), Fade(RAYGUI_CLITERAL(Color){ 0, 0, 0, 255 }, guiAlpha));
    }
    else DrawRectangleGradientEx(bounds, Fade(GetColor(GuiGetStyle(COLORPICKER, BASE_COLOR_DISABLED)), 0.1f), Fade(GetColor(GuiGetStyle(COLORPICKER, BASE_COLOR_DISABLED)), 0.1f), Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER_COLOR_DISABLED)), guiAlpha), Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER_COLOR_DISABLED)), guiAlpha));

    DrawRectangleLinesEx(bounds, 1, Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER + state*3)), guiAlpha));

    // Draw alpha bar: selector
    DrawRectangleRec(selector, Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER + state*3)), guiAlpha));
    //--------------------------------------------------------------------

    return alpha;
}

// Color Bar Hue control
// NOTE: Returns hue value normalized [0..1]
float GuiColorBarHue(rect bounds, float hue)
{
    GuiControlState state = guiState;
    rect selector = { (float)bounds.x - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (float)bounds.y + hue/360.0f*bounds.height - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (float)bounds.width + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)*2, (float)GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_HEIGHT) };

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        vec2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds) ||
            CheckCollisionPointRec(mousePoint, selector))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                state = GUI_STATE_PRESSED;
                selector.y = mousePoint.y - selector.height/2;

                hue = (mousePoint.y - bounds.y)*360/bounds.height;
                if (hue <= 0.0f) hue = 0.0f;
                if (hue >= 359.0f) hue = 359.0f;

            }
            else state = GUI_STATE_FOCUSED;

            /*if (IsKeyDown(KEY_UP))
            {
                hue -= 2.0f;
                if (hue <= 0.0f) hue = 0.0f;
            }
            else if (IsKeyDown(KEY_DOWN))
            {
                hue += 2.0f;
                if (hue >= 360.0f) hue = 360.0f;
            }*/
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (state != GUI_STATE_DISABLED)
    {
        // Draw hue bar:color bars
        DrawRectangleGradientV(bounds.x + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.y + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.width - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (int)bounds.height/6, Fade(RAYGUI_CLITERAL(Color){ 255,0,0,255 }, guiAlpha), Fade(RAYGUI_CLITERAL(Color){ 255,255,0,255 }, guiAlpha));
        DrawRectangleGradientV(bounds.x + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.y + (int)bounds.height/6 + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.width - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (int)bounds.height/6, Fade(RAYGUI_CLITERAL(Color){ 255,255,0,255 }, guiAlpha), Fade(RAYGUI_CLITERAL(Color){ 0,255,0,255 }, guiAlpha));
        DrawRectangleGradientV(bounds.x + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.y + 2*((int)bounds.height/6) + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.width - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (int)bounds.height/6, Fade(RAYGUI_CLITERAL(Color){ 0,255,0,255 }, guiAlpha), Fade(RAYGUI_CLITERAL(Color){ 0,255,255,255 }, guiAlpha));
        DrawRectangleGradientV(bounds.x + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.y + 3*((int)bounds.height/6) + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.width - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (int)bounds.height/6, Fade(RAYGUI_CLITERAL(Color){ 0,255,255,255 }, guiAlpha), Fade(RAYGUI_CLITERAL(Color){ 0,0,255,255 }, guiAlpha));
        DrawRectangleGradientV(bounds.x + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.y + 4*((int)bounds.height/6) + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.width - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (int)bounds.height/6, Fade(RAYGUI_CLITERAL(Color){ 0,0,255,255 }, guiAlpha), Fade(RAYGUI_CLITERAL(Color){ 255,0,255,255 }, guiAlpha));
        DrawRectangleGradientV(bounds.x + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.y + 5*((int)bounds.height/6) + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.width - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (int)bounds.height/6 - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), Fade(RAYGUI_CLITERAL(Color){ 255,0,255,255 }, guiAlpha), Fade(RAYGUI_CLITERAL(Color){ 255,0,0,255 }, guiAlpha));
    }
    else DrawRectangleGradientV(bounds.x, bounds.y, bounds.width, bounds.height, Fade(Fade(GetColor(GuiGetStyle(COLORPICKER, BASE_COLOR_DISABLED)), 0.1f), guiAlpha), Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER_COLOR_DISABLED)), guiAlpha));

    DrawRectangleLinesEx(bounds, 1, Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER + state*3)), guiAlpha));

    // Draw hue bar: selector
    DrawRectangleRec(selector, Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER + state*3)), guiAlpha));
    //--------------------------------------------------------------------

    return hue;
}

// TODO: Color GuiColorBarSat() [WHITE->color]
// TODO: Color GuiColorBarValue() [BLACK->color], HSV / HSL
// TODO: float GuiColorBarLuminance() [BLACK->WHITE]

// Color Picker control
// NOTE: It's divided in multiple controls:
//      Color GuiColorPanel() - Color select panel
//      float GuiColorBarAlpha(rect bounds, float alpha)
//      float GuiColorBarHue(rect bounds, float value)
// NOTE: bounds define GuiColorPanel() size
Color GuiColorPicker(rect bounds, Color color)
{
    color = GuiColorPanel(bounds, color);

    rect boundsHue = { (float)bounds.x + bounds.width + GuiGetStyle(COLORPICKER, HUEBAR_PADDING), (float)bounds.y, (float)GuiGetStyle(COLORPICKER, HUEBAR_WIDTH), (float)bounds.height };
    //rect boundsAlpha = { bounds.x, bounds.y + bounds.height + GuiGetStyle(COLORPICKER, BARS_PADDING), bounds.width, GuiGetStyle(COLORPICKER, BARS_THICK) };

    vec3 hsv = ConvertRGBtoHSV(RAYGUI_CLITERAL(vec3){ color.r/255.0f, color.g/255.0f, color.b/255.0f });
    hsv.x = GuiColorBarHue(boundsHue, hsv.x);
    //color.a = (unsigned char)(GuiColorBarAlpha(boundsAlpha, (float)color.a/255.0f)*255.0f);
    vec3 rgb = ConvertHSVtoRGB(hsv);
    color = RAYGUI_CLITERAL(Color){ (unsigned char)(rgb.x*255.0f), (unsigned char)(rgb.y*255.0f), (unsigned char)(rgb.z*255.0f), color.a };

    return color;
}

// Message Box control
int GuiMessageBox(rect bounds, const char *title, const char *message, const char *buttons)
{
    #define MESSAGEBOX_BUTTON_HEIGHT    24
    #define MESSAGEBOX_BUTTON_PADDING   10

    int clicked = -1;    // Returns clicked button from buttons list, 0 refers to closed window button

    int buttonsCount = 0;
    const char **buttonsText = GuiTextSplit(buttons, &buttonsCount, NULL);
    rect buttonBounds = { 0 };
    buttonBounds.x = bounds.x + MESSAGEBOX_BUTTON_PADDING;
    buttonBounds.y = bounds.y + bounds.height - MESSAGEBOX_BUTTON_HEIGHT - MESSAGEBOX_BUTTON_PADDING;
    buttonBounds.width = (bounds.width - MESSAGEBOX_BUTTON_PADDING*(buttonsCount + 1))/buttonsCount;
    buttonBounds.height = MESSAGEBOX_BUTTON_HEIGHT;

    vec2 textSize = MeasureTextEx(guiFont, message, GuiGetStyle(DEFAULT, TEXT_SIZE), 1);

    rect textBounds = { 0 };
    textBounds.x = bounds.x + bounds.width/2 - textSize.x/2;
    textBounds.y = bounds.y + WINDOW_STATUSBAR_HEIGHT + (bounds.height - WINDOW_STATUSBAR_HEIGHT - MESSAGEBOX_BUTTON_HEIGHT - MESSAGEBOX_BUTTON_PADDING)/2 - textSize.y/2;
    textBounds.width = textSize.x;
    textBounds.height = textSize.y;

    // Draw control
    //--------------------------------------------------------------------
    if (GuiWindowBox(bounds, title)) clicked = 0;

    int prevTextAlignment = GuiGetStyle(LABEL, TEXT_ALIGNMENT);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
    GuiLabel(textBounds, message);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, prevTextAlignment);

    prevTextAlignment = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

    for (int i = 0; i < buttonsCount; i++)
    {
        if (GuiButton(buttonBounds, buttonsText[i])) clicked = i + 1;
        buttonBounds.x += (buttonBounds.width + MESSAGEBOX_BUTTON_PADDING);
    }

    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, prevTextAlignment);
    //--------------------------------------------------------------------

    return clicked;
}

// Text Input Box control, ask for text
int GuiTextInputBox(rect bounds, const char *title, const char *message, const char *buttons, char *text)
{
    #define TEXTINPUTBOX_BUTTON_HEIGHT      24
    #define TEXTINPUTBOX_BUTTON_PADDING     10
    #define TEXTINPUTBOX_HEIGHT             30

    #define TEXTINPUTBOX_MAX_TEXT_LENGTH   256

    // Used to enable text edit mode
    // WARNING: No more than one GuiTextInputBox() should be open at the same time
    static bool textEditMode = false;

    int btnIndex = -1;

    int buttonsCount = 0;
    const char **buttonsText = GuiTextSplit(buttons, &buttonsCount, NULL);
    rect buttonBounds = { 0 };
    buttonBounds.x = bounds.x + TEXTINPUTBOX_BUTTON_PADDING;
    buttonBounds.y = bounds.y + bounds.height - TEXTINPUTBOX_BUTTON_HEIGHT - TEXTINPUTBOX_BUTTON_PADDING;
    buttonBounds.width = (bounds.width - TEXTINPUTBOX_BUTTON_PADDING*(buttonsCount + 1))/buttonsCount;
    buttonBounds.height = TEXTINPUTBOX_BUTTON_HEIGHT;

    int messageInputHeight = bounds.height - WINDOW_STATUSBAR_HEIGHT - GuiGetStyle(STATUSBAR, BORDER_WIDTH) - TEXTINPUTBOX_BUTTON_HEIGHT - 2*TEXTINPUTBOX_BUTTON_PADDING;

    rect textBounds = { 0 };
    if (message != NULL)
    {
        vec2 textSize = MeasureTextEx(guiFont, message, GuiGetStyle(DEFAULT, TEXT_SIZE), 1);

        textBounds.x = bounds.x + bounds.width/2 - textSize.x/2;
        textBounds.y = bounds.y + WINDOW_STATUSBAR_HEIGHT + messageInputHeight/4 - textSize.y/2;
        textBounds.width = textSize.x;
        textBounds.height = textSize.y;
    }

    rect textBoxBounds = { 0 };
    textBoxBounds.x = bounds.x + TEXTINPUTBOX_BUTTON_PADDING;
    textBoxBounds.y = bounds.y + WINDOW_STATUSBAR_HEIGHT - TEXTINPUTBOX_HEIGHT/2;
    if (message == NULL) textBoxBounds.y += messageInputHeight/2;
    else textBoxBounds.y += (messageInputHeight/2 + messageInputHeight/4);
    textBoxBounds.width = bounds.width - TEXTINPUTBOX_BUTTON_PADDING*2;
    textBoxBounds.height = TEXTINPUTBOX_HEIGHT;

    // Draw control
    //--------------------------------------------------------------------
    if (GuiWindowBox(bounds, title)) btnIndex = 0;

    // Draw message if available
    if (message != NULL)
    {
        int prevTextAlignment = GuiGetStyle(LABEL, TEXT_ALIGNMENT);
        GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
        GuiLabel(textBounds, message);
        GuiSetStyle(LABEL, TEXT_ALIGNMENT, prevTextAlignment);
    }

    if (GuiTextBox(textBoxBounds, text, TEXTINPUTBOX_MAX_TEXT_LENGTH, textEditMode)) textEditMode = !textEditMode;

    int prevBtnTextAlignment = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

    for (int i = 0; i < buttonsCount; i++)
    {
        if (GuiButton(buttonBounds, buttonsText[i])) btnIndex = i + 1;
        buttonBounds.x += (buttonBounds.width + MESSAGEBOX_BUTTON_PADDING);
    }

    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, prevBtnTextAlignment);
    //--------------------------------------------------------------------

    return btnIndex;
}

// Grid control
// NOTE: Returns grid mouse-hover selected cell
// About drawing lines at subpixel spacing, simple put, not easy solution:
// https://stackoverflow.com/questions/4435450/2d-opengl-drawing-lines-that-dont-exactly-fit-pixel-raster
vec2 GuiGrid(rect bounds, float spacing, int subdivs)
{
    #if !defined(GRID_COLOR_ALPHA)
        #define GRID_COLOR_ALPHA    0.15f           // Grid lines alpha amount
    #endif

    GuiControlState state = guiState;
    vec2 mousePoint = GetMousePosition();
    vec2 currentCell = { -1, -1 };

    int linesV = ((int)(bounds.width/spacing) + 1)*subdivs;
    int linesH = ((int)(bounds.height/spacing) + 1)*subdivs;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            currentCell.x = (int)((mousePoint.x - bounds.x)/spacing);
            currentCell.y = (int)((mousePoint.y - bounds.y)/spacing);
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case GUI_STATE_NORMAL:
        {
            // Draw vertical grid lines
            for (int i = 0; i < linesV; i++)
            {
                DrawRectangleRec(RAYGUI_CLITERAL(rect){ bounds.x + spacing*i, bounds.y, 1, bounds.height }, ((i%subdivs) == 0)? Fade(GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)), GRID_COLOR_ALPHA*4) : Fade(GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)), GRID_COLOR_ALPHA));
            }

            // Draw horizontal grid lines
            for (int i = 0; i < linesH; i++)
            {
                DrawRectangleRec(RAYGUI_CLITERAL(rect){ bounds.x, bounds.y + spacing*i, bounds.width, 1 }, ((i%subdivs) == 0)? Fade(GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)), GRID_COLOR_ALPHA*4) : Fade(GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)), GRID_COLOR_ALPHA));
            }

        } break;
        default: break;
    }

    return currentCell;
}

//----------------------------------------------------------------------------------
// Styles loading functions
//----------------------------------------------------------------------------------

// Load raygui style file (.rgs)
void GuiLoadStyle(const char *fileName)
{
    bool tryBinary = false;

    // Try reading the files as text file first
    FILE *rgsFile = fopen(fileName, "rt");

    if (rgsFile != NULL)
    {
        char buffer[256] = { 0 };
        fgets(buffer, 256, rgsFile);

        if (buffer[0] == '#')
        {
            int controlId = 0;
            int propertyId = 0;
            int propertyValue = 0;

            while (!feof(rgsFile))
            {
                switch (buffer[0])
                {
                    case 'p':
                    {
                        // Style property: p <control_id> <property_id> <property_value> <property_name>

                        sscanf(buffer, "p %d %d 0x%x", &controlId, &propertyId, &propertyValue);

                        GuiSetStyle(controlId, propertyId, propertyValue);

                    } break;
                    case 'f':
                    {
                        // Style font: f <gen_font_size> <charmap_file> <font_file>

                        int fontSize = 0;
                        char charmapFileName[256] = { 0 };
                        char fontFileName[256] = { 0 };
                        sscanf(buffer, "f %d %s %[^\n]s", &fontSize, charmapFileName, fontFileName);

                        Font font = { 0 };

                        if (charmapFileName[0] != '0')
                        {
                            // Load characters from charmap file,
                            // expected '\n' separated list of integer values
                            char *charValues = LoadText(charmapFileName);
                            if (charValues != NULL)
                            {
                                int charsCount = 0;
                                const char **chars = TextSplit(charValues, '\n', &charsCount);   // WARNING: TextSplit only supports 64 strings!

                                int *values = (int *)malloc(charsCount*sizeof(int));
                                for (int i = 0; i < charsCount; i++) values[i] = atoi(chars[i]);

                                font = LoadFontEx(TextFormat("%s/%s", GetDirectoryPath(fileName), fontFileName), fontSize, values, charsCount);

                                free(values);
                            }
                        }
                        else font = LoadFontEx(TextFormat("%s/%s", GetDirectoryPath(fileName), fontFileName), fontSize, NULL, 0);

                        if ((font.texture.id > 0) && (font.charsCount > 0)) GuiSetFont(font);

                    } break;
                    default: break;
                }

                fgets(buffer, 256, rgsFile);
            }
        }
        else tryBinary = true;

        fclose(rgsFile);
    }

    if (tryBinary)
    {
        rgsFile = fopen(fileName, "rb");

        if (rgsFile == NULL) return;

        char signature[5] = "";
        short version = 0;
        short reserved = 0;
        int propertiesCount = 0;

        fread(signature, 1, 4, rgsFile);
        fread(&version, 1, sizeof(short), rgsFile);
        fread(&reserved, 1, sizeof(short), rgsFile);
        fread(&propertiesCount, 1, sizeof(int), rgsFile);

        if ((signature[0] == 'r') &&
            (signature[1] == 'G') &&
            (signature[2] == 'S') &&
            (signature[3] == ' '))
        {
            short controlId = 0;
            short propertyId = 0;
            int propertyValue = 0;

            for (int i = 0; i < propertiesCount; i++)
            {
                fread(&controlId, 1, sizeof(short), rgsFile);
                fread(&propertyId, 1, sizeof(short), rgsFile);
                fread(&propertyValue, 1, sizeof(int), rgsFile);

                if (controlId == 0) // DEFAULT control
                {
                    // If a DEFAULT property is loaded, it is propagated to all controls
                    // NOTE: All DEFAULT properties should be defined first in the file
                    GuiSetStyle(0, (int)propertyId, propertyValue);

                    if (propertyId < NUM_PROPS_DEFAULT) for (int i = 1; i < NUM_CONTROLS; i++) GuiSetStyle(i, (int)propertyId, propertyValue);
                }
                else GuiSetStyle((int)controlId, (int)propertyId, propertyValue);
            }
        }

        fclose(rgsFile);
    }
}

// Load style default over global style
void GuiLoadStyleDefault(void)
{
    // We set this variable first to avoid cyclic function calls
    // when calling GuiSetStyle() and GuiGetStyle()
    guiStyleLoaded = true;

    // Initialize default LIGHT style property values
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x838383ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0xc9c9c9ff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0x686868ff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, 0x5bb2d9ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0xc9effeff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0x6c9bbcff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, 0x0492c7ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x97e8ffff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, 0x368bafff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_DISABLED, 0xb5c1c2ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_DISABLED, 0xe6e9e9ff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_DISABLED, 0xaeb7b8ff);
    GuiSetStyle(DEFAULT, BORDER_WIDTH, 1);                       // WARNING: Some controls use other values
    GuiSetStyle(DEFAULT, TEXT_PADDING, 0);                       // WARNING: Some controls use other values
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER); // WARNING: Some controls use other values

    // Initialize control-specific property values
    // NOTE: Those properties are in default list but require specific values by control type
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 2);
    GuiSetStyle(SLIDER, TEXT_PADDING, 5);
    GuiSetStyle(CHECKBOX, TEXT_PADDING, 5);
    GuiSetStyle(CHECKBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);
    GuiSetStyle(TEXTBOX, TEXT_PADDING, 5);
    GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
    GuiSetStyle(VALUEBOX, TEXT_PADDING, 4);
    GuiSetStyle(VALUEBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
    GuiSetStyle(SPINNER, TEXT_PADDING, 4);
    GuiSetStyle(SPINNER, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
    GuiSetStyle(STATUSBAR, TEXT_PADDING, 6);
    GuiSetStyle(STATUSBAR, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);

    // Initialize extended property values
    // NOTE: By default, extended property values are initialized to 0
    GuiSetStyle(DEFAULT, TEXT_SIZE, 10);                // DEFAULT, shared by all controls
    GuiSetStyle(DEFAULT, TEXT_SPACING, 1);              // DEFAULT, shared by all controls
    GuiSetStyle(DEFAULT, LINE_COLOR, 0x90abb5ff);       // DEFAULT specific property
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0xf5f5f5ff); // DEFAULT specific property
    GuiSetStyle(TOGGLE, GROUP_PADDING, 2);
    GuiSetStyle(SLIDER, SLIDER_WIDTH, 15);
    GuiSetStyle(SLIDER, SLIDER_PADDING, 1);
    GuiSetStyle(PROGRESSBAR, PROGRESS_PADDING, 1);
    GuiSetStyle(CHECKBOX, CHECK_PADDING, 1);
    GuiSetStyle(COMBOBOX, COMBO_BUTTON_WIDTH, 30);
    GuiSetStyle(COMBOBOX, COMBO_BUTTON_PADDING, 2);
    GuiSetStyle(DROPDOWNBOX, ARROW_PADDING, 16);
    GuiSetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_PADDING, 2);
    GuiSetStyle(TEXTBOX, TEXT_LINES_PADDING, 5);
    GuiSetStyle(TEXTBOX, TEXT_INNER_PADDING, 4);
    GuiSetStyle(TEXTBOX, COLOR_SELECTED_FG, 0xf0fffeff);
    GuiSetStyle(TEXTBOX, COLOR_SELECTED_BG, 0x839affe0);
    GuiSetStyle(SPINNER, SPIN_BUTTON_WIDTH, 20);
    GuiSetStyle(SPINNER, SPIN_BUTTON_PADDING, 2);
    GuiSetStyle(SCROLLBAR, BORDER_WIDTH, 0);
    GuiSetStyle(SCROLLBAR, ARROWS_VISIBLE, 0);
    GuiSetStyle(SCROLLBAR, ARROWS_SIZE, 6);
    GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING, 0);
    GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, 16);
    GuiSetStyle(SCROLLBAR, SCROLL_PADDING, 0);
    GuiSetStyle(SCROLLBAR, SCROLL_SPEED, 10);
    GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, 0x1e);
    GuiSetStyle(LISTVIEW, LIST_ITEMS_PADDING, 2);
    GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, 10);
    GuiSetStyle(LISTVIEW, SCROLLBAR_SIDE, SCROLLBAR_RIGHT_SIDE);
    GuiSetStyle(COLORPICKER, COLOR_SELECTOR_SIZE, 6);
    GuiSetStyle(COLORPICKER, HUEBAR_WIDTH, 0x14);
    GuiSetStyle(COLORPICKER, HUEBAR_PADDING, 0xa);
    GuiSetStyle(COLORPICKER, HUEBAR_SELECTOR_HEIGHT, 6);
    GuiSetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW, 2);

    guiFont = GetFontDefault();     // Initialize default font
}

// Get text with icon id prepended
// NOTE: Useful to add icons by name id (enum) instead of
// a number that can change between ricon versions
const char *GuiIconText(int iconId, const char *text)
{
#if defined(GUI_SUPPORT_ICONS)
    static char buffer[1024] = { 0 };
    memset(buffer, 0, 1024);

    sprintf(buffer, "#%03i#", iconId);

    if (text != NULL)
    {
        for (int i = 5; i < 1024; i++)
        {
            buffer[i] = text[i - 5];
            if (text[i - 5] == '\0') break;
        }
    }

    return buffer;
#else
    return NULL;
#endif
}

#if defined(GUI_SUPPORT_ICONS)

// Get full icons data pointer
unsigned int *GuiGetIcons(void) { return guiIcons; }

// Load raygui icons file (.rgi)
// NOTE: In case nameIds are required, they can be requested with loadIconsName,
// they are returned as a guiIconsName[iconsCount][RICON_MAX_NAME_LENGTH],
// guiIconsName[]][] memory should be manually freed!
char **GuiLoadIcons(const char *fileName, bool loadIconsName)
{
    // Style File Structure (.rgi)
    // ------------------------------------------------------
    // Offset  | Size    | Type       | Description
    // ------------------------------------------------------
    // 0       | 4       | char       | Signature: "rGI "
    // 4       | 2       | short      | Version: 100
    // 6       | 2       | short      | reserved
    
    // 8       | 2       | short      | Num icons (N)
    // 10      | 2       | short      | Icons size (Options: 16, 32, 64) (S)

    // Icons name id (32 bytes per name id)
    // foreach (icon)
    // {
    //   12+32*i  | 32   | char       | Icon NameId
    // }
    
    // Icons data: One bit per pixel, stored as unsigned int array (depends on icon size)
    // S*S pixels/32bit per unsigned int = K unsigned int per icon
    // foreach (icon)
    // {
    //   ...   | K       | unsigned int | Icon Data
    // }

    FILE *rgiFile = fopen(fileName, "rb");
    
    char **guiIconsName = NULL;

    if (rgiFile != NULL)
    {
        char signature[5] = "";
        short version = 0;
        short reserved = 0;
        short iconsCount = 0;
        short iconsSize = 0;

        fread(signature, 1, 4, rgiFile);
        fread(&version, 1, sizeof(short), rgiFile);
        fread(&reserved, 1, sizeof(short), rgiFile);
        fread(&iconsCount, 1, sizeof(short), rgiFile);
        fread(&iconsSize, 1, sizeof(short), rgiFile);

        if ((signature[0] == 'r') &&
            (signature[1] == 'G') &&
            (signature[2] == 'I') &&
            (signature[3] == ' '))
        {
            if (loadIconsName)
            {
                guiIconsName = (char **)malloc(iconsCount*sizeof(char **));
                for (int i = 0; i < iconsCount; i++)
                {
                    guiIconsName[i] = (char *)malloc(RICON_MAX_NAME_LENGTH);
                    fread(guiIconsName[i], 32, 1, rgiFile);
                }
            }

            // Read icons data directly over guiIcons data array
            fread(guiIcons, iconsCount*(iconsSize*iconsSize/32), sizeof(unsigned int), rgiFile);
        }
    
        fclose(rgiFile);
    }
    
    return guiIconsName;
}

// Draw selected icon using rectangles pixel-by-pixel
void GuiDrawIcon(int iconId, vec2 position, int pixelSize, Color color)
{
    #define BIT_CHECK(a,b) ((a) & (1<<(b)))

    for (int i = 0, y = 0; i < RICON_SIZE*RICON_SIZE/32; i++)
    {
        for (int k = 0; k < 32; k++)
        {
            if (BIT_CHECK(guiIcons[iconId*RICON_DATA_ELEMENTS + i], k))
            {
                DrawRectangle(position.x + (k%RICON_SIZE)*pixelSize, position.y + y*pixelSize, pixelSize, pixelSize, color);
            }
            
            if ((k == 15) || (k == 31)) y++;
        }
    }
}

// Get icon bit data
// NOTE: Bit data array grouped as unsigned int (ICON_SIZE*ICON_SIZE/32 elements)
unsigned int *GuiGetIconData(int iconId)
{
    static unsigned int iconData[RICON_DATA_ELEMENTS] = { 0 };
    memset(iconData, 0, RICON_DATA_ELEMENTS*sizeof(unsigned int));

    if (iconId < RICON_MAX_ICONS) memcpy(iconData, &guiIcons[iconId*RICON_DATA_ELEMENTS], RICON_DATA_ELEMENTS*sizeof(unsigned int));
    
    return iconData;
}

// Set icon bit data
// NOTE: Data must be provided as unsigned int array (ICON_SIZE*ICON_SIZE/32 elements)
void GuiSetIconData(int iconId, unsigned int *data)
{
    if (iconId < RICON_MAX_ICONS) memcpy(&guiIcons[iconId*RICON_DATA_ELEMENTS], data, RICON_DATA_ELEMENTS*sizeof(unsigned int));
}

// Set icon pixel value
void GuiSetIconPixel(int iconId, int x, int y)
{
    #define BIT_SET(a,b)   ((a) |= (1<<(b)))
    
    // This logic works for any RICON_SIZE pixels icons,
    // For example, in case of 16x16 pixels, every 2 lines fit in one unsigned int data element
    BIT_SET(guiIcons[iconId*RICON_DATA_ELEMENTS + y/(sizeof(unsigned int)*8/RICON_SIZE)], x + (y%(sizeof(unsigned int)*8/RICON_SIZE)*RICON_SIZE));
}

// Clear icon pixel value
void GuiClearIconPixel(int iconId, int x, int y)
{
    #define BIT_CLEAR(a,b) ((a) &= ~((1)<<(b)))

    // This logic works for any RICON_SIZE pixels icons,
    // For example, in case of 16x16 pixels, every 2 lines fit in one unsigned int data element
    BIT_CLEAR(guiIcons[iconId*RICON_DATA_ELEMENTS + y/(sizeof(unsigned int)*8/RICON_SIZE)], x + (y%(sizeof(unsigned int)*8/RICON_SIZE)*RICON_SIZE));
}

// Check icon pixel value 
bool GuiCheckIconPixel(int iconId, int x, int y)
{
    #define BIT_CHECK(a,b) ((a) & (1<<(b)))

    return (BIT_CHECK(guiIcons[iconId*8 + y/2], x + (y%2*16)));
}
#endif      // GUI_SUPPORT_ICONS

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// Split controls text into multiple strings
// Also check for multiple columns (required by GuiToggleGroup())
static const char **GuiTextSplit(const char *text, int *count, int *textRow)
{
    // NOTE: Current implementation returns a copy of the provided string with '\0' (string end delimiter)
    // inserted between strings defined by "delimiter" parameter. No memory is dynamically allocated,
    // all used memory is static... it has some limitations:
    //      1. Maximum number of possible split strings is set by TEXTSPLIT_MAX_TEXT_ELEMENTS
    //      2. Maximum size of text to split is TEXTSPLIT_MAX_TEXT_LENGTH
    // NOTE: Those definitions could be externally provided if required

    #if !defined(TEXTSPLIT_MAX_TEXT_LENGTH)
        #define TEXTSPLIT_MAX_TEXT_LENGTH      1024
    #endif

    #if !defined(TEXTSPLIT_MAX_TEXT_ELEMENTS)
        #define TEXTSPLIT_MAX_TEXT_ELEMENTS     128
    #endif

    static const char *result[TEXTSPLIT_MAX_TEXT_ELEMENTS] = { NULL };
    static char buffer[TEXTSPLIT_MAX_TEXT_LENGTH] = { 0 };
    memset(buffer, 0, TEXTSPLIT_MAX_TEXT_LENGTH);

    result[0] = buffer;
    int counter = 1;

    if (textRow != NULL) textRow[0] = 0;

    // Count how many substrings we have on text and point to every one
    for (int i = 0; i < TEXTSPLIT_MAX_TEXT_LENGTH; i++)
    {
        buffer[i] = text[i];
        if (buffer[i] == '\0') break;
        else if ((buffer[i] == ';') || (buffer[i] == '\n'))
        {
            result[counter] = buffer + i + 1;

            if (textRow != NULL)
            {
                if (buffer[i] == '\n') textRow[counter] = textRow[counter - 1] + 1;
                else textRow[counter] = textRow[counter - 1];
            }

            buffer[i] = '\0';   // Set an end of string at this point

            counter++;
            if (counter == TEXTSPLIT_MAX_TEXT_ELEMENTS) break;
        }
    }

    *count = counter;

    return result;
}

// Convert color data from RGB to HSV
// NOTE: Color data should be passed normalized
static vec3 ConvertRGBtoHSV(vec3 rgb)
{
    vec3 hsv = { 0.0f };
    float min = 0.0f;
    float max = 0.0f;
    float delta = 0.0f;

    min = (rgb.x < rgb.y)? rgb.x : rgb.y;
    min = (min < rgb.z)? min  : rgb.z;

    max = (rgb.x > rgb.y)? rgb.x : rgb.y;
    max = (max > rgb.z)? max  : rgb.z;

    hsv.z = max;            // Value
    delta = max - min;

    if (delta < 0.00001f)
    {
        hsv.y = 0.0f;
        hsv.x = 0.0f;       // Undefined, maybe NAN?
        return hsv;
    }

    if (max > 0.0f)
    {
        // NOTE: If max is 0, this divide would cause a crash
        hsv.y = (delta/max);    // Saturation
    }
    else
    {
        // NOTE: If max is 0, then r = g = b = 0, s = 0, h is undefined
        hsv.y = 0.0f;
        hsv.x = 0.0f;        // Undefined, maybe NAN?
        return hsv;
    }

    // NOTE: Comparing float values could not work properly
    if (rgb.x >= max) hsv.x = (rgb.y - rgb.z)/delta;    // Between yellow & magenta
    else
    {
        if (rgb.y >= max) hsv.x = 2.0f + (rgb.z - rgb.x)/delta;  // Between cyan & yellow
        else hsv.x = 4.0f + (rgb.x - rgb.y)/delta;      // Between magenta & cyan
    }

    hsv.x *= 60.0f;     // Convert to degrees

    if (hsv.x < 0.0f) hsv.x += 360.0f;

    return hsv;
}

// Convert color data from HSV to RGB
// NOTE: Color data should be passed normalized
static vec3 ConvertHSVtoRGB(vec3 hsv)
{
    vec3 rgb = { 0.0f };
    float hh = 0.0f, p = 0.0f, q = 0.0f, t = 0.0f, ff = 0.0f;
    long i = 0;

    // NOTE: Comparing float values could not work properly
    if (hsv.y <= 0.0f)
    {
        rgb.x = hsv.z;
        rgb.y = hsv.z;
        rgb.z = hsv.z;
        return rgb;
    }

    hh = hsv.x;
    if (hh >= 360.0f) hh = 0.0f;
    hh /= 60.0f;

    i = (long)hh;
    ff = hh - i;
    p = hsv.z*(1.0f - hsv.y);
    q = hsv.z*(1.0f - (hsv.y*ff));
    t = hsv.z*(1.0f - (hsv.y*(1.0f - ff)));

    switch (i)
    {
        case 0:
        {
            rgb.x = hsv.z;
            rgb.y = t;
            rgb.z = p;
        } break;
        case 1:
        {
            rgb.x = q;
            rgb.y = hsv.z;
            rgb.z = p;
        } break;
        case 2:
        {
            rgb.x = p;
            rgb.y = hsv.z;
            rgb.z = t;
        } break;
        case 3:
        {
            rgb.x = p;
            rgb.y = q;
            rgb.z = hsv.z;
        } break;
        case 4:
        {
            rgb.x = t;
            rgb.y = p;
            rgb.z = hsv.z;
        } break;
        case 5:
        default:
        {
            rgb.x = hsv.z;
            rgb.y = p;
            rgb.z = q;
        } break;
    }

    return rgb;
}
