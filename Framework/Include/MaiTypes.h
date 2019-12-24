#pragma once

#include <stdarg.h>

//----------------------------------------------------------------------------------
// Some basic Defines
//----------------------------------------------------------------------------------
#ifndef PI
#   define PI 3.14159265358979323846f
#endif

#define DEG2RAD (PI/180.0f)
#define RAD2DEG (180.0f/PI)

#define MAX_TOUCH_POINTS        10      // Maximum number of touch points supported

// Shader and material limits
#define MAX_SHADER_LOCATIONS    32      // Maximum number of predefined locations stored in shader struct
#define MAX_MATERIAL_MAPS       12      // Maximum number of texture maps stored in shader struct

// NOTE: MSC C++ compiler does not support compound literals (C99 feature)
// Plain structures in C++ (without constructors) can be initialized from { } initializers.
#if defined(__cplusplus)
#   define CLITERAL    Color
#else
#   define CLITERAL    (Color)
#endif

// Some Basic Colors
// NOTE: Custom raylib color palette for amazing visuals on WHITE background
#define LIGHTGRAY  CLITERAL{ 200, 200, 200, 255 }   // Light Gray
#define GRAY       CLITERAL{ 130, 130, 130, 255 }   // Gray
#define DARKGRAY   CLITERAL{ 80, 80, 80, 255 }      // Dark Gray
#define YELLOW     CLITERAL{ 253, 249, 0, 255 }     // Yellow
#define GOLD       CLITERAL{ 255, 203, 0, 255 }     // Gold
#define ORANGE     CLITERAL{ 255, 161, 0, 255 }     // Orange
#define PINK       CLITERAL{ 255, 109, 194, 255 }   // Pink
#define RED        CLITERAL{ 230, 41, 55, 255 }     // Red
#define MAROON     CLITERAL{ 190, 33, 55, 255 }     // Maroon
#define GREEN      CLITERAL{ 0, 228, 48, 255 }      // Green
#define LIME       CLITERAL{ 0, 158, 47, 255 }      // Lime
#define DARKGREEN  CLITERAL{ 0, 117, 44, 255 }      // Dark Green
#define SKYBLUE    CLITERAL{ 102, 191, 255, 255 }   // Sky Blue
#define BLUE       CLITERAL{ 0, 121, 241, 255 }     // Blue
#define DARKBLUE   CLITERAL{ 0, 82, 172, 255 }      // Dark Blue
#define PURPLE     CLITERAL{ 200, 122, 255, 255 }   // Purple
#define VIOLET     CLITERAL{ 135, 60, 190, 255 }    // Violet
#define DARKPURPLE CLITERAL{ 112, 31, 126, 255 }    // Dark Purple
#define BEIGE      CLITERAL{ 211, 176, 131, 255 }   // Beige
#define BROWN      CLITERAL{ 127, 106, 79, 255 }    // Brown
#define DARKBROWN  CLITERAL{ 76, 63, 47, 255 }      // Dark Brown

#define WHITE      CLITERAL{ 255, 255, 255, 255 }   // White
#define BLACK      CLITERAL{ 0, 0, 0, 255 }         // Black
#define BLANK      CLITERAL{ 0, 0, 0, 0 }           // Blank (Transparent)
#define MAGENTA    CLITERAL{ 255, 0, 255, 255 }     // Magenta
#define RAYWHITE   CLITERAL{ 245, 245, 245, 255 }   // My own White (raylib logo)

//----------------------------------------------------------------------------------
// Structures Definition
//----------------------------------------------------------------------------------

typedef unsigned char       byte;
typedef signed char         sbyte;

typedef signed char         i8;
typedef unsigned char       u8;
typedef signed short        i16;
typedef unsigned short      u16;
typedef signed int          i32;
typedef unsigned int        u32;
typedef signed long long    i64;
typedef unsigned long long  u64;

typedef va_list             ArgList;

#if !defined(__cplusplus) && !defined(bool)
enum { false, true };
typedef byte bool;
#endif

// vec2 type
typedef struct vec2 {
    float x;
    float y;
} vec2;

// vec3 type
typedef struct vec3 {
    float x;
    float y;
    float z;
} vec3;

// vec4 type
typedef struct vec4 {
    float x;
    float y;
    float z;
    float w;
} vec4;

// quat type, same as vec4
typedef struct quat {
    float x;
    float y;
    float z;
    float w;
} quat;

// mat4 type (OpenGL style 4x4 - right handed, column major)
typedef struct mat4 {
    float m0, m4, m8, m12;
    float m1, m5, m9, m13;
    float m2, m6, m10, m14;
    float m3, m7, m11, m15;
} mat4;

// rect type
typedef struct rect {
    float x;
    float y;
    float width;
    float height;
} rect;

// Color type, RGBA (32bit)
typedef struct Color {
    byte r;
    byte g;
    byte b;
    byte a;
} Color;

// Image type, bpp always RGBA (32bit)
// NOTE: Data stored in CPU memory (RAM)
typedef struct Image {
    byte*   pixels;         // Image raw data
    int     width;          // Image base width
    int     height;         // Image base height
    int     mipmaps;        // Mipmap levels, 1 by default
    int     format;         // Data format (PixelFormat type)
} Image;

// Texture type
// NOTE: Data stored in GPU memory
typedef struct Texture {
    u32 id;                 // OpenGL texture id
    int width;              // Texture base width
    int height;             // Texture base height
    int mipmaps;            // Mipmap levels, 1 by default
    int format;             // Data format (PixelFormat type)
} Texture;

// TextureCubemap type, actually, same as Texture
typedef Texture TextureCubemap;

// RenderTexture2D type, for texture rendering
typedef struct RenderTexture {
    u32             id;                 // OpenGL Framebuffer Object (FBO) id
    Texture         texture;            // Color buffer attachment texture
    Texture         depth;              // Depth buffer attachment texture
    bool            depthTexture;       // Track if depth attachment is a texture or renderbuffer
} RenderTexture;

// N-Patch layout info
typedef struct NPatchInfo {
    rect sourceRec;         // Region in the texture
    int left;              // left border offset
    int top;               // top border offset
    int right;             // right border offset
    int bottom;            // bottom border offset
    int type;              // layout of the n-patch: 3x3, 1x3 or 3x1
} NPatchInfo;

// Font character info
typedef struct CharInfo {
    int value;              // Character value (Unicode)
    rect rec;               // Character rectangle in sprite font
    int offsetX;            // Character offset X when drawing
    int offsetY;            // Character offset Y when drawing
    int advanceX;           // Character advance position X
    unsigned char *data;    // Character pixel data (grayscale)
} CharInfo;

// Font type, includes texture and charSet array data
typedef struct Font {
    Texture texture;        // Font texture
    int baseSize;           // Base size (default chars height)
    int charsCount;         // Number of characters
    CharInfo *chars;        // Characters info data
} Font;

// Camera type, defines a camera position/orientation in 3d space
typedef struct Camera {
    vec3 position;          // Camera position
    vec3 target;            // Camera target it looks-at
    vec3 up;                // Camera up vector (rotation over its axis)
    float fovy;             // Camera field-of-view apperture in Y (degrees) in perspective, used as near plane width in orthographic
    int type;               // Camera type, defines projection type: CAMERA_PERSPECTIVE or CAMERA_ORTHOGRAPHIC
} Camera;

// Camera2D type, defines a 2d camera
typedef struct Camera2D {
    vec2    offset;         // Camera offset (displacement from target)
    vec2    target;         // Camera target (rotation and zoom origin)
    float   rotation;       // Camera rotation in degrees
    float   zoom;           // Camera zoom (scaling), should be 1.0f by default
} Camera2D;

// Vertex data definning a mesh
// NOTE: Data stored in CPU memory (and GPU)
typedef struct Mesh {
    int             vertexCount;        // Number of vertices stored in arrays
    int             triangleCount;      // Number of triangles stored (indexed or not)

    // Default vertex data
    float*          vertices;           // Vertex position (XYZ - 3 components per vertex) (shader-location = 0)
    float*          texcoords;          // Vertex texture coordinates (UV - 2 components per vertex) (shader-location = 1)
    float*          texcoords2;         // Vertex second texture coordinates (useful for lightmaps) (shader-location = 5)
    float*          normals;            // Vertex normals (XYZ - 3 components per vertex) (shader-location = 2)
    float*          tangents;           // Vertex tangents (XYZW - 4 components per vertex) (shader-location = 4)
    Color*          colors;             // Vertex colors (RGBA - 4 components per vertex) (shader-location = 3)
    u16*            indices;            // Vertex indices (in case vertex data comes indexed)

    // Animation vertex data
    float*          animVertices;       // Animated vertex positions (after bones transformations)
    float*          animNormals;        // Animated normals (after bones transformations)
    int*            boneIds;            // Vertex bone ids, up to 4 bones influence by vertex (skinning)
    float*          boneWeights;        // Vertex bone weight, up to 4 bones influence by vertex (skinning)

    // OpenGL identifiers
    u32             vaoId;              // OpenGL Vertex Array Object id
    u32             vboId[7];           // OpenGL Vertex Buffer Objects id (default vertex data)
} Mesh;

// Shader type (generic)
typedef struct Shader {
    unsigned int id;                // Shader program id
    int locs[MAX_SHADER_LOCATIONS]; // Shader locations array
} Shader;

// Material texture map
typedef struct MaterialMap {
    Texture texture;      // Material map texture
    Color color;            // Material map color
    float value;            // Material map value
} MaterialMap;

// Material type (generic)
typedef struct Material {
    Shader shader;          // Material shader
    MaterialMap maps[MAX_MATERIAL_MAPS]; // Material maps
    float *params;          // Material generic parameters (if required)
} Material;

// Transformation properties
typedef struct Transform {
    vec3 translation;       // Translation
    quat rotation;          // Rotation
    vec3 scale;             // Scale
} Transform;

// Bone information
typedef struct BoneInfo {
    char name[32];          // Bone name
    int parent;             // Bone parent
} BoneInfo;

// Model type
typedef struct Model {
    mat4 transform;         // Local transform matrix

    int meshCount;          // Number of meshes
    Mesh *meshes;           // Meshes array

    int materialCount;      // Number of materials
    Material *materials;    // Materials array
    int *meshMaterial;      // Mesh material number

    // Animation data
    int boneCount;          // Number of bones
    BoneInfo *bones;        // Bones information (skeleton)
    Transform *bindPose;    // Bones base transformation (pose)
} Model;

// Model animation
typedef struct ModelAnimation {
    int boneCount;          // Number of bones
    BoneInfo *bones;        // Bones information (skeleton)

    int frameCount;         // Number of animation frames
    Transform **framePoses; // Poses array by frame
} ModelAnimation;

// Ray type (useful for raycast)
typedef struct Ray {
    vec3 position;       // Ray position (origin)
    vec3 direction;      // Ray direction
} Ray;

// Raycast hit information
typedef struct RayHitInfo {
    bool hit;               // Did the ray hit something?
    float distance;         // Distance to nearest hit
    vec3 position;       // Position of nearest hit
    vec3 normal;         // Surface normal of hit
} RayHitInfo;

// Bounding box type
typedef struct BoundingBox {
    vec3 min;            // Minimum vertex box-corner
    vec3 max;            // Maximum vertex box-corner
} BoundingBox;

// Wave type, defines audio wave data
typedef struct Wave {
    unsigned int sampleCount;   // Number of samples
    unsigned int sampleRate;    // Frequency (samples per second)
    unsigned int sampleSize;    // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    unsigned int channels;      // Number of channels (1-mono, 2-stereo)
    void *data;                 // Buffer data pointer
} Wave;

// Sound source type
typedef struct Sound {
    void *audioBuffer;      // Pointer to internal data used by the audio system

    unsigned int source;    // Audio source id
    unsigned int buffer;    // Audio buffer id
    int format;             // Audio format specifier
} Sound;

// Music type (file streaming from memory)
// NOTE: Anything longer than ~10 seconds should be streamed
typedef struct MusicData *Music;

// Audio stream type
// NOTE: Useful to create custom audio streams not bound to a specific file
typedef struct AudioStream {
    unsigned int sampleRate;    // Frequency (samples per second)
    unsigned int sampleSize;    // Bit depth (bits per sample): 8, 16, 32 (24 not supported)
    unsigned int channels;      // Number of channels (1-mono, 2-stereo)

    void *audioBuffer;          // Pointer to internal data used by the audio system.

    int format;                 // Audio format specifier
    unsigned int source;        // Audio source id
    unsigned int buffers[2];    // Audio buffers (double buffering)
} AudioStream;

// Head-Mounted-Display device parameters
typedef struct VrDeviceInfo {
    int hResolution;                // HMD horizontal resolution in pixels
    int vResolution;                // HMD vertical resolution in pixels
    float hScreenSize;              // HMD horizontal size in meters
    float vScreenSize;              // HMD vertical size in meters
    float vScreenCenter;            // HMD screen center in meters
    float eyeToScreenDistance;      // HMD distance between eye and display in meters
    float lensSeparationDistance;   // HMD lens separation distance in meters
    float interpupillaryDistance;   // HMD IPD (distance between pupils) in meters
    float lensDistortionValues[4];  // HMD lens distortion constant parameters
    float chromaAbCorrection[4];    // HMD chromatic aberration correction parameters
} VrDeviceInfo;

//----------------------------------------------------------------------------------
// Enumerators Definition
//----------------------------------------------------------------------------------
// System config flags
// NOTE: Used for bit masks
typedef enum {
    FLAG_SHOW_LOGO          = 1,    // Set to show raylib logo at startup
    FLAG_FULLSCREEN_MODE    = 2,    // Set to run program in fullscreen
    FLAG_WINDOW_RESIZABLE   = 4,    // Set to allow resizable window
    FLAG_WINDOW_UNDECORATED = 8,    // Set to disable window decoration (frame and buttons)
    FLAG_WINDOW_TRANSPARENT = 16,   // Set to allow transparent window
    FLAG_WINDOW_HIDDEN      = 128,  // Set to create the window initially hidden
    FLAG_MSAA_4X_HINT       = 32,   // Set to try enabling MSAA 4X
    FLAG_VSYNC_HINT         = 64    // Set to try enabling V-Sync on GPU
} ConfigFlag;

// Trace log type
typedef enum {
    LOG_ALL = 0,        // Display all logs
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL,
    LOG_NONE            // Disable logging
} TraceLogType;

// Keyboard keys
typedef enum {
    // Alphanumeric keys
    KEY_APOSTROPHE      = 39,
    KEY_COMMA           = 44,
    KEY_MINUS           = 45,
    KEY_PERIOD          = 46,
    KEY_SLASH           = 47,
    KEY_ZERO            = 48,
    KEY_ONE             = 49,
    KEY_TWO             = 50,
    KEY_THREE           = 51,
    KEY_FOUR            = 52,
    KEY_FIVE            = 53,
    KEY_SIX             = 54,
    KEY_SEVEN           = 55,
    KEY_EIGHT           = 56,
    KEY_NINE            = 57,
    KEY_SEMICOLON       = 59,
    KEY_EQUAL           = 61,
    KEY_A               = 65,
    KEY_B               = 66,
    KEY_C               = 67,
    KEY_D               = 68,
    KEY_E               = 69,
    KEY_F               = 70,
    KEY_G               = 71,
    KEY_H               = 72,
    KEY_I               = 73,
    KEY_J               = 74,
    KEY_K               = 75,
    KEY_L               = 76,
    KEY_M               = 77,
    KEY_N               = 78,
    KEY_O               = 79,
    KEY_P               = 80,
    KEY_Q               = 81,
    KEY_R               = 82,
    KEY_S               = 83,
    KEY_T               = 84,
    KEY_U               = 85,
    KEY_V               = 86,
    KEY_W               = 87,
    KEY_X               = 88,
    KEY_Y               = 89,
    KEY_Z               = 90,

    // Function keys
    KEY_SPACE           = 32,
    KEY_ESCAPE          = 256,
    KEY_ENTER           = 257,
    KEY_TAB             = 258,
    KEY_BACKSPACE       = 259,
    KEY_INSERT          = 260,
    KEY_DELETE          = 261,
    KEY_RIGHT           = 262,
    KEY_LEFT            = 263,
    KEY_DOWN            = 264,
    KEY_UP              = 265,
    KEY_PAGE_UP         = 266,
    KEY_PAGE_DOWN       = 267,
    KEY_HOME            = 268,
    KEY_END             = 269,
    KEY_CAPS_LOCK       = 280,
    KEY_SCROLL_LOCK     = 281,
    KEY_NUM_LOCK        = 282,
    KEY_PRINT_SCREEN    = 283,
    KEY_PAUSE           = 284,
    KEY_F1              = 290,
    KEY_F2              = 291,
    KEY_F3              = 292,
    KEY_F4              = 293,
    KEY_F5              = 294,
    KEY_F6              = 295,
    KEY_F7              = 296,
    KEY_F8              = 297,
    KEY_F9              = 298,
    KEY_F10             = 299,
    KEY_F11             = 300,
    KEY_F12             = 301,
    KEY_LEFT_SHIFT      = 340,
    KEY_LEFT_CONTROL    = 341,
    KEY_LEFT_ALT        = 342,
    KEY_LEFT_SUPER      = 343,
    KEY_RIGHT_SHIFT     = 344,
    KEY_RIGHT_CONTROL   = 345,
    KEY_RIGHT_ALT       = 346,
    KEY_RIGHT_SUPER     = 347,
    KEY_KB_MENU         = 348,
    KEY_LEFT_BRACKET    = 91,
    KEY_BACKSLASH       = 92,
    KEY_RIGHT_BRACKET   = 93,
    KEY_GRAVE           = 96,

    // Keypad keys
    KEY_KP_0            = 320,
    KEY_KP_1            = 321,
    KEY_KP_2            = 322,
    KEY_KP_3            = 323,
    KEY_KP_4            = 324,
    KEY_KP_5            = 325,
    KEY_KP_6            = 326,
    KEY_KP_7            = 327,
    KEY_KP_8            = 328,
    KEY_KP_9            = 329,
    KEY_KP_DECIMAL      = 330,
    KEY_KP_DIVIDE       = 331,
    KEY_KP_MULTIPLY     = 332,
    KEY_KP_SUBTRACT     = 333,
    KEY_KP_ADD          = 334,
    KEY_KP_ENTER        = 335,
    KEY_KP_EQUAL        = 336
} KeyboardKey;

// Android buttons
typedef enum {
    KEY_BACK            = 4,
    KEY_MENU            = 82,
    KEY_VOLUME_UP       = 24,
    KEY_VOLUME_DOWN     = 25
} AndroidButton;

// Mouse buttons
typedef enum {
    MOUSE_LEFT_BUTTON   = 0,
    MOUSE_RIGHT_BUTTON  = 1,
    MOUSE_MIDDLE_BUTTON = 2
} MouseButton;

// Gamepad number
typedef enum {
    GAMEPAD_PLAYER1     = 0,
    GAMEPAD_PLAYER2     = 1,
    GAMEPAD_PLAYER3     = 2,
    GAMEPAD_PLAYER4     = 3
} GamepadNumber;

// Gamepad Buttons
typedef enum {
    // This is here just for error checking
    GAMEPAD_BUTTON_UNKNOWN = 0,

    // This is normally [A,B,X,Y]/[Circle,Triangle,Square,Cross]
    // No support for 6 button controllers though..
    GAMEPAD_BUTTON_LEFT_FACE_UP,
    GAMEPAD_BUTTON_LEFT_FACE_RIGHT,
    GAMEPAD_BUTTON_LEFT_FACE_DOWN,
    GAMEPAD_BUTTON_LEFT_FACE_LEFT,
  
    // This is normally a DPAD
    GAMEPAD_BUTTON_RIGHT_FACE_UP,
    GAMEPAD_BUTTON_RIGHT_FACE_RIGHT,
    GAMEPAD_BUTTON_RIGHT_FACE_DOWN,
    GAMEPAD_BUTTON_RIGHT_FACE_LEFT,

    // Triggers
    GAMEPAD_BUTTON_LEFT_TRIGGER_1,
    GAMEPAD_BUTTON_LEFT_TRIGGER_2,
    GAMEPAD_BUTTON_RIGHT_TRIGGER_1,
    GAMEPAD_BUTTON_RIGHT_TRIGGER_2,

    // These are buttons in the center of the gamepad
    GAMEPAD_BUTTON_MIDDLE_LEFT,     //PS3 Select
    GAMEPAD_BUTTON_MIDDLE,          //PS Button/XBOX Button
    GAMEPAD_BUTTON_MIDDLE_RIGHT,    //PS3 Start

    // These are the joystick press in buttons
    GAMEPAD_BUTTON_LEFT_THUMB,
    GAMEPAD_BUTTON_RIGHT_THUMB
} GamepadButton;

typedef enum {
    // This is here just for error checking
    GAMEPAD_AXIS_UNKNOWN = 0,

    // Left stick
    GAMEPAD_AXIS_LEFT_X,
    GAMEPAD_AXIS_LEFT_Y,

    // Right stick
    GAMEPAD_AXIS_RIGHT_X,
    GAMEPAD_AXIS_RIGHT_Y,

    // Pressure levels for the back triggers
    GAMEPAD_AXIS_LEFT_TRIGGER,      // [1..-1] (pressure-level)
    GAMEPAD_AXIS_RIGHT_TRIGGER      // [1..-1] (pressure-level)
} GamepadAxis;

// Shader location point type
typedef enum {
    LOC_VERTEX_POSITION = 0,
    LOC_VERTEX_TEXCOORD01,
    LOC_VERTEX_TEXCOORD02,
    LOC_VERTEX_NORMAL,
    LOC_VERTEX_TANGENT,
    LOC_VERTEX_COLOR,
    LOC_MATRIX_MVP,
    LOC_MATRIX_MODEL,
    LOC_MATRIX_VIEW,
    LOC_MATRIX_PROJECTION,
    LOC_VECTOR_VIEW,
    LOC_COLOR_DIFFUSE,
    LOC_COLOR_SPECULAR,
    LOC_COLOR_AMBIENT,
    LOC_MAP_ALBEDO,          // LOC_MAP_DIFFUSE
    LOC_MAP_METALNESS,       // LOC_MAP_SPECULAR
    LOC_MAP_NORMAL,
    LOC_MAP_ROUGHNESS,
    LOC_MAP_OCCLUSION,
    LOC_MAP_EMISSION,
    LOC_MAP_HEIGHT,
    LOC_MAP_CUBEMAP,
    LOC_MAP_IRRADIANCE,
    LOC_MAP_PREFILTER,
    LOC_MAP_BRDF
} ShaderLocationIndex;

#define LOC_MAP_DIFFUSE      LOC_MAP_ALBEDO
#define LOC_MAP_SPECULAR     LOC_MAP_METALNESS

// Shader uniform data types
typedef enum {
    UNIFORM_FLOAT = 0,
    UNIFORM_VEC2,
    UNIFORM_VEC3,
    UNIFORM_VEC4,
    UNIFORM_INT,
    UNIFORM_IVEC2,
    UNIFORM_IVEC3,
    UNIFORM_IVEC4,
    UNIFORM_SAMPLER2D
} ShaderUniformDataType;

// Material map type
typedef enum {
    MAP_ALBEDO    = 0,       // MAP_DIFFUSE
    MAP_METALNESS = 1,       // MAP_SPECULAR
    MAP_NORMAL    = 2,
    MAP_ROUGHNESS = 3,
    MAP_OCCLUSION,
    MAP_EMISSION,
    MAP_HEIGHT,
    MAP_CUBEMAP,             // NOTE: Uses GL_TEXTURE_CUBE_MAP
    MAP_IRRADIANCE,          // NOTE: Uses GL_TEXTURE_CUBE_MAP
    MAP_PREFILTER,           // NOTE: Uses GL_TEXTURE_CUBE_MAP
    MAP_BRDF
} MaterialMapType;

#define MAP_DIFFUSE      MAP_ALBEDO
#define MAP_SPECULAR     MAP_METALNESS

// Pixel formats
// NOTE: Support depends on OpenGL version and platform
typedef enum {
    UNCOMPRESSED_GRAYSCALE = 1,     // 8 bit per pixel (no alpha)
    UNCOMPRESSED_GRAY_ALPHA,        // 8*2 bpp (2 channels)
    UNCOMPRESSED_R5G6B5,            // 16 bpp
    UNCOMPRESSED_R8G8B8,            // 24 bpp
    UNCOMPRESSED_R5G5B5A1,          // 16 bpp (1 bit alpha)
    UNCOMPRESSED_R4G4B4A4,          // 16 bpp (4 bit alpha)
    UNCOMPRESSED_R8G8B8A8,          // 32 bpp
    UNCOMPRESSED_R32,               // 32 bpp (1 channel - float)
    UNCOMPRESSED_R32G32B32,         // 32*3 bpp (3 channels - float)
    UNCOMPRESSED_R32G32B32A32,      // 32*4 bpp (4 channels - float)
    COMPRESSED_DXT1_RGB,            // 4 bpp (no alpha)
    COMPRESSED_DXT1_RGBA,           // 4 bpp (1 bit alpha)
    COMPRESSED_DXT3_RGBA,           // 8 bpp
    COMPRESSED_DXT5_RGBA,           // 8 bpp
    COMPRESSED_ETC1_RGB,            // 4 bpp
    COMPRESSED_ETC2_RGB,            // 4 bpp
    COMPRESSED_ETC2_EAC_RGBA,       // 8 bpp
    COMPRESSED_PVRT_RGB,            // 4 bpp
    COMPRESSED_PVRT_RGBA,           // 4 bpp
    COMPRESSED_ASTC_4x4_RGBA,       // 8 bpp
    COMPRESSED_ASTC_8x8_RGBA        // 2 bpp
} PixelFormat;

// Texture parameters: filter mode
// NOTE 1: Filtering considers mipmaps if available in the texture
// NOTE 2: Filter is accordingly set for minification and magnification
typedef enum {
    FILTER_POINT = 0,               // No filter, just pixel aproximation
    FILTER_BILINEAR,                // Linear filtering
    FILTER_TRILINEAR,               // Trilinear filtering (linear with mipmaps)
    FILTER_ANISOTROPIC_4X,          // Anisotropic filtering 4x
    FILTER_ANISOTROPIC_8X,          // Anisotropic filtering 8x
    FILTER_ANISOTROPIC_16X,         // Anisotropic filtering 16x
} TextureFilterMode;

// Cubemap layout type
typedef enum {
    CUBEMAP_AUTO_DETECT = 0,        // Automatically detect layout type
    CUBEMAP_LINE_VERTICAL,          // Layout is defined by a vertical line with faces
    CUBEMAP_LINE_HORIZONTAL,        // Layout is defined by an horizontal line with faces
    CUBEMAP_CROSS_THREE_BY_FOUR,    // Layout is defined by a 3x4 cross with cubemap faces
    CUBEMAP_CROSS_FOUR_BY_THREE,    // Layout is defined by a 4x3 cross with cubemap faces
    CUBEMAP_PANORAMA                // Layout is defined by a panorama image (equirectangular map)
} CubemapLayoutType;

// Texture parameters: wrap mode
typedef enum {
    WRAP_REPEAT = 0,        // Repeats texture in tiled mode
    WRAP_CLAMP,             // Clamps texture to edge pixel in tiled mode
    WRAP_MIRROR_REPEAT,     // Mirrors and repeats the texture in tiled mode
    WRAP_MIRROR_CLAMP       // Mirrors and clamps to border the texture in tiled mode
} TextureWrapMode;

// Font type, defines generation method
typedef enum {
    FONT_DEFAULT = 0,       // Default font generation, anti-aliased
    FONT_BITMAP,            // Bitmap font generation, no anti-aliasing
    FONT_SDF                // SDF font generation, requires external shader
} FontType;

// Color blending modes (pre-defined)
typedef enum {
    BLEND_ALPHA = 0,        // Blend textures considering alpha (default)
    BLEND_ADDITIVE,         // Blend textures adding colors
    BLEND_MULTIPLIED        // Blend textures multiplying colors
} BlendMode;

// Gestures type
// NOTE: It could be used as flags to enable only some gestures
typedef enum {
    GESTURE_NONE        = 0,
    GESTURE_TAP         = 1,
    GESTURE_DOUBLETAP   = 2,
    GESTURE_HOLD        = 4,
    GESTURE_DRAG        = 8,
    GESTURE_SWIPE_RIGHT = 16,
    GESTURE_SWIPE_LEFT  = 32,
    GESTURE_SWIPE_UP    = 64,
    GESTURE_SWIPE_DOWN  = 128,
    GESTURE_PINCH_IN    = 256,
    GESTURE_PINCH_OUT   = 512
} GestureType;

// Camera system modes
typedef enum {
    CAMERA_CUSTOM = 0,
    CAMERA_FREE,
    CAMERA_ORBITAL,
    CAMERA_FIRST_PERSON,
    CAMERA_THIRD_PERSON
} CameraMode;

// Camera projection modes
typedef enum {
    CAMERA_PERSPECTIVE = 0,
    CAMERA_ORTHOGRAPHIC
} CameraType;

// Type of n-patch
typedef enum {
    NPT_9PATCH = 0,         // Npatch defined by 3x3 tiles
    NPT_3PATCH_VERTICAL,    // Npatch defined by 1x3 tiles
    NPT_3PATCH_HORIZONTAL   // Npatch defined by 3x1 tiles
} NPatchType;

// Callbacks to be implemented by users
typedef void (*TraceLogCallback)(int logType, const char *text, ArgList args);

