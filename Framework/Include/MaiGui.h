#pragma once

#include "./MaiDef.h"

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define NUM_CONTROLS                    16      // Number of standard controls
#define NUM_PROPS_DEFAULT               16      // Number of standard properties
#define NUM_PROPS_EXTENDED               8      // Number of extended properties

#define TEXTEDIT_CURSOR_BLINK_FRAMES    20      // Text edit controls cursor blink timming

// Style property
typedef struct GuiStyleProp {
    unsigned short controlId;
    unsigned short propertyId;
    int propertyValue;
} GuiStyleProp;

// Gui control state
typedef enum {
    GUI_STATE_NORMAL = 0,
    GUI_STATE_FOCUSED,
    GUI_STATE_PRESSED,
    GUI_STATE_DISABLED,
} GuiControlState;

// Gui control text alignment
typedef enum {
    GUI_TEXT_ALIGN_LEFT = 0,
    GUI_TEXT_ALIGN_CENTER,
    GUI_TEXT_ALIGN_RIGHT,
} GuiTextAlignment;

// Gui controls
typedef enum {
    DEFAULT = 0,
    LABEL,          // LABELBUTTON
    BUTTON,         // IMAGEBUTTON
    TOGGLE,         // TOGGLEGROUP
    SLIDER,         // SLIDERBAR
    PROGRESSBAR,
    CHECKBOX,
    COMBOBOX,
    DROPDOWNBOX,
    TEXTBOX,        // TEXTBOXMULTI
    VALUEBOX,
    SPINNER,
    LISTVIEW,
    COLORPICKER,
    SCROLLBAR,
    STATUSBAR
} GuiControl;

// Gui base properties for every control
typedef enum {
    BORDER_COLOR_NORMAL = 0,
    BASE_COLOR_NORMAL,
    TEXT_COLOR_NORMAL,
    BORDER_COLOR_FOCUSED,
    BASE_COLOR_FOCUSED,
    TEXT_COLOR_FOCUSED,
    BORDER_COLOR_PRESSED,
    BASE_COLOR_PRESSED,
    TEXT_COLOR_PRESSED,
    BORDER_COLOR_DISABLED,
    BASE_COLOR_DISABLED,
    TEXT_COLOR_DISABLED,
    BORDER_WIDTH,
    TEXT_PADDING,
    TEXT_ALIGNMENT,
    RESERVED
} GuiControlProperty;

// Gui extended properties depend on control
// NOTE: We reserve a fixed size of additional properties per control

// DEFAULT properties
typedef enum {
    TEXT_SIZE = 16,
    TEXT_SPACING,
    LINE_COLOR,
    BACKGROUND_COLOR,
} GuiDefaultProperty;

// Label
//typedef enum { } GuiLabelProperty;

// Button
//typedef enum { } GuiButtonProperty;

// Toggle / ToggleGroup
typedef enum {
    GROUP_PADDING = 16,
} GuiToggleProperty;

// Slider / SliderBar
typedef enum {
    SLIDER_WIDTH = 16,
    SLIDER_PADDING
} GuiSliderProperty;

// ProgressBar
typedef enum {
    PROGRESS_PADDING = 16,
} GuiProgressBarProperty;

// CheckBox
typedef enum {
    CHECK_PADDING = 16
} GuiCheckBoxProperty;

// ComboBox
typedef enum {
    COMBO_BUTTON_WIDTH = 16,
    COMBO_BUTTON_PADDING
} GuiComboBoxProperty;

// DropdownBox
typedef enum {
    ARROW_PADDING = 16,
    DROPDOWN_ITEMS_PADDING
} GuiDropdownBoxProperty;

// TextBox / TextBoxMulti / ValueBox / Spinner
typedef enum {
    TEXT_INNER_PADDING = 16,
    TEXT_LINES_PADDING,
    COLOR_SELECTED_FG,
    COLOR_SELECTED_BG
} GuiTextBoxProperty;

// Spinner
typedef enum {
    SPIN_BUTTON_WIDTH = 16,
    SPIN_BUTTON_PADDING,
} GuiSpinnerProperty;

// ScrollBar
typedef enum {
    ARROWS_SIZE = 16,
    ARROWS_VISIBLE,
    SCROLL_SLIDER_PADDING,
    SCROLL_SLIDER_SIZE,
    SCROLL_PADDING,
    SCROLL_SPEED,
} GuiScrollBarProperty;

// ScrollBar side
typedef enum {
    SCROLLBAR_LEFT_SIDE = 0,
    SCROLLBAR_RIGHT_SIDE
} GuiScrollBarSide;

// ListView
typedef enum {
    LIST_ITEMS_HEIGHT = 16,
    LIST_ITEMS_PADDING,
    SCROLLBAR_WIDTH,
    SCROLLBAR_SIDE,
} GuiListViewProperty;

// ColorPicker
typedef enum {
    COLOR_SELECTOR_SIZE = 16,
    HUEBAR_WIDTH,                  // Right hue bar width
    HUEBAR_PADDING,                // Right hue bar separation from panel
    HUEBAR_SELECTOR_HEIGHT,        // Right hue bar selector height
    HUEBAR_SELECTOR_OVERFLOW       // Right hue bar selector overflow
} GuiColorPickerProperty;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif

// Global gui modification functions
MAILIB_API void GuiEnable(void);                                         // Enable gui controls (global state)
MAILIB_API void GuiDisable(void);                                        // Disable gui controls (global state)
MAILIB_API void GuiLock(void);                                           // Lock gui controls (global state)
MAILIB_API void GuiUnlock(void);                                         // Unlock gui controls (global state)
MAILIB_API void GuiFade(float alpha);                                    // Set gui controls alpha (global state), alpha goes from 0.0f to 1.0f

MAILIB_API void GuiSetState(int state);                                  // Set gui state (global state)
MAILIB_API int GuiGetState(void);                                        // Get gui state (global state)

MAILIB_API void GuiSetFont(Font font);                                   // Set gui custom font (global state)
MAILIB_API Font GuiGetFont(void);                                        // Get gui custom font (global state)

// Style set/get functions
MAILIB_API void GuiSetStyle(int control, int property, int value);       // Set one style property
MAILIB_API int GuiGetStyle(int control, int property);                   // Get one style property

// Container/separator controls, useful for controls organization
MAILIB_API bool GuiWindowBox(rect bounds, const char *title);                                       // Window Box control, shows a window that can be closed
MAILIB_API void GuiGroupBox(rect bounds, const char *text);                                         // Group Box control with text name
MAILIB_API void GuiLine(rect bounds, const char *text);                                             // Line separator control, could contain text
MAILIB_API void GuiPanel(rect bounds);                                                              // Panel control, useful to group controls
MAILIB_API rect GuiScrollPanel(rect bounds, rect content, vec2 *scroll);               // Scroll Panel control

// Basic controls set
MAILIB_API void GuiLabel(rect bounds, const char *text);                                            // Label control, shows text
MAILIB_API bool GuiButton(rect bounds, const char *text);                                           // Button control, returns true when clicked
MAILIB_API bool GuiLabelButton(rect bounds, const char *text);                                      // Label button control, show true when clicked
MAILIB_API bool GuiImageButton(rect bounds, const char *text, Texture texture);                   // Image button control, returns true when clicked
MAILIB_API bool GuiImageButtonEx(rect bounds, const char *text, Texture texture, rect texSource);    // Image button extended control, returns true when clicked
MAILIB_API bool GuiToggle(rect bounds, const char *text, bool active);                              // Toggle Button control, returns true when active
MAILIB_API int GuiToggleGroup(rect bounds, const char *text, int active);                           // Toggle Group control, returns active toggle index
MAILIB_API bool GuiCheckBox(rect bounds, const char *text, bool checked);                           // Check Box control, returns true when active
MAILIB_API int GuiComboBox(rect bounds, const char *text, int active);                              // Combo Box control, returns selected item index
MAILIB_API bool GuiDropdownBox(rect bounds, const char *text, int *active, bool editMode);          // Dropdown Box control, returns selected item
MAILIB_API bool GuiSpinner(rect bounds, const char *text, int *value, int minValue, int maxValue, bool editMode);     // Spinner control, returns selected value
MAILIB_API bool GuiValueBox(rect bounds, const char *text, int *value, int minValue, int maxValue, bool editMode);    // Value Box control, updates input text with numbers
MAILIB_API bool GuiTextBox(rect bounds, char *text, int textSize, bool editMode);                   // Text Box control, updates input text
MAILIB_API bool GuiTextBoxMulti(rect bounds, char *text, int textSize, bool editMode);              // Text Box control with multiple lines
MAILIB_API float GuiSlider(rect bounds, const char *textLeft, const char *textRight, float value, float minValue, float maxValue);       // Slider control, returns selected value
MAILIB_API float GuiSliderBar(rect bounds, const char *textLeft, const char *textRight, float value, float minValue, float maxValue);    // Slider Bar control, returns selected value
MAILIB_API float GuiProgressBar(rect bounds, const char *textLeft, const char *textRight, float value, float minValue, float maxValue);  // Progress Bar control, shows current progress value
MAILIB_API void GuiStatusBar(rect bounds, const char *text);                                        // Status Bar control, shows info text
MAILIB_API void GuiDummyRec(rect bounds, const char *text);                                         // Dummy control for placeholders
MAILIB_API int GuiScrollBar(rect bounds, int value, int minValue, int maxValue);                    // Scroll Bar control
MAILIB_API vec2 GuiGrid(rect bounds, float spacing, int subdivs);                                // Grid control

// Advance controls set
MAILIB_API int GuiListView(rect bounds, const char *text, int *scrollIndex, int active);            // List View control, returns selected list item index
MAILIB_API int GuiListViewEx(rect bounds, const char **text, int count, int *focus, int *scrollIndex, int active);      // List View with extended parameters
MAILIB_API int GuiMessageBox(rect bounds, const char *title, const char *message, const char *buttons);                 // Message Box control, displays a message
MAILIB_API int GuiTextInputBox(rect bounds, const char *title, const char *message, const char *buttons, char *text);   // Text Input Box control, ask for text
MAILIB_API Color GuiColorPicker(rect bounds, Color color);                                          // Color Picker control

// Styles loading functions
MAILIB_API void GuiLoadStyle(const char *fileName);              // Load style file (.rgs)
MAILIB_API void GuiLoadStyleDefault(void);                       // Load style default over global style

/*
typedef GuiStyle (unsigned int *)
MAILIB_API GuiStyle LoadGuiStyle(const char *fileName);          // Load style from file (.rgs)
MAILIB_API void UnloadGuiStyle(GuiStyle style);                  // Unload style
*/

MAILIB_API const char *GuiIconText(int iconId, const char *text); // Get text with icon id prepended (if supported)

#if defined(GUI_SUPPORT_ICONS)
// Gui icons functionality
MAILIB_API void GuiDrawIcon(int iconId, vec2 position, int pixelSize, Color color);

MAILIB_API unsigned int *GuiGetIcons(void);                      // Get full icons data pointer
MAILIB_API unsigned int *GuiGetIconData(int iconId);             // Get icon bit data
MAILIB_API void GuiSetIconData(int iconId, unsigned int *data);  // Set icon bit data

MAILIB_API void GuiSetIconPixel(int iconId, int x, int y);       // Set icon pixel value
MAILIB_API void GuiClearIconPixel(int iconId, int x, int y);     // Clear icon pixel value
MAILIB_API bool GuiCheckIconPixel(int iconId, int x, int y);     // Check icon pixel value
#endif

#ifdef __cplusplus
}
#endif

