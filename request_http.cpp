#include "request_http.h"

namespace fs = std::filesystem;

int RequestHttp::access_to_file_directory(const std::string &path_file_system, const std::string &url) {
    std::string full_path = path_file_system;
    for (int i = 0; i < url.size(); ++i) {
        if (url[i] == '/') {
            fs::file_status status = fs::status(full_path);
            fs::path resource(full_path);
            if (fs::exists (resource)){
                if ((status.permissions() & fs::perms::owner_read) == fs::perms::none) {
                    return FORBIDDEN;
                }
                full_path += '/';
            }
            else{
                return NOT_FOUND;
            }
            
        }
        else{
            full_path += url[i];
        }
    }
    fs::file_status status = fs::status(full_path);
    fs::path resource(full_path);
    if (fs::exists(resource)){
        if ((status.permissions() & fs::perms::owner_read) == fs::perms::none) {
            return FORBIDDEN;
        }
    }
    else{
        return NOT_FOUND;
    }

    return OK;
}