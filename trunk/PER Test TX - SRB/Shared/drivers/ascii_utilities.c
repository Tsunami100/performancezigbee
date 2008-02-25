#include "pub_def.h"
#include "ascii_utilities.h"
#include "string.h"

void int2string (UINT32 value, char *string) {

	UINT8 remainder;
	UINT8 offset = 9;
	
	(void)strcpy(string, "          ");
	
	do {
		remainder = value % 10;
		string[offset] = UTILDec2Char(remainder);
		value = value / 10;
		offset--;
	} 
	while (value != 0);

	string[10] = 0;		//Set to null.

}

char UTILDec2Char(UINT8 value) {

	if (value <=9) // ((value >= 0) && (value <=9))
		return (char) (value + 0x30);
	else
		return (char) (value + 0x37);
}

UINT8 char2num(char ch) {

   UINT8 value=0;

   if ((ch >= '0') && (ch <='9')) {
      value = ch - 0x30;
   } else {
      value = 0;
   }
   
   return value;
}

UINT32 String2Uint32(char *str) {
   
   UINT8 u8i, u8length;
   UINT32 u32value=0;
   UINT32 u32total=0;
   UINT16 u16base=1;
   
   //Check for Null String
   u8length = (UINT8) strlen(str);
   if (u8length == 0) {
      return 0;
   }
   
   //Check for Overflow of the UINT32 variable.
   if (u8length > 8) {
      return 0xFFFFFFFF;
   }
   
   for (u8i=0; u8i<u8length; u8i++) {
     u32value = char2num((char) str[u8length-u8i-1]);
     u32value *= u16base;
     u16base *= 10;
     u32total += u32value;
   }
   
   return u32total;
}

UINT16 String2Uint16(char *str) {
   
   UINT8 u8i, u8length;
   UINT32 u32value=0;
   UINT16 u16total=0;
   UINT16 u16base=1;
   
   //Check for Null String
   u8length = (UINT8)strlen(str);
   if (u8length == 0) {
      return 0;
   }
   
   //Check for Overflow of the UINT32 variable.
   if (u8length > 8) {
      return 0xFFFFFFFF;
   }
   
   for (u8i=0; u8i<u8length; u8i++) {
     u32value = char2num((char) str[u8length-u8i-1]);
     u32value *= u16base;
     u16base *= 10;
     u16total += (UINT16)u32value;
   }
   
   return u16total;
}

void UTILInt2String (UINT32 value, UINT8 *string) {

    UINT8 remainder;
    UINT8 offset = 9;
    
    (void)strcpy(string, "          ");
    
    do {
        remainder = value % 10;
        string[offset] = UTILDec2Char(remainder);
        value = value / 10;
        offset--;
    } 
    while (value != 0);

    string[10] = 0;        //Set to null.

}

void Uint32ToHexString(UINT32 u32Num, UINT8 *pu8Str) {

	UINT8 u8Mod = 0;
	UINT8 u8Offset = 9;
	
	(void)strcpy(pu8Str, "00000000000");
	
	while(u32Num != 0){
	  u8Mod = u32Num % 16;
	  pu8Str[u8Offset] = UTILDec2Char(u8Mod);
	  u32Num/=16;
	  u8Offset--;
	}
	pu8Str[0] = '0';
	pu8Str[1] = 'x';
	pu8Str[10] = '\0';
	 
}

void trimWhiteSpace(char *str) {
   
   UINT8 length, i=0;
   UINT8 leading_ws=0;
   UINT8 ws_flag = TRUE;
   
   length = (UINT8) strlen(str);
   

      while ((i < length) && (ws_flag == TRUE)) {
         if (str[i] >= 33 && str[i] <=126) {
            ws_flag = FALSE;
         } else {
            i++;
         }
      }
      
      leading_ws = i;
      (void)strcpy(str, str + leading_ws);   
}
