// src/tools/workspace.cpp — YOU IMPLEMENT (spec §4.4).
//
// Workspace::resolve: a purely lexical check, steps 1–6 of the spec.
// Do not touch the filesystem here (no exists(), canonical(), ...).
// The constructor, root() and check_symlinks() are provided in
// workspace_provided.cpp.

#include "tools/workspace.h"

namespace fs = std::filesystem;

bool Workspace::resolve(std::string_view user_path, fs::path* out,
                        std::string* reason) const {
    
    // reject if user_path empty
    if(user_path.empty()){
        *reason = "empty path";
        return false;
    }

    // reject if user_path has null character
    if(user_path.find('\0') != std::string::npos){
        *reason = "contains null character";
        return false;
    }

    // Build path
    std::filesystem::path p{std::string(user_path)};

    // reject if path is absolute or has root path
    if(p.is_absolute() || p.has_root_path()){
        *reason = "absolute path; should be workspace-relative";
        return false;
    }

    // normalizes file path
    auto n = p.lexically_normal();

    // reject if path climbs above root
    if(*n.begin() == ".."){
        *reason = "path escapes root folder; should stay within";
        return false;
    }

    // Relate path to root
    if(n == "."){
        *out = root();
    } else{
        *out = root() / n;
    }

    *reason = "pass";
    return true;
}
