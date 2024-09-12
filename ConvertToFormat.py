#!/bin/python
# ./ConvertToFormat.py
# Made by WafflesExploits
from base64 import b64encode 
from sys import argv

# These variables are not to be changed.
delimiter = ""
output_path = ""
arguments_count = len(argv)

if(arguments_count < 2):
    print("./ConvertToFormat.py <file> [<delimiter> <output-path>]")
    print("[i] File content example: ")
    print("""unsigned char Encrypted_Shellcode[] = {0x99,0x12 }; 
unsigned char Encrypted_Key[] = {0xF6,0x6B}; 
unsigned char Protected_Key_for_bruteforce[] = {0x00,0xF0}; 
BYTE Hint_Byte = 0xCB;""")
    exit()
elif(arguments_count < 3):
    delimiter = "-"
elif(arguments_count < 4):
    delimiter = argv[2]
else:
    delimiter = argv[2]
    output_path = argv[3]
    

file_path = argv[1]

f = open(file_path, "r")
variables = f.read()


# Remove strings
text = variables
text = text.replace("\n","");
text = text.replace("unsigned char ", "");
text = text.replace("BYTE ", "");
text = text.replace("[]", "");
text = text.replace(";","\n");
text = text.replace("{", "").replace("}", "");
text = text.replace(" = ",delimiter);
text = text.replace(" ","");
text = text[:-1] # Remove last \n character

#print(text)
# Example Text final format:
#Encrypted_hex_code-0x99,0x12  
#Encrypted_Key-0xF6,0x6B 
#Protected_Key_for_bruteforce-0x00,0xF0 
#HintByte-0xCB

text_bytes = text.encode('utf-8')

# Now, base64 encode the bytes
encoded_text = b64encode(text_bytes).decode('utf-8')
if(output_path):
    f = open(output_path, "w")
    f.write(encoded_text)
    f.close()
    print(f"[i] Using delimiter \"{delimiter}\"")
    print(f"[i] Saved file to \"{output_path}\"")
else:
    print(encoded_text)
    if(arguments_count < 3):
        print("[i] Using default delimiter \"-\"")
    else:
        print(f"[i] Using delimiter \"{delimiter}\"")
