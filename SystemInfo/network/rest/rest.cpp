#include "rest.h"
#include "multipart_parser.h"
bool uploadImage(RESULT_STRUCT&, TCHAR*) {
	//Use MultipartParser to get the encoded body content and boundary
	MultipartParser parser;
	parser.AddParameter("upload_preset", "tnlvl6s7");
	parser.AddFile("file", "image.jpg");
	std::string boundary = parser.boundary();
	std::string body = parser.GenBodyContent();

	//Set up http client and request
	http_request req;
	http_client client(U("https://api.cloudinary.com/v1_1/:^)/image/upload"));
	req.set_method(web::http::methods::POST);
	req.set_body(body, "multipart/form-data; boundary=" + boundary);
	pplx::task<http_response> response_task = client.request(req);

	http_response response = response_task.get();
	if (response.status_code() == status_codes::OK) {
		pplx::task<json::value> json_task = response.extract_json();
		json::value v = json_task.get();

		for (auto iter = v.as_object().cbegin(); iter != v.as_object().cend(); ++iter)
		{
			// This change lets you get the string straight up from "first"
			const utility::string_t &str = iter->first;
			const json::value &v = iter->second;

		}
	}
}
