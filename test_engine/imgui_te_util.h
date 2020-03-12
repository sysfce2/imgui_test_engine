#pragma once
#include <math.h>   // fabsf

typedef int ImGuiKeyModFlags;       // See ImGuiKeyModFlags_

enum ImGuiKeyModFlags_
{
    ImGuiKeyModFlags_None           = 0,
    ImGuiKeyModFlags_Ctrl           = 1 << 0,
    ImGuiKeyModFlags_Alt            = 1 << 1,
    ImGuiKeyModFlags_Shift          = 1 << 2,
    ImGuiKeyModFlags_Super          = 1 << 3,
#if defined(__APPLE__)
    ImGuiKeyModFlags_Shortcut = ImGuiKeyModFlags_Super
#else
    ImGuiKeyModFlags_Shortcut = ImGuiKeyModFlags_Ctrl
#endif
};

enum ImGuiKeyState
{
    ImGuiKeyState_Unknown,
    ImGuiKeyState_Up,       // Released
    ImGuiKeyState_Down      // Pressed/held
};

// Maths helpers
static inline bool  ImFloatEq(float f1, float f2) { float d = f2 - f1; return fabsf(d) <= FLT_EPSILON; }

// Miscellaneous functions
ImGuiID             ImHashDecoratedPath(const char* str, ImGuiID seed = 0);
const char*         GetImGuiKeyName(ImGuiKey key);
void                GetImGuiKeyModsPrefixStr(ImGuiKeyModFlags mod_flags, char* out_buf, size_t out_buf_size);
ImFont*             FindFontByName(const char* name);

// Helper: maintain/calculate moving average
template<typename TYPE>
struct ImMovingAverage
{
    ImVector<TYPE>  Samples;
    TYPE            Accum;
    int             Idx;
    int             FillAmount;

    ImMovingAverage()               { Accum = (TYPE)0; Idx = FillAmount = 0; }
    void    Init(int count)         { Samples.resize(count); memset(Samples.Data, 0, Samples.Size * sizeof(TYPE)); Accum = (TYPE)0; Idx = FillAmount = 0; }
    void    AddSample(TYPE v)       { Accum += v - Samples[Idx]; Samples[Idx] = v; if (++Idx == Samples.Size) Idx = 0; if (FillAmount < Samples.Size) FillAmount++;  }
    TYPE    GetAverage() const      { return Accum / (TYPE)FillAmount; }
    int     GetSampleCount() const  { return Samples.Size; }
    bool    IsFull() const          { return FillAmount == Samples.Size; }
};

//-----------------------------------------------------------------------------
// Misc ImGui extensions
//-----------------------------------------------------------------------------

class Str;

namespace ImGui
{

void    PushDisabled();
void    PopDisabled();

// STR + InputText bindings (FIXME: move to Str.cpp?)
bool    InputText(const char* label, Str* str, ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
bool    InputTextMultiline(const char* label, Str* str, const ImVec2& size = ImVec2(0, 0), ImGuiInputTextFlags flags = 0, ImGuiInputTextCallback callback = NULL, void* user_data = NULL);
}
