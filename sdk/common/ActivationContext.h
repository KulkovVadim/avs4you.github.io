#pragma once
#include <Windows.h>

class ActivationContext
{
public:
    ActivationContext();
    ActivationContext(const ActivationContext&) = delete;
    ~ActivationContext();
    
    ActivationContext& operator=(const ActivationContext&) = delete;
    
private:
    HANDLE m_hActCtx = INVALID_HANDLE_VALUE;
    ULONG_PTR m_cookie = 0;
};
