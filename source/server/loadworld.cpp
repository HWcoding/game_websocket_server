/*#include <cppcms/applications_pool.h>
#include <cppcms/http_context.h>
#include <cppcms/http_content_type.h>
#include <cppcms/http_response.h>
#include <cppcms/http_request.h>
#include <cppcms/defs.h>

#include <stdlib.h>
#include "main/fileloader.h"
#include "server/loadworld.h"

void LoadWorld::main(std::string){
	response().set_content_header("multipart/form-data");
	response().set_header("Cache-Control", "no-cache");
	if(read_file_gzip(contents,FileName))response().set_header("Content-Encoding", "gzip");
	response().io_mode(cppcms::http::response::asynchronous);
	response().set_header("Content-Length", std::to_string(contents.size()));
	booster::shared_ptr<cppcms::http::context> context = release_context();
	context->response().out() << contents;
	context->async_complete_response();
}*/
