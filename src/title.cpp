#include "title.hpp"

std::string getTitle(int code) {
	if (code < 200) {
		return "Starward";
	}

	return std::to_string(code);
}

std::string getSubTitle(int code) {

	if (code < 100) {
		return "Invasion";
	}

	// informational
	if (code == 100) return "Continue";
	if (code == 101) return "Protocols";
	if (code == 102) return "Processing";
	if (code == 103) return "Early Hints";

	// success
	if (code == 200) return "Okay";
	if (code == 201) return "Created";
	if (code == 202) return "Accepted";
	if (code == 203) return "Non-Authoritative";
	if (code == 204) return "No Content";
	if (code == 205) return "Reset Content";
	if (code == 206) return "Partial Content";
	if (code == 207) return "Multi-Status";
	if (code == 208) return "Reported";
	if (code == 226) return "Used";

	// redirection
	if (code == 300) return "Choices";
	if (code == 301) return "Moved Permanently";
	if (code == 302) return "Found";
	if (code == 303) return "See Other";
	if (code == 304) return "Not Modified";
	if (code == 305) return "Use Proxy";
	if (code == 306) return "Switch Proxy";
	if (code == 307) return "Temporary Redirect";
	if (code == 308) return "Permanent Redirect";

	// client errors
	if (code == 400) return "Bad Request";
	if (code == 401) return "Unauthorized";
	if (code == 402) return "Payment Required";
	if (code == 403) return "Forbidden";
	if (code == 404) return "Not Found";
	if (code == 405) return "Not Allowed";
	if (code == 406) return "Not Acceptable";
	if (code == 407) return "Proxy Login Required";
	if (code == 408) return "Timeout";
	if (code == 409) return "Conflict";
	if (code == 410) return "Gone";
	if (code == 411) return "Length Required";
	if (code == 412) return "Precondition Failed";
	if (code == 413) return "Payload Too Large";
	if (code == 414) return "Too Long";
	if (code == 415) return "Unsupported Media";
	if (code == 416) return "Not Satisfiable";
	if (code == 417) return "Expectation";
	if (code == 418) return "I'm a Teapot";
	if (code == 421) return "Misdirected";
	if (code == 422) return "Unprocessable";
	if (code == 423) return "Locked";
	if (code == 424) return "Failed";
	if (code == 425) return "Too Early";
	if (code == 426) return "Upgrade Required";
	if (code == 428) return "Precondition";
	if (code == 429) return "Too Many Requests";
	if (code == 431) return "Header Won't Fit";
	if (code == 451) return "[REDACTED]";

	// server errors
	if (code == 500) return "Internal Server Error";
	if (code == 501) return "Not Implemented";
	if (code == 502) return "Bad Gateway";
	if (code == 503) return "Service Unavailable";
	if (code == 504) return "Gateway Timeout";
	if (code == 505) return "Version Not Supported";
	if (code == 506) return "Diplomatic Failure";
	if (code == 507) return "Insufficient Storage";
	if (code == 508) return "Loop Detected";
	if (code == 510) return "Not Extended";
	if (code == 511) return "Authentication Required";

	if (code > 400 && code < 500) return "Client Error";
	if (code > 500 && code < 600) return "Server Error";

	return "Undefined";
}
