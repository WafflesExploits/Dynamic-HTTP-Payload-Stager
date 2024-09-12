#!/bin/python
# ./ConvertToFormat.py
# Made by WafflesExploits
from base64 import b64encode 

#--Replace these values--
delimiter = "-"
variables = """
unsigned char Encrypted_Shellcode[] = {0x99,0x12 }; 

unsigned char Encrypted_Key[] = {0xF6,0x6B}; 

unsigned char Protected_Key_for_bruteforce[] = {0x00,0xF0}; 

BYTE Hint_Byte = 0xCB;"""

#--Script Start--
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
print(encoded_text);

# Output:
#RW5jcnlwdGVkX1NoZWxsY29kZS0weDk5LDB4MTIgIApFbmNyeXB0ZWRfS2V5LTB4RjYsMHg2QiAKUHJvdGVjdGVkX0tleV9mb3JfYnJ1dGVmb3JjZS0weDAwLDB4RjAgCkhpbnRfQnl0ZS0weENCCg==
