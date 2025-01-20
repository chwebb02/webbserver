#ifndef _HTTP_H_
#define _HTTP_H_

/// @brief Enum describing the different types of HTTP request
typedef enum httpRequestTypeEnum {
	GET,
	HEAD,
	POST,
	PUT,
	DELETE,
	CONNECT,
	OPTIONS,
	TRACE,
	PATCH
} httpRequestType_t;

/// @brief Structure defining the header of an HTTP request
typedef struct httpRequestHeaderStruct {
	httpRequestType_t type;
	char *httpStandard;
	char *uri;
} httpRequestHeader_t;

/// @brief Structure defining the payload of an HTTP request
typedef struct httpRequestPayloadStruct {

} httpRequestPayload_t;

/// @brief Structure defining an HTTP request
typedef struct httpRequestStruct {
	httpRequestHeader_t header;
} httpRequest_t;

/// @brief Structure defining the header of an HTTP response
typedef struct httpResponseHeaderStruct {

} httpResponseHeader_t;

/// @brief Structure defining an HTTP response
typedef struct httpResponseStruct {
	httpResponseHeader_t header;
} httpResponse_t;

/// @brief Convert a raw HTTP request into an HTTP request structure
/// @param buffer The raw HTTP request coming from the network
/// @return An HTTP request object or NULL on error
httpRequest_t *deserializeHttpRequest(const char *buffer);

/// @brief Convert an HTTP response object for network transmission
/// @param response The HTTP response object
/// @return A raw string containing the HTTP request 
char *serializeHttpResponse(httpResponse_t *response);

#endif /* _HTTP_H_ */