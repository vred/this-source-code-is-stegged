This is a test of steganography software applied against source code. 

DEBUG:

00 = 000 000 00 = 09 -- -- -- -- -- -- -- | 09 -- -- -- -- -- -- -- | 09 09 
01 = 000 000 01 = 09 -- -- -- -- -- -- -- | 09 -- -- -- -- -- -- -- | 09 20 20 --
FF = 111 111 11 = 09 20 20 20 20 20 20 20 | 09 20 20 20 20 20 20 20 | 09 20 20 20
FE = 111 111 10 = 09 20 20 20 20 20 20 20 | 09 20 20 20 20 20 20 20 | 09 20 -- --
FD = 111 111 01 = 09 20 20 20 20 20 20 20 | 09 20 20 20 20 20 20 20 | 09 20 20 --
FC = 111 111 00 = 09 20 20 20 20 20 20 20 | 09 20 20 20 20 20 20 20 | 09 09 -- --

24 = 001 001 00 = 09 20 20 20 20 -- -- -- | 09 20 20 20 20 -- -- -- | 09 09 -- --
30 = 001 100 00 = 09 20 20 20 20 -- -- -- | 09 20 -- -- -- -- -- -- | 09 09 -- --

		3b  = 8B sequence 	 	   Whitespaces 
		-----------------------------    ---------------
		000 = 09 -- -- -- -- -- -- --   	0
		001 = 09 20 20 20 20 -- -- --   	4
		010 = 09 20 20 -- -- -- -- --   	2
		011 = 09 20 20 20 20 20 20 --   	6
		100 = 09 20 -- -- -- -- -- --   	1
		101 = 09 20 20 20 20 20 -- --   	5
		110 = 09 20 20 20 -- -- -- --   	3
		111 = 09 20 20 20 20 20 20 20   	7

Scrap:
------	
10100000
10100101
10010001

11001000
00011110
11111011

110
