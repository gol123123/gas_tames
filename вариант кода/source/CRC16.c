/*
  Name  : CRC-16 CCITT
  Poly  : 0x1021    x^16 + x^12 + x^5 + 1
  Init  : 0xFFFF
  Revert: false
  XorOut: 0x0000
  Check : 0x29B1 ("123456789")
  MaxLen: 4095 байт (32767 бит) - обнаружение
  одинарных, двойных, тройных и всех нечетных ошибок
*/
  
uint16_t Crc16_element(uint8_t data, uint16_t crc)
	{
    uint8_t i;

 	crc ^= (uint16_t)(data)*256;
 	for (i = 0; i < 8; i++) crc = crc & 0x8000 ? (crc*2) ^ 0x1021 : crc*2;
    return crc;
	}

uint16_t Crc16 (uint8_t *buf, uint16_t len)
	{
    uint16_t crc = 0xFFFF;
   	uint8_t i = 0;

    while (i++ < len) crc = Crc16_element (*buf++, crc);
 	return crc;
	}
