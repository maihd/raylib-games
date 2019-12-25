#pragma once

#include "./MaiTypes.h"

#if defined(_WIN32) && defined(BUILD_LIBTYPE_SHARED)
#   define MAILIB_API __declspec(dllexport)
#elif defined(_WIN32) && defined(USE_LIBTYPE_SHARED)
#   define MAILIB_API __declspec(dllimport)
#else
#   define MAILIB_API extern
#endif

#if defined(__cplusplus)
extern "C" {            // Prevents name mangling of functions
#endif

//------------------------------------------------------------------------------------
// Window and Graphics Device Functions (Module: core)
//------------------------------------------------------------------------------------

// Window-related functions
MAILIB_API void InitWindow(int width, int height, const char *title);  // Initialize window and OpenGL context
MAILIB_API bool WindowShouldClose(void);                               // Check if KEY_ESCAPE pressed or Close icon pressed
MAILIB_API bool PollWindowEvents(void);                                // Update window
MAILIB_API void CloseWindow(void);                                     // Close window and unload OpenGL context
MAILIB_API bool IsWindowReady(void);                                   // Check if window has been initialized successfully
MAILIB_API bool IsWindowMinimized(void);                               // Check if window has been minimized (or lost focus)
MAILIB_API bool IsWindowResized(void);                                 // Check if window has been resized
MAILIB_API bool IsWindowHidden(void);                                  // Check if window is currently hidden
MAILIB_API void ToggleFullscreen(void);                                // Toggle fullscreen mode (only PLATFORM_DESKTOP)
MAILIB_API void UnhideWindow(void);                                    // Show the window
MAILIB_API void HideWindow(void);                                      // Hide the window
MAILIB_API void SetWindowIcon(Image image);                            // Set icon for window (only PLATFORM_DESKTOP)
MAILIB_API void SetWindowTitle(const char *title);                     // Set title for window (only PLATFORM_DESKTOP)
MAILIB_API void SetWindowPosition(int x, int y);                       // Set window position on screen (only PLATFORM_DESKTOP)
MAILIB_API void SetWindowMonitor(int monitor);                         // Set monitor for the current window (fullscreen mode)
MAILIB_API void SetWindowMinSize(int width, int height);               // Set window minimum dimensions (for FLAG_WINDOW_RESIZABLE)
MAILIB_API void SetWindowSize(int width, int height);                  // Set window dimensions
MAILIB_API void *GetWindowHandle(void);                                // Get native window handle
MAILIB_API int GetScreenWidth(void);                                   // Get current screen width
MAILIB_API int GetScreenHeight(void);                                  // Get current screen height
MAILIB_API int GetMonitorCount(void);                                  // Get number of connected monitors
MAILIB_API int GetMonitorWidth(int monitor);                           // Get primary monitor width
MAILIB_API int GetMonitorHeight(int monitor);                          // Get primary monitor height
MAILIB_API int GetMonitorPhysicalWidth(int monitor);                   // Get primary monitor physical width in millimetres
MAILIB_API int GetMonitorPhysicalHeight(int monitor);                  // Get primary monitor physical height in millimetres
MAILIB_API const char *GetMonitorName(int monitor);                    // Get the human-readable, UTF-8 encoded name of the primary monitor
MAILIB_API const char *GetClipboardText(void);                         // Get clipboard text content
MAILIB_API void SetClipboardText(const char *text);                    // Set clipboard text content

// Cursor-related functions
MAILIB_API void ShowCursor(void);                                      // Shows cursor
MAILIB_API void HideCursor(void);                                      // Hides cursor
MAILIB_API bool IsCursorHidden(void);                                  // Check if cursor is not visible
MAILIB_API void EnableCursor(void);                                    // Enables cursor (unlock cursor)
MAILIB_API void DisableCursor(void);                                   // Disables cursor (lock cursor)

// Drawing-related functions
MAILIB_API void ClearBackground(Color color);                          // Set background color (framebuffer clear color)
MAILIB_API void BeginDrawing(void);                                    // Setup canvas (framebuffer) to start drawing
MAILIB_API void EndDrawing(void);                                      // End canvas drawing and swap buffers (double buffering)
MAILIB_API void BeginMode2D(Camera2D camera);                          // Initialize 2D mode with custom camera (2D)
MAILIB_API void EndMode2D(void);                                       // Ends 2D mode with custom camera
MAILIB_API void BeginMode3D(Camera camera);                          // Initializes 3D mode with custom camera (3D)
MAILIB_API void EndMode3D(void);                                       // Ends 3D mode and returns to default 2D orthographic mode
MAILIB_API void BeginTextureMode(RenderTexture target);              // Initializes render texture for drawing
MAILIB_API void EndTextureMode(void);                                  // Ends drawing to render texture

// Screen-space-related functions
MAILIB_API Ray GetMouseRay(vec2 mousePosition, Camera camera);      // Returns a ray trace from mouse position
MAILIB_API vec2 GetWorldToScreen(vec3 position, Camera camera);  // Returns the screen space position for a 3d world space position
MAILIB_API mat4 GetCameraMatrix(Camera camera);                      // Returns camera transform matrix (view matrix)

// Timing-related functions
MAILIB_API void SetTargetFPS(int fps);                                 // Set target FPS (maximum)
MAILIB_API int GetFPS(void);                                           // Returns current FPS
MAILIB_API int GetFrameCount(void);                                    // Get total frames have passed
MAILIB_API float GetFrameTime(void);                                   // Returns time in seconds for last frame drawn
MAILIB_API float GetDeltaTime(void);                                   // Returns time in seconds for last frame drawn
MAILIB_API float GetTotalTime(void);                                   // Returns time in seconds for total frames taken
MAILIB_API double GetTime(void);                                       // Returns elapsed time in seconds since InitWindow()

// Color-related functions
MAILIB_API int ColorToInt(Color color);                                // Returns hexadecimal value for a Color
MAILIB_API vec4 ColorNormalize(Color color);                        // Returns color normalized as float [0..1]
MAILIB_API vec3 ColorToHSV(Color color);                            // Returns HSV values for a Color
MAILIB_API Color ColorFromHSV(vec3 hsv);                            // Returns a Color from HSV values
MAILIB_API Color ColorFromVec4(vec4 color);
MAILIB_API Color GetColor(int hexValue);                               // Returns a Color struct from hexadecimal value
MAILIB_API Color Fade(Color color, float alpha);                       // Color fade-in or fade-out, alpha goes from 0.0f to 1.0f

// Misc. functions
MAILIB_API void SetConfigFlags(unsigned char flags);                   // Setup window configuration flags (view FLAGS)
MAILIB_API void SetTraceLogLevel(int logType);                         // Set the current threshold (minimum) log level
MAILIB_API void SetTraceLogExit(int logType);                          // Set the exit threshold (minimum) log level
MAILIB_API void SetTraceLogCallback(TraceLogCallback callback);        // Set a trace log callback to enable custom logging
MAILIB_API void TraceLog(int logType, const char *text, ...);          // Show trace log messages (LOG_DEBUG, LOG_INFO, LOG_WARNING, LOG_ERROR)
MAILIB_API void TakeScreenshot(const char *fileName);                  // Takes a screenshot of current screen (saved a .png)
MAILIB_API int GetRandomValue(int min, int max);                       // Returns a random value between min and max (both included)

// Files management functions
MAILIB_API bool FileExists(const char *fileName);                      // Check if file exists
MAILIB_API bool IsFileExtension(const char *fileName, const char *ext);// Check file extension
MAILIB_API const char *GetExtension(const char *fileName);             // Get pointer to extension for a filename string
MAILIB_API const char *GetFileName(const char *filePath);              // Get pointer to filename for a path string
MAILIB_API const char *GetFileNameWithoutExt(const char *filePath);    // Get filename string without extension (memory should be freed)
MAILIB_API const char *GetDirectoryPath(const char *fileName);         // Get full path for a given fileName (uses static string)
MAILIB_API const char *GetWorkingDirectory(void);                      // Get current working directory (uses static string)
MAILIB_API char **GetDirectoryFiles(const char *dirPath, int *count);  // Get filenames in a directory path (memory should be freed)
MAILIB_API void ClearDirectoryFiles(void);                             // Clear directory files paths buffers (free memory)
MAILIB_API bool ChangeDirectory(const char *dir);                      // Change working directory, returns true if success
MAILIB_API bool IsFileDropped(void);                                   // Check if a file has been dropped into window
MAILIB_API char **GetDroppedFiles(int *count);                         // Get dropped files names (memory should be freed)
MAILIB_API void ClearDroppedFiles(void);                               // Clear dropped files paths buffer (free memory)
MAILIB_API long GetFileModTime(const char *fileName);                  // Get file modification time (last write time)

// Persistent storage management
MAILIB_API void StorageSaveValue(int position, int value);             // Save integer value to storage file (to defined position)
MAILIB_API int StorageLoadValue(int position);                         // Load integer value from storage file (from defined position)

MAILIB_API void OpenURL(const char *url);                              // Open URL with default system browser (if available)

//------------------------------------------------------------------------------------
// Input Handling Functions (Module: core)
//------------------------------------------------------------------------------------

// Input-related functions: keyboard
MAILIB_API bool IsKeyPressed(int key);                             // Detect if a key has been pressed once
MAILIB_API bool IsKeyDown(int key);                                // Detect if a key is being pressed
MAILIB_API bool IsKeyReleased(int key);                            // Detect if a key has been released once
MAILIB_API bool IsKeyUp(int key);                                  // Detect if a key is NOT being pressed
MAILIB_API int GetKeyPressed(void);                                // Get latest key pressed
MAILIB_API void SetExitKey(int key);                               // Set a custom key to exit program (default is ESC)

// Input-related functions: gamepads
MAILIB_API bool IsGamepadAvailable(int gamepad);                   // Detect if a gamepad is available
MAILIB_API bool IsGamepadName(int gamepad, const char *name);      // Check gamepad name (if available)
MAILIB_API const char *GetGamepadName(int gamepad);                // Return gamepad internal name id
MAILIB_API bool IsGamepadButtonPressed(int gamepad, int button);   // Detect if a gamepad button has been pressed once
MAILIB_API bool IsGamepadButtonDown(int gamepad, int button);      // Detect if a gamepad button is being pressed
MAILIB_API bool IsGamepadButtonReleased(int gamepad, int button);  // Detect if a gamepad button has been released once
MAILIB_API bool IsGamepadButtonUp(int gamepad, int button);        // Detect if a gamepad button is NOT being pressed
MAILIB_API int GetGamepadButtonPressed(void);                      // Get the last gamepad button pressed
MAILIB_API int GetGamepadAxisCount(int gamepad);                   // Return gamepad axis count for a gamepad
MAILIB_API float GetGamepadAxisMovement(int gamepad, int axis);    // Return axis movement value for a gamepad axis

// Input-related functions: mouse
MAILIB_API bool IsMouseButtonPressed(int button);                  // Detect if a mouse button has been pressed once
MAILIB_API bool IsMouseButtonDown(int button);                     // Detect if a mouse button is being pressed
MAILIB_API bool IsMouseButtonReleased(int button);                 // Detect if a mouse button has been released once
MAILIB_API bool IsMouseButtonUp(int button);                       // Detect if a mouse button is NOT being pressed
MAILIB_API int GetMouseX(void);                                    // Returns mouse position X
MAILIB_API int GetMouseY(void);                                    // Returns mouse position Y
MAILIB_API vec2 GetMousePosition(void);                         // Returns mouse position XY
MAILIB_API void SetMousePosition(int x, int y);                    // Set mouse position XY
MAILIB_API void SetMouseOffset(int offsetX, int offsetY);          // Set mouse offset
MAILIB_API void SetMouseScale(float scaleX, float scaleY);         // Set mouse scaling
MAILIB_API int GetMouseWheelMove(void);                            // Returns mouse wheel movement Y

// Input-related functions: touch
MAILIB_API int GetTouchX(void);                                    // Returns touch position X for touch point 0 (relative to screen size)
MAILIB_API int GetTouchY(void);                                    // Returns touch position Y for touch point 0 (relative to screen size)
MAILIB_API vec2 GetTouchPosition(int index);                    // Returns touch position XY for a touch point index (relative to screen size)

//------------------------------------------------------------------------------------
// Gestures and Touch Handling Functions (Module: gestures)
//------------------------------------------------------------------------------------
MAILIB_API void SetGesturesEnabled(unsigned int gestureFlags);     // Enable a set of gestures using flags
MAILIB_API bool IsGestureDetected(int gesture);                    // Check if a gesture have been detected
MAILIB_API int GetGestureDetected(void);                           // Get latest detected gesture
MAILIB_API int GetTouchPointsCount(void);                          // Get touch points count
MAILIB_API float GetGestureHoldDuration(void);                     // Get gesture hold time in milliseconds
MAILIB_API vec2 GetGestureDragVector(void);                     // Get gesture drag vector
MAILIB_API float GetGestureDragAngle(void);                        // Get gesture drag angle
MAILIB_API vec2 GetGesturePinchVector(void);                    // Get gesture pinch delta
MAILIB_API float GetGesturePinchAngle(void);                       // Get gesture pinch angle

//------------------------------------------------------------------------------------
// Camera System Functions (Module: camera)
//------------------------------------------------------------------------------------
MAILIB_API void SetCameraMode(Camera camera, int mode);                // Set camera mode (multiple camera modes available)
MAILIB_API void UpdateCamera(Camera *camera);                          // Update camera position for selected mode

MAILIB_API void SetCameraPanControl(int panKey);                       // Set camera pan key to combine with mouse movement (free camera)
MAILIB_API void SetCameraAltControl(int altKey);                       // Set camera alt key to combine with mouse movement (free camera)
MAILIB_API void SetCameraSmoothZoomControl(int szKey);                 // Set camera smooth zoom key to combine with mouse (free camera)
MAILIB_API void SetCameraMoveControls(int frontKey, int backKey, int rightKey, int leftKey, int upKey, int downKey); // Set camera move controls (1st person and 3rd person cameras)

//------------------------------------------------------------------------------------
// Basic Shapes Drawing Functions (Module: shapes)
//------------------------------------------------------------------------------------

// Basic shapes drawing functions
MAILIB_API void DrawPixel(int posX, int posY, Color color);                                                   // Draw a pixel
MAILIB_API void DrawPixelV(vec2 position, Color color);                                                    // Draw a pixel (Vector version)
MAILIB_API void DrawLine(int startPosX, int startPosY, int endPosX, int endPosY, Color color);                // Draw a line
MAILIB_API void DrawLineV(vec2 startPos, vec2 endPos, Color color);                                     // Draw a line (Vector version)
MAILIB_API void DrawLineEx(vec2 startPos, vec2 endPos, float thick, Color color);                       // Draw a line defining thickness
MAILIB_API void DrawLineBezier(vec2 startPos, vec2 endPos, float thick, Color color);                   // Draw a line using cubic-bezier curves in-out
MAILIB_API void DrawLineStrip(vec2 *points, int numPoints, Color color);                                   // Draw lines sequence
MAILIB_API void DrawCircle(int centerX, int centerY, float radius, Color color);                              // Draw a color-filled circle
MAILIB_API void DrawCircleSector(vec2 center, float radius, int startAngle, int endAngle, int segments, Color color);     // Draw a piece of a circle
MAILIB_API void DrawCircleSectorLines(vec2 center, float radius, int startAngle, int endAngle, int segments, Color color);    // Draw circle sector outline
MAILIB_API void DrawCircleGradient(int centerX, int centerY, float radius, Color color1, Color color2);       // Draw a gradient-filled circle
MAILIB_API void DrawCircleV(vec2 center, float radius, Color color);                                       // Draw a color-filled circle (Vector version)
MAILIB_API void DrawCircleLines(int centerX, int centerY, float radius, Color color);                         // Draw circle outline
MAILIB_API void DrawRing(vec2 center, float innerRadius, float outerRadius, int startAngle, int endAngle, int segments, Color color); // Draw ring
MAILIB_API void DrawRingLines(vec2 center, float innerRadius, float outerRadius, int startAngle, int endAngle, int segments, Color color);    // Draw ring outline
MAILIB_API void DrawRectangle(int posX, int posY, int width, int height, Color color);                        // Draw a color-filled rectangle
MAILIB_API void DrawRectangleV(vec2 position, vec2 size, Color color);                                  // Draw a color-filled rectangle (Vector version)
MAILIB_API void DrawRectangleRec(rect rec, Color color);                                                 // Draw a color-filled rectangle
MAILIB_API void DrawRectanglePro(rect rec, vec2 origin, float rotation, Color color);                 // Draw a color-filled rectangle with pro parameters
MAILIB_API void DrawRectangleGradientV(int posX, int posY, int width, int height, Color color1, Color color2);// Draw a vertical-gradient-filled rectangle
MAILIB_API void DrawRectangleGradientH(int posX, int posY, int width, int height, Color color1, Color color2);// Draw a horizontal-gradient-filled rectangle
MAILIB_API void DrawRectangleGradientEx(rect rec, Color col1, Color col2, Color col3, Color col4);       // Draw a gradient-filled rectangle with custom vertex colors
MAILIB_API void DrawRectangleLines(int posX, int posY, int width, int height, Color color);                   // Draw rectangle outline
MAILIB_API void DrawRectangleLinesEx(rect rec, int lineThick, Color color);                              // Draw rectangle outline with extended parameters
MAILIB_API void DrawRectangleRounded(rect rec, float roundness, int segments, Color color);              // Draw rectangle with rounded edges
MAILIB_API void DrawRectangleRoundedLines(rect rec, float roundness, int segments, int lineThick, Color color); // Draw rectangle with rounded edges outline
MAILIB_API void DrawTriangle(vec2 v1, vec2 v2, vec2 v3, Color color);                                // Draw a color-filled triangle
MAILIB_API void DrawTriangleLines(vec2 v1, vec2 v2, vec2 v3, Color color);                           // Draw triangle outline
MAILIB_API void DrawTriangleFan(vec2 *points, int numPoints, Color color);                                 // Draw a triangle fan defined by points
MAILIB_API void DrawPoly(vec2 center, int sides, float radius, float rotation, Color color);               // Draw a regular polygon (Vector version)

MAILIB_API void SetShapesTexture(Texture texture, rect source);                                        // Define default texture used to draw shapes

// Basic shapes collision detection functions
MAILIB_API bool CheckCollisionRecs(rect rec1, rect rec2);                                           // Check collision between two rectangles
MAILIB_API bool CheckCollisionCircles(vec2 center1, float radius1, vec2 center2, float radius2);        // Check collision between two circles
MAILIB_API bool CheckCollisionCircleRec(vec2 center, float radius, rect rec);                         // Check collision between circle and rectangle
MAILIB_API rect GetCollisionRec(rect rec1, rect rec2);                                         // Get collision rectangle for two rectangles collision
MAILIB_API bool CheckCollisionPointRec(vec2 point, rect rec);                                         // Check if point is inside rectangle
MAILIB_API bool CheckCollisionPointCircle(vec2 point, vec2 center, float radius);                       // Check if point is inside circle
MAILIB_API bool CheckCollisionPointTriangle(vec2 point, vec2 p1, vec2 p2, vec2 p3);               // Check if point is inside a triangle

//------------------------------------------------------------------------------------
// Texture Loading and Drawing Functions (Module: textures)
//------------------------------------------------------------------------------------

// Image/Texture data loading/unloading/saving functions
MAILIB_API Image LoadImage(const char *fileName);                                                             // Load image from file into CPU memory (RAM)
MAILIB_API Image LoadImageEx(Color *pixels, int width, int height);                                           // Load image from Color array data (RGBA - 32bit)
MAILIB_API Image LoadImagePro(void *data, int width, int height, int format);                                 // Load image from raw data with parameters
MAILIB_API Image LoadImageRaw(const char *fileName, int width, int height, int format, int headerSize);       // Load image from RAW file data
MAILIB_API void ExportImage(Image image, const char *fileName);                                               // Export image data to file
MAILIB_API void ExportImageAsCode(Image image, const char *fileName);                                         // Export image as code file defining an array of bytes
MAILIB_API Texture LoadTexture(const char *fileName);                                                       // Load texture from file into GPU memory (VRAM)
MAILIB_API Texture LoadTextureFromImage(Image image);                                                       // Load texture from image data
MAILIB_API TextureCubemap LoadTextureCubemap(Image image, int layoutType);                                    // Load cubemap from image, multiple image cubemap layouts supported
MAILIB_API RenderTexture LoadRenderTexture(int width, int height);                                          // Load texture for rendering (framebuffer)
MAILIB_API void UnloadImage(Image image);                                                                     // Unload image from CPU memory (RAM)
MAILIB_API void UnloadTexture(Texture texture);                                                             // Unload texture from GPU memory (VRAM)
MAILIB_API void UnloadRenderTexture(RenderTexture target);                                                  // Unload render texture from GPU memory (VRAM)
MAILIB_API Color *GetImageData(Image image);                                                                  // Get pixel data from image as a Color struct array
MAILIB_API vec4 *GetImageDataNormalized(Image image);                                                      // Get pixel data from image as vec4 array (float normalized)
MAILIB_API int GetPixelDataSize(int width, int height, int format);                                           // Get pixel data size in bytes (image or texture)
MAILIB_API Image GetTextureData(Texture texture);                                                           // Get pixel data from GPU texture and return an Image
MAILIB_API Image GetScreenData(void);                                                                         // Get pixel data from screen buffer and return an Image (screenshot)
MAILIB_API void UpdateTexture(Texture texture, const void *pixels);                                         // Update GPU texture with new data

// Image manipulation functions
MAILIB_API Image ImageCopy(Image image);                                                                      // Create an image duplicate (useful for transformations)
MAILIB_API void ImageToPOT(Image *image, Color fillColor);                                                    // Convert image to POT (power-of-two)
MAILIB_API void ImageFormat(Image *image, int newFormat);                                                     // Convert image data to desired format
MAILIB_API void ImageAlphaMask(Image *image, Image alphaMask);                                                // Apply alpha mask to image
MAILIB_API void ImageAlphaClear(Image *image, Color color, float threshold);                                  // Clear alpha channel to desired color
MAILIB_API void ImageAlphaCrop(Image *image, float threshold);                                                // Crop image depending on alpha value
MAILIB_API void ImageAlphaPremultiply(Image *image);                                                          // Premultiply alpha channel
MAILIB_API void ImageCrop(Image *image, rect crop);                                                      // Crop an image to a defined rectangle
MAILIB_API void ImageResize(Image *image, int newWidth, int newHeight);                                       // Resize image (Bicubic scaling algorithm)
MAILIB_API void ImageResizeNN(Image *image, int newWidth,int newHeight);                                      // Resize image (Nearest-Neighbor scaling algorithm)
MAILIB_API void ImageResizeCanvas(Image *image, int newWidth, int newHeight, int offsetX, int offsetY, Color color);  // Resize canvas and fill with color
MAILIB_API void ImageMipmaps(Image *image);                                                                   // Generate all mipmap levels for a provided image
MAILIB_API void ImageDither(Image *image, int rBpp, int gBpp, int bBpp, int aBpp);                            // Dither image data to 16bpp or lower (Floyd-Steinberg dithering)
MAILIB_API Color *ImageExtractPalette(Image image, int maxPaletteSize, int *extractCount);                    // Extract color palette from image to maximum size (memory should be freed)
MAILIB_API Image ImageText(const char *text, int fontSize, Color color);                                      // Create an image from text (default font)
MAILIB_API Image ImageTextEx(Font font, const char *text, float fontSize, float spacing, Color tint);         // Create an image from text (custom sprite font)
MAILIB_API void ImageDraw(Image *dst, Image src, rect srcRec, rect dstRec);                         // Draw a source image within a destination image
MAILIB_API void ImageDrawRectangle(Image *dst, rect rec, Color color);                                   // Draw rectangle within an image
MAILIB_API void ImageDrawRectangleLines(Image *dst, rect rec, int thick, Color color);                   // Draw rectangle lines within an image
MAILIB_API void ImageDrawText(Image *dst, vec2 position, const char *text, int fontSize, Color color);     // Draw text (default font) within an image (destination)
MAILIB_API void ImageDrawTextEx(Image *dst, vec2 position, Font font, const char *text, float fontSize, float spacing, Color color); // Draw text (custom sprite font) within an image (destination)
MAILIB_API void ImageFlipVertical(Image *image);                                                              // Flip image vertically
MAILIB_API void ImageFlipHorizontal(Image *image);                                                            // Flip image horizontally
MAILIB_API void ImageRotateCW(Image *image);                                                                  // Rotate image clockwise 90deg
MAILIB_API void ImageRotateCCW(Image *image);                                                                 // Rotate image counter-clockwise 90deg
MAILIB_API void ImageColorTint(Image *image, Color color);                                                    // Modify image color: tint
MAILIB_API void ImageColorInvert(Image *image);                                                               // Modify image color: invert
MAILIB_API void ImageColorGrayscale(Image *image);                                                            // Modify image color: grayscale
MAILIB_API void ImageColorContrast(Image *image, float contrast);                                             // Modify image color: contrast (-100 to 100)
MAILIB_API void ImageColorBrightness(Image *image, int brightness);                                           // Modify image color: brightness (-255 to 255)
MAILIB_API void ImageColorReplace(Image *image, Color color, Color replace);                                  // Modify image color: replace color

// Image generation functions
MAILIB_API Image GenImageColor(int width, int height, Color color);                                           // Generate image: plain color
MAILIB_API Image GenImageGradientV(int width, int height, Color top, Color bottom);                           // Generate image: vertical gradient
MAILIB_API Image GenImageGradientH(int width, int height, Color left, Color right);                           // Generate image: horizontal gradient
MAILIB_API Image GenImageGradientRadial(int width, int height, float density, Color inner, Color outer);      // Generate image: radial gradient
MAILIB_API Image GenImageChecked(int width, int height, int checksX, int checksY, Color col1, Color col2);    // Generate image: checked
MAILIB_API Image GenImageWhiteNoise(int width, int height, float factor);                                     // Generate image: white noise
MAILIB_API Image GenImagePerlinNoise(int width, int height, int offsetX, int offsetY, float scale);           // Generate image: perlin noise
MAILIB_API Image GenImageCellular(int width, int height, int tileSize);                                       // Generate image: cellular algorithm. Bigger tileSize means bigger cells

// Texture configuration functions
MAILIB_API void GenTextureMipmaps(Texture *texture);                                                        // Generate GPU mipmaps for a texture
MAILIB_API void SetTextureFilter(Texture texture, int filterMode);                                          // Set texture scaling filter mode
MAILIB_API void SetTextureWrap(Texture texture, int wrapMode);                                              // Set texture wrapping mode

// Texture drawing functions
MAILIB_API void DrawTexture(Texture texture, int posX, int posY, Color tint);                               // Draw a Texture
MAILIB_API void DrawTextureV(Texture texture, vec2 position, Color tint);                                // Draw a Texture with position defined as vec2
MAILIB_API void DrawTextureEx(Texture texture, vec2 position, float rotation, float scale, Color tint);  // Draw a Texture with extended parameters
MAILIB_API void DrawTextureRec(Texture texture, rect sourceRec, vec2 position, Color tint);         // Draw a part of a texture defined by a rectangle
MAILIB_API void DrawTextureQuad(Texture texture, vec2 tiling, vec2 offset, rect quad, Color tint);  // Draw texture quad with tiling and offset parameters
MAILIB_API void DrawTexturePro(Texture texture, rect sourceRec, rect destRec, vec2 origin, float rotation, Color tint);       // Draw a part of a texture defined by a rectangle with 'pro' parameters
MAILIB_API void DrawTextureNPatch(Texture texture, NPatchInfo nPatchInfo, rect destRec, vec2 origin, float rotation, Color tint);  // Draws a texture (or part of it) that stretches or shrinks nicely

//------------------------------------------------------------------------------------
// Font Loading and Text Drawing Functions (Module: text)
//------------------------------------------------------------------------------------

// Font loading/unloading functions
MAILIB_API Font GetFontDefault(void);                                                            // Get the default Font
MAILIB_API Font LoadFont(const char *fileName);                                                  // Load font from file into GPU memory (VRAM)
MAILIB_API Font LoadFontEx(const char *fileName, int fontSize, int *fontChars, int charsCount);  // Load font from file with extended parameters
MAILIB_API Font LoadFontFromImage(Image image, Color key, int firstChar);                        // Load font from Image (XNA style)
MAILIB_API CharInfo *LoadFontData(const char *fileName, int fontSize, int *fontChars, int charsCount, int type); // Load font data for further use
MAILIB_API Image GenImageFontAtlas(CharInfo *chars, int charsCount, int fontSize, int padding, int packMethod);  // Generate image font atlas using chars info
MAILIB_API void UnloadFont(Font font);                                                           // Unload Font from GPU memory (VRAM)

// Text drawing functions
MAILIB_API void DrawFPS(int posX, int posY);                                                     // Shows current FPS
MAILIB_API void DrawText(const char *text, int posX, int posY, int fontSize, Color color);       // Draw text (using default font)
MAILIB_API void DrawTextEx(Font font, const char *text, vec2 position, float fontSize, float spacing, Color tint);                // Draw text using font and additional parameters
MAILIB_API void DrawTextRec(Font font, const char *text, rect rec, float fontSize, float spacing, bool wordWrap, Color tint);   // Draw text using font inside rectangle limits
MAILIB_API void DrawTextRecEx(Font font, const char *text, rect rec, float fontSize, float spacing, bool wordWrap, Color tint,
                         int selectStart, int selectLength, Color selectText, Color selectBack);    // Draw text using font inside rectangle limits with support for text selection

// Text misc. functions
MAILIB_API int MeasureText(const char *text, int fontSize);                                      // Measure string width for default font
MAILIB_API vec2 MeasureTextEx(Font font, const char *text, float fontSize, float spacing);    // Measure string size for Font
MAILIB_API int GetGlyphIndex(Font font, int character);                                          // Get index position for a unicode character on font
MAILIB_API int GetNextCodepoint(const char *text, int *count);                                   // Returns next codepoint in a UTF8 encoded string
                                                                                            // NOTE: 0x3f(`?`) is returned on failure, `count` will hold the total number of bytes processed

// Text strings management functions
// NOTE: Some strings allocate memory internally for returned strings, just be careful!
MAILIB_API bool TextIsEqual(const char *text1, const char *text2);                               // Check if two text string are equal
MAILIB_API unsigned int TextLength(const char *text);                                            // Get text length, checks for '\0' ending
MAILIB_API unsigned int TextCountCodepoints(const char *text);                                   // Get total number of characters (codepoints) in a UTF8 encoded string
MAILIB_API const char *TextFormat(const char *text, ...);                                        // Text formatting with variables (sprintf style)
MAILIB_API const char *TextSubtext(const char *text, int position, int length);                  // Get a piece of a text string
MAILIB_API const char *TextReplace(char *text, const char *replace, const char *by);             // Replace text string (memory should be freed!)
MAILIB_API const char *TextInsert(const char *text, const char *insert, int position);           // Insert text in a position (memory should be freed!)
MAILIB_API const char *TextJoin(const char **textList, int count, const char *delimiter);        // Join text strings with delimiter
MAILIB_API const char **TextSplit(const char *text, char delimiter, int *count);                 // Split text into multiple strings
MAILIB_API void TextAppend(char *text, const char *append, int *position);                       // Append text at specific position and move cursor!
MAILIB_API int TextFindIndex(const char *text, const char *find);                                // Find first text occurrence within a string
MAILIB_API const char *TextToUpper(const char *text);                      // Get upper case version of provided string
MAILIB_API const char *TextToLower(const char *text);                      // Get lower case version of provided string
MAILIB_API const char *TextToPascal(const char *text);                     // Get Pascal case notation version of provided string
MAILIB_API int TextToInteger(const char *text);                            // Get integer value from text (negative values not supported)

//------------------------------------------------------------------------------------
// Basic 3d Shapes Drawing Functions (Module: models)
//------------------------------------------------------------------------------------

// Basic geometric 3D shapes drawing functions
MAILIB_API void DrawLine3D(vec3 startPos, vec3 endPos, Color color);                                    // Draw a line in 3D world space
MAILIB_API void DrawCircle3D(vec3 center, float radius, vec3 rotationAxis, float rotationAngle, Color color); // Draw a circle in 3D world space
MAILIB_API void DrawCube(vec3 position, float width, float height, float length, Color color);             // Draw cube
MAILIB_API void DrawCubeV(vec3 position, vec3 size, Color color);                                       // Draw cube (Vector version)
MAILIB_API void DrawCubeWires(vec3 position, float width, float height, float length, Color color);        // Draw cube wires
MAILIB_API void DrawCubeWiresV(vec3 position, vec3 size, Color color);                                  // Draw cube wires (Vector version)
MAILIB_API void DrawCubeTexture(Texture texture, vec3 position, float width, float height, float length, Color color); // Draw cube textured
MAILIB_API void DrawSphere(vec3 centerPos, float radius, Color color);                                     // Draw sphere
MAILIB_API void DrawSphereEx(vec3 centerPos, float radius, int rings, int slices, Color color);            // Draw sphere with extended parameters
MAILIB_API void DrawSphereWires(vec3 centerPos, float radius, int rings, int slices, Color color);         // Draw sphere wires
MAILIB_API void DrawCylinder(vec3 position, float radiusTop, float radiusBottom, float height, int slices, Color color); // Draw a cylinder/cone
MAILIB_API void DrawCylinderWires(vec3 position, float radiusTop, float radiusBottom, float height, int slices, Color color); // Draw a cylinder/cone wires
MAILIB_API void DrawPlane(vec3 centerPos, vec2 size, Color color);                                      // Draw a plane XZ
MAILIB_API void DrawRay(Ray ray, Color color);                                                                // Draw a ray line
MAILIB_API void DrawGrid(int slices, float spacing);                                                          // Draw a grid (centered at (0, 0, 0))
MAILIB_API void DrawGizmo(vec3 position);                                                                  // Draw simple gizmo
//DrawTorus(), DrawTeapot() could be useful?

//------------------------------------------------------------------------------------
// Model 3d Loading and Drawing Functions (Module: models)
//------------------------------------------------------------------------------------

// Model loading/unloading functions
MAILIB_API Model LoadModel(const char *fileName);                                                            // Load model from files (meshes and materials)
MAILIB_API Model LoadModelFromMesh(Mesh mesh);                                                               // Load model from generated mesh (default material)
MAILIB_API void UnloadModel(Model model);                                                                    // Unload model from memory (RAM and/or VRAM)

// Mesh loading/unloading functions
MAILIB_API Mesh *LoadMeshes(const char *fileName, int *meshCount);                                           // Load meshes from model file
MAILIB_API void ExportMesh(Mesh mesh, const char *fileName);                                                 // Export mesh data to file
MAILIB_API void UnloadMesh(Mesh *mesh);                                                                      // Unload mesh from memory (RAM and/or VRAM)

// Material loading/unloading functions
MAILIB_API Material *LoadMaterials(const char *fileName, int *materialCount);                                // Load materials from model file
MAILIB_API Material LoadMaterialDefault(void);                                                               // Load default material (Supports: DIFFUSE, SPECULAR, NORMAL maps)
MAILIB_API void UnloadMaterial(Material material);                                                           // Unload material from GPU memory (VRAM)
MAILIB_API void SetMaterialTexture(Material *material, int mapType, Texture texture);                      // Set texture for a material map type (MAP_DIFFUSE, MAP_SPECULAR...)
MAILIB_API void SetModelMeshMaterial(Model *model, int meshId, int materialId);                              // Set material for a mesh

// Model animations loading/unloading functions
MAILIB_API ModelAnimation *LoadModelAnimations(const char *fileName, int *animsCount);                       // Load model animations from file
MAILIB_API void UpdateModelAnimation(Model model, ModelAnimation anim, int frame);                           // Update model animation pose
MAILIB_API void UnloadModelAnimation(ModelAnimation anim);                                                   // Unload animation data
MAILIB_API bool IsModelAnimationValid(Model model, ModelAnimation anim);                                     // Check model animation skeleton match

// Mesh generation functions
MAILIB_API Mesh GenMeshPoly(int sides, float radius);                                                        // Generate polygonal mesh
MAILIB_API Mesh GenMeshPlane(float width, float length, int resX, int resZ);                                 // Generate plane mesh (with subdivisions)
MAILIB_API Mesh GenMeshCube(float width, float height, float length);                                        // Generate cuboid mesh
MAILIB_API Mesh GenMeshSphere(float radius, int rings, int slices);                                          // Generate sphere mesh (standard sphere)
MAILIB_API Mesh GenMeshHemiSphere(float radius, int rings, int slices);                                      // Generate half-sphere mesh (no bottom cap)
MAILIB_API Mesh GenMeshCylinder(float radius, float height, int slices);                                     // Generate cylinder mesh
MAILIB_API Mesh GenMeshTorus(float radius, float size, int radSeg, int sides);                               // Generate torus mesh
MAILIB_API Mesh GenMeshKnot(float radius, float size, int radSeg, int sides);                                // Generate trefoil knot mesh
MAILIB_API Mesh GenMeshHeightmap(Image heightmap, vec3 size);                                             // Generate heightmap mesh from image data
MAILIB_API Mesh GenMeshCubicmap(Image cubicmap, vec3 cubeSize);                                           // Generate cubes-based map mesh from image data

// Mesh manipulation functions
MAILIB_API BoundingBox MeshBoundingBox(Mesh mesh);                                                           // Compute mesh bounding box limits
MAILIB_API void MeshTangents(Mesh *mesh);                                                                    // Compute mesh tangents
MAILIB_API void MeshBinormals(Mesh *mesh);                                                                   // Compute mesh binormals

// Model drawing functions
MAILIB_API void DrawModel(Model model, vec3 position, float scale, Color tint);                           // Draw a model (with texture if set)
MAILIB_API void DrawModelEx(Model model, vec3 position, vec3 rotationAxis, float rotationAngle, vec3 scale, Color tint); // Draw a model with extended parameters
MAILIB_API void DrawModelWires(Model model, vec3 position, float scale, Color tint);                      // Draw a model wires (with texture if set)
MAILIB_API void DrawModelWiresEx(Model model, vec3 position, vec3 rotationAxis, float rotationAngle, vec3 scale, Color tint); // Draw a model wires (with texture if set) with extended parameters
MAILIB_API void DrawBoundingBox(BoundingBox box, Color color);                                               // Draw bounding box (wires)
MAILIB_API void DrawBillboard(Camera camera, Texture texture, vec3 center, float size, Color tint);     // Draw a billboard texture
MAILIB_API void DrawBillboardRec(Camera camera, Texture texture, rect sourceRec, vec3 center, float size, Color tint); // Draw a billboard texture defined by sourceRec

// Collision detection functions
MAILIB_API bool CheckCollisionSpheres(vec3 centerA, float radiusA, vec3 centerB, float radiusB);       // Detect collision between two spheres
MAILIB_API bool CheckCollisionBoxes(BoundingBox box1, BoundingBox box2);                                     // Detect collision between two bounding boxes
MAILIB_API bool CheckCollisionBoxSphere(BoundingBox box, vec3 centerSphere, float radiusSphere);          // Detect collision between box and sphere
MAILIB_API bool CheckCollisionRaySphere(Ray ray, vec3 spherePosition, float sphereRadius);                // Detect collision between ray and sphere
MAILIB_API bool CheckCollisionRaySphereEx(Ray ray, vec3 spherePosition, float sphereRadius, vec3 *collisionPoint); // Detect collision between ray and sphere, returns collision point
MAILIB_API bool CheckCollisionRayBox(Ray ray, BoundingBox box);                                              // Detect collision between ray and box
MAILIB_API RayHitInfo GetCollisionRayModel(Ray ray, Model *model);                                           // Get collision info between ray and model
MAILIB_API RayHitInfo GetCollisionRayTriangle(Ray ray, vec3 p1, vec3 p2, vec3 p3);                  // Get collision info between ray and triangle
MAILIB_API RayHitInfo GetCollisionRayGround(Ray ray, float groundHeight);                                    // Get collision info between ray and ground plane (Y-normal plane)

//------------------------------------------------------------------------------------
// Shaders System Functions (Module: rlgl)
// NOTE: This functions are useless when using OpenGL 1.1
//------------------------------------------------------------------------------------

// Shader loading/unloading functions
MAILIB_API char *LoadText(const char *fileName);                               // Load chars array from text file
MAILIB_API Shader LoadShader(const char *vsFileName, const char *fsFileName);  // Load shader from files and bind default locations
MAILIB_API Shader LoadShaderCode(char *vsCode, char *fsCode);                  // Load shader from code strings and bind default locations
MAILIB_API void UnloadShader(Shader shader);                                   // Unload shader from GPU memory (VRAM)

MAILIB_API Shader GetShaderDefault(void);                                      // Get default shader
MAILIB_API Texture GetTextureDefault(void);                                  // Get default texture

// Shader configuration functions
MAILIB_API int GetShaderLocation(Shader shader, const char *uniformName);      // Get shader uniform location
MAILIB_API void SetShaderValue(Shader shader, int uniformLoc, const void *value, int uniformType);               // Set shader uniform value
MAILIB_API void SetShaderValueV(Shader shader, int uniformLoc, const void *value, int uniformType, int count);   // Set shader uniform value vector
MAILIB_API void SetShaderValueMatrix(Shader shader, int uniformLoc, mat4 mat);         // Set shader uniform value (matrix 4x4)
MAILIB_API void SetShaderValueTexture(Shader shader, int uniformLoc, Texture texture); // Set shader uniform value for texture
MAILIB_API void SetMatrixProjection(mat4 proj);                              // Set a custom projection matrix (replaces internal projection matrix)
MAILIB_API void SetMatrixModelview(mat4 view);                               // Set a custom modelview matrix (replaces internal modelview matrix)
MAILIB_API mat4 GetMatrixModelview();                                        // Get internal modelview matrix

// Texture maps generation (PBR)
// NOTE: Required shaders should be provided
MAILIB_API Texture GenTextureCubemap(Shader shader, Texture skyHDR, int size);       // Generate cubemap texture from HDR texture
MAILIB_API Texture GenTextureIrradiance(Shader shader, Texture cubemap, int size);   // Generate irradiance texture using cubemap data
MAILIB_API Texture GenTexturePrefilter(Shader shader, Texture cubemap, int size);    // Generate prefilter texture using cubemap data
MAILIB_API Texture GenTextureBRDF(Shader shader, int size);                  // Generate BRDF texture

// Shading begin/end functions
MAILIB_API void BeginShaderMode(Shader shader);                                // Begin custom shader drawing
MAILIB_API void EndShaderMode(void);                                           // End custom shader drawing (use default shader)
MAILIB_API void BeginBlendMode(int mode);                                      // Begin blending mode (alpha, additive, multiplied)
MAILIB_API void EndBlendMode(void);                                            // End blending mode (reset to default: alpha blending)
MAILIB_API void BeginScissorMode(int x, int y, int width, int height);         // Begin scissor mode (define screen area for following drawing)
MAILIB_API void EndScissorMode(void);                                          // End scissor mode

// VR control functions
MAILIB_API void InitVrSimulator(void);                       // Init VR simulator for selected device parameters
MAILIB_API void CloseVrSimulator(void);                      // Close VR simulator for current device
MAILIB_API void UpdateVrTracking(Camera *camera);            // Update VR tracking (position and orientation) and camera
MAILIB_API void SetVrConfiguration(VrDeviceInfo info, Shader distortion);      // Set stereo rendering configuration parameters 
MAILIB_API bool IsVrSimulatorReady(void);                    // Detect if VR simulator is ready
MAILIB_API void ToggleVrMode(void);                          // Enable/Disable VR experience
MAILIB_API void BeginVrDrawing(void);                        // Begin VR simulator stereo rendering
MAILIB_API void EndVrDrawing(void);                          // End VR simulator stereo rendering

//------------------------------------------------------------------------------------
// Audio Loading and Playing Functions (Module: audio)
//------------------------------------------------------------------------------------

// Audio device management functions
MAILIB_API void InitAudioDevice(void);                                     // Initialize audio device and context
MAILIB_API void CloseAudioDevice(void);                                    // Close the audio device and context
MAILIB_API bool IsAudioDeviceReady(void);                                  // Check if audio device has been initialized successfully
MAILIB_API void SetMasterVolume(float volume);                             // Set master volume (listener)

// Wave/Sound loading/unloading functions
MAILIB_API Wave LoadWave(const char *fileName);                            // Load wave data from file
MAILIB_API Wave LoadWaveEx(void *data, int sampleCount, int sampleRate, int sampleSize, int channels); // Load wave data from raw array data
MAILIB_API Sound LoadSound(const char *fileName);                          // Load sound from file
MAILIB_API Sound LoadSoundFromWave(Wave wave);                             // Load sound from wave data
MAILIB_API void UpdateSound(Sound sound, const void *data, int samplesCount);// Update sound buffer with new data
MAILIB_API void UnloadWave(Wave wave);                                     // Unload wave data
MAILIB_API void UnloadSound(Sound sound);                                  // Unload sound
MAILIB_API void ExportWave(Wave wave, const char *fileName);               // Export wave data to file
MAILIB_API void ExportWaveAsCode(Wave wave, const char *fileName);         // Export wave sample data to code (.h)

// Wave/Sound management functions
MAILIB_API void PlaySound(Sound sound);                                    // Play a sound
MAILIB_API void PauseSound(Sound sound);                                   // Pause a sound
MAILIB_API void ResumeSound(Sound sound);                                  // Resume a paused sound
MAILIB_API void StopSound(Sound sound);                                    // Stop playing a sound
MAILIB_API bool IsSoundPlaying(Sound sound);                               // Check if a sound is currently playing
MAILIB_API void SetSoundVolume(Sound sound, float volume);                 // Set volume for a sound (1.0 is max level)
MAILIB_API void SetSoundPitch(Sound sound, float pitch);                   // Set pitch for a sound (1.0 is base level)
MAILIB_API void WaveFormat(Wave *wave, int sampleRate, int sampleSize, int channels);  // Convert wave data to desired format
MAILIB_API Wave WaveCopy(Wave wave);                                       // Copy a wave to a new wave
MAILIB_API void WaveCrop(Wave *wave, int initSample, int finalSample);     // Crop a wave to defined samples range
MAILIB_API float *GetWaveData(Wave wave);                                  // Get samples data from wave as a floats array

// Music management functions
MAILIB_API Music LoadMusicStream(const char *fileName);                    // Load music stream from file
MAILIB_API void UnloadMusicStream(Music music);                            // Unload music stream
MAILIB_API void PlayMusicStream(Music music);                              // Start music playing
MAILIB_API void UpdateMusicStream(Music music);                            // Updates buffers for music streaming
MAILIB_API void StopMusicStream(Music music);                              // Stop music playing
MAILIB_API void PauseMusicStream(Music music);                             // Pause music playing
MAILIB_API void ResumeMusicStream(Music music);                            // Resume playing paused music
MAILIB_API bool IsMusicPlaying(Music music);                               // Check if music is playing
MAILIB_API void SetMusicVolume(Music music, float volume);                 // Set volume for music (1.0 is max level)
MAILIB_API void SetMusicPitch(Music music, float pitch);                   // Set pitch for a music (1.0 is base level)
MAILIB_API void SetMusicLoopCount(Music music, int count);                 // Set music loop count (loop repeats)
MAILIB_API float GetMusicTimeLength(Music music);                          // Get music time length (in seconds)
MAILIB_API float GetMusicTimePlayed(Music music);                          // Get current music time played (in seconds)

// AudioStream management functions
MAILIB_API AudioStream InitAudioStream(unsigned int sampleRate, unsigned int sampleSize, unsigned int channels); // Init audio stream (to stream raw audio pcm data)
MAILIB_API void UpdateAudioStream(AudioStream stream, const void *data, int samplesCount); // Update audio stream buffers with data
MAILIB_API void CloseAudioStream(AudioStream stream);                      // Close audio stream and free memory
MAILIB_API bool IsAudioBufferProcessed(AudioStream stream);                // Check if any audio stream buffers requires refill
MAILIB_API void PlayAudioStream(AudioStream stream);                       // Play audio stream
MAILIB_API void PauseAudioStream(AudioStream stream);                      // Pause audio stream
MAILIB_API void ResumeAudioStream(AudioStream stream);                     // Resume audio stream
MAILIB_API bool IsAudioStreamPlaying(AudioStream stream);                  // Check if audio stream is playing
MAILIB_API void StopAudioStream(AudioStream stream);                       // Stop audio stream
MAILIB_API void SetAudioStreamVolume(AudioStream stream, float volume);    // Set volume for audio stream (1.0 is max level)
MAILIB_API void SetAudioStreamPitch(AudioStream stream, float pitch);      // Set pitch for audio stream (1.0 is base level)

#if defined(__cplusplus)
}
#endif

