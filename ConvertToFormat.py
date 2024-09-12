#!/bin/python
# Made by WafflesExploits
from base64 import b64encode 

text = """unsigned char Encrypted_Shellcode[] = {0x99,0x12 }; 
unsigned char Encrypted_Key[] = {0xF6,0x6B}; 
unsigned char Protected_Key_for_bruteforce[] = {0x00,0xF0}; 
BYTE Hint_Byte = 0xCB;"""
# Remove strings
text = text.replace("unsigned char ", "");
text = text.replace("BYTE ", "");
text = text.replace("[]", "");
text = text.replace("{", "").replace("};", "");
text = text.replace(";","\n");
text = text.replace(" = ","-");

# Text final format:
#Encrypted_Shellcode-0x99,0x12  
#Encrypted_Key-0xF6,0x6B 
#Protected_Key_for_bruteforce-0x00,0xF0 
#Hint_Byte-0xCB

text_bytes = text.encode('utf-8')

# Now, base64 encode the bytes
encoded_text = b64encode(text_bytes).decode('utf-8')
print(encoded_text);

# Output:
#RW5jcnlwdGVkX1NoZWxsY29kZS0weDk5LDB4MTIgIApFbmNyeXB0ZWRfS2V5LTB4RjYsMHg2QiAKUHJvdGVjdGVkX0tleV9mb3JfYnJ1dGVmb3JjZS0weDAwLDB4RjAgCkhpbnRfQnl0ZS0weENCCg==
