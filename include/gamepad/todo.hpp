#pragma once

#define DO_PRAGMA(x) _Pragma(#x)
#define TODO(x) DO_PRAGMA(message("TODO - " #x))
#define FIXME(x) DO_PRAGMA(warning("FIXME - " #x))