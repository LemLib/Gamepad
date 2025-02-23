#pragma once

#define DO_PRAGMA(x) _Pragma(#x)

// We only define the TODO/FIXME macros if the file is being compiled by Microsoft Intellisense
// or clangd. This way, the TODO/FIXME messages don't clutter the compilation messages.
#if defined(_debug) || defined(__clang__)
#define TODO(x) DO_PRAGMA(message("TODO - " x))
#define FIXME(x) DO_PRAGMA(warning("FIXME - " x))
#else
#define TODO(x)
#define FIXME(x)
#endif