unsigned char Busy_Check();
void Com_Write(unsigned char Command);
void Data_Write(unsigned char Data);
void Char_Write(unsigned char Hang, unsigned char Lie, unsigned char n);
void String_Write(unsigned char Hang1, unsigned char Lie1, unsigned char *s);
void Clear();
void Lcd_Init();