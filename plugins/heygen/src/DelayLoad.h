
namespace utils {
    namespace delayload {
		FARPROC WINAPI delayHook(unsigned dliNotify, PDelayLoadInfo pdli)
		{
			if (dliNotify == dliNotePreLoadLibrary)
			{
				std::string s_name{pdli->szDll};
				for(auto& c : s_name) {
				   c = tolower(c);
				}

				if ( strcmp(s_name.c_str(), "libcurl.dll") == 0 ) {
					char path[MAX_PATH];
					if ( GetModuleFileName(HInstance, path, sizeof(path)) == 0 ) {
						int ret = GetLastError();
					} else {
						auto s_path = (std::filesystem::path(path).parent_path() / s_name).string();

						HMODULE hMod = LoadLibraryA(s_path.c_str());
						if (hMod != NULL) {
							return (FARPROC)hMod; // Return the loaded module handle
						}
					}
				}
			}

			return NULL;
		}
    }
}
