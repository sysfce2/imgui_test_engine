// dear imgui
// (test engine, core)

#pragma once

#include "imgui.h"
#include "imgui_internal.h"         // ImPool<>, ImGuiItemStatusFlags, ImFormatString
#include "imgui_te_util.h"

//-------------------------------------------------------------------------
// Forward Declarations
//-------------------------------------------------------------------------

struct ImGuiTest;
struct ImGuiTestContext;
struct ImGuiTestCoroutineInterface;
struct ImGuiTestEngine;
struct ImGuiTestEngineIO;
struct ImGuiTestItemInfo;
struct ImGuiTestItemList;
struct ImGuiTestInputs;
struct ImGuiTestGatherTask;
struct ImGuiTestFindByLabelTask;
struct ImGuiTestInfoTask;
struct ImGuiTestRunTask;

struct ImGuiCaptureArgs;
struct ImGuiPerfTool;
struct ImRect;

typedef int ImGuiTestFlags;         // Flags: See ImGuiTestFlags_
typedef int ImGuiTestCheckFlags;    // Flags: See ImGuiTestCheckFlags_
typedef int ImGuiTestLogFlags;      // Flags: See ImGuiTestLogFlags_
typedef int ImGuiTestOpFlags;       // Flags: See ImGuiTestOpFlags_
typedef int ImGuiTestRunFlags;      // Flags: See ImGuiTestRunFlags_

//-------------------------------------------------------------------------
// Types
//-------------------------------------------------------------------------

enum ImGuiTestVerboseLevel
{
    ImGuiTestVerboseLevel_Silent    = 0, // -v0
    ImGuiTestVerboseLevel_Error     = 1, // -v1
    ImGuiTestVerboseLevel_Warning   = 2, // -v2
    ImGuiTestVerboseLevel_Info      = 3, // -v3
    ImGuiTestVerboseLevel_Debug     = 4, // -v4
    ImGuiTestVerboseLevel_Trace     = 5,
    ImGuiTestVerboseLevel_COUNT     = 6
};

enum ImGuiTestStatus
{
    ImGuiTestStatus_Unknown     = -1,
    ImGuiTestStatus_Success     = 0,
    ImGuiTestStatus_Queued      = 1,
    ImGuiTestStatus_Running     = 2,
    ImGuiTestStatus_Error       = 3,
    ImGuiTestStatus_Suspended   = 4,
};

enum ImGuiTestGroup
{
    ImGuiTestGroup_Unknown      = -1,
    ImGuiTestGroup_Tests        = 0,
    ImGuiTestGroup_Perfs        = 1,
    ImGuiTestGroup_COUNT
};

enum ImGuiTestFlags_
{
    ImGuiTestFlags_None                 = 0,
    ImGuiTestFlags_NoWarmUp             = 1 << 0,   // By default, we run the GUI func twice before starting the test code
    ImGuiTestFlags_NoAutoFinish         = 1 << 1,   // By default, tests with no test func end on Frame 0 (after the warm up). Setting this require test to call ctx->Finish().
    ImGuiTestFlags_NoRecoverWarnings    = 1 << 2
    //ImGuiTestFlags_RequireViewports   = 1 << 10
};

// Flags for IM_CHECK* macros.
enum ImGuiTestCheckFlags_
{
    ImGuiTestCheckFlags_None            = 0,
    ImGuiTestCheckFlags_SilentSuccess   = 1 << 0
};

// Flags for ImGuiTestContext::Log* functions.
enum ImGuiTestLogFlags_
{
    ImGuiTestLogFlags_None              = 0,
    ImGuiTestLogFlags_NoHeader          = 1 << 0    // Do not display frame count and depth padding
};

// Generic flags for various ImGuiTestContext functions
enum ImGuiTestOpFlags_
{
    ImGuiTestOpFlags_None               = 0,
    ImGuiTestOpFlags_Verbose            = 1 << 0,
    ImGuiTestOpFlags_NoCheckHoveredId   = 1 << 1,
    ImGuiTestOpFlags_NoError            = 1 << 2,   // Don't abort/error e.g. if the item cannot be found
    ImGuiTestOpFlags_NoFocusWindow      = 1 << 3,
    ImGuiTestOpFlags_NoAutoUncollapse   = 1 << 4,   // Disable automatically uncollapsing windows (useful when specifically testing Collapsing behaviors)
    ImGuiTestOpFlags_IsSecondAttempt    = 1 << 5,
    ImGuiTestOpFlags_MoveToEdgeL        = 1 << 6,   // Dumb aiming helpers to test widget that care about clicking position. May need to replace will better functionalities.
    ImGuiTestOpFlags_MoveToEdgeR        = 1 << 7,
    ImGuiTestOpFlags_MoveToEdgeU        = 1 << 8,
    ImGuiTestOpFlags_MoveToEdgeD        = 1 << 9
};

enum ImGuiTestRunFlags_
{
    ImGuiTestRunFlags_None              = 0,
    ImGuiTestRunFlags_GuiFuncDisable    = 1 << 0,   // Used internally to temporarily disable the GUI func (at the end of a test, etc)
    ImGuiTestRunFlags_GuiFuncOnly       = 1 << 1,   // Set when user selects "Run GUI func"
    ImGuiTestRunFlags_NoSuccessMsg      = 1 << 2,
    ImGuiTestRunFlags_NoStopOnError     = 1 << 3,
    ImGuiTestRunFlags_NoBreakOnError    = 1 << 4,
    ImGuiTestRunFlags_ManualRun         = 1 << 5,
    ImGuiTestRunFlags_CommandLine       = 1 << 6
};

enum ImGuiTestInputType
{
    ImGuiTestInputType_None,
    ImGuiTestInputType_Key,
    ImGuiTestInputType_Nav,
    ImGuiTestInputType_Char
};

struct ImGuiTestInput
{
    ImGuiTestInputType      Type = ImGuiTestInputType_None;
    ImGuiKey                Key = ImGuiKey_COUNT;
    ImGuiKeyModFlags        KeyMods = ImGuiKeyModFlags_None;
    ImGuiNavInput           NavInput = ImGuiNavInput_COUNT;
    ImWchar                 Char = 0;
    ImGuiKeyState           State = ImGuiKeyState_Unknown;

    static ImGuiTestInput   FromKey(ImGuiKey v, ImGuiKeyState state, ImGuiKeyModFlags mods = ImGuiKeyModFlags_None)
    {
        ImGuiTestInput inp;
        inp.Type = ImGuiTestInputType_Key;
        inp.Key = v;
        inp.KeyMods = mods;
        inp.State = state;
        return inp;
    }

    static ImGuiTestInput   FromNav(ImGuiNavInput v, ImGuiKeyState state)
    {
        ImGuiTestInput inp;
        inp.Type = ImGuiTestInputType_Nav;
        inp.NavInput = v;
        inp.State = state;
        return inp;
    }

    static ImGuiTestInput   FromChar(ImWchar v)
    {
        ImGuiTestInput inp;
        inp.Type = ImGuiTestInputType_Char;
        inp.Char = v;
        return inp;
    }
};

//-------------------------------------------------------------------------
// Hooks for Core Library
//-------------------------------------------------------------------------

extern void     ImGuiTestEngineHook_Shutdown(ImGuiContext* ctx);
extern void     ImGuiTestEngineHook_PreNewFrame(ImGuiContext* ui_ctx);
extern void     ImGuiTestEngineHook_PostNewFrame(ImGuiContext* ui_ctx);
extern void     ImGuiTestEngineHook_ItemAdd(ImGuiContext* ui_ctx, const ImRect& bb, ImGuiID id);
#ifdef IMGUI_HAS_IMSTR
extern void     ImGuiTestEngineHook_ItemInfo(ImGuiContext* ui_ctx, ImGuiID id, ImStrv label, ImGuiItemStatusFlags flags);
#else
extern void     ImGuiTestEngineHook_ItemInfo(ImGuiContext* ui_ctx, ImGuiID id, const char* label, ImGuiItemStatusFlags flags);
static inline int ImStrcmp(const char* str1, const char* str2) { return strcmp(str1, str2); } // FIXME: to remove once this gets added in core library
#endif
extern void     ImGuiTestEngineHook_Log(ImGuiContext* ui_ctx, const char* fmt, ...);
extern void     ImGuiTestEngineHook_AssertFunc(const char* expr, const char* file, const char* function, int line);
const char*     ImGuiTestEngine_FindItemDebugLabel(ImGuiContext* ui_ctx, ImGuiID id);

//-------------------------------------------------------------------------
// Macros for Tests
//-------------------------------------------------------------------------

// Register a new test
#define IM_REGISTER_TEST(_ENGINE, _CAT, _NAME)    ImGuiTestEngine_RegisterTest(_ENGINE, _CAT, _NAME, __FILE__, __LINE__)

// We embed every macro in a do {} while(0) statement as a trick to allow using them as regular single statement, e.g. if (XXX) IM_CHECK(A); else IM_CHECK(B)
// We leave the assert call (which will trigger a debugger break) outside of the check function to step out faster.
#define IM_CHECK_NO_RET(_EXPR)              do { if (ImGuiTestEngineHook_Check(__FILE__, __func__, __LINE__, ImGuiTestCheckFlags_None, (bool)(_EXPR), #_EXPR))          { IM_ASSERT(_EXPR); } } while (0)
#define IM_CHECK(_EXPR)                     do { if (ImGuiTestEngineHook_Check(__FILE__, __func__, __LINE__, ImGuiTestCheckFlags_None, (bool)(_EXPR), #_EXPR))          { IM_ASSERT(_EXPR); } if (!(bool)(_EXPR)) return; } while (0)
#define IM_CHECK_SILENT(_EXPR)              do { if (ImGuiTestEngineHook_Check(__FILE__, __func__, __LINE__, ImGuiTestCheckFlags_SilentSuccess, (bool)(_EXPR), #_EXPR)) { IM_ASSERT(0); } if (!(bool)(_EXPR)) return; } while (0)
#define IM_CHECK_RETV(_EXPR,_RETV)          do { if (ImGuiTestEngineHook_Check(__FILE__, __func__, __LINE__, ImGuiTestCheckFlags_None, (bool)(_EXPR), #_EXPR))          { IM_ASSERT(_EXPR); } if (!(bool)(_EXPR)) return _RETV; } while (0)
#define IM_CHECK_SILENT_RETV(_EXPR,_RETV)   do { if (ImGuiTestEngineHook_Check(__FILE__, __func__, __LINE__, ImGuiTestCheckFlags_SilentSuccess, (bool)(_EXPR), #_EXPR)) { IM_ASSERT(_EXPR); } if (!(bool)(_EXPR)) return _RETV; } while (0)
#define IM_ERRORF(_FMT,...)                 do { if (ImGuiTestEngineHook_Error(__FILE__, __func__, __LINE__, ImGuiTestCheckFlags_None, _FMT, __VA_ARGS__))              { IM_ASSERT(0); } } while (0)
#define IM_ERRORF_NOHDR(_FMT,...)           do { if (ImGuiTestEngineHook_Error(NULL, NULL, 0, ImGuiTestCheckFlags_None, _FMT, __VA_ARGS__))                             { IM_ASSERT(0); } } while (0)

template<typename T> void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, T value)         { buf.appendf("???"); IM_UNUSED(value); } // FIXME-TESTS: Could improve with some template magic
template<> inline void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, const char* value)  { buf.appendf("\"%s\"", value); }
template<> inline void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, bool value)         { buf.append(value ? "true" : "false"); }
template<> inline void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, ImS8 value)         { buf.appendf("%d", value); }
template<> inline void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, ImU8 value)         { buf.appendf("%u", value); }
template<> inline void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, ImS16 value)        { buf.appendf("%hd", value); }
template<> inline void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, ImU16 value)        { buf.appendf("%hu", value); }
template<> inline void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, ImS32 value)        { buf.appendf("%d", value); }
template<> inline void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, ImU32 value)        { buf.appendf("%u", value); }
template<> inline void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, ImS64 value)        { buf.appendf("%lld", value); }
template<> inline void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, ImU64 value)        { buf.appendf("%llu", value); }
template<> inline void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, float value)        { buf.appendf("%.3f", value); }
template<> inline void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, double value)       { buf.appendf("%f", value); }
template<> inline void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, ImVec2 value)       { buf.appendf("(%.3f, %.3f)", value.x, value.y); }
template<> inline void ImGuiTestEngineUtil_AppendStrValue(ImGuiTextBuffer& buf, const void* value)  { buf.appendf("%p", value); }

static inline void ImGuiTestEngineUtil_AppendStrCompareOp(ImGuiTextBuffer& buf, const char* lhs_var, const char* lhs_val, const char* op, const char* rhs_var, const char* rhs_val)
{
    bool lhs_is_literal = lhs_var[0] == '\"';
    bool rhs_is_literal = rhs_var[0] == '\"';
    if (strchr(lhs_val, '\n') != NULL || strchr(rhs_var, '\n') != NULL)
    {
        // Multi line strings
        size_t lhs_val_len = strlen(lhs_val);
        size_t rhs_val_len = strlen(rhs_val);
        if (lhs_val_len > 0 && lhs_val[lhs_val_len - 1] == '\n') // Strip trailing carriage return as we are adding one ourselves
            lhs_val_len--;
        if (rhs_val_len > 0 && rhs_val[rhs_val_len - 1] == '\n')
            rhs_val_len--;
        buf.appendf(
            "\n"
            "---------------------------------------- // lhs: %s\n"
            "%.*s\n"
            "---------------------------------------- // rhs: %s, compare op: %s\n"
            "%.*s\n"
            "----------------------------------------\n",
            lhs_is_literal ? "literal" : lhs_var,
            (int)lhs_val_len, lhs_val,
            rhs_is_literal ? "literal" : rhs_var,
            op,
            (int)rhs_val_len, rhs_val);
    }
    else
    {
        // Single line strings
        buf.appendf(
            "%s [\"%s\"] %s %s [\"%s\"]",
            lhs_is_literal ? "" : lhs_var, lhs_val,
            op,
            rhs_is_literal ? "" : rhs_var, rhs_val);
    }
}

// Those macros allow us to print out the values of both lhs and rhs expressions involved in a check.
#define IM_CHECK_OP(_LHS, _RHS, _OP, _RETURN)                       \
    do                                                              \
    {                                                               \
        auto __lhs = _LHS;  /* Cache to avoid side effects */       \
        auto __rhs = _RHS;                                          \
        bool __res = __lhs _OP __rhs;                               \
        ImGuiTextBuffer expr_buf;                                   \
        expr_buf.appendf("%s [", #_LHS);                            \
        ImGuiTestEngineUtil_AppendStrValue(expr_buf, __lhs);        \
        expr_buf.appendf("] " #_OP " %s [", #_RHS);                 \
        ImGuiTestEngineUtil_AppendStrValue(expr_buf, __rhs);        \
        expr_buf.append("]");                                       \
        if (ImGuiTestEngineHook_Check(__FILE__, __func__, __LINE__, ImGuiTestCheckFlags_None, __res, expr_buf.c_str())) \
            IM_ASSERT(__res);                                       \
        if (_RETURN && !__res)                                      \
            return;                                                 \
    } while (0)

#define IM_CHECK_STR_OP(_LHS, _RHS, _OP, _RETURN, _FLAGS)           \
    do                                                              \
    {                                                               \
        Str256 __lhs(_LHS);  /* Cache to avoid side effects */      \
        Str256 __rhs(_RHS);                                         \
        bool __res = strcmp(__lhs.c_str(), __rhs.c_str()) _OP 0;    \
        ImGuiTextBuffer expr_buf;                                   \
        ImGuiTestEngineUtil_AppendStrCompareOp(expr_buf, #_LHS, __lhs.c_str(), #_OP, #_RHS, __rhs.c_str()); \
        if (ImGuiTestEngineHook_Check(__FILE__, __func__, __LINE__, _FLAGS, __res, expr_buf.c_str())) \
            IM_ASSERT(__res);                                               \
        if (_RETURN && !__res)                                              \
            return;                                                         \
    } while (0)

#define IM_CHECK_STR_EQ(_LHS, _RHS)         IM_CHECK_STR_OP(_LHS, _RHS, ==, true, ImGuiTestCheckFlags_None)
#define IM_CHECK_STR_NE(_LHS, _RHS)         IM_CHECK_STR_OP(_LHS, _RHS, !=, true, ImGuiTestCheckFlags_None)
#define IM_CHECK_STR_EQ_NO_RET(_LHS, _RHS)  IM_CHECK_STR_OP(_LHS, _RHS, ==, false, ImGuiTestCheckFlags_None)
#define IM_CHECK_STR_NE_NO_RET(_LHS, _RHS)  IM_CHECK_STR_OP(_LHS, _RHS, !=, false, ImGuiTestCheckFlags_None)
#define IM_CHECK_STR_EQ_SILENT(_LHS, _RHS)  IM_CHECK_STR_OP(_LHS, _RHS, ==, true, ImGuiTestCheckFlags_SilentSuccess)

#define IM_CHECK_EQ(_LHS, _RHS)             IM_CHECK_OP(_LHS, _RHS, ==, true)   // Equal
#define IM_CHECK_NE(_LHS, _RHS)             IM_CHECK_OP(_LHS, _RHS, !=, true)   // Not Equal
#define IM_CHECK_LT(_LHS, _RHS)             IM_CHECK_OP(_LHS, _RHS, < , true)   // Less Than
#define IM_CHECK_LE(_LHS, _RHS)             IM_CHECK_OP(_LHS, _RHS, <=, true)   // Less or Equal
#define IM_CHECK_GT(_LHS, _RHS)             IM_CHECK_OP(_LHS, _RHS, > , true)   // Greater Than
#define IM_CHECK_GE(_LHS, _RHS)             IM_CHECK_OP(_LHS, _RHS, >=, true)   // Greater or Equal

#define IM_CHECK_EQ_NO_RET(_LHS, _RHS)      IM_CHECK_OP(_LHS, _RHS, ==, false)  // Equal
#define IM_CHECK_NE_NO_RET(_LHS, _RHS)      IM_CHECK_OP(_LHS, _RHS, !=, false)  // Not Equal
#define IM_CHECK_LT_NO_RET(_LHS, _RHS)      IM_CHECK_OP(_LHS, _RHS, < , false)  // Less Than
#define IM_CHECK_LE_NO_RET(_LHS, _RHS)      IM_CHECK_OP(_LHS, _RHS, <=, false)  // Less or Equal
#define IM_CHECK_GT_NO_RET(_LHS, _RHS)      IM_CHECK_OP(_LHS, _RHS, > , false)  // Greater Than
#define IM_CHECK_GE_NO_RET(_LHS, _RHS)      IM_CHECK_OP(_LHS, _RHS, >=, false)  // Greater or Equal

#define IM_CHECK_FLOAT_EQ_EPS(_LHS, _RHS)               IM_CHECK_LE(ImFabs(_LHS - (_RHS)), FLT_EPSILON)   // Float Equal
#define IM_CHECK_FLOAT_NEAR(_LHS, _RHS, _EPS)           IM_CHECK_LE(ImFabs(_LHS - (_RHS)), _EPS)
#define IM_CHECK_FLOAT_NEAR_NO_RET(_LHS, _RHS, _EPS)    IM_CHECK_LE_NO_RET(ImFabs(_LHS - (_RHS)), _EPS)

bool        ImGuiTestEngineHook_Check(const char* file, const char* func, int line, ImGuiTestCheckFlags flags, bool result, const char* expr);
bool        ImGuiTestEngineHook_Error(const char* file, const char* func, int line, ImGuiTestCheckFlags flags, const char* fmt, ...);

//-------------------------------------------------------------------------
// Macros for Debug / Control Flow
//-------------------------------------------------------------------------

#define IM_DEBUG_HALT_TESTFUNC()            do { if (ctx->DebugHaltTestFunc(__FILE__, __LINE__)) return; } while (0)

//-------------------------------------------------------------------------
// ImGuiTestEngine API
//-------------------------------------------------------------------------

// Functions: Initialization
ImGuiTestEngine*    ImGuiTestEngine_CreateContext(ImGuiContext* ui_ctx);            // Create test engine
void                ImGuiTestEngine_ShutdownContext(ImGuiTestEngine* engine);       // Destroy test engine. Call after ImGui::DestroyContext() so test engine specific ini data gets saved.
void                ImGuiTestEngine_Start(ImGuiTestEngine* engine);
void                ImGuiTestEngine_Stop(ImGuiTestEngine* engine);
void                ImGuiTestEngine_PostSwap(ImGuiTestEngine* engine);              // Call every frame after framebuffer swap, will process screen capture.
ImGuiTestEngineIO&  ImGuiTestEngine_GetIO(ImGuiTestEngine* engine);
void                ImGuiTestEngine_RebootUiContext(ImGuiTestEngine* engine);

// Functions: Main
ImGuiTest*          ImGuiTestEngine_RegisterTest(ImGuiTestEngine* engine, const char* category, const char* name, const char* src_file = NULL, int src_line = 0);
void                ImGuiTestEngine_QueueTests(ImGuiTestEngine* engine, ImGuiTestGroup group, const char* filter = NULL, ImGuiTestRunFlags run_flags = 0);
void                ImGuiTestEngine_QueueTest(ImGuiTestEngine* engine, ImGuiTest* test, ImGuiTestRunFlags run_flags = 0);
void                ImGuiTestEngine_AbortTest(ImGuiTestEngine* engine);
bool                ImGuiTestEngine_TryAbortEngine(ImGuiTestEngine* engine);
bool                ImGuiTestEngine_IsRunningTests(ImGuiTestEngine* engine);        // FIXME: Clarify difference between this and io.RunningTests
void                ImGuiTestEngine_CoroutineStopRequest(ImGuiTestEngine* engine);
void                ImGuiTestEngine_UpdateHooks(ImGuiTestEngine* engine);
void                ImGuiTestEngine_GetResult(ImGuiTestEngine* engine, int& count_tested, int& success_count);
void                ImGuiTestEngine_PrintResultSummary(ImGuiTestEngine* engine);
ImGuiPerfTool*      ImGuiTestEngine_GetPerfTool(ImGuiTestEngine* engine);

// Functions: UI
void                ImGuiTestEngine_ShowTestWindows(ImGuiTestEngine* engine, bool* p_open);

// Function pointers for IO structure
// (also see imgui_te_coroutine.h for coroutine functions)
typedef void        (*ImGuiTestEngineSrcFileOpenFunc)(const char* filename, int line, void* user_data);
typedef bool        (*ImGuiTestEngineScreenCaptureFunc)(ImGuiID viewport_id, int x, int y, int w, int h, unsigned int* pixels, void* user_data);

// IO structure
struct ImGuiTestEngineIO
{
    // Inputs: Functions
    ImGuiTestEngineSrcFileOpenFunc      SrcFileOpenFunc = NULL;         // (Optional) To open source files
    void*                               SrcFileOpenUserData = NULL;     // (Optional) User data for SrcFileOpenFunc
    ImGuiTestEngineScreenCaptureFunc    ScreenCaptureFunc = NULL;       // (Optional) To capture graphics output
    void*                               ScreenCaptureUserData = NULL;   // (Optional) User data for ScreenCaptureFunc
    ImGuiTestCoroutineInterface*        CoroutineFuncs = NULL;          // (Required) Coroutine functions (see imgui_te_coroutines.h)

    // Inputs: Options
    bool                        ConfigRunWithGui = false;       // Run without graphics output (e.g. command-line)
    bool                        ConfigRunFast = true;           // Run tests as fast as possible (teleport mouse, skip delays, etc.)
    bool                        ConfigRunBlind = false;         // Run tests in a blind ImGuiContext separated from the visible context
    bool                        ConfigStopOnError = false;      // Stop queued tests on test error
    bool                        ConfigBreakOnError = false;     // Break debugger on test error
    bool                        ConfigKeepGuiFunc = false;      // Keep test GUI running at the end of the test
    ImGuiTestVerboseLevel       ConfigVerboseLevel = ImGuiTestVerboseLevel_Warning;
    ImGuiTestVerboseLevel       ConfigVerboseLevelOnError = ImGuiTestVerboseLevel_Info;
    bool                        ConfigLogToTTY = false;
    bool                        ConfigLogToDebugger = false;
    bool                        ConfigTakeFocusBackAfterTests = true;
    bool                        ConfigCaptureEnabled = true;
    bool                        ConfigCaptureOnError = false;
    bool                        ConfigNoThrottle = false;       // Disable vsync for performance measurement or fast test running
    float                       ConfigFixedDeltaTime = 0.0f;    // Use fixed delta time instead of calculating it from wall clock
    float                       DpiScale = 1.0f;
    float                       MouseSpeed = 1000.0f;           // Mouse speed (pixel/second) when not running in fast mode
    float                       MouseWobble = 0.25f;            // How much wobble to apply to the mouse (pixels per pixel of move distance) when not running in fast mode
    float                       ScrollSpeed = 1600.0f;          // Scroll speed (pixel/second) when not running in fast mode
    float                       TypingSpeed = 30.0f;            // Char input speed (characters/second) when not running in fast mode
    int                         PerfStressAmount = 1;           // Integer to scale the amount of items submitted in test
    char                        GitBranchName[64] = "";         // e.g. fill in branch name

    // Outputs: State
    bool                        RunningTests = false;
    bool                        RenderWantMaxSpeed = false;
};

// Result of an ItemInfo query
struct ImGuiTestItemInfo
{
    int                         RefCount : 8;               // User can increment this if they want to hold on the result pointer across frames, otherwise the task will be GC-ed.
    unsigned int                NavLayer : 1;               // Nav layer of the item
    int                         Depth : 16;                 // Depth from requested parent id. 0 == ID is immediate child of requested parent id.
    int                         TimestampMain = -1;         // Timestamp of main result (all fields)
    int                         TimestampStatus = -1;       // Timestamp of StatusFlags
    ImGuiID                     ID = 0;                     // Item ID
    ImGuiID                     ParentID = 0;               // Item Parent ID (value at top of the ID stack)
    ImGuiWindow*                Window = NULL;              // Item Window
    ImRect                      RectFull = ImRect();        // Item Rectangle
    ImRect                      RectClipped = ImRect();     // Item Rectangle (clipped with window->ClipRect at time of item submission)
    ImGuiItemStatusFlags        StatusFlags = 0;            // Item Status flags (fully updated for some items only, compare TimestampStatus to FrameCount)
    char                        DebugLabel[32] = {};        // Shortened label for debugging purpose

    ImGuiTestItemInfo()
    {
        RefCount = 0;
        NavLayer = 0;
        Depth = 0;
    }
};

// Result of an ItemGather query
struct ImGuiTestItemList
{
    ImPool<ImGuiTestItemInfo>   Pool;

    void                        Clear()                 { Pool.Clear(); }
    void                        Reserve(int capacity)   { Pool.Reserve(capacity); }
    int                         GetSize() const         { return Pool.GetMapSize(); }
    const ImGuiTestItemInfo*    GetByIndex(int n)       { return Pool.GetByIndex(n); }
    const ImGuiTestItemInfo*    GetByID(ImGuiID id)     { return Pool.GetByKey(id); }

    // For range-for
    size_t                      size() const            { return (size_t)Pool.GetMapSize(); }
    const ImGuiTestItemInfo*    begin() const           { return Pool.Buf.begin(); }
    const ImGuiTestItemInfo*    end() const             { return Pool.Buf.end(); }
    const ImGuiTestItemInfo*    operator[] (size_t n)   { return &Pool.Buf[(int)n]; }
};

//-------------------------------------------------------------------------
// ImGuiTestLog
//-------------------------------------------------------------------------

struct ImGuiTestLogLineInfo
{
    ImGuiTestVerboseLevel Level;
    int                   LineOffset;
};

struct ImGuiTestLog
{
    ImGuiTextBuffer                Buffer;
    ImVector<ImGuiTestLogLineInfo> LineInfo;
    ImVector<ImGuiTestLogLineInfo> LineInfoError;
    bool                           CachedLinesPrintedToTTY;

    ImGuiTestLog()
    {
        CachedLinesPrintedToTTY = false;
    }

    void Clear()
    {
        Buffer.clear();
        LineInfo.clear();
        LineInfoError.clear();
        CachedLinesPrintedToTTY = false;
    }

    void UpdateLineOffsets(ImGuiTestEngineIO* engine_io, ImGuiTestVerboseLevel level, const char* start)
    {
        IM_ASSERT(Buffer.begin() <= start && start < Buffer.end());
        const char* p_begin = start;
        const char* p_end = Buffer.end();
        const char* p = p_begin;
        while (p < p_end)
        {
            const char* p_bol = p;
            const char* p_eol = strchr(p, '\n');

            bool last_empty_line = (p_bol + 1 == p_end);

            if (!last_empty_line)
            {
                int offset = (int)(p_bol - Buffer.c_str());
                if (engine_io->ConfigVerboseLevel >= level)
                    LineInfo.push_back({level, offset});
                if (engine_io->ConfigVerboseLevelOnError >= level)
                    LineInfoError.push_back({level, offset});
            }
            p = p_eol ? p_eol + 1 : NULL;
        }
    }
};

//-------------------------------------------------------------------------
// ImGuiTest
//-------------------------------------------------------------------------

typedef void    (*ImGuiTestRunFunc)(ImGuiTestContext* ctx);
typedef void    (*ImGuiTestGuiFunc)(ImGuiTestContext* ctx);
typedef void    (*ImGuiTestTestFunc)(ImGuiTestContext* ctx);

// Wraps a placement new of a given type (where 'buffer' is the allocated memory)
typedef void    (*ImGuiTestUserDataConstructor)(void* buffer);
typedef void    (*ImGuiTestUserDataPostConstructor)(void* ptr, void* fn);
typedef void    (*ImGuiTestUserDataDestructor)(void* ptr);

// Storage for one test
struct ImGuiTest
{
    ImGuiTestGroup                  Group;              // Coarse groups: 'Tests' or 'Perf'
    bool                            NameOwned;          //
    const char*                     Category;           // Literal, not owned
    const char*                     Name;               // Literal, generally not owned unless NameOwned=true
    const char*                     SourceFile;         // __FILE__
    const char*                     SourceFileShort;    // Pointer within SourceFile, skips filename.
    int                             SourceLine;         // __LINE__
    int                             SourceLineEnd;      //
    int                             ArgVariant;         // User parameter, for use by GuiFunc/TestFunc. Generally we use it to run variations of a same test.
    size_t                          UserDataSize;       // When SetUserDataType() is used, we create an instance of user structure so we can be used by GuiFunc/TestFunc.
    ImGuiTestUserDataConstructor    UserDataConstructor;
    ImGuiTestUserDataPostConstructor UserDataPostConstructor;
    void*                           UserDataPostConstructorFn;
    ImGuiTestUserDataDestructor     UserDataDestructor;
    ImGuiTestStatus                 Status;
    ImGuiTestFlags                  Flags;              // See ImGuiTestFlags_
    ImGuiTestGuiFunc                GuiFunc;            // GUI functions can be reused
    ImGuiTestTestFunc               TestFunc;           // Test function
    int                             GuiFuncLastFrame;
    ImGuiTestLog                    TestLog;

    ImGuiTest()
    {
        Group = ImGuiTestGroup_Unknown;
        NameOwned = false;
        Category = NULL;
        Name = NULL;
        SourceFile = SourceFileShort = NULL;
        SourceLine = SourceLineEnd = 0;
        ArgVariant = 0;
        UserDataSize = 0;
        UserDataConstructor = NULL;
        UserDataPostConstructor = NULL;
        UserDataPostConstructorFn = NULL;
        UserDataDestructor = NULL;
        Status = ImGuiTestStatus_Unknown;
        Flags = ImGuiTestFlags_None;
        GuiFunc = NULL;
        TestFunc = NULL;
        GuiFuncLastFrame = -1;
    }
    ~ImGuiTest();

    void SetOwnedName(const char* name);

    template <typename T>
    void SetUserDataType(void(*post_initialize)(T& vars) = NULL)
    {
        UserDataSize = sizeof(T);
        UserDataConstructor = [](void* ptr) { IM_PLACEMENT_NEW(ptr) T; };
        UserDataDestructor = [](void* ptr) { IM_UNUSED(ptr); reinterpret_cast<T*>(ptr)->~T(); };
        if (post_initialize != NULL)
        {
            UserDataPostConstructorFn = (void*)post_initialize;
            UserDataPostConstructor = [](void* ptr, void* fn) { ((void (*)(T&))(fn))(*(T*)ptr); };
        }
    }
};