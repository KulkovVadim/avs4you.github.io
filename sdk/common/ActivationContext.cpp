#include "ActivationContext.h"
#include <commctrl.h>

static HMODULE GetCurrentModule()
{
	HMODULE hModule = NULL;
	GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                      (LPCTSTR)GetCurrentModule, &hModule);
	return hModule;
}

ActivationContext::ActivationContext()
{
    ACTCTX actCtx = { sizeof(ACTCTX) };
    actCtx.dwFlags = ACTCTX_FLAG_RESOURCE_NAME_VALID | ACTCTX_FLAG_HMODULE_VALID;
    actCtx.lpResourceName = MAKEINTRESOURCE(CREATEPROCESS_MANIFEST_RESOURCE_ID);
    actCtx.hModule = GetCurrentModule();

    m_hActCtx = CreateActCtx(&actCtx);
    if (m_hActCtx != INVALID_HANDLE_VALUE) {
        ActivateActCtx(m_hActCtx, &m_cookie);
    }

    INITCOMMONCONTROLSEX icex = { sizeof(INITCOMMONCONTROLSEX), ICC_STANDARD_CLASSES };
    InitCommonControlsEx(&icex);
}

ActivationContext::~ActivationContext()
{
    if (m_cookie != 0) {
        DeactivateActCtx(0, m_cookie);
        m_cookie = 0;
    }
    
    if (m_hActCtx != INVALID_HANDLE_VALUE) {
        ReleaseActCtx(m_hActCtx);
        m_hActCtx = INVALID_HANDLE_VALUE;
    }
}
