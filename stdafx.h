#define STRICT
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <MMSystem.h>

#include <comdef.h>
#include <atlbase.h>
#include <atlcom.h>

#include <d3d9.h>
#include <d3dx9.h>

#include <string>
using std::string;

#include <sstream>

#include <memory>
using std::shared_ptr;
using std::make_shared;
using std::unique_ptr;

_COM_SMARTPTR_TYPEDEF(ID3DXConstantTable, IID_ID3DXConstantTable);
_COM_SMARTPTR_TYPEDEF(ID3DXBuffer, IID_ID3DXBuffer);