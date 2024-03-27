//#pragma warning(suppress : 4996)
/*
--   ----------------------------------------------------------------------
--   Project:          DLL creation
--
--   Author:           Hamza Qureshi
--
--   File name:        DataDecryption.h
--
--   Rev.:             1.0
--   Creation date:    DEC 2023
--
--   ---------------------------------------------------------------------
Purpose of this module: MAIN MODULE

----------Main body--------------

Comments:

WARININGS

---------------------------------------------------------------------------
*/
// DataDecryption.cpp : implementation file
//

//#define DEBUG
# undef DEBUG
# undef CMD_LINE



#include "pch.h"
#include <conio.h>
#include <cstdarg>
#include <limits.h>
#include <iostream>
#include <windows.h>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include "aes_enc_dec.h"
#include "config_parser.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "DataDecryption.h"
#include "framework.h"


# define NEW_LINE "\n"
# define ROTATING_FILES_QTY 3

constexpr size_t FILE_SIZE = 1024 * 1024 * 5; //5 MB Size

using json = nlohmann::json;


// DLL internal state variables:
static std::string buffer_table_name_;
static std::string encrption_key_;

std::shared_ptr<spdlog::logger> logger = nullptr; // Declare the logger outside the function to reuse it
std::string temp_path;



// Function declarations 
int readLogEnable();
void readLogPath();
void spdLogger(const std::string& message);
void initializeLogger();

inline std::string read_api_endpoint();
inline int read_api_active();
inline unsigned char extract_from_json(const std::string& parse_string_json, std::string& resultStr);
unsigned char api_call_post_method(const char* url, const char* table_name, const char* iccid_name, std::string & resultstr);


// Callback function to handle the response
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output)
{
	size_t totalSize = size * nmemb;
	output->append(static_cast<char*>(contents), totalSize);
	return totalSize;
}

unsigned char api_call_post_method(const char* url, const char* table_name, const char* iccid_name, std::string & resultstr)
{
	struct curl_slist* slist1;

	slist1 = NULL;
	slist1 = curl_slist_append(slist1, "Content-Type: application/json");

	nlohmann::json jsonData;
	jsonData["accessToken"] = "9AD202FC-FCD1-4E16-A11F-592356F9B7C31";
	jsonData["tableName"] = table_name;
	jsonData["pcId"] = "PC-1";
	jsonData["macAddress"] = "12345";
	
	// Convert the JSON object to a string
	std::string jsonString = jsonData.dump();
	// Initialize cURL
	CURL* curl = curl_easy_init();
	if (!curl)
	{
		std::cerr << "Error initializing cURL." << std::endl;
		return 0;
	}

	json m_handle;
	// Set the API endpoint URL
	curl_easy_setopt(curl, CURLOPT_URL, url);
	// Set the HTTP method to POST
	curl_easy_setopt(curl, CURLOPT_POST, 1L);

	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist1);

	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, jsonString.c_str());

	
#if DEBUG
	std::cout << std::endl << "JSON SENT: "<< jsonString << std::endl;
#endif


	// Set the callback function to handle the response
	std::string responseData;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &responseData);

	// Perform the HTTP request
	CURLcode res = curl_easy_perform(curl);

	// Check for errors
	if (res != CURLE_OK)
	{

		std::wstring errorMessage = L"Server access failed. API POST FUNCTION ERROR: CURLE_OK NOT FOUND! ";
		errorMessage += std::wstring(curl_easy_strerror(res), curl_easy_strerror(res) + strlen(curl_easy_strerror(res)));


#ifdef CMD_LINE
		std::wcout << "cURL request failed: " << errorMessage << std::endl;
#else
		MessageBox(NULL, errorMessage.c_str(), L"STC API Server Error", NULL);
#endif

		return 0;
	}
	else
	{
		// Get HTTP response code
		long response_code;
		char errorbuffer[CURL_ERROR_SIZE]; // Buffer to store error message

		curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
		curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, &errorbuffer);

		if (response_code >= 200 && response_code < 300) 		// Check if the response code is in the 2xx range (indicating success)

		{
#ifdef DEBUG
			std::cout << "Data send is " << jsonData << "\n";
			std::cout << "Response: " << responseData << std::endl;
#endif
			curl_easy_cleanup(curl);
			resultstr = responseData;
		//	return responseData;
			return 1;

		}
		else
		{
			
			std::wstring errorMessage = L"API POST FUNCTION ERROR:  ";
			errorMessage += std::wstring(curl_easy_strerror(res), curl_easy_strerror(res) + strlen(curl_easy_strerror(res))  + response_code);

#ifdef CMD_LINE
			std::wcout << "cURL request failed: " << errorMessage << std::endl;
#else
			MessageBox(NULL, errorMessage.c_str(), L"STC API Error", NULL);
#endif

			curl_easy_cleanup(curl);
			return 0;

		}
		return 0;
	}
	return 0;

}



inline unsigned char extract_from_json(const std::string& parse_string_json, std::string& resultStr)
{
//	try
//	{
		// Parse the JSON string
		json responseData = json::parse(parse_string_json);

		// Extract values
		std::string key = responseData.value("aesKey", ""); // Using value() to avoid exception if key is not found
		std::cout << "RESP: " << responseData << std::endl;

#ifdef DEBUG
		// Displaying additional debug information
		std::cout << "Key: " << key << std::endl;
#endif // DEBUG
		resultStr = key;
//		return (std::size(key) != 0) ? key : "";
		return 1;
		
//	}

	//catch (const json::parse_error& e)
	//{
	//	std::cerr << "JSON parsing error: " << e.what() << std::endl;

	//	// Convert the JSON string to a wide string for display in MessageBox
	//	std::wstring errorMsg = L"JSON Parsing failed\nJSON returned is: " + std::wstring(parse_string_json.begin(), parse_string_json.end());
	//	MessageBox(NULL, errorMsg.c_str(), L"STC JSON Parsing Error", MB_OK);
	//}
	//catch (const json::exception& e)
	//{
	//	std::cerr << "JSON exception: " << e.what() << std::endl;

	//	// Displaying additional debug information
	//	std::wstring errorMsg = L"JSON Parsing failed\nJSON returned is: " + std::wstring(parse_string_json.begin(), parse_string_json.end()) + L"\nException: " + std::wstring(e.what(), e.what() + strlen(e.what()));
	//	MessageBox(NULL, errorMsg.c_str(), L"STC JSON Parsing Error", MB_OK);
	//}
	//catch (const std::exception& e)
	//{
	//	std::cerr << "Exception: " << e.what() << std::endl;

	//	// Displaying additional debug information
	//	std::wstring errorMsg = L"Exception occurred\nException: " + std::wstring(e.what(), e.what() + strlen(e.what()));
	//	MessageBox(NULL, errorMsg.c_str(), L"STC Exception", MB_OK);
	//}

	//return 0;
}


#define INI_FILE_NAME "DataDecryption.ini"


inline std::string read_api_endpoint()
{
	static std::string api_endpoint;

	if (api_endpoint.empty())
	{
		configParser iniParser;

		if (iniParser.load(INI_FILE_NAME))
		{
			api_endpoint = iniParser.getValue("API_PATH", "API_ENDPOINT");
		}
		else
		{
//			MessageBox(NULL, L"API END POINT NOT MENTIONED ", L"STC INI Error", NULL);
			api_endpoint = "";  // Replace with a default value
		}
	}

	return api_endpoint;
}


inline int read_api_active()
{
	std::string enc_enable = "0"; // Initialize to a default value

	configParser iniParser;

	if (iniParser.load(INI_FILE_NAME))
	{
		enc_enable = iniParser.getValue("API_ACTIVE", "ENCRYPTION");
	}

	return atoi(enc_enable.c_str());
}

const char* decryptRecord(const char* table_name, const char* record)
{
	static int enc_enable = read_api_active();
	static std::string api_endpoint = read_api_endpoint();
	std::string key;
	std::string decrypted_record;
	unsigned int extractor_success;

	if (enc_enable)
	{
#ifdef DEBUG
		std::cout << "API :" << api_endpoint << std::endl;
		std::cout << "Encrypted record is : " << record << std::endl;
#endif // DEBUG

		const char* url = api_endpoint.c_str();

		// Check if table name changes
		std::string new_table_name = std::string(table_name);

		if (buffer_table_name_ != new_table_name)
		{
#ifdef DEBUG
			std::cout << "==================> Table name changed" << std::endl;
			std::cout << "==================> Previous: " << buffer_table_name_ << " New: " << new_table_name << std::endl;
#endif

			try
			{
				std::string apiResp;

#ifdef DEBUG
				std::cout << "==================> api return " << temp_str << std::endl;
#endif // DEBUG
				if (api_call_post_method(url, table_name, "", apiResp))
				{

					extractor_success = extract_from_json(apiResp, key);

					encrption_key_ = key;
					buffer_table_name_ = table_name;
#if DEBUG
					std::cout << "==================> API requested for key " << "key received is :" << key << std::endl;

#endif // DEBUG

				}
			}
			catch (...)
			{

				std::cout << "==================> Error calling API " << std::endl;
			}
		}
		else
		{
			// Use the previously obtained key
			key = encrption_key_;
		}

		// Perform AES decryption
		if (extractor_success==1)
		{
			unsigned char* temp_key_var = (unsigned char*)encrption_key_.c_str();
			AESEncDec m_AES(temp_key_var);

			decrypted_record = m_AES.decrypt_string_ecb_actual(record, encrption_key_);

#ifdef DEBUG
			std::cout << "" << std::endl;
			std::cout << "in dll: Decrypted record is : " << decrypted_record << std::endl;
#endif // DEBUG

			char* result = new char[decrypted_record.length() + 1];
			strcpy_s(result, decrypted_record.length() + 1, decrypted_record.c_str());
			return result;
		}
		//else
		//{
		//	MessageBox(NULL, L"API Response failed Function :\"DecryptRecord\" \nAdditional info: Error might be with key length or Json response.", L"STC Key Error", NULL);
		//	return "";
		//}
	}
	else
	{
		// return record when encryption is not enabled
		return record;
	}

	return "";
}



// Initialize logger during program initialization
void initializeLogger()
{
	try
	{
		// Create a rotating logger with a maximum size of 5 MB and 3 rotated files
		logger = spdlog::rotating_logger_mt("RECORD_LOGGER", temp_path, FILE_SIZE, ROTATING_FILES_QTY);
	}
	catch (const spdlog::spdlog_ex& ex) 
	{
		// Handle initialization error
		MessageBox(NULL, L"Log init failed Function \"init_logger\" ex.what()", L"STC Logging Error", NULL);
		std::cout << "Log init failed: " << ex.what() << std::endl;
	}
}

void spdLogger(const std::string& message)
{
	if (!logger) 
	{
		std::cout << "Logger not initialized. Initializing now..." << std::endl;
		initializeLogger(); // Initialize logger if not already initialized
	}
	logger->info(message);
}

// Read log settings from INI file
int readLogEnable()
{
	unsigned char log_enable;
	configParser iniParser;
	if (iniParser.load(INI_FILE_NAME))
	{
		// Read logging status
		log_enable = atoi(iniParser.getValue("LOGS", "LOGS_ACTIVE").c_str());
		return log_enable;
	}
	else
	{
		return 0;
	}
}

// Read log settings from INI file
void readLogPath()
{
	configParser iniParser;
	if (iniParser.load(INI_FILE_NAME))
	{
		// Read log file path
		temp_path = iniParser.getValue("LOGS", "LOGS_PATH");
	}
}

// Write log entry with variadic arguments
void writeLogEntry(const unsigned char count, const char* first, ...) 
{
	
	// Initialize logging variables if not already initialized
	if (readLogEnable())
	{
		readLogPath();
	}

	if (readLogEnable()) 
	{
		// Initialize logger if not already initialized
		if (!logger) 
		{
			initializeLogger();
		}

		va_list args;
		va_start(args, first);
		std::stringstream stream;
		const char* current = first;
		for (unsigned char iteration = 0; iteration < count; ++iteration) 
		{
#ifdef DEBUG
			std::cout << current << std::endl;
#endif // DEBUG
			stream << current << ",";
			current = va_arg(args, const char*);
		}
		va_end(args);

#ifdef DEBUG
		std::cout << stream.str() << std::endl;
		std::cout << "Log File Path is :" << temp_path << std::endl;
#endif // DEBUG
		spdLogger(stream.str());
	}
}
